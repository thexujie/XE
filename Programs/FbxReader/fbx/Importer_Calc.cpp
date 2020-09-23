#include "Importer.h"
#include "mikktspace/mikktspace.h"

namespace fbx
{
    // MikkTSpace implementations for skeletal meshes, where tangents/bitangents are ultimately derived from lists of attributes.

    // Holder for skeletal data to be passed to MikkTSpace.
    // Holds references to the wedge, face and points vectors that BuildSkeletalMesh is given.
    // Holds reference to the calculated normals array, which will be fleshed out if they've been calculated.
    // Holds reference to the newly created tangent and bitangent arrays, which MikkTSpace will fleshed out if required.

    class MikkTSpace_Skeletal_Mesh
    {
    public:
        int nFaces = 0;
        Wedge * wedges;
        Triangle * faces;
        Float3 * points;
        bool bComputeNormals;
        Float3 * TangentsX;
        Float3 * TangentsY;
        Float3 * TangentsZ;
    };

    class MikkTSpaceInterface : public SMikkTSpaceInterface
    {
    public:
        MikkTSpaceInterface()
        {
            m_getNumFaces = [](const SMikkTSpaceContext * Context)
            {
                MikkTSpace_Skeletal_Mesh * UserData = (MikkTSpace_Skeletal_Mesh *)(Context->m_pUserData);
                return UserData->nFaces;
            };

            m_getNumVerticesOfFace = [](const SMikkTSpaceContext * Context, const int FaceIdx) 
            {
                // Confirmed?
                return 3;
            };

            m_getPosition = [](const SMikkTSpaceContext * Context, float Position[3], const int FaceIdx, const int VertIdx)
            {
                MikkTSpace_Skeletal_Mesh * UserData = (MikkTSpace_Skeletal_Mesh *)(Context->m_pUserData);
                const Float3 & VertexPosition = UserData->points[UserData->wedges[UserData->faces[FaceIdx].WedgeIndex[VertIdx]].PointIndex];
                Position[0] = VertexPosition.X;
                Position[1] = VertexPosition.Y;
                Position[2] = VertexPosition.Z;
            };

            m_getNormal = [](const SMikkTSpaceContext * Context, float Normal[3], const int FaceIdx, const int VertIdx)
            {
                MikkTSpace_Skeletal_Mesh * UserData = (MikkTSpace_Skeletal_Mesh *)(Context->m_pUserData);
                // Get different normals depending on whether they've been calculated or not.
                if (UserData->bComputeNormals)
                {
                    Float3 & VertexNormal = UserData->TangentsZ[FaceIdx * 3 + VertIdx];
                    Normal[0] = VertexNormal.X;
                    Normal[1] = VertexNormal.Y;
                    Normal[2] = VertexNormal.Z;
                }
                else
                {
                    const Float3 & VertexNormal = UserData->faces[FaceIdx].TangentZ[VertIdx];
                    Normal[0] = VertexNormal.X;
                    Normal[1] = VertexNormal.Y;
                    Normal[2] = VertexNormal.Z;
                }
            };

            m_getTexCoord = [](const SMikkTSpaceContext * Context, float UV[2], const int FaceIdx, const int VertIdx)
            {
                MikkTSpace_Skeletal_Mesh * UserData = (MikkTSpace_Skeletal_Mesh *)(Context->m_pUserData);
                const Float2 & TexCoord = UserData->wedges[UserData->faces[FaceIdx].WedgeIndex[VertIdx]].UVs[0];
                UV[0] = TexCoord.X;
                UV[1] = TexCoord.Y;
            };

            m_setTSpaceBasic = [](const SMikkTSpaceContext * Context, const float Tangent[3], const float BitangentSign, const int FaceIdx, const int VertIdx)
            {
                MikkTSpace_Skeletal_Mesh * UserData = (MikkTSpace_Skeletal_Mesh *)(Context->m_pUserData);
                Float3 & VertexTangent = UserData->TangentsX[FaceIdx * 3 + VertIdx];
                VertexTangent.X = Tangent[0];
                VertexTangent.Y = Tangent[1];
                VertexTangent.Z = Tangent[2];

                Float3 Bitangent;
                // Get different normals depending on whether they've been calculated or not.
                if (UserData->bComputeNormals)
                {
                    Bitangent = BitangentSign * (UserData->TangentsZ[FaceIdx * 3 + VertIdx] ^ VertexTangent);
                }
                else
                {
                    Bitangent = BitangentSign * (UserData->faces[FaceIdx].TangentZ[VertIdx] ^ VertexTangent);
                }
                Float3 & VertexBitangent = UserData->TangentsY[FaceIdx * 3 + VertIdx];
                // Switch the tangent space swizzle to X+Y-Z+ for legacy reasons.
                VertexBitangent.X = -Bitangent[0];
                VertexBitangent.Y = -Bitangent[1];
                VertexBitangent.Z = -Bitangent[2];
            };

            m_setTSpace = nullptr;
        }
    };


    static void MikkGetNormal_Skeletal(const SMikkTSpaceContext * Context, float Normal[3], const int FaceIdx, const int VertIdx)
    {
        MikkTSpace_Skeletal_Mesh * UserData = (MikkTSpace_Skeletal_Mesh *)(Context->m_pUserData);
        // Get different normals depending on whether they've been calculated or not.
        if (UserData->bComputeNormals)
        {
            Float3 & VertexNormal = UserData->TangentsZ[FaceIdx * 3 + VertIdx];
            Normal[0] = VertexNormal.X;
            Normal[1] = VertexNormal.Y;
            Normal[2] = VertexNormal.Z;
        }
        else
        {
            const Float3 & VertexNormal = UserData->faces[FaceIdx].TangentZ[VertIdx];
            Normal[0] = VertexNormal.X;
            Normal[1] = VertexNormal.Y;
            Normal[2] = VertexNormal.Z;
        }
    }

    static void Skeletal_ComputeTriangleTangents(
        std::vector<Float3> & TriangleTangentX,
        std::vector<Float3> & TriangleTangentY,
        std::vector<Float3> & TriangleTangentZ,
        const std::vector<Float3> & points,const std::vector<Wedge> & wedges, std::vector<Triangle> & faces,
        float ComparisonThreshold
    )
    {
        auto GetVertexPosition = [&](uint32_t FaceIndex, uint32_t TriIndex)
        {
            return points[wedges[faces[FaceIndex].WedgeIndex[TriIndex]].PointIndex];
        };

        auto GetVertexUV = [&](uint32_t FaceIndex, uint32_t TriIndex, uint32_t UVIndex)
        {
            return wedges[faces[FaceIndex].WedgeIndex[TriIndex]].UVs[UVIndex];
        };

        auto CreateOrthonormalBasis = [](Float3 & XAxis, Float3 & YAxis, Float3 & ZAxis)
        {
            // Project the X and Y axes onto the plane perpendicular to the Z axis.
            XAxis -= (XAxis | ZAxis) / (ZAxis | ZAxis) * ZAxis;
            YAxis -= (YAxis | ZAxis) / (ZAxis | ZAxis) * ZAxis;

            // If the X axis was parallel to the Z axis, choose a vector which is orthogonal to the Y and Z axes.
            if (XAxis.SizeSquared() < FBX_DELTA * FBX_DELTA)
            {
                XAxis = YAxis ^ ZAxis;
            }

            // If the Y axis was parallel to the Z axis, choose a vector which is orthogonal to the X and Z axes.
            if (YAxis.SizeSquared() < FBX_DELTA * FBX_DELTA)
            {
                YAxis = XAxis ^ ZAxis;
            }

            // Normalize the basis vectors.
            XAxis.Normalize();
            YAxis.Normalize();
            ZAxis.Normalize();
        };

        int32_t NumTriangles = faces.size();
        TriangleTangentX.reserve(NumTriangles);
        TriangleTangentY.reserve(NumTriangles);
        TriangleTangentZ.reserve(NumTriangles);

        for (int32_t TriangleIndex = 0; TriangleIndex < NumTriangles; TriangleIndex++)
        {
            const int32_t UVIndex = 0;
            Float3 P[3];

            for (int32_t i = 0; i < 3; ++i)
            {
                P[i] = GetVertexPosition(TriangleIndex, i);
            }

            const Float3 Normal = ((P[1] - P[2]) ^ (P[0] - P[2])).GetSafeNormal(ComparisonThreshold);
            Matrix	ParameterToLocal = 
            {
                (Float4)Plane(P[1].X - P[0].X, P[1].Y - P[0].Y, P[1].Z - P[0].Z, 0),
                (Float4)Plane(P[2].X - P[0].X, P[2].Y - P[0].Y, P[2].Z - P[0].Z, 0),
                (Float4)Plane(P[0].X, P[0].Y, P[0].Z, 0),
                (Float4)Plane(0, 0, 0, 1)
            };

            Float2 T1 = GetVertexUV(TriangleIndex, 0, UVIndex);
            Float2 T2 = GetVertexUV(TriangleIndex, 1, UVIndex);
            Float2 T3 = GetVertexUV(TriangleIndex, 2, UVIndex);
            Matrix ParameterToTexture =
            {
                Plane(T2.X - T1.X, T2.Y - T1.Y, 0, 0),
                Plane(T3.X - T1.X, T3.Y - T1.Y, 0, 0),
                Plane(T1.X, T1.Y, 1, 0),
                Plane(0, 0, 0, 1)
            };

            // Use InverseSlow to catch singular matrices.  Inverse can miss this sometimes.
            const Matrix TextureToLocal = ParameterToTexture.Inverse() * ParameterToLocal;

            TriangleTangentX.push_back(TextureToLocal.TransformVector(Float3(1, 0, 0)).GetSafeNormal());
            TriangleTangentY.push_back(TextureToLocal.TransformVector(Float3(0, 1, 0)).GetSafeNormal());
            TriangleTangentZ.push_back(Normal);

            CreateOrthonormalBasis(
                TriangleTangentX[TriangleIndex],
                TriangleTangentY[TriangleIndex],
                TriangleTangentZ[TriangleIndex]
            );
        }
    }

    fbx::error_e Importer::ComputeTangents_MikkTSpace()
    {
        auto GetWedgeIndex = [this](uint32_t FaceIndex, uint32_t TriIndex)
        {
            return _faces[FaceIndex].WedgeIndex[TriIndex];
        };

        auto GetVertexPosition1 = [this](uint32_t WedgeIndex)
        {
            return _points[_wedges[WedgeIndex].PointIndex];
        };

        auto GetVertexPosition = [this](uint32_t FaceIndex, uint32_t TriIndex)
        {
            return _points[_wedges[_faces[FaceIndex].WedgeIndex[TriIndex]].PointIndex];
        };

        auto GetFaceSmoothingGroups = [this](uint32_t FaceIndex)
        {
            return _faces[FaceIndex].SmoothingGroups;
        };

        auto GetVertexIndex = [this](uint32_t FaceIndex, uint32_t TriIndex)
        {
            return _wedges[_faces[FaceIndex].WedgeIndex[TriIndex]].PointIndex;
        };

        auto GetVertexUV = [this](uint32_t FaceIndex, uint32_t TriIndex, uint32_t UVIndex)
        {
            return _wedges[_faces[FaceIndex].WedgeIndex[TriIndex]].UVs[UVIndex];
        };

        auto IsNearlyEqual = [](float A, float B, float ErrorTolerance = FBX_SMALL_NUMBER)
        {
            return std::abs(A - B) <= ErrorTolerance;
        };

        auto IsTriangleMirror = [&, this](const std::vector<Float3> & TriangleTangentZ, const uint32_t FaceIdxA, const uint32_t FaceIdxB)
        {
            if (FaceIdxA == FaceIdxB)
            {
                return false;
            }
            for (int32_t CornerA = 0; CornerA < 3; ++CornerA)
            {
                const Float3 & CornerAPosition = GetVertexPosition1((FaceIdxA * 3) + CornerA);
                bool bFoundMatch = false;
                for (int32_t CornerB = 0; CornerB < 3; ++CornerB)
                {
                    const Float3 & CornerBPosition = GetVertexPosition1((FaceIdxB * 3) + CornerB);
                    if (PointsEqual(CornerAPosition, CornerBPosition, _options.OverlappingThresholds))
                    {
                        bFoundMatch = true;
                        break;
                    }
                }

                if (!bFoundMatch)
                {
                    return false;
                }
            }
            //Check if the triangles normals are opposite and parallel. Dot product equal -1.0f
            if (IsNearlyEqual(TriangleTangentZ[FaceIdxA] | TriangleTangentZ[FaceIdxB], -1.0f, FBX_KINDA_SMALL_NUMBER))
            {
                return true;
            }
            return false;
        };

        {
            int32_t NumFaces = _faces.size();
            int32_t NumWedges = _wedges.size();
            assert(NumFaces * 3 <= NumWedges);

            // Create a list of vertex Z/index pairs
            std::vector<IndexAndZ> VertIndexAndZ;
            for (int32_t FaceIndex = 0; FaceIndex < NumFaces; FaceIndex++)
            {
                for (int32_t TriIndex = 0; TriIndex < 3; ++TriIndex)
                {
                    uint32_t Index = GetWedgeIndex(FaceIndex, TriIndex);
                    //VertIndexAndZ.push_back(IndexAndZ(Index, GetVertexPosition1(Index)));
                    Float3 Pos = GetVertexPosition1(Index);
                    VertIndexAndZ.push_back({ (int32_t)Index, 0.30f * Pos.X + 0.33f * Pos.Y + 0.37f * Pos.Z });
                }
            }

            // Sort the vertices by z value
            std::sort(VertIndexAndZ.begin(), VertIndexAndZ.end());
            //std::stable_sort(VertIndexAndZ.begin(), VertIndexAndZ.end(), FCompareIndexAndZ());

            _overlappingCorners.Init(NumWedges);

            // Search for duplicates, quickly!
            for (int32_t i = 0; i < VertIndexAndZ.size(); i++)
            {
                // only need to search forward, since we add pairs both ways
                for (int32_t j = i + 1; j < VertIndexAndZ.size(); j++)
                {
                    if (std::abs(VertIndexAndZ[j].Z - VertIndexAndZ[i].Z) > FBX_THRESH_POINTS_ARE_SAME)
                        break; // can't be any more dups

                    Float3 PositionA = GetVertexPosition1(VertIndexAndZ[i].Index);
                    Float3 PositionB = GetVertexPosition1(VertIndexAndZ[j].Index);

                    if (PointsEqual(PositionA, PositionB, FBX_THRESH_POINTS_ARE_SAME))
                    {
                        _overlappingCorners.Add(VertIndexAndZ[i].Index, VertIndexAndZ[j].Index);
                    }
                }
            }

            _overlappingCorners.FinishAdding();
        }

        bool bBlendOverlappingNormals = true;
        bool bIgnoreDegenerateTriangles = _options.bRemoveDegenerateTriangles;

        // Compute per-triangle tangents.
        std::vector<Float3> TriangleTangentX;
        std::vector<Float3> TriangleTangentY;
        std::vector<Float3> TriangleTangentZ;

        Skeletal_ComputeTriangleTangents(
            TriangleTangentX,
            TriangleTangentY,
            TriangleTangentZ,
            _points, _wedges, _faces,
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

        int32_t NumFaces = _faces.size();
        int32_t NumWedges = _wedges.size();
        assert(NumFaces * 3 == NumWedges);

        bool bWedgeTSpace = false;

        ////if (WedgeTangentX.Num() > 0 && WedgeTangentY.Num() > 0)
        ////{
        ////    bWedgeTSpace = true;
        ////    for (int32_t WedgeIdx = 0; WedgeIdx < WedgeTangentX.Num()
        ////        && WedgeIdx < WedgeTangentY.Num(); ++WedgeIdx)
        ////    {
        ////        bWedgeTSpace = bWedgeTSpace && (!WedgeTangentX[WedgeIdx].IsNearlyZero()) && (!WedgeTangentY[WedgeIdx].IsNearlyZero());
        ////    }
        ////}

        // Allocate storage for tangents if none were provided, and calculate normals for MikkTSpace.
        ////if (WedgeTangentZ.Num() != NumWedges)
        ////{
        ////    // normals are not included, so we should calculate them
        ////    WedgeTangentZ.Empty(NumWedges);
        ////    WedgeTangentZ.AddZeroed(NumWedges);
        ////}

        // we need to calculate normals for MikkTSpace

        for (int32_t FaceIndex = 0; FaceIndex < NumFaces; FaceIndex++)
        {
            int32_t WedgeOffset = FaceIndex * 3;
            Float3 CornerPositions[3];
            Float3 CornerNormal[3];

            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                CornerNormal[CornerIndex] = Float3_Zero;
                CornerPositions[CornerIndex] = GetVertexPosition(FaceIndex, CornerIndex);
                RelevantFacesForCorner[CornerIndex].clear();
            }

            // Don't process degenerate triangles.
            if (PointsEqual(CornerPositions[0], CornerPositions[1], _options.OverlappingThresholds)
                || PointsEqual(CornerPositions[0], CornerPositions[2], _options.OverlappingThresholds)
                || PointsEqual(CornerPositions[1], CornerPositions[2], _options.OverlappingThresholds))
            {
                continue;
            }

            // No need to process triangles if tangents already exist.
            bool bCornerHasNormal[3] = { 0 };
            for (int32_t CornerIndex = 0; CornerIndex < 3; CornerIndex++)
            {
                bCornerHasNormal[CornerIndex] = !_tangentsZ[WedgeOffset + CornerIndex].IsZero();
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
                const std::vector<int32_t> & DupVerts = _overlappingCorners.FindIfOverlapping(ThisCornerIndex);
                if (DupVerts.size() == 0)
                {
                    AddUnique(AdjacentFaces, ThisCornerIndex / 3); // I am a "dup" of myself
                }
                for (int32_t k = 0; k < DupVerts.size(); k++)
                {
                    int32_t PotentialTriangleIndex = DupVerts[k] / 3;
                    //Do not add mirror triangle to the adjacentFaces
                    if (!IsTriangleMirror(TriangleTangentZ, FaceIndex, PotentialTriangleIndex))
                    {
                        AddUnique(AdjacentFaces, PotentialTriangleIndex);
                    }
                }
            }

            // We need to sort these here because the criteria for point equality is
            // exact, so we must ensure the exact same order for all dups.
            std::sort(AdjacentFaces.begin(), AdjacentFaces.end());

            // Process adjacent faces
            for (int32_t AdjacentFaceIndex = 0; AdjacentFaceIndex < AdjacentFaces.size(); AdjacentFaceIndex++)
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
                                GetVertexPosition(OtherFaceIndex, OtherCornerIndex),
                                _options.OverlappingThresholds
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
                                        && (GetFaceSmoothingGroups(NextFace.FaceIndex) & GetFaceSmoothingGroups(OtherFace.FaceIndex)))
                                    {
                                        int32_t CommonVertices = 0;
                                        int32_t CommonNormalVertices = 0;
                                        for (int32_t OtherCornerIndex = 0; OtherCornerIndex < 3; OtherCornerIndex++)
                                        {
                                            for (int32_t NextCornerIndex = 0; NextCornerIndex < 3; NextCornerIndex++)
                                            {
                                                int32_t NextVertexIndex = GetVertexIndex(NextFace.FaceIndex, NextCornerIndex);
                                                int32_t OtherVertexIndex = GetVertexIndex(OtherFace.FaceIndex, OtherCornerIndex);
                                                if (PointsEqual(
                                                    GetVertexPosition(NextFace.FaceIndex, NextCornerIndex),
                                                    GetVertexPosition(OtherFace.FaceIndex, OtherCornerIndex),
                                                    _options.OverlappingThresholds))
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
                    CornerNormal[CornerIndex] = _tangentsZ[WedgeOffset + CornerIndex];
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
                    if (!_tangentsZ[WedgeOffset + CornerIndex].IsZero())
                    {
                        CornerNormal[CornerIndex] = _tangentsZ[WedgeOffset + CornerIndex];
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
                _tangentsZ[WedgeOffset + CornerIndex] = CornerNormal[CornerIndex];
            }
        }

        if (_tangentsX.size() != NumWedges)
        {
            _tangentsX.resize(NumWedges);
        }
        if (_tangentsY.size() != NumWedges)
            _tangentsX.resize(NumWedges);

        //if (!bWedgeTSpace)
        {
            MikkTSpaceInterface MikkTInterface;
            MikkTSpace_Skeletal_Mesh MikkTUserData;
            MikkTUserData.nFaces = _faces.size();
            MikkTUserData.wedges = _wedges.data();
            MikkTUserData.faces = _faces.data();
            MikkTUserData.points = _points.data();
            MikkTUserData.bComputeNormals = !_hasNormals || !_options.ShouldImportNormals();
            MikkTUserData.TangentsX = _tangentsX.data();
            MikkTUserData.TangentsY = _tangentsY.data();
            MikkTUserData.TangentsZ = _tangentsZ.data();

            // we can use mikktspace to calculate the tangents		
            SMikkTSpaceContext MikkTContext;
            MikkTContext.m_pInterface = &MikkTInterface;
            MikkTContext.m_pUserData = &MikkTUserData;
            //MikkTContext.m_bIgnoreDegenerates = bIgnoreDegenerateTriangles;

            genTangSpaceDefault(&MikkTContext);
        }

        assert(_tangentsX.size() == NumWedges);
        assert(_tangentsY.size() == NumWedges);
        assert(_tangentsZ.size() == NumWedges);
        return fbx::error_ok;
    }

    fbx::error_e Importer::ComputeTangents()
    {
        return fbx::error_ok;
    }
}
