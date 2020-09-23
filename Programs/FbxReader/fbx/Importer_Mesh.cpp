#include "Importer.h"

namespace fbx
{
    fbx::error_e Importer::ImportMeshs()
    {
		FbxNode* SceneRootNode = Scene->GetRootNode();
		if (SceneRootNode)
		{
			FbxAMatrix TransformMatrix = _transform;

			FbxDouble3 ExistingTranslation = SceneRootNode->LclTranslation.Get();
			FbxDouble3 ExistingRotation = SceneRootNode->LclRotation.Get();
			FbxDouble3 ExistingScaling = SceneRootNode->LclScaling.Get();

			// A little slower to build up this information from the matrix, but it means we get
			// the same result across all import types, as other areas can use the matrix directly
			FbxVector4 AddedTranslation = TransformMatrix.GetT();
			FbxVector4 AddedRotation = TransformMatrix.GetR();
			FbxVector4 AddedScaling = TransformMatrix.GetS();

			FbxDouble3 NewTranslation = FbxDouble3(ExistingTranslation[0] + AddedTranslation[0], ExistingTranslation[1] + AddedTranslation[1], ExistingTranslation[2] + AddedTranslation[2]);
			FbxDouble3 NewRotation = FbxDouble3(ExistingRotation[0] + AddedRotation[0], ExistingRotation[1] + AddedRotation[1], ExistingRotation[2] + AddedRotation[2]);
			FbxDouble3 NewScaling = FbxDouble3(ExistingScaling[0] * AddedScaling[0], ExistingScaling[1] * AddedScaling[1], ExistingScaling[2] * AddedScaling[2]);

			SceneRootNode->LclTranslation.Set(NewTranslation);
			SceneRootNode->LclRotation.Set(NewRotation);
			SceneRootNode->LclScaling.Set(NewScaling);
			//Reset all the transform evaluation cache since we change some node transform
			Scene->GetAnimationEvaluator()->Reset();
		}
    	
        for (size_t NodeIndex = 0; NodeIndex < _meshs.size(); ++NodeIndex)
        {
            FbxNode * RootNode = _meshs[0];
            FbxNode * Node = _meshs[NodeIndex];
            FbxMesh * Mesh = Node->GetMesh();
            FbxSkin * Skin = (FbxSkin *)Mesh->GetDeformer(0, FbxDeformer::eSkin);
            FbxShape * FbxShape = nullptr;
            ////if (FbxShapeArray)
            ////{
            ////    FbxShape = (*FbxShapeArray)[NodeIndex];
            ////}

            // NOTE: This function may invalidate FbxMesh and set it to point to a an updated version
            auto err_import = importMesh(Mesh, Skin, FbxShape, RootNode);
            if (err_import)
                return err_import;


            //if (bUseT0AsRefPose && bDiffPose && !_options.bImportScene)
            //{
            //    // deform skin vertex to the frame 0 from bind pose
            //    SkinControlPointsToPose(ImportData, Mesh, FbxShape, true);
            //}
        }

        BoundingBox BBox = CalcBoundingBox(_points.data(), _points.size());
        BoundingBox Temp = BBox;
        Float3 MidMesh = 0.5f * (Temp.Min + Temp.Max);
        BBox.Min = Temp.Min + 1.0f * (Temp.Min - MidMesh);
        BBox.Max = Temp.Max + 1.0f * (Temp.Max - MidMesh);
        // Tuck up the bottom as this rarely extends lower than a reference pose's (e.g. having its feet on the floor).
        // Maya has Y in the vertical, other packages have Z.
        //BEN const int32_t CoordToTuck = bAssumeMayaCoordinates ? 1 : 2;
        //BEN BoundingBox.Min[CoordToTuck]	= Temp.Min[CoordToTuck] + 0.1f*(Temp.Min[CoordToTuck] - MidMesh[CoordToTuck]);
        BBox.Min[2] = Temp.Min[2] + 0.1f * (Temp.Min[2] - MidMesh[2]);
        Float3 BoundingBoxSize = BBox.size();

        _tangentsX.resize(_wedges.size());
        _tangentsY.resize(_wedges.size());
        _tangentsZ.resize(_wedges.size());

        if (_options.bPreserveSmoothingGroups)
        {
            bool bDuplicateUnSmoothWedges = (_options.NormalGenerationMethod != EFBXNormalGenerationMethod::MikkTSpace);
            unsmoothVerts(bDuplicateUnSmoothWedges);
        }
        else
        {
            _pointToRawMap.resize(_points.size());
            for (int32_t PointIdx = 0; PointIdx < _points.size(); PointIdx++)
                _pointToRawMap[PointIdx] = PointIdx;
        }

        auto err_inf = processInfluences();
        if (err_inf)
            return err_inf;
        return fbx::error_ok;
    }

    fbx::error_e Importer::importMeshPoints(std::vector<Float3> & points, FbxGeometryBase * geometry, const FbxAMatrix & amatrix)
    {
        const int32_t ControlPointsCount = geometry->GetControlPointsCount();

        // Construct the matrices for the conversion from right handed to left handed system

        bool bInvalidPositionFound = false;
        for (int32_t ControlPointsIndex = 0; ControlPointsIndex < ControlPointsCount; ControlPointsIndex++)
        {
            FbxVector4 Position = geometry->GetControlPoints()[ControlPointsIndex];

            FbxVector4 FinalPosition = amatrix.MultT(Position);
            Float3 ConvertedPosition = _converter.ConvertPos(FinalPosition);

            assert(ConvertedPosition.ContainsNaN() == false);
            if (ConvertedPosition.ContainsNaN())
            // ensure user when this happens if attached to debugger
            {
                if (!bInvalidPositionFound)
                {
                    bInvalidPositionFound = true;
                }

                ConvertedPosition = Float3_Zero;
            }

            points.emplace_back(ConvertedPosition);
        }

        if (bInvalidPositionFound)
        {
			log_war(__FUNCTION__ " Invalid position (NaN or Inf) found from source position for mesh");
        }
        return fbx::error_ok;
    }

    fbx::error_e Importer::importMesh(FbxMesh * Mesh, FbxSkin * Skin, FbxShape * FbxShape, FbxNode * RootNode)
    {
        FbxNode * Node = Mesh->GetNode();

        //remove the bad polygons before getting any data from mesh
        Mesh->RemoveBadPolygons();

        //Get the base layer of the mesh
        FbxLayer * BaseLayer = Mesh->GetLayer(0);
        if (BaseLayer == NULL)
        {
			log_war(__FUNCTION__ "There is no geometry information in mesh");
            return fbx::error_inner;
        }

        //
        //	store the UVs in arrays for fast access in the later looping of triangles 
        //
        // mapping from UVSets to Fbx LayerElementUV
        // Fbx UVSets may be duplicated, remove the duplicated UVSets in the mapping 
        int32_t LayerCount = Mesh->GetLayerCount();
        std::vector<std::string> UVSets;

        if (LayerCount > 0)
        {
            for (int32_t UVLayerIndex = 0; UVLayerIndex < LayerCount; UVLayerIndex++)
            {
                FbxLayer * lLayer = Mesh->GetLayer(UVLayerIndex);
                int32_t UVSetCount = lLayer->GetUVSetCount();
                if (UVSetCount)
                {
                    FbxArray<FbxLayerElementUV const *> EleUVs = lLayer->GetUVSets();
                    for (int32_t UVIndex = 0; UVIndex < UVSetCount; UVIndex++)
                    {
                        FbxLayerElementUV const * ElementUV = EleUVs[UVIndex];
                        if (ElementUV)
                        {
                            const char * UVSetName = ElementUV->GetName();
                            if (!UVSetName || !UVSetName[0])
                                UVSets.emplace_back(fbx::format("UVmap_", UVLayerIndex));
                            else
                                UVSets.emplace_back(UVSetName);
                        }
                    }
                }
            }
        }

        // If the the UV sets are named using the following format (UVChannel_X; where X ranges from 1 to 4)
        // we will re-order them based on these names.  Any UV sets that do not follow this naming convention
        // will be slotted into available spaces.
        if (UVSets.size() > 0)
        {
            for (int32_t ChannelNumIdx = 0; ChannelNumIdx < 4; ChannelNumIdx++)
            {
                std::string ChannelName = fbx::format("UVChannel_", ChannelNumIdx + 1);
                auto iter_uvset = std::find(UVSets.begin(), UVSets.end(), ChannelName);
                if (iter_uvset == UVSets.end())
                    continue;

                int32_t SetIdx = iter_uvset - UVSets.begin();

                // If the specially formatted UVSet name appears in the list and it is in the wrong spot,
                // we will swap it into the correct spot.
                if (SetIdx != FBX_INDEX_NONE && SetIdx != ChannelNumIdx)
                {
                    // If we are going to swap to a position that is outside the bounds of the
                    // array, then we pad out to that spot with empty data.
                    for (size_t ArrSize = UVSets.size(); ArrSize < ChannelNumIdx + 1; ArrSize++)
                    {
                        UVSets.push_back(std::string());
                    }
                    //Swap the entry into the appropriate spot.
                    std::swap(UVSets[SetIdx], UVSets[ChannelNumIdx]);
                }
            }
        }

        if (LayerCount > 0 && _options.bPreserveSmoothingGroups)
        {
            // Check and see if the smooothing data is valid.  If not generate it from the normals
            BaseLayer = Mesh->GetLayer(0);
            if (BaseLayer)
            {
                const FbxLayerElementSmoothing * SmoothingLayer = BaseLayer->GetSmoothing();

                if (SmoothingLayer)
                {
                    bool bValidSmoothingData = false;
                    FbxLayerElementArrayTemplate<int32_t> & Array = SmoothingLayer->GetDirectArray();
                    for (int32_t SmoothingIndex = 0; SmoothingIndex < Array.GetCount(); ++SmoothingIndex)
                    {
                        if (Array[SmoothingIndex] != 0)
                        {
                            bValidSmoothingData = true;
                            break;
                        }
                    }

                    if (!bValidSmoothingData && Mesh->GetPolygonVertexCount() > 0)
                    {
                        GeometryConverter->ComputeEdgeSmoothingFromNormals(Mesh);
                    }
                }
            }
        }

        // Must do this before triangulating the mesh due to an FBX bug in TriangulateMeshAdvance
        int32_t LayerSmoothingCount = Mesh->GetLayerCount(FbxLayerElement::eSmoothing);
        for (int32_t i = 0; i < LayerSmoothingCount; i++)
        {
            GeometryConverter->ComputePolygonSmoothingFromEdgeSmoothing(Mesh, i);
        }

        //
        // Convert data format to unreal-compatible
        //

        if (!Mesh->IsTriangleMesh())
        {
            ////UE_LOG(LogFbx, Log, TEXT("Triangulating skeletal mesh %s"), UTF8_TO_TCHAR(Node->GetName()));

            const bool bReplace = true;
            FbxNodeAttribute * ConvertedNode = GeometryConverter->Triangulate(Mesh, bReplace);
            if (ConvertedNode != NULL && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                Mesh = ConvertedNode->GetNode()->GetMesh();
            }
            else
            {
                log_war(__FUNCTION__ " Unable to triangulate mesh");
                return fbx::error_bad_data;
            }
        }

        // renew the base layer
        BaseLayer = Mesh->GetLayer(0);
        Skin = (FbxSkin *)static_cast<FbxGeometry *>(Mesh)->GetDeformer(0, FbxDeformer::eSkin);

        //
        //	store the UVs in arrays for fast access in the later looping of triangles 
        //
        size_t UniqueUVCount = UVSets.size();
        FbxLayerElementUV ** LayerElementUV = NULL;
        FbxLayerElement::EReferenceMode * UVReferenceMode = NULL;
        FbxLayerElement::EMappingMode * UVMappingMode = NULL;
        if (UniqueUVCount > 0)
        {
            LayerElementUV = new FbxLayerElementUV * [UniqueUVCount];
            UVReferenceMode = new FbxLayerElement::EReferenceMode[UniqueUVCount];
            UVMappingMode = new FbxLayerElement::EMappingMode[UniqueUVCount];
        }
        else
        {
			log_war(__FUNCTION__ " Mesh has no UV set.Creating a default set");
        }

        LayerCount = Mesh->GetLayerCount();
        for (size_t UVIndex = 0; UVIndex < UniqueUVCount; UVIndex++)
        {
            LayerElementUV[UVIndex] = NULL;
            for (int32_t UVLayerIndex = 0; UVLayerIndex < LayerCount; UVLayerIndex++)
            {
                FbxLayer * lLayer = Mesh->GetLayer(UVLayerIndex);
                int32_t UVSetCount = lLayer->GetUVSetCount();
                if (UVSetCount)
                {
                    FbxArray<FbxLayerElementUV const *> EleUVs = lLayer->GetUVSets();
                    for (int32_t FbxUVIndex = 0; FbxUVIndex < UVSetCount; FbxUVIndex++)
                    {
                        FbxLayerElementUV const * ElementUV = EleUVs[FbxUVIndex];
                        if (ElementUV)
                        {
                            const char * UVSetName = ElementUV->GetName();
                            std::string LocalUVSetName = UVSetName;
                            if (LocalUVSetName.empty())
                            {
                                LocalUVSetName = fbx::format("UVmap_", UVLayerIndex);
                            }
                            if (LocalUVSetName == UVSets[UVIndex])
                            {
                                LayerElementUV[UVIndex] = const_cast<FbxLayerElementUV *>(ElementUV);
                                UVReferenceMode[UVIndex] = LayerElementUV[UVIndex]->GetReferenceMode();
                                UVMappingMode[UVIndex] = LayerElementUV[UVIndex]->GetMappingMode();
                                break;
                            }
                        }
                    }
                }
            }
        }

        //
        // get the smoothing group layer
        //
        bool bSmoothingAvailable = false;

        FbxLayerElementSmoothing const * SmoothingInfo = BaseLayer->GetSmoothing();
        FbxLayerElement::EReferenceMode SmoothingReferenceMode(FbxLayerElement::eDirect);
        FbxLayerElement::EMappingMode SmoothingMappingMode(FbxLayerElement::eByEdge);
        if (SmoothingInfo)
        {
            if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByEdge)
            {
                if (!GeometryConverter->ComputePolygonSmoothingFromEdgeSmoothing(Mesh))
                {
					log_war(__FUNCTION__ " Unable to fully convert the smoothing groups for mesh");
                    bSmoothingAvailable = false;
                }
            }

            if (SmoothingInfo->GetMappingMode() == FbxLayerElement::eByPolygon)
            {
                bSmoothingAvailable = true;
            }


            SmoothingReferenceMode = SmoothingInfo->GetReferenceMode();
            SmoothingMappingMode = SmoothingInfo->GetMappingMode();
        }


        //
        //	get the "material index" layer
        //
        FbxLayerElementMaterial * LayerElementMaterial = BaseLayer->GetMaterials();
        FbxLayerElement::EMappingMode MaterialMappingMode = LayerElementMaterial ?
            LayerElementMaterial->GetMappingMode() : FbxLayerElement::eByPolygon;

        UniqueUVCount = std::min<size_t>(UniqueUVCount, FBX_MAX_TEXCOORDS);

        // One UV set is required but only import up to FBX_MAX_TEXCOORDS number of uv layers
        _nTexCoords = std::max<size_t>(_nTexCoords, UniqueUVCount);

        //
        // get the first vertex color layer
        //

        FbxLayerElementVertexColor * LayerElementVertexColor = BaseLayer->GetVertexColors();
        FbxLayerElement::EReferenceMode VertexColorReferenceMode(FbxLayerElement::eDirect);
        FbxLayerElement::EMappingMode VertexColorMappingMode(FbxLayerElement::eByControlPoint);
        if (LayerElementVertexColor && _options.VertexColorImportOption == EVertexColorImportOption::Replace)
        {
            VertexColorReferenceMode = LayerElementVertexColor->GetReferenceMode();
            VertexColorMappingMode = LayerElementVertexColor->GetMappingMode();
            _hasVertexColors = true;
        }
        else if (_options.VertexColorImportOption == EVertexColorImportOption::Override)
        {
            _hasVertexColors = true;
        }

        //
        // get the first normal layer
        //
        FbxLayerElementNormal * LayerElementNormal = BaseLayer->GetNormals();
        FbxLayerElementTangent * LayerElementTangent = BaseLayer->GetTangents();
        FbxLayerElementBinormal * LayerElementBinormal = BaseLayer->GetBinormals();

        //whether there is normal, tangent and binormal data in this mesh
        bool bHasNormalInformation = LayerElementNormal != NULL;
        bool bHasTangentInformation = LayerElementTangent != NULL && LayerElementBinormal != NULL;

        _hasNormals = bHasNormalInformation;
        _hasTangents = bHasTangentInformation;

        FbxLayerElement::EReferenceMode NormalReferenceMode(FbxLayerElement::eDirect);
        FbxLayerElement::EMappingMode NormalMappingMode(FbxLayerElement::eByControlPoint);
        if (LayerElementNormal)
        {
            NormalReferenceMode = LayerElementNormal->GetReferenceMode();
            NormalMappingMode = LayerElementNormal->GetMappingMode();
        }

        FbxLayerElement::EReferenceMode TangentReferenceMode(FbxLayerElement::eDirect);
        FbxLayerElement::EMappingMode TangentMappingMode(FbxLayerElement::eByControlPoint);
        if (LayerElementTangent)
        {
            TangentReferenceMode = LayerElementTangent->GetReferenceMode();
            TangentMappingMode = LayerElementTangent->GetMappingMode();
        }

        //
        // create the points / wedges / faces
        //
        int32_t ControlPointsCount = Mesh->GetControlPointsCount();
        size_t PointIndexBase = _points.size();
        //FillSkeletalMeshImportPoints(&ImportData, RootNode, Node, Shape);
        FbxAMatrix amatrix = ComputeSkeletalMeshTotalMatrix(Node, RootNode);

        importMeshPoints(_points, Node->GetMesh(), amatrix);

        int32_t MaterialCount = Node->GetMaterialCount();
        std::vector<int32_t> MaterialMapping(MaterialCount);

        for (int32_t MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
        {
            FbxSurfaceMaterial * FbxMaterial = Node->GetMaterial(MaterialIndex);
            std::string name = FbxMaterial->GetName();
            auto iter = std::find_if(_materials.begin(), _materials.end(), [&name](const Material & material) { return material.name == name; });
            if (iter == _materials.end())
                MaterialMapping[MaterialIndex] = 0;
            else
                MaterialMapping[MaterialIndex] = iter - _materials.begin();

            //int32_t ExistingMatIndex = FBX_INDEX_NONE;
            //FbxMaterials.Find(FbxMaterial, ExistingMatIndex);
            //if (ExistingMatIndex != FBX_INDEX_NONE)
            //{
            //    // Reuse existing material
            //    MaterialMapping[MaterialIndex] = ExistingMatIndex;

            //    if (Materials.IsValidIndex(MaterialIndex))
            //    {
            //        Materials[ExistingMatIndex].Material = Materials[MaterialIndex];
            //    }
            //}
            //else
            //{
            //    MaterialMapping[MaterialIndex] = 0;
            //}
        }

        // Construct the matrices for the conversion from right handed to left handed system
        FbxAMatrix TotalMatrix;
        FbxAMatrix TotalMatrixForNormal;
        TotalMatrix = ComputeSkeletalMeshTotalMatrix(Node, RootNode);
        TotalMatrixForNormal = TotalMatrix.Inverse();
        TotalMatrixForNormal = TotalMatrixForNormal.Transpose();

        bool OddNegativeScale = IsOddNegativeScale(TotalMatrix);

        int32_t TriangleCount = Mesh->GetPolygonCount();
        int32_t ExistFaceNum = _faces.size();
        _faces.resize(_faces.size() + TriangleCount);
        int32_t ExistWedgesNum = _wedges.size();
        Wedge TmpWedges[3];


        for (int32_t TriangleIndex = ExistFaceNum, LocalIndex = 0; TriangleIndex < ExistFaceNum + TriangleCount; TriangleIndex++, LocalIndex++)
        {
            Triangle & Triangle = _faces[TriangleIndex];

            //
            // smoothing mask
            //
            // set the face smoothing by default. It could be any number, but not zero
            Triangle.SmoothingGroups = 255;
            if (bSmoothingAvailable)
            {
                if (SmoothingInfo)
                {
                    if (SmoothingMappingMode == FbxLayerElement::eByPolygon)
                    {
                        int32_t lSmoothingIndex = (SmoothingReferenceMode == FbxLayerElement::eDirect) ? LocalIndex : SmoothingInfo->GetIndexArray().GetAt(LocalIndex);
                        Triangle.SmoothingGroups = SmoothingInfo->GetDirectArray().GetAt(lSmoothingIndex);
                    }
                    else
                    {
						log_war(__FUNCTION__ "Unsupported Smoothing group mapping mode on mesh");
                    }
                }
            }

            for (int32_t VertexIndex = 0; VertexIndex < 3; VertexIndex++)
            {
                // If there are odd number negative scale, invert the vertex order for triangles
                int32_t UnrealVertexIndex = OddNegativeScale ? 2 - VertexIndex : VertexIndex;

                int32_t ControlPointIndex = Mesh->GetPolygonVertex(LocalIndex, VertexIndex);
                //
                // normals, tangents and binormals
                //
                if (_options.ShouldImportNormals() && bHasNormalInformation)
                {
                    int32_t TmpIndex = LocalIndex * 3 + VertexIndex;
                    //normals may have different reference and mapping mode than tangents and binormals
                    int32_t NormalMapIndex = (NormalMappingMode == FbxLayerElement::eByControlPoint) ?
                        ControlPointIndex : TmpIndex;
                    int32_t NormalValueIndex = (NormalReferenceMode == FbxLayerElement::eDirect) ?
                        NormalMapIndex : LayerElementNormal->GetIndexArray().GetAt(NormalMapIndex);

                    //tangents and binormals share the same reference, mapping mode and index array
                    int32_t TangentMapIndex = TmpIndex;

                    FbxVector4 TempValue;

                    if (_options.ShouldImportTangents() && bHasTangentInformation)
                    {
                        TempValue = LayerElementTangent->GetDirectArray().GetAt(TangentMapIndex);
                        TempValue = TotalMatrixForNormal.MultT(TempValue);
                        Triangle.TangentX[UnrealVertexIndex] = _converter.ConvertDir(TempValue);
                        Triangle.TangentX[UnrealVertexIndex].normalize();

                        TempValue = LayerElementBinormal->GetDirectArray().GetAt(TangentMapIndex);
                        TempValue = TotalMatrixForNormal.MultT(TempValue);
                        Triangle.TangentY[UnrealVertexIndex] = -_converter.ConvertDir(TempValue);
                        Triangle.TangentY[UnrealVertexIndex].normalize();
                    }

                    TempValue = LayerElementNormal->GetDirectArray().GetAt(NormalValueIndex);
                    TempValue = TotalMatrixForNormal.MultT(TempValue);
                    Triangle.TangentZ[UnrealVertexIndex] = _converter.ConvertDir(TempValue);
                    Triangle.TangentZ[UnrealVertexIndex].normalize();

                }
                else
                {
                    int32_t NormalIndex;
                    for (NormalIndex = 0; NormalIndex < 3; ++NormalIndex)
                    {
                        Triangle.TangentX[NormalIndex] = {};
                        Triangle.TangentY[NormalIndex] = {};
                        Triangle.TangentZ[NormalIndex] = {};
                    }
                }
            }

            //
            // material index
            //
            Triangle.MatIndex = 0; // default value
            if (MaterialCount > 0)
            {
                if (LayerElementMaterial)
                {
                    switch (MaterialMappingMode)
                    {
                        // material index is stored in the IndexArray, not the DirectArray (which is irrelevant with 2009.1)
                    case FbxLayerElement::eAllSame:
                    {
                        Triangle.MatIndex = MaterialMapping[LayerElementMaterial->GetIndexArray().GetAt(0)];
						break;
                    }
                    case FbxLayerElement::eByPolygon:
                    {
                        int32_t Index = LayerElementMaterial->GetIndexArray().GetAt(LocalIndex);
                        if (Index < MaterialMapping.size())
                            Triangle.MatIndex = MaterialMapping[Index];
                        else
							log_war(__FUNCTION__ " Face material index inconsistency");
						break;
                    }
					default:
                    	break;
					}
                }

                // When import morph, we don't check the material index 
                // because we don't import material for morph, so the _matls contains zero material
                ////if (!Shape && (Triangle.MatIndex < 0 || Triangle.MatIndex >= FbxMaterials.size()))
                ////{
                ////    AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, LOCTEXT("FbxSkeletaLMeshimport_MaterialIndexInconsistency", "Face material index inconsistency - forcing to 0")), FFbxErrors::Generic_Mesh_MaterialIndexInconsistency);
                ////    Triangle.MatIndex = 0;
                ////}
            }
            _maxMaterialIndex = std::max<uint32_t>(_maxMaterialIndex, Triangle.MatIndex);

            Triangle.AuxMatIndex = 0;
            for (int32_t VertexIndex = 0; VertexIndex < 3; VertexIndex++)
            {
                // If there are odd number negative scale, invert the vertex order for triangles
                int32_t UnrealVertexIndex = OddNegativeScale ? 2 - VertexIndex : VertexIndex;

                TmpWedges[UnrealVertexIndex].MatIndex = Triangle.MatIndex;
                TmpWedges[UnrealVertexIndex].PointIndex = PointIndexBase + Mesh->GetPolygonVertex(LocalIndex, VertexIndex);
                // Initialize all colors to white.
                TmpWedges[UnrealVertexIndex].Color = { 255, 255, 255, 255 };
            }

            //
            // uvs
            //
            uint32_t UVLayerIndex;
            // Some FBX meshes can have no UV sets, so also check the UniqueUVCount
            for (UVLayerIndex = 0; UVLayerIndex < UniqueUVCount; UVLayerIndex++)
            {
                // ensure the layer has data
                if (LayerElementUV[UVLayerIndex] != NULL)
                {
                    // Get each UV from the layer
                    for (int32_t VertexIndex = 0; VertexIndex < 3; VertexIndex++)
                    {
                        // If there are odd number negative scale, invert the vertex order for triangles
                        int32_t UnrealVertexIndex = OddNegativeScale ? 2 - VertexIndex : VertexIndex;

                        int32_t lControlPointIndex = Mesh->GetPolygonVertex(LocalIndex, VertexIndex);
                        int32_t UVMapIndex = (UVMappingMode[UVLayerIndex] == FbxLayerElement::eByControlPoint) ?
                            lControlPointIndex : LocalIndex * 3 + VertexIndex;
                        int32_t UVIndex = (UVReferenceMode[UVLayerIndex] == FbxLayerElement::eDirect) ?
                            UVMapIndex : LayerElementUV[UVLayerIndex]->GetIndexArray().GetAt(UVMapIndex);
                        FbxVector2	UVVector = LayerElementUV[UVLayerIndex]->GetDirectArray().GetAt(UVIndex);

                        TmpWedges[UnrealVertexIndex].UVs[UVLayerIndex].X = static_cast<float>(UVVector[0]);
                        TmpWedges[UnrealVertexIndex].UVs[UVLayerIndex].Y = 1.f - static_cast<float>(UVVector[1]);
                    }
                }
                else if (UVLayerIndex == 0)
                {
                    // Set all UV's to zero.  If we are here the mesh had no UV sets so we only need to do this for the
                    // first UV set which always exists.

                    for (int32_t VertexIndex = 0; VertexIndex < 3; VertexIndex++)
                    {
                        TmpWedges[VertexIndex].UVs[UVLayerIndex].X = 0.0f;
                        TmpWedges[VertexIndex].UVs[UVLayerIndex].Y = 0.0f;
                    }
                }
            }

            // Read vertex colors if they exist.
            if (LayerElementVertexColor && _options.VertexColorImportOption == EVertexColorImportOption::Replace)
            {
                switch (VertexColorMappingMode)
                {
                case FbxLayerElement::eByControlPoint:
                {
                    for (int32_t VertexIndex = 0; VertexIndex < 3; VertexIndex++)
                    {
                        int32_t UnrealVertexIndex = OddNegativeScale ? 2 - VertexIndex : VertexIndex;

                        FbxColor VertexColor = (VertexColorReferenceMode == FbxLayerElement::eDirect)
                            ? LayerElementVertexColor->GetDirectArray().GetAt(Mesh->GetPolygonVertex(LocalIndex, VertexIndex))
                            : LayerElementVertexColor->GetDirectArray().GetAt(LayerElementVertexColor->GetIndexArray().GetAt(Mesh->GetPolygonVertex(LocalIndex, VertexIndex)));

                        TmpWedges[UnrealVertexIndex].Color = 
                        {
                            uint8_t(255.f * VertexColor.mAlpha),
                            uint8_t(255.f * VertexColor.mRed),
                            uint8_t(255.f * VertexColor.mGreen),
                            uint8_t(255.f * VertexColor.mBlue)
                        };
                    }
                }
                break;
                case FbxLayerElement::eByPolygonVertex:
                {
                    for (int32_t VertexIndex = 0; VertexIndex < 3; VertexIndex++)
                    {
                        int32_t UnrealVertexIndex = OddNegativeScale ? 2 - VertexIndex : VertexIndex;

                        FbxColor VertexColor = (VertexColorReferenceMode == FbxLayerElement::eDirect)
                            ? LayerElementVertexColor->GetDirectArray().GetAt(LocalIndex * 3 + VertexIndex)
                            : LayerElementVertexColor->GetDirectArray().GetAt(LayerElementVertexColor->GetIndexArray().GetAt(LocalIndex * 3 + VertexIndex));

                        TmpWedges[UnrealVertexIndex].Color =
                        {
                            uint8_t(255.f * VertexColor.mAlpha),
                            uint8_t(255.f * VertexColor.mRed),
                            uint8_t(255.f * VertexColor.mGreen),
                            uint8_t(255.f * VertexColor.mBlue)
                        };
                    }
                }
                break;
                }
            }
            else if (_options.VertexColorImportOption == EVertexColorImportOption::Override)
            {
                for (int32_t VertexIndex = 0; VertexIndex < 3; VertexIndex++)
                {
                    int32_t UnrealVertexIndex = OddNegativeScale ? 2 - VertexIndex : VertexIndex;
                    TmpWedges[UnrealVertexIndex].Color = _options.VertexOverrideColor;
                }
            }

            //
            // basic wedges matching : 3 unique per face. TODO Can we do better ?
            //
            for (int32_t icorner = 0; icorner < 3; icorner++)
            {
                Wedge wedge;
                wedge.PointIndex = TmpWedges[icorner].PointIndex;
                wedge.MatIndex = TmpWedges[icorner].MatIndex;
                wedge.Color = TmpWedges[icorner].Color;
                wedge.Reserved = 0;
                std::memcpy(wedge.UVs, TmpWedges[icorner].UVs, sizeof(Float2) * FBX_MAX_TEXCOORDS);

                Triangle.WedgeIndex[icorner] = _wedges.size();
                _wedges.emplace_back(wedge);
            }

        }

        // now we can work on a per-cluster basis with good ordering
        if (Skin) // skeletal mesh
        {
            // create influences for each cluster
            int32_t ClusterIndex;
            for (ClusterIndex = 0; ClusterIndex < Skin->GetClusterCount(); ClusterIndex++)
            {
                FbxCluster * Cluster = Skin->GetCluster(ClusterIndex);
                // When Maya plug-in exports rigid binding, it will generate "CompensationCluster" for each ancestor _links.
                // FBX writes these "CompensationCluster" out. The CompensationCluster also has weight 1 for vertices.
                // Unreal importer should skip these clusters.
                if (!Cluster || (std::strcmp(Cluster->GetUserDataID(), "Maya_ClusterHint") == 0 && std::strcmp(Cluster->GetUserData(), "CompensationCluster") == 0))
                {
                    continue;
                }

                FbxNode * Link = Cluster->GetLink();
                // find the bone index
                int32_t BoneIndex = -1;
                for (size_t LinkIndex = 0; LinkIndex < _links.size(); LinkIndex++)
                {
                    if (Link == _links[LinkIndex])
                    {
                        BoneIndex = LinkIndex;
                        break;
                    }
                }

                //	get the vertex indices
                int32_t ControlPointIndicesCount = Cluster->GetControlPointIndicesCount();
                int32_t * ControlPointIndices = Cluster->GetControlPointIndices();
                double * Weights = Cluster->GetControlPointWeights();

                //	for each vertex index in the cluster
                for (int32_t ControlPointIndex = 0; ControlPointIndex < ControlPointIndicesCount; ++ControlPointIndex)
                {
                    BoneInfluence influence;
                    influence.BoneIndex = BoneIndex;
                    influence.Weight = static_cast<float>(Weights[ControlPointIndex]);
                    influence.PointIndex = PointIndexBase + ControlPointIndices[ControlPointIndex];
                    _influences.emplace_back(influence);
                }
            }
        }
        else // for rigid mesh
        {
            // find the bone index
            for (size_t LinkIndex = 0; LinkIndex < _links.size(); LinkIndex++)
            {
                // the bone is the node itself
                if (Node == _links[LinkIndex])
                {
                    //	for each vertex in the mesh
                    for (int32_t ControlPointIndex = 0; ControlPointIndex < ControlPointsCount; ++ControlPointIndex)
                    {
                        BoneInfluence influence;
                        influence.BoneIndex = LinkIndex;
                        influence.Weight = 1.0;
                        influence.PointIndex = PointIndexBase + ControlPointIndex;
                        _influences.emplace_back(influence);
                    }
                    break;
                }
            }
        }

        //
        // clean up
        //
        if (UniqueUVCount > 0)
        {
            delete[] LayerElementUV;
            delete[] UVReferenceMode;
            delete[] UVMappingMode;
        }

        if (!_options.bImportScene && _bOutDiffPose && _useTime0AsRefPose)
        {
            // deform skin vertex to the frame 0 from bind pose
            skinControlPointsToPose(_points, PointIndexBase, Mesh, FbxShape, true);
        }

        return fbx::error_ok;
    }

    fbx::error_e Importer::skinControlPointsToPose(std::vector<Float3> & points, size_t pointIndexBase, FbxMesh * Mesh, FbxShape * FbxShape, bool bUseT0)
    {
        auto GetGeometry = [](FbxNode * pNode)->FbxAMatrix
        {
            FbxVector4 lT, lR, lS;
            FbxAMatrix lGeometry;

            lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
            lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
            lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

            lGeometry.SetT(lT);
            lGeometry.SetR(lR);
            lGeometry.SetS(lS);

            return lGeometry;
        };
        // Scale all the elements of a matrix.
        auto MatrixScale = [](FbxAMatrix & pMatrix, double pValue)
        {
            int32_t i, j;

            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    pMatrix[i][j] *= pValue;
                }
            }
        };

        // Add a value to all the elements in the diagonal of the matrix.
        auto MatrixAddToDiagonal = [](FbxAMatrix & pMatrix, double pValue)
        {
            pMatrix[0][0] += pValue;
            pMatrix[1][1] += pValue;
            pMatrix[2][2] += pValue;
            pMatrix[3][3] += pValue;
        };

        // Sum two matrices element by element.
        auto MatrixAdd = [](FbxAMatrix & pDstMatrix, FbxAMatrix & pSrcMatrix)
        {
            int32_t i, j;

            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    pDstMatrix[i][j] += pSrcMatrix[i][j];
                }
            }
        };

        FbxTime poseTime = FBXSDK_TIME_INFINITE;
        if (bUseT0)
        {
            poseTime = 0;
        }

        int32_t VertexCount = Mesh->GetControlPointsCount();

        // Create a copy of the vertex array to receive vertex deformations.
        std::vector<FbxVector4> VertexArray(VertexCount);

        // If a shape is provided, then it is the morphed version of the mesh.
        // So we want to deform that instead of the base mesh vertices
        if (FbxShape)
        {
            assert(FbxShape->GetControlPointsCount() == VertexCount);
            std::memcpy(VertexArray.data(), FbxShape->GetControlPoints(), VertexCount * sizeof(FbxVector4));
        }
        else
        {
            std::memcpy(VertexArray.data(), Mesh->GetControlPoints(), VertexCount * sizeof(FbxVector4));
        }


        int32_t ClusterCount = 0;
        int32_t SkinCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);
        for (int32_t i = 0; i < SkinCount; i++)
        {
            ClusterCount += ((FbxSkin *)(Mesh->GetDeformer(i, FbxDeformer::eSkin)))->GetClusterCount();
        }

        if (!ClusterCount)
            return fbx::error_ok;

    // Deform the vertex array with the links contained in the mesh.
        FbxAMatrix MeshMatrix = ComputeTotalMatrix(Mesh->GetNode());
        // All the links must have the same link mode.
        FbxCluster::ELinkMode lClusterMode = ((FbxSkin *)Mesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

        int32_t i, j;
        int32_t lClusterCount = 0;

        int32_t lSkinCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);

        std::vector<FbxAMatrix> ClusterDeformations(VertexCount);
        std::vector<double> ClusterWeights(VertexCount, 0);

        if (lClusterMode == FbxCluster::eAdditive)
        {
            for (i = 0; i < VertexCount; i++)
            {
                ClusterDeformations[i].SetIdentity();
            }
        }
        else
        {
            std::memset(ClusterDeformations.data(), 0, sizeof(FbxAMatrix) * ClusterDeformations.size());
        }

        for (i = 0; i < lSkinCount; ++i)
        {
            lClusterCount = ((FbxSkin *)Mesh->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
            for (j = 0; j < lClusterCount; ++j)
            {
                FbxCluster * Cluster = ((FbxSkin *)Mesh->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);
                if (!Cluster->GetLink())
                    continue;

                FbxNode * Link = Cluster->GetLink();

                FbxAMatrix lReferenceGlobalInitPosition;
                FbxAMatrix lReferenceGlobalCurrentPosition;
                FbxAMatrix lClusterGlobalInitPosition;
                FbxAMatrix lClusterGlobalCurrentPosition;
                FbxAMatrix lReferenceGeometry;
                FbxAMatrix lClusterGeometry;

                FbxAMatrix lClusterRelativeInitPosition;
                FbxAMatrix lClusterRelativeCurrentPositionInverse;
                FbxAMatrix lVertexTransformMatrix;

                if (lClusterMode == FbxCluster::eAdditive && Cluster->GetAssociateModel())
                {
                    Cluster->GetTransformAssociateModelMatrix(lReferenceGlobalInitPosition);
                    lReferenceGlobalCurrentPosition = Scene->GetAnimationEvaluator()->GetNodeGlobalTransform(Cluster->GetAssociateModel(), poseTime);
                    // Geometric transform of the model
                    lReferenceGeometry = GetGeometry(Cluster->GetAssociateModel());
                    lReferenceGlobalCurrentPosition *= lReferenceGeometry;
                }
                else
                {
                    Cluster->GetTransformMatrix(lReferenceGlobalInitPosition);
                    lReferenceGlobalCurrentPosition = MeshMatrix; //pGlobalPosition;
                    // Multiply lReferenceGlobalInitPosition by Geometric Transformation
                    lReferenceGeometry = GetGeometry(Mesh->GetNode());
                    lReferenceGlobalInitPosition *= lReferenceGeometry;
                }
                // Get the link initial global position and the link current global position.
                Cluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
                lClusterGlobalCurrentPosition = Link->GetScene()->GetAnimationEvaluator()->GetNodeGlobalTransform(Link, poseTime);

                // Compute the initial position of the link relative to the reference.
                lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

                // Compute the current position of the link relative to the reference.
                lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

                // Compute the shift of the link relative to the reference.
                lVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;

                int32_t k;
                int32_t lVertexIndexCount = Cluster->GetControlPointIndicesCount();

                for (k = 0; k < lVertexIndexCount; ++k)
                {
                    int32_t lIndex = Cluster->GetControlPointIndices()[k];

                    // Sometimes, the mesh can have less points than at the time of the skinning
                    // because a smooth operator was active when skinning but has been deactivated during export.
                    if (lIndex >= VertexCount)
                    {
                        continue;
                    }

                    double lWeight = Cluster->GetControlPointWeights()[k];

                    if (lWeight == 0.0)
                    {
                        continue;
                    }

                    // Compute the influence of the link on the vertex.
                    FbxAMatrix lInfluence = lVertexTransformMatrix;
                    MatrixScale(lInfluence, lWeight);

                    if (lClusterMode == FbxCluster::eAdditive)
                    {
                        // Multiply with to the product of the deformations on the vertex.
                        MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
                        ClusterDeformations[lIndex] = lInfluence * ClusterDeformations[lIndex];

                        // Set the link to 1.0 just to know this vertex is influenced by a link.
                        ClusterWeights[lIndex] = 1.0;
                    }
                    else // lLinkMode == KFbxLink::eNORMALIZE || lLinkMode == KFbxLink::eTOTAL1
                    {
                        // Add to the sum of the deformations on the vertex.
                        MatrixAdd(ClusterDeformations[lIndex], lInfluence);

                        // Add to the sum of weights to either normalize or complete the vertex.
                        ClusterWeights[lIndex] += lWeight;
                    }

                }
            }
        }

        for (i = 0; i < VertexCount; i++)
        {
            FbxVector4 lSrcVertex = VertexArray[i];
            double lWeight = ClusterWeights[i];

            // Deform the vertex if there was at least a link with an influence on the vertex,
            if (lWeight != 0.0)
            {
                VertexArray[i] = ClusterDeformations[i].MultT(lSrcVertex);

                if (lClusterMode == FbxCluster::eNormalize)
                {
                    // In the normalized link mode, a vertex is always totally influenced by the links. 
                    VertexArray[i] /= lWeight;
                }
                else if (lClusterMode == FbxCluster::eTotalOne)
                {
                    // In the total 1 link mode, a vertex can be partially influenced by the links. 
                    lSrcVertex *= (1.0 - lWeight);
                    VertexArray[i] += lSrcVertex;
                }
            }
        }

        // change the vertex position
        for (size_t ControlPointsIndex = 0; ControlPointsIndex < VertexCount; ControlPointsIndex++)
        {
            points[ControlPointsIndex + pointIndexBase] = _converter.ConvertPos(MeshMatrix.MultT(VertexArray[ControlPointsIndex]));
        }

        return fbx::error_ok;
    }

    fbx::error_e Importer::unsmoothVerts(bool bDuplicateUnSmoothWedges)
    {
        // Check whether faces have at least two vertices in common. These must be POINTS - don't care about wedges.
        auto FacesAreSmoothlyConnected = [this](int32_t Face1, int32_t Face2)
        {
            //if( ( Face1 >= Thing->SkinData.Faces.size()) || ( Face2 >= Thing->SkinData.Faces.size()) ) return false;

            if (Face1 == Face2)
            {
                return true;
            }

            // Smoothing groups match at least one bit in binary AND ?
            if ((_faces[Face1].SmoothingGroups & _faces[Face2].SmoothingGroups) == 0)
            {
                return false;
            }

            int32_t VertMatches = 0;
            for (int32_t i = 0; i < 3; i++)
            {
                int32_t Point1 = _wedges[_faces[Face1].WedgeIndex[i]].PointIndex;

                for (int32_t j = 0; j < 3; j++)
                {
                    int32_t Point2 = _wedges[_faces[Face2].WedgeIndex[j]].PointIndex;
                    if (Point2 == Point1)
                    {
                        VertMatches++;
                    }
                }
            }

            return (VertMatches >= 2);
        };

        //
        // Connectivity: triangles with non-matching smoothing groups will be physically split.
        //
        // -> Splitting involves: the UV+material-contaning vertex AND the 3d point.
        //
        // -> Tally smoothing groups for each and every (textured) vertex.
        //
        // -> Collapse: 
        // -> start from a vertex and all its adjacent triangles - go over
        // each triangle - if any connecting one (sharing more than one vertex) gives a smoothing match,
        // accumulate it. Then IF more than one resulting section, 
        // ensure each boundary 'vert' is split _if not already_ to give each smoothing group
        // independence from all others.
        //

        struct PointFace
        {
            int32_t FaceIndex = 0;
            int32_t HoekIndex = 0;
            int32_t WedgeIndex = 0;
            uint32_t SmoothFlags = 0;
            uint32_t FanFlags = 0;
            bool processed = false;
        };

        int32_t DuplicatedVertCount = 0;
        int32_t RemappedHoeks = 0;

        int32_t TotalSmoothMatches = 0;
        int32_t TotalConnexChex = 0;

        // Link _all_ faces to vertices.	
        std::vector<std::vector<PointFace>> PointFaces(_points.size());
        std::vector<std::vector<int32_t>> PointInfluences(_points.size());
        std::vector<std::vector<int32_t>> PointWedges(_points.size());

        // Existing points map 1:1
        _pointToRawMap.resize(_points.size());
        for (int32_t ipoint = 0; ipoint < _points.size(); ipoint++)
            _pointToRawMap[ipoint] = ipoint;

        for (int32_t iinfluence = 0; iinfluence < _influences.size(); iinfluence++)
        {
            //if (PointInfluences.size() <= _influences[iinfluence].PointIndex)
            //{
            //    int32_t count = _influences[iinfluence].PointIndex - PointInfluences.size() + 1;
            //    PointInfluences.resize(PointInfluences.size() + count);
            //}
            PointInfluences[_influences[iinfluence].PointIndex].push_back(iinfluence);
        }

        for (int32_t iwedge = 0; iwedge < _wedges.size(); iwedge++)
        {
            //if (PointWedges.size() <= _wedges[iwedge].PointIndex)
            //{
            //    int32_t count = _wedges[iwedge].PointIndex - PointWedges.size() + 1;
            //    PointWedges.resize(PointWedges.size() + count);
            //}
            PointWedges[_wedges[iwedge].PointIndex].push_back(iwedge);
        }

        for (int32_t iface = 0; iface < _faces.size(); iface++)
        {
            // For each face, add a pointer to that face into the Fans[vertex].
            for (int32_t icorner = 0; icorner < 3; icorner++)
            {
                int32_t WedgeIndex = _faces[iface].WedgeIndex[icorner];
                int32_t PointIndex = _wedges[WedgeIndex].PointIndex;
                PointFace NewFR;

                NewFR.FaceIndex = iface;
                NewFR.HoekIndex = icorner;
                NewFR.WedgeIndex = WedgeIndex; // This face touches the point courtesy of Wedges[Wedgeindex].
                NewFR.SmoothFlags = _faces[iface].SmoothingGroups;
                NewFR.FanFlags = 0;
                NewFR.processed = false;
                PointFaces[PointIndex].emplace_back(NewFR);
            }
        }

        // Investigate connectivity and assign common group numbers (1..+) to the fans' individual FanFlags.
        for (int32_t ipoint = 0; ipoint < PointFaces.size(); ipoint++) // The fan of faces for each 3d point 'p'.
        {
            // All faces connecting.
            // 点被多个面共享，需要检测是否需要分离
            if (PointFaces[ipoint].size() > 0)
            {
                int32_t faceProcessed = 0;
                std::vector<std::vector<int32_t>> PointFaceSets; // Sets with indices INTO FANS, not into face array.			

                // Digest all faces connected to this vertex (p) into one or more smooth sets. only need to check 
                // all faces MINUS on
                //for (int32_t FacesProcessed; FacesProcessed < Fans[p].FaceRecord.size();)
                while (faceProcessed < PointFaces[ipoint].size())
                {
                    // One loop per group. For the current ThisFaceIndex, tally all truly connected ones
                    // and put them in a new std::vector. Once no more can be connected, stop.

                    PointFaceSets.push_back({});						// first one will be just ThisFaceIndex.
                    std::vector<int32_t> & PointFaceSet = PointFaceSets.back();
                    // Find the first non-processed face. There will be at least one.
                    int32_t pointfaceIndexCurr = 0;
                    {
                        int32_t pointfaceIndex = 0;
                        //while (Fans[p].FaceRecord[SearchIndex].FanFlags == -1) // -1 indicates already  processed. 
                        while (PointFaces[ipoint][pointfaceIndex].processed) // -1 indicates already  processed. 
                        {
                            pointfaceIndex++;
                        }
                        pointfaceIndexCurr = pointfaceIndex; //Fans[p].FaceRecord[SearchIndex].FaceIndex; 
                    }

                    // Initial face.
                    PointFaceSet.push_back(pointfaceIndexCurr);   // Add the unprocessed Face index to the "local smoothing group" [NewSetIndex].
                    //Fans[p].FaceRecord[ThisFaceFanIndex].FanFlags = -1;			  // Mark as processed.
                    PointFaces[ipoint][pointfaceIndexCurr].processed = true;			  // Mark as processed.
                    faceProcessed++;

                    // Find all faces connected to this face, and if there's any
                    // smoothing group matches, put it in current face set and mark it as processed;
                    // until no more match. 
                    int32_t NewMatches = 0;
                    do
                    {
                        NewMatches = 0;
                        // Go over all current faces in this faceset and set if the FaceRecord (local smoothing groups) has any matches.
                        // there will be at least one face already in this faceset - the first face in the fan.
                        for (int32_t ipointface = 0; ipointface < PointFaceSet.size(); ipointface++)
                        {
                            int32_t HookFaceIdx = PointFaces[ipoint][PointFaceSet[ipointface]].FaceIndex;

                            //Go over the fan looking for matches.
                            for (int32_t irecord = 0; irecord < PointFaces[ipoint].size(); irecord++)
                            {
                                // Skip if same face, skip if face already processed.
                                //if ((HookFaceIdx != Fans[p].FaceRecord[s].FaceIndex) && (Fans[p].FaceRecord[s].FanFlags != -1))
                                if ((HookFaceIdx != PointFaces[ipoint][irecord].FaceIndex) && (PointFaces[ipoint][irecord].processed != true))
                                {
                                    TotalConnexChex++;
                                    // Process if connected with more than one vertex, AND smooth..
                                    if (FacesAreSmoothlyConnected(HookFaceIdx, PointFaces[ipoint][irecord].FaceIndex))
                                    {
                                        TotalSmoothMatches++;
                                        //Fans[p].FaceRecord[s].FanFlags = -1; // Mark as processed.
                                        PointFaces[ipoint][irecord].processed = true; // Mark as processed.
                                        faceProcessed++;
                                        // Add 
                                        PointFaceSet.push_back(irecord); // Store FAN index of this face index into smoothing group's faces. 
                                        // Tally
                                        NewMatches++;
                                    }
                                } // not the same...
                            }// all faces in fan
                        } // all faces in FaceSet
                    }
                    while (NewMatches);

                }// Repeat until all faces processed.

                // For the new non-initialized  face sets, 
                // Create a new point, influences, and uv-vertex(-ices) for all individual FanFlag groups with an index of 2+ and also remap
                // the face's vertex into those new ones.
                if (PointFaceSets.size() > 1)
                {
                    for (int32_t ipointfaceset = 1; ipointfaceset < PointFaceSets.size(); ipointfaceset++)
                    {
                        assert(_points.size() == _pointToRawMap.size());

                        // We duplicate the current vertex. (3d point)
                        _points.push_back(_points[ipoint]);
                        _pointToRawMap.push_back(ipoint);
                        int32_t pointIndexNew = _points.size() - 1;

                        DuplicatedVertCount++;

                        // Duplicate all related weights.
                        for (int32_t t = 0; t < PointInfluences[ipoint].size(); t++)
                        {
                            BoneInfluence influence = _influences[PointInfluences[ipoint][t]];
                            influence.PointIndex = pointIndexNew;
                            _influences.push_back(influence);
                        }

                        // Duplicate any and all Wedges associated with it; and all Faces' wedges involved.					
                        for (int32_t iwedge = 0; iwedge < PointWedges[ipoint].size(); iwedge++)
                        {
                            int32_t wedgeIndexOld = PointWedges[ipoint][iwedge];
                            int32_t wedgeIndexNew = _wedges.size();

                            if (bDuplicateUnSmoothWedges)
                            {
                                Wedge wedge = _wedges[wedgeIndexOld];
                                wedge.PointIndex = pointIndexNew;
                                _wedges.push_back(wedge);

                                //  Update relevant face's Wedges. Inelegant: just check all associated faces for every new wedge.
                                for (int32_t ipointface = 0; ipointface < PointFaceSets[ipointfaceset].size(); ipointface++)
                                {
                                    int32_t pointfaceIndex = PointFaceSets[ipointfaceset][ipointface];
                                    if (PointFaces[ipoint][pointfaceIndex].WedgeIndex == wedgeIndexOld)
                                    {
                                        // Update just the right one for this face (HoekIndex!) 
                                        _faces[PointFaces[ipoint][pointfaceIndex].FaceIndex].WedgeIndex[PointFaces[ipoint][pointfaceIndex].HoekIndex] = wedgeIndexNew;
                                        RemappedHoeks++;
                                    }
                                }
                            }
                            else
                            {
                                _wedges[wedgeIndexOld].PointIndex = pointIndexNew;
                            }
                        }
                    }
                } //  if FaceSets.size(). -> duplicate stuff
            }//	while( FacesProcessed < Fans[p].size() )
        } // Fans for each 3d point

        assert(_points.size() == _pointToRawMap.size());

        return fbx::error_ok;
    }

    fbx::error_e Importer::processInfluences()
    {
        std::vector <Float3> & Points = _points;
        std::vector <Wedge> & Wedges = _wedges;
        std::vector <BoneInfluence> & Influences = _influences;

        // Sort influences by vertex index.
        struct FCompareVertexIndex
        {
            bool operator()(const BoneInfluence & A, const BoneInfluence & B) const
            {
                if (A.PointIndex > B.PointIndex) return false;
                else if (A.PointIndex < B.PointIndex) return true;
                else if (A.Weight < B.Weight) return false;
                else if (A.Weight > B.Weight) return true;
                else if (A.BoneIndex > B.BoneIndex) return false;
                else if (A.BoneIndex < B.BoneIndex) return true;
                else									  return  false;
            }
        };
        std::sort(Influences.begin(), Influences.end(), FCompareVertexIndex());

        std::vector <BoneInfluence> NewInfluences;
        int32_t LastNewInfluenceIndex = 0;
        int32_t LastPointIndex = FBX_INDEX_NONE;
        int32_t InfluenceCount = 0;

        float TotalWeight = 0.f;
        const float MINWEIGHT = 0.01f;

        int MaxVertexInfluence = 0;
        float MaxIgnoredWeight = 0.0f;
		float InfluenceOneEplison = 0.0001f;

        //We have to normalize the data before filtering influences
        //Because influence filtering is base on the normalize value.
        //Some DCC like Daz studio don't have normalized weight
        for (int32_t iInfluence = 0; iInfluence < Influences.size(); iInfluence++)
        {
            // if less than min weight, or it's more than 8, then we clear it to use weight
            InfluenceCount++;
            TotalWeight += Influences[iInfluence].Weight;
            // we have all influence for the same vertex, normalize it now
            if (iInfluence + 1 >= Influences.size() || Influences[iInfluence].PointIndex != Influences[iInfluence + 1].PointIndex)
            {
                // Normalize the last set of influences.
                if (InfluenceCount && std::abs(TotalWeight - 1.0f) > InfluenceOneEplison)
                {
                    float OneOverTotalWeight = 1.f / TotalWeight;
                    for (int r = 0; r < InfluenceCount; r++)
                    {
                        Influences[iInfluence - r].Weight *= OneOverTotalWeight;
                    }
                }

				MaxVertexInfluence = std::max(MaxVertexInfluence, InfluenceCount);
                InfluenceCount = 0;
                TotalWeight = 0.0f;
            }

            if (InfluenceCount > FBX_MAX_TOTAL_INFLUENCES && Influences[iInfluence].Weight > MaxIgnoredWeight)
            {
                MaxIgnoredWeight = Influences[iInfluence].Weight;
            }
        }

        // warn about too many influences
        if (MaxVertexInfluence > FBX_MAX_TOTAL_INFLUENCES)
        {
			log_war(__FUNCTION__ " Warning skeletal mesh influence count of {0} exceeds max count of {1}. Influence truncation will occur");
        }

        for (int32_t iInfluence = 0; iInfluence < Influences.size(); iInfluence++)
        {
            // we found next verts, normalize it now
            if (LastPointIndex != Influences[iInfluence].PointIndex)
            {
                // Normalize the last set of influences.
                if (InfluenceCount && std::abs(TotalWeight - 1.0f) > InfluenceOneEplison)
                {
                    float OneOverTotalWeight = 1.f / TotalWeight;
                    for (int r = 0; r < InfluenceCount; r++)
                    {
                        NewInfluences[LastNewInfluenceIndex - r].Weight *= OneOverTotalWeight;
                    }
                }

                // now we insert missing verts
                if (LastPointIndex != FBX_INDEX_NONE)
                {
                    int32_t CurrentPointIndex = Influences[iInfluence].PointIndex;
                    for (int32_t iPoint = LastPointIndex + 1; iPoint < CurrentPointIndex; iPoint++)
                    {
                        // Add a 0-bone weight if none other present (known to happen with certain MAX skeletal setups).
                        NewInfluences.push_back({});
                        LastNewInfluenceIndex = NewInfluences.size() - 1;
                        NewInfluences[LastNewInfluenceIndex].PointIndex = iPoint;
                        NewInfluences[LastNewInfluenceIndex].BoneIndex = 0;
                        NewInfluences[LastNewInfluenceIndex].Weight = 1.f;
                    }
                }

                // clear to count next one
                InfluenceCount = 0;
                TotalWeight = 0.f;
                LastPointIndex = Influences[iInfluence].PointIndex;
            }

            // if less than min weight, or it's more than 8, then we clear it to use weight
            if (Influences[iInfluence].Weight > MINWEIGHT && InfluenceCount < FBX_MAX_TOTAL_INFLUENCES)
            {
                NewInfluences.push_back(Influences[iInfluence]);
                LastNewInfluenceIndex = NewInfluences.size() - 1;
                InfluenceCount++;
                TotalWeight += Influences[iInfluence].Weight;
            }
        }

        Influences = NewInfluences;

        // Ensure that each vertex has at least one influence as e.g. CreateSkinningStream relies on it.
        // The below code relies on influences being sorted by vertex index.
        // 没有就没有
        if (Influences.size() == 0)
        {
            // add one for each wedge entry
            Influences.resize(Wedges.size());
            for (int32_t WedgeIdx = 0; WedgeIdx < Wedges.size(); WedgeIdx++)
            {
                Influences[WedgeIdx].PointIndex = WedgeIdx;
                Influences[WedgeIdx].BoneIndex = 0;
                Influences[WedgeIdx].Weight = 1.0f;
            }
            for (int32_t i = 0; i < Influences.size(); i++)
            {
                int32_t CurrentVertexIndex = Influences[i].PointIndex;

                if (LastPointIndex != CurrentVertexIndex)
                {
                    for (int32_t j = LastPointIndex + 1; j < CurrentVertexIndex; j++)
                    {
                        // Add a 0-bone weight if none other present (known to happen with certain MAX skeletal setups).
                        Influences.insert(Influences.begin() + i, {});
                        //Influences.insert(Influences.begin() + )
                        Influences[i].PointIndex = j;
                        Influences[i].BoneIndex = 0;
                        Influences[i].Weight = 1.f;
                    }
                    LastPointIndex = CurrentVertexIndex;
                }
            }
        }
        return fbx::error_ok;
    }
}
