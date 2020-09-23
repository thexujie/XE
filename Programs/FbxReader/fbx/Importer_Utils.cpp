#include "Importer.h"

namespace fbx
{
    bool Importer::IsUnrealBone(FbxNode * link)
    {
        FbxNodeAttribute * Attr = link->GetNodeAttribute();
        if (Attr)
        {
            FbxNodeAttribute::EType AttrType = Attr->GetAttributeType();
            if (AttrType == FbxNodeAttribute::eSkeleton ||
                AttrType == FbxNodeAttribute::eMesh ||
                AttrType == FbxNodeAttribute::eNull)
            {
                return true;
            }
        }

        return false;
    }

    FbxAMatrix Importer::ComputeTotalMatrix(FbxScene * scene, FbxNode * Node, bool transformToAbs, bool bakePivotInVertex)
    {
        FbxAMatrix Geometry;
        FbxVector4 Translation, Rotation, Scaling;
        Translation = Node->GetGeometricTranslation(FbxNode::eSourcePivot);
        Rotation = Node->GetGeometricRotation(FbxNode::eSourcePivot);
        Scaling = Node->GetGeometricScaling(FbxNode::eSourcePivot);
        Geometry.SetT(Translation);
        Geometry.SetR(Rotation);
        Geometry.SetS(Scaling);

        //For Single Matrix situation, obtain transfrom matrix from eDESTINATION_SET, which include pivot offsets and pre/post rotations.
        FbxAMatrix & GlobalTransform = scene->GetAnimationEvaluator()->GetNodeGlobalTransform(Node);

        //We can bake the pivot only if we don't transform the vertex to the absolute position
        if (!transformToAbs)
        {
            if (bakePivotInVertex)
            {
                FbxAMatrix PivotGeometry;
                FbxVector4 RotationPivot = Node->GetRotationPivot(FbxNode::eSourcePivot);
                FbxVector4 FullPivot;
                FullPivot[0] = -RotationPivot[0];
                FullPivot[1] = -RotationPivot[1];
                FullPivot[2] = -RotationPivot[2];
                PivotGeometry.SetT(FullPivot);
                Geometry = Geometry * PivotGeometry;
            }
            else
            {
                //No Wedge transform and no bake pivot, it will be the mesh as-is.
                Geometry.SetIdentity();
            }
        }
        //We must always add the geometric transform. Only Max use the geometric transform which is an offset to the local transform of the node
        FbxAMatrix TotalMatrix = transformToAbs ? GlobalTransform * Geometry : Geometry;

        return TotalMatrix;
    }

    FbxNode * Importer::GetRootSkeleton(FbxScene * scene, FbxNode * link, EFbxCreator creator)
    {
        FbxNode * RootBone = link;

        // get Unreal skeleton root
        // mesh and dummy are used as bone if they are in the skeleton hierarchy
        while (RootBone && RootBone->GetParent())
        {
            bool bIsBlenderArmatureBone = false;
            if (creator == EFbxCreator::Blender)
            {
                //Hack to support armature dummy node from blender
                //Users do not want the null attribute node named armature which is the parent of the real root bone in blender fbx file
                //This is a hack since if a rigid mesh group root node is named "armature" it will be skip
                std::string RootBoneParentName(RootBone->GetParent()->GetName());
                FbxNode * GrandFather = RootBone->GetParent()->GetParent();
                bIsBlenderArmatureBone = (GrandFather == nullptr || GrandFather == scene->GetRootNode()) && (_strcmpi("armature", RootBoneParentName.c_str()) == 0);
            }

            FbxNodeAttribute * Attr = RootBone->GetParent()->GetNodeAttribute();
            if (Attr &&
                (Attr->GetAttributeType() == FbxNodeAttribute::eMesh ||
                (Attr->GetAttributeType() == FbxNodeAttribute::eNull && !bIsBlenderArmatureBone) ||
                Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton) &&
                RootBone->GetParent() != scene->GetRootNode())
            {
                // in some case, skeletal mesh can be ancestor of bones
                // this avoids this situation
                if (Attr->GetAttributeType() == FbxNodeAttribute::eMesh)
                {
                    FbxMesh * Mesh = (FbxMesh *)Attr;
                    if (Mesh->GetDeformerCount(FbxDeformer::eSkin) > 0)
                    {
                        break;
                    }
                }

                RootBone = RootBone->GetParent();
            }
            else
            {
                break;
            }
        }

        return RootBone;
    }

    std::vector<FbxPose *> Importer::RetrievePoseFromBindPose(FbxScene * scene, std::vector<FbxNode *> nodes)
    {
        std::vector<FbxPose *> poses;
        const int32_t PoseCount = scene->GetPoseCount();
        for (int32_t PoseIndex = 0; PoseIndex < PoseCount; PoseIndex++)
        {
            FbxPose * CurrentPose = scene->GetPose(PoseIndex);

            // current pose is bind pose, 
            if (CurrentPose && CurrentPose->IsBindPose())
            {
                // IsValidBindPose doesn't work reliably
                // It checks all the parent chain(regardless root given), and if the parent doesn't have correct bind pose, it fails
                // It causes more false positive issues than the real issue we have to worry about
                // If you'd like to try this, set CHECK_VALID_BIND_POSE to 1, and try the error message
                // when Autodesk fixes this bug, then we might be able to re-open this
                std::string PoseName = CurrentPose->GetName();
                // all error report status
                FbxStatus Status;

                // it does not make any difference of checking with different node
                // it is possible pose 0 -> node array 2, but isValidBindPose function returns true even with node array 0
                for (auto Current : nodes)
                {
                    std::string CurrentName = Current->GetName();
                    NodeList pMissingAncestors, pMissingDeformers, pMissingDeformersAncestors, pWrongMatrices;

                    if (CurrentPose->IsValidBindPoseVerbose(Current, pMissingAncestors, pMissingDeformers, pMissingDeformersAncestors, pWrongMatrices, 0.0001, &Status))
                    {
                        poses.push_back(CurrentPose);
                        ////UE_LOG(LogFbx, Log, TEXT("Valid bind pose for Pose (%s) - %s"), *PoseName, *FString(Current->GetName()));
                        break;
                    }
                    else
                    {
                        // first try to fix up
                        // add missing ancestors
                        for (int i = 0; i < pMissingAncestors.GetCount(); i++)
                        {
                            FbxAMatrix mat = pMissingAncestors.GetAt(i)->EvaluateGlobalTransform(FBXSDK_TIME_ZERO);
                            CurrentPose->Add(pMissingAncestors.GetAt(i), mat);
                        }

                        pMissingAncestors.Clear();
                        pMissingDeformers.Clear();
                        pMissingDeformersAncestors.Clear();
                        pWrongMatrices.Clear();

                        // check it again
                        if (CurrentPose->IsValidBindPose(Current))
                        {
                            poses.push_back(CurrentPose);
                            ////UE_LOG(LogFbx, Log, TEXT("Valid bind pose for Pose (%s) - %s"), *PoseName, *FString(Current->GetName()));
                            break;
                        }
                        else
                        {
                            // first try to find parent who is null group and see if you can try test it again
                            FbxNode * ParentNode = Current->GetParent();
                            while (ParentNode)
                            {
                                FbxNodeAttribute * Attr = ParentNode->GetNodeAttribute();
                                if (Attr && Attr->GetAttributeType() == FbxNodeAttribute::eNull)
                                {
                                    // found it 
                                    break;
                                }

                                // find next parent
                                ParentNode = ParentNode->GetParent();
                            }

                            if (ParentNode && CurrentPose->IsValidBindPose(ParentNode))
                            {
                                poses.push_back(CurrentPose);
                                ////UE_LOG(LogFbx, Log, TEXT("Valid bind pose for Pose (%s) - %s"), *PoseName, *FString(Current->GetName()));
                                break;
                            }
                            else
                            {
                                std::string ErrorString = Status.GetErrorString();
                                ////if (!GIsAutomationTesting)
                                ////    UE_LOG(LogFbx, Warning, TEXT("Not valid bind pose for Pose (%s) - Node %s : %s"), *PoseName, *FString(Current->GetName()), *ErrorString);
                            }
                        }
                    }
                }
            }
        }

        return poses;
    }

    bool Importer::IsOddNegativeScale(FbxAMatrix & TotalMatrix)
    {
        FbxVector4 Scale = TotalMatrix.GetS();
        int32_t NegativeNum = 0;

        if (Scale[0] < 0) NegativeNum++;
        if (Scale[1] < 0) NegativeNum++;
        if (Scale[2] < 0) NegativeNum++;

        return NegativeNum == 1 || NegativeNum == 3;
    }

    BoundingBox Importer::CalcBoundingBox(const Float3 * points, size_t count)
    {
        BoundingBox bbox;
        for (size_t index = 0; index < count; ++index)
            bbox += points[index];
        return bbox;
    }

    bool Importer::AreSkelMeshVerticesEqual(const Vertex & V1, const Vertex & V2, const ImportThresholds & Threshold)
    {
        if (!PointsEqual(V1.Position, V2.Position, Threshold))
        {
            return false;
        }

        for (int32_t UVIdx = 0; UVIdx < FBX_MAX_TEXCOORDS; ++UVIdx)
        {
            if (!UVsEqual(V1.UVs[UVIdx], V2.UVs[UVIdx], Threshold))
            {
                return false;
            }
        }

        // 多调用这一层是为了和虚幻保持一致
        auto NormalsEqual2 = [](const Float3 & N1, const Float3 & N2, const ImportThresholds & Threshold)
        {
            Float3 N11, N22;
            N11.X = fbx::clamp((int)std::roundf(N1.X * 127), -128, 127) / 127.0;
            N11.Y = fbx::clamp((int)std::roundf(N1.Y * 127), -128, 127) / 127.0;
            N11.Z = fbx::clamp((int)std::roundf(N1.Z * 127), -128, 127) / 127.0;
            N22.X = fbx::clamp((int)std::roundf(N2.X * 127), -128, 127) / 127.0;
            N22.Y = fbx::clamp((int)std::roundf(N2.Y * 127), -128, 127) / 127.0;
            N22.Z = fbx::clamp((int)std::roundf(N2.Z * 127), -128, 127) / 127.0;
            return NormalsEqual(N11, N22, Threshold);
        };

        if (!NormalsEqual2(V1.TangentX, V2.TangentX, Threshold))
        {
            return false;
        }

        if (!NormalsEqual2(V1.TangentY, V2.TangentY, Threshold))
        {
            return false;
        }

        if (!NormalsEqual2(V1.TangentZ, V2.TangentZ, Threshold))
        {
            return false;
        }

        bool	InfluencesMatch = 1;
        for (uint32_t InfluenceIndex = 0; InfluenceIndex < FBX_MAX_TOTAL_INFLUENCES; InfluenceIndex++)
        {
            if (V1.InfluenceBones[InfluenceIndex] != V2.InfluenceBones[InfluenceIndex] ||
                V1.InfluenceWeights[InfluenceIndex] != V2.InfluenceWeights[InfluenceIndex])
            {
                InfluencesMatch = 0;
                break;
            }
        }

        if (V1.Color != V2.Color)
        {
            return false;
        }

        if (!InfluencesMatch)
        {
            return false;
        }

        return true;
    }

    void Importer::ComputeTriangleTangents(const std::vector<Float3> & points, const std::vector<uint32_t> & indices, const std::vector<Float2> & uvs,
        std::vector<Float3> & tangentsX, std::vector<Float3> & tangentsY, std::vector<Float3> & tangentsZ, 
        float ComparisonThreshold)
    {
        size_t NumTriangles = indices.size() / 3;
        tangentsX.clear();
        tangentsY.clear();
        tangentsZ.clear();

        for (size_t TriangleIndex = 0; TriangleIndex < NumTriangles; TriangleIndex++)
        {
            Float3 P[3];
            for (int32_t i = 0; i < 3; ++i)
            {
                P[i] = points[indices[TriangleIndex * 3 + i]];
            }

            const Float3 Normal = ((P[1] - P[2]) ^ (P[0] - P[2])).GetSafeNormal(ComparisonThreshold);
            Matrix ParameterToLocal(
                Float4(P[1].X - P[0].X, P[1].Y - P[0].Y, P[1].Z - P[0].Z, 0),
                Float4(P[2].X - P[0].X, P[2].Y - P[0].Y, P[2].Z - P[0].Z, 0),
                Float4(P[0].X, P[0].Y, P[0].Z, 0),
                Float4(0, 0, 0, 1)
            );

            const Float2 T1 = uvs[TriangleIndex * 3 + 0];
            const Float2 T2 = uvs[TriangleIndex * 3 + 1];
            const Float2 T3 = uvs[TriangleIndex * 3 + 2];

            Matrix ParameterToTexture(
                Float4(T2.X - T1.X, T2.Y - T1.Y, 0, 0),
                Float4(T3.X - T1.X, T3.Y - T1.Y, 0, 0),
                Float4(T1.X, T1.Y, 1, 0),
                Float4(0, 0, 0, 1)
            );

            // Use InverseSlow to catch singular matrices.  Inverse can miss this sometimes.
            Matrix TextureToLocal = ParameterToTexture.Inverse() * ParameterToLocal;

            tangentsX.push_back(TextureToLocal.TransformVector(Float3(1, 0, 0)).GetSafeNormal());
            tangentsY.push_back(TextureToLocal.TransformVector(Float3(0, 1, 0)).GetSafeNormal());
            tangentsZ.push_back(Normal);

            Float3::CreateOrthonormalBasis(
                tangentsX[TriangleIndex],
                tangentsY[TriangleIndex],
                tangentsZ[TriangleIndex]
            );
        }

        assert(tangentsX.size() == NumTriangles);
        assert(tangentsY.size() == NumTriangles);
        assert(tangentsZ.size() == NumTriangles);
    }

    void Importer::ComputeNormals(const std::vector<Float3> & points, const std::vector<uint32_t> & indices, const std::vector<uint32_t> & groupIndices, const std::vector<Float2> & uvs,
        std::vector<Float3> & normals,
        bool ignoreDegenerateTriangles, bool blendOverlappingNormals)
    {
        const float ComparisonThreshold = ignoreDegenerateTriangles ? FBX_THRESH_POINTS_ARE_SAME : 0.0f;
        FOverlappingCorners OverlappingCorners(points, indices, ComparisonThreshold);

        bool bBlendOverlappingNormals = blendOverlappingNormals;
        bool bIgnoreDegenerateTriangles = ignoreDegenerateTriangles;

        // Compute per-triangle tangents.
        std::vector<Float3> TriangleTangentX;
        std::vector<Float3> TriangleTangentY;
        std::vector<Float3> TriangleTangentZ;

        ComputeTriangleTangents(
            points,
            indices,
            uvs,
            TriangleTangentX,
            TriangleTangentY,
            TriangleTangentZ,
            bIgnoreDegenerateTriangles ? FBX_SMALL_NUMBER : 0.0f
        );
        struct FFanFace
        {
            int32_t FaceIndex;
            int32_t LinkedVertexIndex;
            bool bFilled;
            bool bBlendTangents;
            bool bBlendNormals;
        };
        // Declare these out here to avoid reallocations.
        std::vector<FFanFace> RelevantFacesForCorner[3];
        std::vector<int32_t> AdjacentFaces;

        int32_t NumWedges = indices.size();
        int32_t NumFaces = NumWedges / 3;

        // Allocate storage for tangents if none were provided, and calculate normals for MikkTSpace.
        if (normals.size() != NumWedges)
        {
            // normals are not included, so we should calculate them
            normals.resize(NumWedges);
        }

        // we need to calculate normals for MikkTSpace
        for (int32_t FaceIndex = 0; FaceIndex < NumFaces; FaceIndex++)
        {
            int32_t WedgeOffset = FaceIndex * 3;
            Float3 CornerPositions[3];
            Float3 CornerNormal[3];

            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                CornerNormal[CornerIndex] = Float3_Zero;
                CornerPositions[CornerIndex] = points[indices[WedgeOffset + CornerIndex]];
                RelevantFacesForCorner[CornerIndex].clear();
            }

            // Don't process degenerate triangles.
            if (PointsEqual(CornerPositions[0], CornerPositions[1], ComparisonThreshold)
                || PointsEqual(CornerPositions[0], CornerPositions[2], ComparisonThreshold)
                || PointsEqual(CornerPositions[1], CornerPositions[2], ComparisonThreshold))
            {
                continue;
            }

            // No need to process triangles if tangents already exist.
            bool bCornerHasNormal[3] = { 0 };
            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                bCornerHasNormal[CornerIndex] = !normals[WedgeOffset + CornerIndex].IsZero();
            }
            if (bCornerHasNormal[0] && bCornerHasNormal[1] && bCornerHasNormal[2])
            {
                continue;
            }

            // Start building a list of faces adjacent to this face.
            AdjacentFaces.clear();
            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                int32_t ThisCornerIndex = WedgeOffset + CornerIndex;
                const std::vector<int32_t> & DupVerts = OverlappingCorners.FindIfOverlapping(ThisCornerIndex);
                if (DupVerts.size() == 0)
                {
                    AddUnique(AdjacentFaces, ThisCornerIndex / 3); // I am a "dup" of myself
                }
                for (int32_t k = 0; k < DupVerts.size(); k++)
                {
                    AddUnique(AdjacentFaces, DupVerts[k] / 3);
                }
            }

            // We need to sort these here because the criteria for point equality is
            // exact, so we must ensure the exact same order for all dups.
            std::sort(AdjacentFaces.begin(), AdjacentFaces.end());

            // Process adjacent faces
            for (size_t AdjacentFaceIndex = 0; AdjacentFaceIndex < AdjacentFaces.size(); AdjacentFaceIndex++)
            {
                int32_t OtherFaceIndex = AdjacentFaces[AdjacentFaceIndex];
                for (int32_t OurCornerIndex = 0; OurCornerIndex < 3; OurCornerIndex++)
                {
                    if (bCornerHasNormal[OurCornerIndex])
                        continue;

                    FFanFace NewFanFace;
                    int32_t CommonIndexCount = 0;

                    // Check for vertices in common.
                    if (FaceIndex == OtherFaceIndex)
                    {
                        CommonIndexCount = 3;
                        NewFanFace.LinkedVertexIndex = OurCornerIndex;
                    }
                    else
                    {
                        // Check matching vertices against main vertex .
                        for (int32_t OtherCornerIndex = 0; OtherCornerIndex < 3; OtherCornerIndex++)
                        {
                            if (PointsEqual(
                                CornerPositions[OurCornerIndex],
                                points[indices[OtherFaceIndex * 3 + OtherCornerIndex]],
                                ComparisonThreshold
                                ))
                            {
                                CommonIndexCount++;
                                NewFanFace.LinkedVertexIndex = OtherCornerIndex;
                            }
                        }
                    }

                    // Add if connected by at least one point. Smoothing matches are considered later.
                    if (CommonIndexCount > 0)
                    {
                        NewFanFace.FaceIndex = OtherFaceIndex;
                        NewFanFace.bFilled = (OtherFaceIndex == FaceIndex); // Starter face for smoothing floodfill.
                        NewFanFace.bBlendTangents = NewFanFace.bFilled;
                        NewFanFace.bBlendNormals = NewFanFace.bFilled;
                        RelevantFacesForCorner[OurCornerIndex].push_back(NewFanFace);
                    }
                }
            }

            // Find true relevance of faces for a vertex normal by traversing
            // smoothing-group-compatible connected triangle fans around common vertices.
            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                if (bCornerHasNormal[CornerIndex])
                    continue;

                int32_t NewConnections;
                do
                {
                    NewConnections = 0;
                    for (int32_t OtherFaceIdx = 0; OtherFaceIdx < RelevantFacesForCorner[CornerIndex].size(); OtherFaceIdx++)
                    {
                        FFanFace & OtherFace = RelevantFacesForCorner[CornerIndex][OtherFaceIdx];
                        // The vertex' own face is initially the only face with bFilled == true.
                        if (OtherFace.bFilled)
                        {
                            for (int32_t NextFaceIndex = 0; NextFaceIndex < RelevantFacesForCorner[CornerIndex].size(); NextFaceIndex++)
                            {
                                FFanFace & NextFace = RelevantFacesForCorner[CornerIndex][NextFaceIndex];
                                if (!NextFace.bFilled) // && !NextFace.bBlendTangents)
                                {
                                    if ((NextFaceIndex != OtherFaceIdx)
                                        && (groupIndices[NextFace.FaceIndex] & groupIndices[OtherFace.FaceIndex]))
                                    {
                                        int32_t CommonVertices = 0;
                                        int32_t CommonNormalVertices = 0;
                                        for (int32_t OtherCornerIndex = 0; OtherCornerIndex < 3; OtherCornerIndex++)
                                        {
                                            for (int32_t NextCornerIndex = 0; NextCornerIndex < 3; NextCornerIndex++)
                                            {
                                                int32_t NextVertexIndex = indices[NextFace.FaceIndex * 3 + NextCornerIndex];
                                                int32_t OtherVertexIndex = indices[OtherFace.FaceIndex * 3 + OtherCornerIndex];
                                                if (PointsEqual(
                                                    points[NextVertexIndex],
                                                    points[OtherVertexIndex],
                                                    ComparisonThreshold))
                                                {
                                                    CommonVertices++;
                                                    if (bBlendOverlappingNormals
                                                        || NextVertexIndex == OtherVertexIndex)
                                                    {
                                                        CommonNormalVertices++;
                                                    }
                                                }
                                            }
                                        }
                                        // Flood fill faces with more than one common vertices which must be touching edges.
                                        if (CommonVertices > 1)
                                        {
                                            NextFace.bFilled = true;
                                            NextFace.bBlendNormals = (CommonNormalVertices > 1);
                                            NewConnections++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                while (NewConnections > 0);
            }


            // Vertex normal construction.
            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                if (bCornerHasNormal[CornerIndex])
                {
                    CornerNormal[CornerIndex] = normals[WedgeOffset + CornerIndex];
                }
                else
                {
                    for (int32_t RelevantFaceIdx = 0; RelevantFaceIdx < RelevantFacesForCorner[CornerIndex].size(); RelevantFaceIdx++)
                    {
                        FFanFace const & RelevantFace = RelevantFacesForCorner[CornerIndex][RelevantFaceIdx];
                        if (RelevantFace.bFilled)
                        {
                            int32_t OtherFaceIndex = RelevantFace.FaceIndex;
                            if (RelevantFace.bBlendNormals)
                            {
                                CornerNormal[CornerIndex] += TriangleTangentZ[OtherFaceIndex];
                            }
                        }
                    }
                    if (!normals[WedgeOffset + CornerIndex].IsZero())
                    {
                        CornerNormal[CornerIndex] = normals[WedgeOffset + CornerIndex];
                    }
                }
            }

            // Normalization.
            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                CornerNormal[CornerIndex].Normalize();
            }

            // Copy back to the mesh.
            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                normals[WedgeOffset + CornerIndex] = CornerNormal[CornerIndex].ContainsNaN() ? Float3_Zero : CornerNormal[CornerIndex];
            }
        }

        assert(normals.size() == NumWedges);
    }
}
