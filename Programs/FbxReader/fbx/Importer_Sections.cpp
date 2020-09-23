#include "Importer.h"

namespace fbx
{
    fbx::error_e Importer::BuildSections()
    {
		std::vector<Vertex> WedgeVertices;
        std::vector<int32_t> DupVerts;
        std::multimap<int32_t, int32_t> RawVerts2Dupes;
        {
            std::vector<IndexAndZ> VertIndexAndZ;

            // Find wedge influences.
            std::vector<int32_t>	WedgeInfluenceIndices;
            std::map<uint32_t, uint32_t> VertexIndexToInfluenceIndexMap;

            for (uint32_t LookIdx = 0; LookIdx < _influences.size(); LookIdx++)
            {
                // Order matters do not allow the map to overwrite an existing value.
                auto iter = VertexIndexToInfluenceIndexMap.find(_influences[LookIdx].PointIndex);
                if (iter == VertexIndexToInfluenceIndexMap.end())
                {
                    VertexIndexToInfluenceIndexMap[_influences[LookIdx].PointIndex] = LookIdx;
                }
            }

            for (uint32_t WedgeIndex = 0; WedgeIndex < _wedges.size(); WedgeIndex++)
            {
                auto iter = VertexIndexToInfluenceIndexMap.find(_wedges[WedgeIndex].PointIndex);
                if (iter != VertexIndexToInfluenceIndexMap.end())
                {
                    WedgeInfluenceIndices.push_back(iter->second);
                }
                else
                {
                    // we have missing influence vert,  we weight to root
                    WedgeInfluenceIndices.push_back(0);
                    //fbx::war() << fbx::logger_once(__FILE__, __LINE__) << " Missing influence on vert";
                }
            }

        	// 注意:按照材质而非Mesh来划分Section
            for (int32_t FaceIndex = 0; FaceIndex < _faces.size(); FaceIndex++)
            {
                const Triangle & face = _faces[FaceIndex];
                size_t sectionIndex = FBX_INDEX_NONE;
                for (size_t isection = 0; isection < _sections.size(); ++isection)
                {
                    if (_sections[isection].MaterialIndex == face.MatIndex)
                    {
                        sectionIndex = isection;
                        break;
                    }
                }

                if (sectionIndex == FBX_INDEX_NONE)
                {
                    _sections.push_back({});
                    sectionIndex = _sections.size() - 1;
                    _sections.back().MaterialIndex = face.MatIndex;
                    _sections.back().OriginalSectionIndex = face.MatIndex;
                }

                const auto packetNormal = [](const Float3 & InVector)
                {
                    static const int32_t MIN_INT8 = -128;
                    static const int32_t MAX_INT8 = 127;
                    static const float SCALE = MAX_INT8;
                    Float3 ret;
                    //N11.X = fbx::clamp((int)std::roundf(N1.X * 127), -128, 127) / 127.0;
                    //N11.Y = fbx::clamp((int)std::roundf(N1.Y * 127), -128, 127) / 127.0;
                    //N11.Z = fbx::clamp((int)std::roundf(N1.Z * 127), -128, 127) / 127.0;
                    ret.X = fbx::clamp((int32_t)std::roundf(InVector.X * SCALE), MIN_INT8, MAX_INT8) / SCALE;
                    ret.Y = fbx::clamp((int32_t)std::roundf(InVector.Y * SCALE), MIN_INT8, MAX_INT8) / SCALE;
                    ret.Z = fbx::clamp((int32_t)std::roundf(InVector.Z * SCALE), MIN_INT8, MAX_INT8) / SCALE;
                    return ret;
                };

                Section & section = _sections[sectionIndex];
                for (size_t iedge = 0; iedge < 3; ++iedge)
                {
                    int32_t wedgeIndex = face.WedgeIndex[iedge];
                    const Wedge & wedge = _wedges[face.WedgeIndex[iedge]];
                    Vertex vertex;
                    vertex.Position = _points[wedge.PointIndex];
                    vertex.TangentX = _tangentsX[wedgeIndex].GetSafeNormal();
                    vertex.TangentY = _tangentsY[wedgeIndex].GetSafeNormal();
                    vertex.TangentZ = _tangentsZ[wedgeIndex].GetSafeNormal();
                    vertex.TangentX.normalize();
                    vertex.TangentY.normalize();
                    vertex.TangentZ.normalize();

                    // 为了和 UE 保持一致，这里先不转换 Importer::AreSkelMeshVerticesEqual 特殊处理
                    //vertex.TangentX = packetNormal(vertex.TangentX);
                    //vertex.TangentY = packetNormal(vertex.TangentY);
                    //vertex.TangentZ = packetNormal(vertex.TangentZ);

                    std::memcpy(vertex.UVs, wedge.UVs, sizeof(Float2) * FBX_MAX_TEXCOORDS);
                    vertex.Color = wedge.Color;

                    // Count the influences.
                    int32_t InfIdx = WedgeInfluenceIndices[face.WedgeIndex[iedge]];
                    int32_t LookIdx = InfIdx;

                    uint32_t InfluenceCount = 0;
                    while (LookIdx < _influences.size() && (_influences[LookIdx].PointIndex == wedge.PointIndex))
                    {
                        InfluenceCount++;
                        LookIdx++;
                    }
                    InfluenceCount = std::min<uint32_t>(InfluenceCount, FBX_MAX_TOTAL_INFLUENCES);

                    // Setup the vertex influences.
                    vertex.InfluenceBones[0] = 0;
                    vertex.InfluenceWeights[0] = 255;
                    for (uint32_t i = 1; i < FBX_MAX_TOTAL_INFLUENCES; i++)
                    {
                        vertex.InfluenceBones[i] = 0;
                        vertex.InfluenceWeights[i] = 0;
                    }

					uint32_t TotalInfluenceWeight = 0;
                    for (uint32_t iInfluence = 0; iInfluence < InfluenceCount; iInfluence++)
                    {
                        uint16_t BoneIndex = _influences[InfIdx + iInfluence].BoneIndex;
                        if (BoneIndex >= _bones.size())
                        {
							log_war("invalid bone index");
                            continue;
                        }

                        vertex.InfluenceBones[iInfluence] = BoneIndex;
                        vertex.InfluenceWeights[iInfluence] = (uint8_t)(_influences[InfIdx + iInfluence].Weight * 255.0f);
                        TotalInfluenceWeight += vertex.InfluenceWeights[iInfluence];
                    }
                    vertex.InfluenceWeights[0] += 255 - TotalInfluenceWeight;

                    // Add the vertex as well as its original index in the points array
                    vertex.PointWedgeIdx = wedge.PointIndex;

                    WedgeVertices.push_back(vertex);
                    // Add an efficient way to find dupes of this vertex later for fast combining of vertices
                    IndexAndZ IAndZ;
                    IAndZ.Index = WedgeVertices.size() - 1;
                    IAndZ.Z = vertex.Position.Z;

                    VertIndexAndZ.push_back(IAndZ);
                }
            }

            std::sort(VertIndexAndZ.begin(), VertIndexAndZ.end());
            //std::stable_sort(VertIndexAndZ.begin(), VertIndexAndZ.end(), FCompareIndexAndZ());

            // Search for duplicates, quickly!
            for (int32_t i = 0; i < VertIndexAndZ.size(); i++)
            {
                // only need to search forward, since we add pairs both ways
                for (int32_t j = i + 1; j < VertIndexAndZ.size(); j++)
                {
                    if (std::abs(VertIndexAndZ[j].Z - VertIndexAndZ[i].Z) > _options.OverlappingThresholds.ThresholdPosition)
                    {
                        // our list is sorted, so there can't be any more dupes
                        break;
                    }

                    // check to see if the points are really overlapping
                    if (PointsEqual(
                        WedgeVertices[VertIndexAndZ[i].Index].Position,
                        WedgeVertices[VertIndexAndZ[j].Index].Position, _options.OverlappingThresholds))
                    {
                        RawVerts2Dupes.insert({ VertIndexAndZ[i].Index, VertIndexAndZ[j].Index });
                        RawVerts2Dupes.insert({ VertIndexAndZ[j].Index, VertIndexAndZ[i].Index });
                    }
                }
            }
        }


        std::map<Section *, std::map<int32_t, int32_t> > ChunkToFinalVerts;
        uint32_t TriangleIndices[3];
        for (int32_t FaceIndex = 0; FaceIndex < _faces.size(); FaceIndex++)
        {
            const Triangle & Face = _faces[FaceIndex];

            // Find a chunk which matches this triangle.
            size_t sectionIndex = -1;
            for (size_t isection = 0; isection < _sections.size(); ++isection)
            {
                if (_sections[isection].MaterialIndex == Face.MatIndex)
                {
                    sectionIndex = isection;
                    break;
                }
            }

            if (sectionIndex == -1)
            {
                _sections.push_back({});
                sectionIndex = _sections.size() - 1;
            }
            Section & section = _sections[sectionIndex];

            auto iter_finalverts = ChunkToFinalVerts.find(&section);
            if (iter_finalverts == ChunkToFinalVerts.end())
            {
                ChunkToFinalVerts.insert({ &section, {} });
            }
            std::map<int32_t, int32_t> & FinalVerts = ChunkToFinalVerts[&section];

            for (int32_t VertexIndex = 0; VertexIndex < 3; ++VertexIndex)
            {
                int32_t WedgeIndex = FaceIndex * 3 + VertexIndex;
                const Vertex & Vertex = WedgeVertices[WedgeIndex];

                int32_t FinalVertIndex = FBX_INDEX_NONE;
                DupVerts.clear();
                auto range = RawVerts2Dupes.equal_range(WedgeIndex);
                //std::copy(range.first, range.second, std::back_inserter(DupVerts));
                for (auto i = range.first; i != range.second; ++i)
                    DupVerts.push_back(i->second);

                std::sort(DupVerts.begin(), DupVerts.end());


                for (int32_t k = 0; k < DupVerts.size(); k++)
                {
                    if (DupVerts[k] >= WedgeIndex)
                    {
                        // the verts beyond me haven't been placed yet, so these duplicates are not relevant
                        break;
                    }

                    auto iter_vert = FinalVerts.find(DupVerts[k]);
                    if (iter_vert != FinalVerts.end())
                    {
                        // TODO
                        if (AreSkelMeshVerticesEqual(Vertex, section.Vertices[iter_vert->second], _options.OverlappingThresholds))
                        {
                            FinalVertIndex = iter_vert->second;
                            break;
                        }
                    }
                }
                if (FinalVertIndex == FBX_INDEX_NONE)
                {
                    section.Vertices.push_back(Vertex);
                    FinalVertIndex = section.Vertices.size() - 1;
                    FinalVerts[WedgeIndex] = FinalVertIndex;
                }

                // set the index entry for the newly added vertex
                // TArray internally has int32_t for capacity, so no need to test for uint32 as it's larger than int32_t
                TriangleIndices[VertexIndex] = (uint32_t)FinalVertIndex;
            }

            if (TriangleIndices[0] != TriangleIndices[1] && TriangleIndices[0] != TriangleIndices[2] && TriangleIndices[1] != TriangleIndices[2])
            {
                for (uint32_t VertexIndex = 0; VertexIndex < 3; VertexIndex++)
                {
                    section.Indices.push_back(TriangleIndices[VertexIndex]);
                }
            }
        }

        // ChunkSkinnedVertices
        {
            int MaxBonesPerChunk = FBX_MAX_GPU_SKIN_BONES;
            // Copy over the old chunks (this is just copying pointers).
            std::vector<Section> SrcChunks;
            std::swap(_sections, SrcChunks);

            // Sort the chunks by material index.
            struct FCompareSkinnedMeshChunk
            {
                bool operator()(const Section & A, const Section & B) const
                {
                    return A.MaterialIndex < B.MaterialIndex;
                }
            };
            std::sort(SrcChunks.begin(), SrcChunks.end(), FCompareSkinnedMeshChunk());

            // Now split chunks to respect the desired bone limit.
            std::vector<std::vector<int32_t> > IndexMaps;
            //TArray<FBoneIndexType, TInlineAllocator<FBX_MAX_TOTAL_INFLUENCES * 3> > UniqueBones;
            std::vector<uint16_t> UniqueBones;
            for (int32_t SrcChunkIndex = 0; SrcChunkIndex < SrcChunks.size(); ++SrcChunkIndex)
            {
                Section * SrcChunk = &SrcChunks[SrcChunkIndex];
                int32_t FirstChunkIndex = _sections.size();

                for (size_t i = 0; i < SrcChunk->Indices.size(); i += 3)
                {
                    // Find all bones needed by this triangle.
                    UniqueBones.clear();
                    for (int32_t Corner = 0; Corner < 3; Corner++)
                    {
                        int32_t VertexIndex = SrcChunk->Indices[i + Corner];
                        Vertex & V = SrcChunk->Vertices[VertexIndex];
                        for (int32_t InfluenceIndex = 0; InfluenceIndex < FBX_MAX_TOTAL_INFLUENCES; InfluenceIndex++)
                        {
                            if (V.InfluenceWeights[InfluenceIndex] > 0)
                            {
                                AddUnique(UniqueBones, V.InfluenceBones[InfluenceIndex]);
                            }
                        }
                    }

                    // Now find a chunk for them.
                    Section * DestChunk = NULL;
                    int32_t DestChunkIndex = FirstChunkIndex;
                    for (; DestChunkIndex < _sections.size(); ++DestChunkIndex)
                    {
                        std::vector<uint16_t> & BoneMap = _sections[DestChunkIndex].BoneMap;
                        int32_t NumUniqueBones = 0;
                        for (int32_t j = 0; j < UniqueBones.size(); ++j)
                        {
                            NumUniqueBones += Contains(BoneMap, UniqueBones[j]) ? 0 : 1;
                        }
                        if (NumUniqueBones + BoneMap.size() <= MaxBonesPerChunk)
                        {
                            DestChunk = &_sections[DestChunkIndex];
                            break;
                        }
                    }

                    // If no chunk was found, create one!
                    if (DestChunk == NULL)
                    {
                        _sections.push_back({});
                        DestChunk = &_sections.back();
                        DestChunk->MaterialIndex = SrcChunk->MaterialIndex;
                        DestChunk->OriginalSectionIndex = SrcChunk->OriginalSectionIndex;
                        IndexMaps.push_back({});
                        std::vector<int32_t> & IndexMap = IndexMaps.back();
                        IndexMap.resize(SrcChunk->Vertices.size());
                        std::memset(IndexMap.data(), 0xff, sizeof(int32_t) * IndexMap.size());
                    }
                    std::vector<int32_t> & IndexMap = IndexMaps[DestChunkIndex];

                    // Add the unique bones to this chunk's bone map.
                    for (int32_t j = 0; j < UniqueBones.size(); ++j)
                    {
                        AddUnique(DestChunk->BoneMap, UniqueBones[j]);
                    }

                    // For each vertex, add it to the chunk's arrays of vertices and indices.
                    for (int32_t Corner = 0; Corner < 3; Corner++)
                    {
                        int32_t VertexIndex = SrcChunk->Indices[i + Corner];
                        int32_t DestIndex = IndexMap[VertexIndex];
                        if (DestIndex == FBX_INDEX_NONE)
                        {
							const Vertex & OldVertex = SrcChunk->Vertices[VertexIndex];
							Vertex NewVertex = OldVertex;

                            for (int32_t InfluenceIndex = 0; InfluenceIndex < FBX_MAX_TOTAL_INFLUENCES; InfluenceIndex++)
                            {
                                if (OldVertex.InfluenceWeights[InfluenceIndex] > 0)
                                {
                                    auto iter = std::find(DestChunk->BoneMap.begin(), DestChunk->BoneMap.end(), OldVertex.InfluenceBones[InfluenceIndex]);
                                    assert(iter != DestChunk->BoneMap.end());
									NewVertex.InfluenceBones[InfluenceIndex] = uint8_t(iter - DestChunk->BoneMap.begin());
									NewVertex.InfluenceWeights[InfluenceIndex] = OldVertex.InfluenceWeights[InfluenceIndex];
                                }
								else
								{
									NewVertex.InfluenceBones[InfluenceIndex] = 0;
									NewVertex.InfluenceWeights[InfluenceIndex] = 0.0f;
								}
                            }

							IndexMap[VertexIndex] = DestChunk->Vertices.size();
							DestChunk->Vertices.push_back(NewVertex);
                        }
                        DestChunk->Indices.push_back(IndexMap[VertexIndex]);
                    }
                }
            }
        }
        return fbx::error_ok;
    }
}
