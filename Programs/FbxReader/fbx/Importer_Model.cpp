#include "Importer.h"
#include "FbxModel.h"
#include "ForsythTriOO/forsythtriangleorderoptimizer.h"

namespace fbx
{
    void CacheOptimizeIndexBuffer(std::vector<uint32_t> & Indices)
    {
        // Count the number of vertices
        uint32_t NumVertices = 0;
        for (size_t Index = 0; Index < Indices.size(); ++Index)
        {
            if (Indices[Index] > NumVertices)
            {
                NumVertices = Indices[Index];
            }
        }
        NumVertices += 1;

        bool Is32Bit = true;
        std::vector<uint32_t> OptimizedIndices(Indices.size());
        uint16_t CacheSize = 32;
        {
            if (Is32Bit)
            {
                Forsyth::OptimizeFaces(Indices.data(), Indices.size(), NumVertices, OptimizedIndices.data(), CacheSize);
            }
            else
            {
                // convert to 32 bit
                std::vector<uint32_t> NewIndices(Indices.size());
                for (uint32_t Idx = 0; Idx < Indices.size(); ++Idx)
                {
                    NewIndices[Idx] = ((uint16_t *)Indices.data())[Idx];
                }
                Forsyth::OptimizeFaces(NewIndices.data(), Indices.size(), NumVertices, OptimizedIndices.data(), CacheSize);
            }
        }

        if (Is32Bit)
        {
            std::memcpy(Indices.data(), OptimizedIndices.data(), Indices.size() * sizeof(uint32_t));
        }
        else
        {
            for (size_t I = 0; I < OptimizedIndices.size(); ++I)
            {
                Indices[I] = (uint16_t)OptimizedIndices[I];
            }
        }
    }

    fbx::error_e Importer::BuildModel()
    {
        // Setup the section and chunk arrays on the model.
        std::vector<bool> boneExists(_bones.size(), false);
        for (size_t ChunkIndex = 0; ChunkIndex < _sections.size(); ++ChunkIndex)
        {
            const Section & src = _sections[ChunkIndex];
            _model.Sections.push_back({});
			ModelData::Section & Section = _model.Sections.back();

            Section.MaterialIndex = src.MaterialIndex;
            Section.BoneMap = src.BoneMap;

            // Update the active bone indices on the LOD model.
            for (int32_t ibone = 0; ibone < src.BoneMap.size(); ++ibone)
            {
                int32_t boneIndex = src.BoneMap[ibone];
                assert(boneIndex < boneExists.size());
                boneExists[boneIndex] = true;
                while (boneIndex > 0)
                {
                    boneIndex = _bones[boneIndex].ParentIndex;
                    assert(boneIndex < boneExists.size());
                    boneExists[boneIndex] = true;
                }
            }
        }

        for (size_t ibone = 0; ibone < boneExists.size(); ++ibone)
        {
            if (boneExists[ibone])
                _model.ActiveBoneIndices.push_back(ibone);
        }

        _model.MaxImportVertex = 0;
        _model.NumVertices = 0;
        _model.NumTexCoords = std::max(1u, _nTexCoords);

        std::vector<std::vector<uint32_t>> VertexIndexRemap(_model.Sections.size());
        std::vector<uint32_t> RawPointIndices;

        int32_t PrevMaterialIndex = -1;
        int32_t CurrentChunkBaseVertexIndex = -1; 	// base vertex index for all chunks of the same material
        int32_t CurrentChunkVertexCount = -1; 		// total vertex count for all chunks of the same material
        int32_t CurrentVertexIndex = 0; 			// current vertex index added to the index buffer for all chunks of the same material
        for (size_t SectionIndex = 0; SectionIndex < _model.Sections.size(); SectionIndex++)
        {
            Section  & SrcChunk = _sections[SectionIndex];
			ModelData::Section & Section = _model.Sections[SectionIndex];
            std::vector<Vertex> & ChunkVertices = SrcChunk.Vertices;
            std::vector<uint32_t> & ChunkIndices = SrcChunk.Indices;

            // Reorder the section index buffer for better vertex cache efficiency.
            CacheOptimizeIndexBuffer(ChunkIndices);

            // Calculate the number of triangles in the section.  Note that CacheOptimize may change the number of triangles in the index buffer!
            Section.NumTriangles = ChunkIndices.size() / 3;
            std::vector<Vertex> OriginalVertices;
            std::swap(ChunkVertices, OriginalVertices);
            ChunkVertices.resize(OriginalVertices.size());

            std::vector<int32_t> IndexCache(ChunkVertices.size());
            std::memset(IndexCache.data(), FBX_INDEX_NONE, IndexCache.size() * sizeof(int32_t));
            int32_t NextAvailableIndex = 0;
            // Go through the indices and assign them new values that are coherent where possible
            for (int32_t Index = 0; Index < ChunkIndices.size(); Index++)
            {
                const int32_t OriginalIndex = ChunkIndices[Index];
                const int32_t CachedIndex = IndexCache[OriginalIndex];

                if (CachedIndex == FBX_INDEX_NONE)
                {
                    // No new index has been allocated for this existing index, assign a new one
                    ChunkIndices[Index] = NextAvailableIndex;
                    // Mark what this index has been assigned to
                    IndexCache[OriginalIndex] = NextAvailableIndex;
                    NextAvailableIndex++;
                }
                else
                {
                    // Reuse an existing index assignment
                    ChunkIndices[Index] = CachedIndex;
                }
                // Reorder the vertices based on the new index assignment
                ChunkVertices[ChunkIndices[Index]] = OriginalVertices[OriginalIndex];
            }
        }

        // Build the arrays of rigid and soft vertices on the model's chunks.
        for (size_t SectionIndex = 0; SectionIndex < _model.Sections.size(); SectionIndex++)
        {
			ModelData::Section & Section = _model.Sections[SectionIndex];
            std::vector<Vertex> & ChunkVertices = _sections[SectionIndex].Vertices;

            CurrentVertexIndex = 0;
            CurrentChunkVertexCount = 0;
            PrevMaterialIndex = Section.MaterialIndex;

            // Calculate the offset to this chunk's vertices in the vertex buffer.
            Section.BaseVertexIndex = CurrentChunkBaseVertexIndex = _model.NumVertices;

            // Update the size of the vertex buffer.
            _model.NumVertices += ChunkVertices.size();

            std::vector<uint32_t> & ChunkVertexIndexRemap = VertexIndexRemap[SectionIndex];
            // Separate the section's vertices into rigid and soft vertices.
            ChunkVertexIndexRemap.resize(ChunkVertices.size());

            for (size_t VertexIndex = 0; VertexIndex < ChunkVertices.size(); VertexIndex++)
            {
                const Vertex & SoftVertex = ChunkVertices[VertexIndex];

				ModelData::Vertex NewVertex;
                NewVertex.Position = SoftVertex.Position;
                NewVertex.TangentX = SoftVertex.TangentX;
                NewVertex.TangentY = SoftVertex.TangentY;
                NewVertex.TangentZ = SoftVertex.TangentZ;
                std::memcpy(NewVertex.UVs, SoftVertex.UVs, sizeof(Float2) * FBX_MAX_TEXCOORDS);
                NewVertex.Color = SoftVertex.Color;
                for (int32_t InfluenceIndex = 0; InfluenceIndex < FBX_MAX_TOTAL_INFLUENCES; ++InfluenceIndex)
                {
                    // it only adds to the bone map if it has weight on it
                    // BoneMap contains only the bones that has influence with weight of >0.f
                    // so here, just make sure it is included before setting the data
                    if (SoftVertex.InfluenceBones[InfluenceIndex] < Section.BoneMap.size())
                    {
                        NewVertex.InfluenceBones[InfluenceIndex] = SoftVertex.InfluenceBones[InfluenceIndex];
                        NewVertex.InfluenceWeights[InfluenceIndex] = SoftVertex.InfluenceWeights[InfluenceIndex];
                    }
					else
					{
						NewVertex.InfluenceBones[InfluenceIndex] = 0;
						NewVertex.InfluenceWeights[InfluenceIndex] = 0.0f;
					}
                }
                NewVertex.PointWedgeIdx = SoftVertex.PointWedgeIdx;
                _model.Vertices.push_back(NewVertex);

                ChunkVertexIndexRemap[VertexIndex] = (uint32_t)(Section.BaseVertexIndex + CurrentVertexIndex);
                CurrentVertexIndex++;
                // add the index to the original wedge point source of this vertex
                RawPointIndices.push_back(SoftVertex.PointWedgeIdx);
                // Also remember import index
                const int32_t RawVertIndex = _pointToRawMap[SoftVertex.PointWedgeIdx];
                _model.MeshToImportVertexMap.push_back(RawVertIndex);
                _model.MaxImportVertex = std::max<float>(_model.MaxImportVertex, RawVertIndex);
            }

            // update NumVertices
            Section.NumVertices = ChunkVertices.size();

            // update max bone influences
            //Section.CalcMaxBoneInfluences();
            if (_bones.empty())
            {
                Section.MaxBoneInfluences = 0;
            }
            else
            {
                // if we only have rigid verts then there is only one bone
                Section.MaxBoneInfluences = 1;
                // iterate over all the soft vertices for this chunk and find max # of bones used
                for (int32_t VertIdx = 0; VertIdx < Section.NumVertices; VertIdx++)
                {
					ModelData::Vertex & SoftVert = _model.Vertices[Section.BaseVertexIndex + VertIdx];

                    // calc # of bones used by this soft skinned vertex
                    int32_t BonesUsed = 0;
                    for (int32_t InfluenceIdx = 0; InfluenceIdx < FBX_MAX_TOTAL_INFLUENCES; InfluenceIdx++)
                    {
                        if (SoftVert.InfluenceWeights[InfluenceIdx] > 0)
                        {
                            BonesUsed++;
                        }
                    }
                    // reorder bones so that there aren't any unused influence entries within the [0,BonesUsed] range
                    for (int32_t InfluenceIdx = 0; InfluenceIdx < BonesUsed; InfluenceIdx++)
                    {
                        if (SoftVert.InfluenceWeights[InfluenceIdx] == 0)
                        {
                            for (int32_t ExchangeIdx = InfluenceIdx + 1; ExchangeIdx < FBX_MAX_TOTAL_INFLUENCES; ExchangeIdx++)
                            {
                                if (SoftVert.InfluenceWeights[ExchangeIdx] != 0)
                                {
                                    std::swap(SoftVert.InfluenceWeights[InfluenceIdx], SoftVert.InfluenceWeights[ExchangeIdx]);
                                    std::swap(SoftVert.InfluenceBones[InfluenceIdx], SoftVert.InfluenceBones[ExchangeIdx]);
                                    break;
                                }
                            }
                        }
                    }

                    // maintain max bones used
                    Section.MaxBoneInfluences = std::max(Section.MaxBoneInfluences, BonesUsed);
                }
            }

            // Log info about the chunk.
            //UE_LOG(LogSkeletalMesh, Log, TEXT("Section %u: %u vertices, %u active bones"),
            //    SectionIndex,
            //    Section.GetNumVertices(),
            //    Section.BoneMap.Num()
            //);
        }

        _model.RawPointIndices = RawPointIndices;
        // Finish building the sections.
        for (int32_t SectionIndex = 0; SectionIndex < _model.Sections.size(); SectionIndex++)
        {
			ModelData::Section & Section = _model.Sections[SectionIndex];

            const std::vector<uint32_t> & SectionIndices = _sections[SectionIndex].Indices;

            Section.BaseIndex = _model.Indicies.size();
            const int32_t NumIndices = SectionIndices.size();
            const std::vector<uint32_t> & SectionVertexIndexRemap = VertexIndexRemap[SectionIndex];
            for (int32_t Index = 0; Index < NumIndices; Index++)
            {
                uint32_t VertexIndex = SectionVertexIndexRemap[SectionIndices[Index]];
                _model.Indicies.push_back(VertexIndex);
            }
        }


        // RequiredBones for base model includes all raw bones.
        for (size_t ibone = 0; ibone < boneExists.size(); ++ibone)
        {
            // È«¶¼Òª£¡
            _model.RequiredBones.push_back(ibone);
        }

        for (int32_t SectionIndex = 0; SectionIndex < _model.Sections.size(); SectionIndex++)
        {
			ModelData::Section & Section = _model.Sections[SectionIndex];
            if (Section.MaxBoneInfluences > FBX_MAX_INFLUENCES_PER_STREAM)
            {
                _model.bExtraBoneInfluences = true;
                break;
            }
        }
        return fbx::error_ok;
    }
}
