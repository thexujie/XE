#include "Importer.h"

namespace fbx
{
    fbx::error_e Importer::ImportMorphTargets()
    {
        bool bHasMorph = false;
        // check if there are morph in this geometry
        for (size_t NodeIndex = 0; NodeIndex < _meshs.size(); NodeIndex++)
        {
            FbxGeometry * Geometry = (FbxGeometry *)_meshs[NodeIndex]->GetNodeAttribute();
            if (Geometry)
            {
                bHasMorph = Geometry->GetDeformerCount(FbxDeformer::eBlendShape) > 0;
                if (bHasMorph)
                {
                    break;
                }
            }
        }

        if (!bHasMorph)
            return fbx::error_ok;

        FbxString ShapeNodeName;
        std::map<std::string, std::vector<FbxShape *>> ShapeNameToShapeArray;

        // Temp arrays to keep track of data being used by threads
        std::vector<std::vector<MorphTargetDelta > *> Results;

        // For each morph in FBX geometries, we create one morph target for the Unreal skeletal mesh
        for (size_t NodeIndex = 0; NodeIndex < _meshs.size(); NodeIndex++)
        {
            FbxGeometry * Geometry = (FbxGeometry *)_meshs[NodeIndex]->GetNodeAttribute();
            if (Geometry)
            {
                const int32_t BlendShapeDeformerCount = Geometry->GetDeformerCount(FbxDeformer::eBlendShape);

                /************************************************************************/
                /* collect all the shapes                                               */
                /************************************************************************/
                for (int32_t BlendShapeIndex = 0; BlendShapeIndex < BlendShapeDeformerCount; ++BlendShapeIndex)
                {
                    FbxBlendShape * BlendShape = (FbxBlendShape *)Geometry->GetDeformer(BlendShapeIndex, FbxDeformer::eBlendShape);
                    const int32_t BlendShapeChannelCount = BlendShape->GetBlendShapeChannelCount();

                    std::string BlendShapeName = BlendShape->GetName();

                    // see below where this is used for explanation...
                    bool bMightBeBadMAXFile = BlendShapeName == "Morpher";

                    for (int32_t ChannelIndex = 0; ChannelIndex < BlendShapeChannelCount; ++ChannelIndex)
                    {
                        FbxBlendShapeChannel * Channel = BlendShape->GetBlendShapeChannel(ChannelIndex);
                        if (Channel)
                        {
                            //Find which shape should we use according to the weight.
                            const int32_t CurrentChannelShapeCount = Channel->GetTargetShapeCount();

                            std::string ChannelName = Channel->GetName();

                            // Maya adds the name of the blendshape and an underscore to the front of the channel name, so remove it
                            if (ChannelName.find(BlendShapeName) == 0)
                            {
                                ChannelName = ChannelName.substr(BlendShapeName.length() + 1);
                            }

                            for (int32_t ShapeIndex = 0; ShapeIndex < CurrentChannelShapeCount; ++ShapeIndex)
                            {
                                FbxShape * Shape = Channel->GetTargetShape(ShapeIndex);

                                std::string ShapeName;
                                if (CurrentChannelShapeCount > 1)
                                {
                                    ShapeName = Shape->GetName();
                                }
                                else
                                {
                                    if (bMightBeBadMAXFile)
                                    {
                                        ShapeName = Shape->GetName();
                                    }
                                    else
                                    {
                                        // Maya concatenates the number of the shape to the end of its name, so instead use the name of the channel
                                        ShapeName = ChannelName;
                                    }
                                }

                                if (ShapeNameToShapeArray.find(ShapeName) == ShapeNameToShapeArray.end())
                                    ShapeNameToShapeArray.insert({ ShapeName, std::vector<FbxShape *>(_meshs.size(), nullptr) });
                                ShapeNameToShapeArray[ShapeName][NodeIndex] = Shape;
                            }
                        }
                    }
                }
            }
        } // for NodeIndex

        bool bIgnoreDegenerateTriangles = false;
        const float ComparisonThreshold = bIgnoreDegenerateTriangles ? FBX_THRESH_POINTS_ARE_SAME : 0.f;

        std::vector<Triangle> bundleFaces;
        std::vector<uint32_t> bundleIndices;
        std::vector<Float2> bundleUVs;
        std::vector<uint32_t> bundleSmoothGroupIncices;
        std::multimap<uint32_t, uint32_t> bundleWedgeToFaces;

        for (size_t iface = 0; iface < _faces.size(); ++iface)
        {
            const Triangle & face = _faces[iface];
            auto & wedge0 = _wedges[face.WedgeIndex[0]];
            auto & wedge1 = _wedges[face.WedgeIndex[1]];
            auto & wedge2 = _wedges[face.WedgeIndex[2]];
            bundleIndices.push_back(wedge0.PointIndex);
            bundleIndices.push_back(wedge1.PointIndex);
            bundleIndices.push_back(wedge2.PointIndex);

            bundleUVs.push_back(_wedges[face.WedgeIndex[0]].UVs[0]);
            bundleUVs.push_back(_wedges[face.WedgeIndex[1]].UVs[0]);
            bundleUVs.push_back(_wedges[face.WedgeIndex[2]].UVs[0]);

            AddUnique(bundleWedgeToFaces, face.WedgeIndex[0], (uint32_t)iface);
            AddUnique(bundleWedgeToFaces, face.WedgeIndex[1], (uint32_t)iface);
            AddUnique(bundleWedgeToFaces, face.WedgeIndex[2], (uint32_t)iface);

            Triangle bundleFace = face;
            bundleFace.WedgeIndex[0] = bundleIndices.size() - 3;
            bundleFace.WedgeIndex[1] = bundleIndices.size() - 2;
            bundleFace.WedgeIndex[2] = bundleIndices.size() - 1;
            bundleFaces.push_back(bundleFace);
            bundleSmoothGroupIncices.push_back(face.SmoothingGroups);
        }


        //std::map<int32_t, int32_t> WedgePointToVertexIndexMap;
        std::unordered_map<int32_t, int32_t> WedgePointToVertexIndexMap;
        for (size_t Idx = 0; Idx < bundleIndices.size(); ++Idx)
        {
            WedgePointToVertexIndexMap.insert_or_assign(bundleIndices[Idx], Idx);
        }

        FOverlappingCorners OverlappingVertices(_points, bundleIndices, ComparisonThreshold);

        std::vector<Float3> baseNormals;
        ComputeNormals(_points, bundleIndices, bundleSmoothGroupIncices, bundleUVs, baseNormals, false, true);


        //MeshUtilities.CalculateOverlappingCorners(MeshDataBundle.Vertices, MeshDataBundle.Indices, false, OverlappingVertices);

        auto GatherPointsForMorphTarget = [&](const std::vector<FbxShape *> & shapes, std::vector<Float3> & newPoints, std::set<uint32_t> & ModifiedPoints)
        {
            FbxNode * const RootNode = _meshs[0];

            assert(shapes.size() == _nodes.size());
            for (size_t inode = 0; inode < _meshs.size(); ++inode)
            {
                FbxNode * Node = _meshs[inode];
                FbxMesh * Mesh = Node->GetMesh();
                FbxShape * Shape = shapes[inode];

                FbxGeometryBase * geometry = Shape ? static_cast<FbxGeometryBase *>(Shape) : static_cast<FbxGeometryBase *>(Node->GetMesh());
                FbxAMatrix amatrix = ComputeSkeletalMeshTotalMatrix(Node, RootNode);
                size_t PointIndexBase = newPoints.size();
                importMeshPoints(newPoints, geometry, amatrix);

                if (_useTime0AsRefPose && _bOutDiffPose && !_options.bImportScene)
                {
                    // deform skin vertex to the frame 0 from bind pose
                    skinControlPointsToPose(newPoints, PointIndexBase, Mesh, Shape, true);
                }
            }

            assert(newPoints.size() <= _points.size());
            for (size_t ipoint = 0; ipoint < _points.size(); ++ipoint)
            {
                int32_t OriginalPointIdx = _pointToRawMap[ipoint];
                // 一定小于基础点数量
                assert(OriginalPointIdx < newPoints.size());

                // THRESH_POINTS_ARE_NEAR is too big, we might not be recomputing some normals that can have changed significantly
                if ((newPoints[OriginalPointIdx] - _points[ipoint]).SizeSquared() > FBX_THRESH_VECTORS_ARE_NEAR * FBX_THRESH_VECTORS_ARE_NEAR)
                    ModifiedPoints.insert(ipoint);
            }

            // 补上新的点
            for (size_t ipoint = newPoints.size(); ipoint < _points.size(); ++ipoint)
            {
                int32_t OriginalPointIdx = _pointToRawMap[ipoint];
                newPoints.push_back(newPoints[OriginalPointIdx]);
            }
        };

        auto PrepareTangents = [&](std::vector<Float3> & TangentZ, const std::set<uint32_t> & ModifiedPoints)
        {
            auto FindIncludingNoOverlapping = [&](const FOverlappingCorners & Corners, int32_t Key, std::vector<int32_t> & NoOverlapping)->const std::vector<int32_t> &
            {
                const std::vector<int32_t> & Found = Corners.FindIfOverlapping(Key);
                if (Found.size() > 0)
                {
                    return Found;
                }
                else
                {
                    NoOverlapping.push_back(Key);
                    return NoOverlapping;
                }
            };

            std::vector<bool> WasProcessed(bundleIndices.size(), false);

            std::vector< int32_t > WedgeFaces;
            std::vector< int32_t > OverlappingWedgesDummy;
            std::vector< int32_t > OtherOverlappingWedgesDummy;

            // For each ModifiedPoints, reset the tangents for the affected wedges
            for (int32_t WedgeIdx = 0; WedgeIdx < bundleIndices.size(); ++WedgeIdx)
            {
                int32_t PointIdx = bundleIndices[WedgeIdx];

                if (ModifiedPoints.find(PointIdx) != ModifiedPoints.end())
                {
                    TangentZ[WedgeIdx] = Float3_Zero;

                    const std::vector<int32_t> & OverlappingWedges = FindIncludingNoOverlapping(OverlappingVertices, WedgeIdx, OverlappingWedgesDummy);

                    for (const int32_t OverlappingWedgeIndex : OverlappingWedges)
                    {
                        if (WasProcessed[OverlappingWedgeIndex])
                        {
                            continue;
                        }

                        WasProcessed[OverlappingWedgeIndex] = true;

                        WedgeFaces.clear();
                        auto range_wedgeToFace = bundleWedgeToFaces.equal_range(OverlappingWedgeIndex);
                        for (auto iter_wedgeToFace = range_wedgeToFace.first; iter_wedgeToFace != range_wedgeToFace.second; ++iter_wedgeToFace)
                            WedgeFaces.push_back(iter_wedgeToFace->second);

                        for (const int32_t FaceIndex : WedgeFaces)
                        {
                            for (int32_t CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
                            {
                                int32_t WedgeIndex = bundleFaces[FaceIndex].WedgeIndex[CornerIndex];

                                TangentZ[WedgeIndex] = Float3_Zero;

                                const std::vector<int32_t> & OtherOverlappingWedges = FindIncludingNoOverlapping(OverlappingVertices, WedgeIndex, OtherOverlappingWedgesDummy);

                                for (const int32_t OtherDupVert : OtherOverlappingWedges)
                                {
                                    std::vector< int32_t > OtherWedgeFaces;
                                    auto range_wedgeToFaceOther = bundleWedgeToFaces.equal_range(OtherDupVert);
                                    for (auto iter_wedgeToFaceOther = range_wedgeToFaceOther.first; iter_wedgeToFaceOther != range_wedgeToFaceOther.second; ++iter_wedgeToFaceOther)
                                        OtherWedgeFaces.push_back(iter_wedgeToFaceOther->second);

                                    for (const int32_t OtherFaceIndex : OtherWedgeFaces)
                                    {
                                        for (int32_t OtherCornerIndex = 0; OtherCornerIndex < 3; ++OtherCornerIndex)
                                        {
                                            int32_t OtherWedgeIndex = bundleFaces[OtherFaceIndex].WedgeIndex[OtherCornerIndex];

                                            TangentZ[OtherWedgeIndex] = Float3_Zero;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        };

        auto ComputeMorphDeltas = [&](std::vector<MorphTargetDelta> & Deltas, const std::vector<Float3> & newPoints, const std::vector<Float3> & morphNormals)
        {
            std::vector<bool> WasProcessed(_model.NumVertices, false);

            for (int32_t Idx = 0; Idx < _model.Indicies.size(); ++Idx)
            {
                uint32_t BaseVertIdx = _model.Indicies[Idx];
                // check for duplicate processing
                if (!WasProcessed[BaseVertIdx])
                {
                    // mark this base vertex as already processed
                    WasProcessed[BaseVertIdx] = true;

                    // clothing can add extra verts, and we won't have source point, so we ignore those
                    if (BaseVertIdx < _model.RawPointIndices.size())
                    {
                        // get the base mesh's original wedge point index
                        uint32_t BasePointIdx = _model.RawPointIndices[BaseVertIdx];
                        if (BasePointIdx < _points.size() && BasePointIdx < newPoints.size())
                        {
                            Float3 BasePosition = _points[BasePointIdx];
                            Float3 TargetPosition = newPoints[BasePointIdx];

                            Float3 PositionDelta = TargetPosition - BasePosition;

                            auto iter_PointToVertexIndex = WedgePointToVertexIndexMap.find(BasePointIdx);
                            Float3 NormalDeltaZ = Float3_Zero;

                            if (iter_PointToVertexIndex != WedgePointToVertexIndexMap.end())
                            {
                                Float3 BaseNormal = baseNormals[iter_PointToVertexIndex->second];
                                Float3 TargetNormal = morphNormals[iter_PointToVertexIndex->second];

                                NormalDeltaZ = TargetNormal - BaseNormal;
                            }

                            // check if position actually changed much
                            if (PositionDelta.SizeSquared() > FBX_THRESH_POINTS_ARE_NEAR * FBX_THRESH_POINTS_ARE_NEAR ||
                                // since we can't get imported morphtarget normal from FBX
                                // we can't compare normal unless it's calculated
                                // this is special flag to ignore normal diff
                                ((_options.ShouldImportNormals() == false) && NormalDeltaZ.SizeSquared() > 0.01f))
                            {
                                // create a new entry
                                MorphTargetDelta NewVertex;
                                // position delta
                                NewVertex.PositionDelta = PositionDelta;
                                // normal delta
                                NewVertex.TangentZDelta = NormalDeltaZ;
                                // index of base mesh vert this entry is to modify
                                NewVertex.SourceIdx = BaseVertIdx;

                                // add it to the list of changed verts
                                Deltas.emplace_back(NewVertex);
                            }
                        }
                    }
                }
            }
        };

        std::vector<std::future<fbx::error_e>> futures;
        for (auto iter_shapes = ShapeNameToShapeArray.begin(); iter_shapes != ShapeNameToShapeArray.end(); ++iter_shapes)
            _morphDeltas.insert({ iter_shapes->first, std::vector<MorphTargetDelta>() });

        for (auto iter_shapes = ShapeNameToShapeArray.begin(); iter_shapes != ShapeNameToShapeArray.end(); ++iter_shapes)
        {
            const std::vector<FbxShape *> & shapes = iter_shapes->second;

            //std::future<fbx::error_e> future = std::async([&, &morphDelta = _morphDeltas[iter_shapes->first]]()
            //{
            //    std::vector<Float3> morphPoints;
            //    std::set<uint32_t> modifiedPoints;

            //    GatherPointsForMorphTarget(shapes, morphPoints, modifiedPoints);
            //    assert(morphPoints.size() == _points.size());

            //    std::vector<Float3> morphNormals = baseNormals;
            //    PrepareTangents(morphNormals, modifiedPoints);
            //    ComputeNormals(morphPoints, bundleIndices, bundleSmoothGroupIncices, bundleUVs, morphNormals, false, true);

            //    //std::vector<MorphTargetDelta> morphDelta;
            //    ComputeMorphDeltas(morphDelta, morphPoints, morphNormals);
            //    //_morphDeltas.insert_or_assign(name, morphDelta);
            //    return fbx::error_ok;
            //});

            //futures.push_back(std::move(future));

            std::vector<Float3> morphPoints;
            std::set<uint32_t> modifiedPoints;
            GatherPointsForMorphTarget(shapes, morphPoints, modifiedPoints);
            assert(morphPoints.size() == _points.size());

            std::vector<Float3> morphNormals = baseNormals;
            PrepareTangents(morphNormals, modifiedPoints);
            ComputeNormals(morphPoints, bundleIndices, bundleSmoothGroupIncices, bundleUVs, morphNormals, false, true);

            ComputeMorphDeltas(_morphDeltas[iter_shapes->first], morphPoints, morphNormals);
        }

        for (auto & future : futures)
            future.get();
        return fbx::error_ok;
    }
}
