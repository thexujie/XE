#include "Importer.h"

namespace fbx
{
    using namespace fbxsdk;
	
    const Matrix Matrix::Identity = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

    Importer::Importer()
    {
        
    }

    Importer::~Importer()
    {
        
    }

    fbx::error_e Importer::LoadFromFile(std::string path)
    {
		//_transform.SetS({ 10, 10, 10, 0 });
		Manager = std::unique_ptr<FbxManager, void(*)(FbxManager *)>(FbxManager::Create(), fbx_object_deleter);
		Scene = std::unique_ptr<FbxScene, void(*)(FbxScene *)>(FbxScene::Create(Manager.get(), ""), fbx_object_deleter);
		GeometryConverter = std::make_unique<FbxGeometryConverter>(Manager.get());
        Imper = std::unique_ptr<FbxImporter, void(*)(FbxImporter *)>(FbxImporter::Create(Manager.get(), ""), fbx_object_deleter);

        bool ok_init = Imper->Initialize(path.c_str(), -1, nullptr);
        if (!ok_init)
        {
            return fbx::error_inner;
        }

        auto ok_import = Imper->Import(Scene.get());
        if (!ok_import)
        {
            return fbx::error_inner;
        }

        FbxManager::GetFileFormatVersion(_sdkVersionMajor, _sdkVersionMinor, _sdkVersionRevision);
        Imper->GetFileVersion(_fileVersionMajor, _fileVersionMinor, _fileVersionRevision);

        fbx::error_e err = fbx::error_ok;
        err = ConvertScene();
        if (err)
            return err;

        auto pfnParseNode = [this](FbxNode * Node, auto & pfnSelf)->void
        {
			_nodes.push_back(Node);
        	
            FbxNodeAttribute * nodeattr = Node->GetNodeAttribute();
            if (nodeattr && nodeattr->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                //if (Node->GetMesh() && Node->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) > 0)
                    _meshs.push_back(Node);
            }

            if (Node->GetSkeleton())
                _skels.push_back(Node);

            for (int ichild = 0; ichild < Node->GetChildCount(); ++ichild)
                pfnSelf(Node->GetChild(ichild), pfnSelf);
        };

        FbxNode * RootNode = Scene->GetRootNode();
        pfnParseNode(RootNode, pfnParseNode);

        err = ConvertScene();
        if (err)
            return err;

        err = ImportBone();
        if (err)
            return err;

        err = ImportMaterials();
        if (err)
            return err;

        err = ImportMeshs();
        if (err)
            return err;

        if (_options.bUseMikkTSpace && (_options.bComputeNormals || _options.bComputeTangents))
        {
            err = ComputeTangents_MikkTSpace();
            if (err)
                return err;
        }
        else
        {
            err = ComputeTangents();
            if (err)
                return err;
        }

        //_sections.clear();
        err = BuildSections();
        if (err)
            return err;

        err = BuildModel();
        if (err)
            return err;

        err = ImportMorphTargets();
        if (err)
            return err;

        err = ImportBoneTrackers();
        if (err)
            return err;

        return fbx::error_ok;
    }

    fbx::error_e Importer::ConvertScene()
    {
        //Set the original file information
        _fileAxis = Scene->GetGlobalSettings().GetAxisSystem();
        _fileUnit = Scene->GetGlobalSettings().GetSystemUnit();

        FbxAMatrix AxisConversionMatrix;
        AxisConversionMatrix.SetIdentity();

        FbxAMatrix JointOrientationMatrix;
        JointOrientationMatrix.SetIdentity();

        if (_options.bConvertScene)
        {
            // we use -Y as forward axis here when we import. This is odd considering our forward axis is technically +X
            // but this is to mimic Maya/Max behavior where if you make a model facing +X facing, 
            // when you import that mesh, you want +X facing in engine. 
            // only thing that doesn't work is hand flipping because Max/Maya is RHS but UE is LHS
            // On the positive note, we now have import transform set up you can do to rotate mesh if you don't like default setting
            FbxAxisSystem::ECoordSystem CoordSystem = FbxAxisSystem::eRightHanded;
            FbxAxisSystem::EUpVector UpVector = FbxAxisSystem::eZAxis;
            FbxAxisSystem::EFrontVector FrontVector = (FbxAxisSystem::EFrontVector) - FbxAxisSystem::eParityOdd;
            if (_options.bForceFrontXAxis)
            {
                FrontVector = FbxAxisSystem::eParityEven;
            }

            FbxAxisSystem UnrealImportAxis(UpVector, FrontVector, CoordSystem);
            UnrealImportAxis = FbxAxisSystem::eOpenGL;
            UnrealImportAxis = FbxAxisSystem::DirectX;

            FbxAxisSystem SourceSetup = Scene->GetGlobalSettings().GetAxisSystem();
            UnrealImportAxis = SourceSetup;
            if (SourceSetup != UnrealImportAxis)
            {
                FbxRootNodeUtility::RemoveAllFbxRoots(Scene.get());
                UnrealImportAxis.ConvertScene(Scene.get());

                FbxAMatrix SourceMatrix;
                SourceSetup.GetMatrix(SourceMatrix);
                FbxAMatrix UE4Matrix;
                UnrealImportAxis.GetMatrix(UE4Matrix);
                AxisConversionMatrix = SourceMatrix.Inverse() * UE4Matrix;


                if (_options.bForceFrontXAxis)
                {
                    JointOrientationMatrix.SetR(FbxVector4(-90.0, -90.0, 0.0));
                }
            }
        }

        _converter.SetJointPostConversionMatrix(JointOrientationMatrix);
        _converter.SetAxisConversionMatrix(AxisConversionMatrix);

        // Convert the scene's units to what is used in this program, if needed.
        // The base unit used in both FBX and Unreal is centimeters.  So unless the units 
        // are already in centimeters (ie: scalefactor 1.0) then it needs to be converted
		FbxSystemUnit::ConversionOptions Options = { true, true, true, true, true, true };
		FbxSystemUnit cm;
        if (_options.bConvertSceneUnit && Scene->GetGlobalSettings().GetSystemUnit() != cm)
        {
            cm.ConvertScene(Scene.get(), Options);
        }

        //Reset all the transform evaluation cache since we change some node transform
        Scene->GetAnimationEvaluator()->Reset();

        const FbxGlobalSettings & GlobalSettings = Scene->GetGlobalSettings();
        FbxTime::EMode TimeMode = GlobalSettings.GetTimeMode();
        //Set the original framerate from the current fbx file
        OriginalFbxFramerate = FbxTime::GetFrameRate(TimeMode);
        return fbx::error_ok;
    }

    fbx::error_e Importer::ImportBone()
    {
		if (_nodes.empty())
			return fbx::error_bad_data;

		auto pfnFindFirstBone = [this](FbxNode * Node, auto & pfnSelf)-> FbxNode*
		{
			if (IsUnrealBone(Node))
				return Node;
			
			for (int ChildIndex = 0; ChildIndex < Node->GetChildCount(); ChildIndex++)
			{
				FbxNode * firstBone = pfnSelf(Node->GetChild(ChildIndex), pfnSelf);
				if (firstBone)
					return firstBone;
			}
			return nullptr;
		};
    	
        auto pfnRecursiveBuildSkeleton = [this](FbxNode * Link, auto & pfnSelf)->void
        {
            if (IsUnrealBone(Link))
            {
                _links.push_back(Link);
                for (int ChildIndex = 0; ChildIndex < Link->GetChildCount(); ChildIndex++)
                    pfnSelf(Link->GetChild(ChildIndex), pfnSelf);
            }
        };

        std::vector<FbxPose *> PoseArray;
        std::vector<FbxCluster *> ClusterArray;

    	if (_meshs.empty())
    	{
    		// 没有网格，可能是纯动画文件，直接导入动画即可
			pfnRecursiveBuildSkeleton(pfnFindFirstBone(Scene->GetRootNode(), pfnFindFirstBone), pfnRecursiveBuildSkeleton);
    	}
        else if (_meshs[0]->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) == 0)
        {
            pfnRecursiveBuildSkeleton(GetRootSkeleton(Scene.get(), _meshs[0], _creator), pfnRecursiveBuildSkeleton);
        }
        else
        {
        	// 分析每个网格被哪些骨骼影响，并保存这些骨骼。
            for (int32_t i = 0; i < _meshs.size(); i++)
            {
                FbxMesh * FbxMesh = _meshs[i]->GetMesh();
                const int32_t SkinDeformerCount = FbxMesh->GetDeformerCount(FbxDeformer::eSkin);
                for (int32_t DeformerIndex = 0; DeformerIndex < SkinDeformerCount; DeformerIndex++)
                {
                    FbxSkin * Skin = (FbxSkin *)FbxMesh->GetDeformer(DeformerIndex, FbxDeformer::eSkin);
                    for (int32_t ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ClusterIndex++)
                    {
                        ClusterArray.push_back(Skin->GetCluster(ClusterIndex));
                    }
                }
            }

            PoseArray = RetrievePoseFromBindPose(Scene.get(), _meshs);
            if (PoseArray.empty())
            {
                int32_t PoseCount = Scene->GetPoseCount();
                for (int32_t PoseIndex = PoseCount - 1; PoseIndex >= 0; --PoseIndex)
                {
                    FbxPose * CurrentPose = Scene->GetPose(PoseIndex);

                    // current pose is bind pose, 
                    if (CurrentPose && CurrentPose->IsBindPose())
                    {
                        Scene->RemovePose(PoseIndex);
                        CurrentPose->Destroy();
                    }
                }

                Manager->CreateMissingBindPoses(Scene.get());
                PoseArray = RetrievePoseFromBindPose(Scene.get(), _meshs);
            	
				if (PoseArray.empty())
					log_inf(__FUNCTION__ " PoseArray is empty!");
            }

            // BuildSkeletonSystem
            {
                FbxNode * Link;
                std::vector<FbxNode *> RootLinks;
                for (size_t ClusterIndex = 0; ClusterIndex < ClusterArray.size(); ClusterIndex++)
                {
                    Link = ClusterArray[ClusterIndex]->GetLink();
                    if (Link)
                    {
                        Link = GetRootSkeleton(Scene.get(), Link, _creator);
                        size_t LinkIndex;
                        for (LinkIndex = 0; LinkIndex < RootLinks.size(); LinkIndex++)
                        {
                            if (Link == RootLinks[LinkIndex])
                            {
                                break;
                            }
                        }

                        // this link is a new root, add it
                        if (LinkIndex == RootLinks.size())
                        {
                            RootLinks.push_back(Link);
                        }
                    }
                }

                for (size_t LinkIndex = 0; LinkIndex < RootLinks.size(); LinkIndex++)
                {
                    pfnRecursiveBuildSkeleton(RootLinks[LinkIndex], pfnRecursiveBuildSkeleton);
                }
            }
        }

		assert(!_links.empty());
        if (_links.empty())
        {
            // 这种情况下，需要将顶点绑定到网格上，使得它一定是一个 Skined Mesh
            _useGeneratedBones = true;
        }

        _useTime0AsRefPose = _options.bUseT0AsRefPose;
        if (PoseArray.empty() && !_useTime0AsRefPose)
        {
            // 没有默认姿态
            if (_options.bImportScene)
            {
                //fbx::war() << __FUNCTION__ " no poses while import scene";
                return fbx::error_inner;
            }

            //fbx::inf() << __FUNCTION__ " no poses, use T0 as ref pose";
            _useTime0AsRefPose = true;
        }

        // link 不能同名
        for (size_t LinkIndex = 0; LinkIndex < _links.size(); ++LinkIndex)
        {
            FbxNode * Link = _links[LinkIndex];

            for (size_t AltLinkIndex = LinkIndex + 1; AltLinkIndex < _links.size(); ++AltLinkIndex)
            {
                FbxNode * AltLink = _links[AltLinkIndex];

                if (_strcmpi(Link->GetName(), AltLink->GetName()) == 0)
                {
                    //fbx::war() << __FUNCTION__ " duplicate bone name " << Link->GetName();
                    return fbx::error_bad_data;
                }
            }
        }

        std::vector<FbxAMatrix> GlobalsPerLink(_links.size());
        GlobalsPerLink[0].SetIdentity();

        bool GlobalLinkFoundFlag;
        FbxAMatrix LocalTransform;

        bool bAnyLinksNotInBindPose = false;
        std::vector<FbxNode *> LinksWithoutBindPoses;
        int32_t NumberOfRoot = 0;

        int32_t RootIdx = -1;

        for (size_t LinkIndex = 0; LinkIndex < _links.size(); LinkIndex++)
        {
            FbxNode * Link = _links[LinkIndex];
            // Add a bone for each FBX Link
            _bones.push_back(Bone());


            // get the link parent and children
            int32_t ParentIndex = FBX_INDEX_NONE; // base value for root if no parent found
            FbxNode * LinkParent = Link->GetParent();
            if (LinkIndex)
            {
                for (int32_t ll = 0; ll < LinkIndex; ++ll) // <LinkIndex because parent is guaranteed to be before child in sortedLink
                {
                    FbxNode * Otherlink = _links[ll];
                    if (Otherlink == LinkParent)
                    {
                        ParentIndex = ll;
                        break;
                    }
                }
            }

            // see how many root this has
            // if more than 
            if (ParentIndex == FBX_INDEX_NONE)
            {
                ++NumberOfRoot;
                RootIdx = LinkIndex;
                if (NumberOfRoot > 1)
                {
                    //fbx::war() << __FUNCTION__ "Multiple roots are found in the bone hierarchy. We only support single root bone";
                    return fbx::error_bad_data;
                }
            }

            GlobalLinkFoundFlag = false;
            int SkelType = 0;
            if (!SkelType) //skeletal mesh
            {
                // there are some _links, they have no cluster, but in bindpose
                if (!PoseArray.empty())
                {
                    for (int32_t PoseIndex = 0; PoseIndex < PoseArray.size(); PoseIndex++)
                    {
                        int32_t PoseLinkIndex = PoseArray[PoseIndex]->Find(Link);
                        if (PoseLinkIndex >= 0)
                        {
                            FbxMatrix NoneAffineMatrix = PoseArray[PoseIndex]->GetMatrix(PoseLinkIndex);
                            FbxAMatrix Matrix = *(FbxAMatrix *)(double *)& NoneAffineMatrix;
                            GlobalsPerLink[LinkIndex] = Matrix;
                            GlobalLinkFoundFlag = true;
                            break;
                        }
                    }
                }

                if (!GlobalLinkFoundFlag)
                {
                    // since now we set use time 0 as ref pose this won't unlikely happen
                    // but leaving it just in case it still has case where it's missing partial bind pose
                    if (!_useTime0AsRefPose)
                    {
                        bAnyLinksNotInBindPose = true;
                        LinksWithoutBindPoses.push_back(Link);
                    }

                    for (int32_t ClusterIndex = 0; ClusterIndex < ClusterArray.size(); ClusterIndex++)
                    {
                        FbxCluster * Cluster = ClusterArray[ClusterIndex];
                        if (Link == Cluster->GetLink())
                        {
                            Cluster->GetTransformLinkMatrix(GlobalsPerLink[LinkIndex]);
                            GlobalLinkFoundFlag = true;
                            break;
                        }
                    }
                }
            }

            if (!GlobalLinkFoundFlag)
            {
                GlobalsPerLink[LinkIndex] = Link->EvaluateGlobalTransform();
            }

            if (_useTime0AsRefPose && !_options.bImportScene)
            {
                FbxAMatrix & T0Matrix = Scene->GetAnimationEvaluator()->GetNodeGlobalTransform(Link, 0);
                if (GlobalsPerLink[LinkIndex] != T0Matrix)
                {
                    _bOutDiffPose = true;
                }

                GlobalsPerLink[LinkIndex] = T0Matrix;
            }

            //Add the join orientation
            GlobalsPerLink[LinkIndex] = GlobalsPerLink[LinkIndex] * _converter.GetJointPostConversionMatrix();
            if (LinkIndex)
            {
                auto ParentIndexMatrixI = GlobalsPerLink[ParentIndex].Inverse();
                LocalTransform = GlobalsPerLink[ParentIndex].Inverse() * GlobalsPerLink[LinkIndex];
            }
            else	// skeleton root
            {
                // for root, this is global coordinate
                LocalTransform = GlobalsPerLink[LinkIndex];
            }

            // set bone
            Bone & Bone = _bones[LinkIndex];
            Bone.Name = Link->GetName();

            FbxSkeleton * Skeleton = Link->GetSkeleton();
            if (Skeleton)
            {
                Bone.BonePos.Length = _converter.ConvertDist(Skeleton->LimbLength.Get());
                Bone.BonePos.XSize = _converter.ConvertDist(Skeleton->Size.Get());
                Bone.BonePos.YSize = _converter.ConvertDist(Skeleton->Size.Get());
                Bone.BonePos.ZSize = _converter.ConvertDist(Skeleton->Size.Get());
            }
            else
            {
                Bone.BonePos.Length = 1.0;
                Bone.BonePos.XSize = 100.0;
                Bone.BonePos.YSize = 100.0;
                Bone.BonePos.ZSize = 100.0;
            }

            // get the link parent and children
            Bone.ParentIndex = ParentIndex;
            Bone.NumChildren = 0;
            for (int32_t ChildIndex = 0; ChildIndex < Link->GetChildCount(); ChildIndex++)
            {
                FbxNode * Child = Link->GetChild(ChildIndex);
                if (IsUnrealBone(Child))
                {
                    Bone.NumChildren++;
                }
            }

            FbxMatrix FM(LocalTransform);
            Bone.BonePos.Transform = LocalTransform;
        }

        //In case we do a scene import we need a relative to skeletal mesh transform instead of a global
        if (_options.bImportScene && !_options.bTransformVertexToAbsolute && !_meshs.empty())
        {
            auto SkeletalMeshNode = _meshs[0];
            FbxAMatrix GlobalSkeletalNodeFbx = Scene->GetAnimationEvaluator()->GetNodeGlobalTransform(SkeletalMeshNode, 0);
            FbxAMatrix GlobalSkeletalNode = GlobalSkeletalNodeFbx.Inverse();

            Bone & RootBone = _bones[RootIdx];
            RootBone.BonePos.Transform = RootBone.BonePos.Transform * GlobalSkeletalNode;
        }

        {
            Bone & RootBone = _bones[RootIdx];
            RootBone.BonePos.Transform = RootBone.BonePos.Transform * _transform;
        }


        return fbx::error_ok;
    }

    fbx::error_e Importer::ImportMaterials()
    {
        std::vector<FbxSurfaceMaterial *> FbxMaterials;
        for (size_t NodeIndex = 0; NodeIndex < _meshs.size(); ++NodeIndex)
        {
            FbxNode * Node = _meshs[NodeIndex];

            int32_t MaterialCount = Node->GetMaterialCount();

            for (int32_t MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
            {
                FbxSurfaceMaterial * FbxMaterial = Node->GetMaterial(MaterialIndex);
                auto iter = std::find(FbxMaterials.begin(), FbxMaterials.end(), FbxMaterial);
                if (iter == FbxMaterials.end())
                {
                    FbxMaterials.push_back(FbxMaterial);
                    Material mat;
                    mat.name = FbxMaterial->GetName();
                    _materials.push_back(mat);
                }
            }
        }
        return fbx::error_ok;
    }
}
