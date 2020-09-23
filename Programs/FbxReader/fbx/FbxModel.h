#pragma once

#include "Fbx.h"

namespace fbx
{
	struct MeshToMeshVertData
	{
		// Barycentric coords and distance along normal for the position of the final vert
		Float4 PositionBaryCoordsAndDist;

		// Barycentric coords and distance along normal for the location of the unit normal endpoint
		// Actual normal = ResolvedNormalPosition - ResolvedPosition
		Float4 NormalBaryCoordsAndDist;

		// Barycentric coords and distance along normal for the location of the unit Tangent endpoint
		// Actual normal = ResolvedNormalPosition - ResolvedPosition
		Float4 TangentBaryCoordsAndDist;

		// Contains the 3 indices for verts in the source mesh forming a triangle, the last element
		// is a flag to decide how the skinning works, 0xffff uses no simulation, and just normal
		// skinning, anything else uses the source mesh and the above skin data to get the final position
		uint16_t SourceMeshVertIndices[4];

		// Dummy for alignment (16 bytes)
		uint32_t Padding[2];
	};

	struct ClothingSectionData
	{
		ClothingSectionData()
			: AssetGuid()
			, AssetLodIndex(FBX_INDEX_NONE) {}

		bool IsValid()
		{
			return !AssetGuid.empty() && AssetLodIndex != FBX_INDEX_NONE;
		}

		/** Guid of the clothing asset applied to this section */
		std::string AssetGuid;

		/** LOD inside the applied asset that is used */
		int32_t AssetLodIndex;
	};

	struct ModelData
	{
		struct Vertex
		{
			Float3 Position;
			Float3 TangentX; // Tangent, U-direction
			Float3 TangentY; // Binormal, V-direction
			Float3 TangentZ; // Normal

			//Int4 TangentX; // Tangent, U-direction
			//Int4 TangentY; // Binormal, V-direction
			//Int4 TangentZ; // Normal
			Float2 UVs[FBX_MAX_TEXCOORDS]; // UVs
			Color32 Color; // VertexColor

			//  这里可能超过 255，不能用 uint8_t
			uint8_t InfluenceBones[FBX_MAX_TOTAL_INFLUENCES];
			uint8_t InfluenceWeights[FBX_MAX_TOTAL_INFLUENCES];
			uint32_t PointWedgeIdx = 0;
		};

		struct Section
		{
			/** Material (texture) used for this section. */
			uint16_t MaterialIndex = 0;

			/** The offset of this section's indices in the LOD's index buffer. */
			uint32_t BaseIndex = 0;

			/** The number of triangles in this section. */
			uint32_t NumTriangles = 0;

			/** This section will recompute tangent in runtime */
			bool bRecomputeTangent = false;

			/** This section will cast shadow */
			bool bCastShadow = true;

			/** The offset into the LOD's vertex buffer of this section's vertices. */
			uint32_t BaseVertexIndex = 0;

			/** The extra vertex data for mapping to an APEX clothing simulation mesh. */
			std::vector<MeshToMeshVertData> ClothMappingData;

			/** The bones which are used by the vertices of this section. Indices of bones in the USkeletalMesh::RefSkeleton array */
			std::vector<uint16_t> BoneMap;

			/** Number of vertices in this section (size of SoftVertices array). Available in non-editor builds. */
			int32_t NumVertices = 0;

			/** max # of bones used to skin the vertices in this section */
			int32_t MaxBoneInfluences = 4;

			// FBX_INDEX_NONE if not set
			int16_t CorrespondClothAssetIndex = FBX_INDEX_NONE;

			/** Clothing data for this section, clothing is only present if ClothingData.IsValid() returns true */
			ClothingSectionData ClothingData;

			/** Map between a vertex index and all vertices that share the same position **/
			std::map<int32_t, std::vector<int32_t>> OverlappingVertices;

			/** If disabled, we won't render this section */
			bool bDisabled = false;

			bool HasExtraBoneInfluences() const
			{
				return MaxBoneInfluences > FBX_MAX_INFLUENCES_PER_STREAM;
			}
		};

		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indicies;
		std::vector<Section> Sections;

		uint32_t NumVertices = 0;
		uint32_t NumTexCoords = 0;
		std::vector<uint16_t> ActiveBoneIndices;
		std::vector<uint16_t> RequiredBones;

		/** Mapping from final mesh vertex index to raw import vertex index. Needed for vertex animation, which only stores positions for import verts. */
		std::vector<int32_t> MeshToImportVertexMap;
		/** The max index in MeshToImportVertexMap, ie. the number of imported (raw) verts. */
		int32_t MaxImportVertex = -1;

		bool DoSectionsNeedExtraBoneInfluences() const;

		std::vector<uint32_t> RawPointIndices;

		bool bExtraBoneInfluences = false;
	};

	struct MorphTargetDelta
	{
		/** change in position */
		Float3 PositionDelta;

		/** Tangent basis normal */
		Float3 TangentZDelta;

		/** index of source vertex to apply deltas to */
		uint32_t SourceIdx;
	};
}
