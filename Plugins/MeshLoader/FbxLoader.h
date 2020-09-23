#pragma once

#include "MeshLoaderInc.h"

#include <fbxsdk.h>

#define ENUM_STRING_BEGIN(Value) switch(Value) {
#define ENUM_STRING_CASE(Enum, Name) case Enum::Name: return Str( #Name );
#define ENUM_STRING_END default: return {}; }

namespace XE
{
	template<>
	class TFormatter<FbxLayerElementUV::EMappingMode>
	{
	public:
		FStringView Format(FbxLayerElementUV::EMappingMode MappingMode, FStringView) const
		{
			ENUM_STRING_BEGIN(MappingMode)
				ENUM_STRING_CASE(FbxLayerElementUV, eNone)
				ENUM_STRING_CASE(FbxLayerElementUV, eByControlPoint)
				ENUM_STRING_CASE(FbxLayerElementUV, eByPolygonVertex)
				ENUM_STRING_CASE(FbxLayerElementUV, eByPolygon)
				ENUM_STRING_CASE(FbxLayerElementUV, eByEdge)
				ENUM_STRING_CASE(FbxLayerElementUV, eAllSame)
				ENUM_STRING_END
		}
	};

	template<>
	class TFormatter<FbxLayerElementUV::EReferenceMode>
	{
	public:
		FStringView Format(FbxLayerElementUV::EReferenceMode ReferenceMode, FStringView) const
		{
			ENUM_STRING_BEGIN(ReferenceMode)
				ENUM_STRING_CASE(FbxLayerElementUV, eDirect)
				ENUM_STRING_CASE(FbxLayerElementUV, eIndex)
				ENUM_STRING_CASE(FbxLayerElementUV, eIndexToDirect)
				ENUM_STRING_END
		}
	};
	
	template<typename T>
	void TFbxObjectDeleter(T * ptr)
	{
		if (ptr)
			ptr->Destroy();
	}
	
	struct FFbxVersion
	{
		int Major = 0;
		int Minor = 0;
		int Revision = 0;
	};

	
	struct FFbxLoadConfig
	{
		bool BindToT0 = false;
	};
	
	class FFbxLoader
	{
	public:
		FFbxLoader() = default;
		~FFbxLoader() = default;

		TSharedPtr<Engine::FStaticMeshPrimitive> LoadMeshFromFile(FStringView FileName, const FFbxLoadConfig & LoadConfig);

		void LoadMesh(FbxNode * FbMeshNode, TVector<Engine::FMeshVertex> & Vertices, TVector<uint32_t> & Indices);

		template<typename FbxElementT, typename ElementT, typename ConverterT>
		static void ExtractLayerElements(const FbxLayerElementTemplate<FbxElementT> * FbLayerElements, TVector<ElementT> & Elements, ConverterT Converter, int32_t NumPolygonVertices, TView<int32_t> PointIndices)
		{
			FbxLayerElementUV::EReferenceMode FbLayerElementUVReferenceMode = FbLayerElements->GetReferenceMode();
			FbxLayerElementUV::EMappingMode FbLayerElementUVMappingMode = FbLayerElements->GetMappingMode();

			for (int32_t PolygonVertexIndex = 0; PolygonVertexIndex < NumPolygonVertices; ++PolygonVertexIndex)
			{
				int32_t ElementIndex = 0;
				if (FbLayerElementUVMappingMode == FbxLayerElement::eByControlPoint)
					ElementIndex = PointIndices[PolygonVertexIndex];
				else if (FbLayerElementUVMappingMode == FbxLayerElement::eByPolygonVertex)
					ElementIndex = PolygonVertexIndex;
				else
				{
					LogWarning(Str("Unsorported LayerElementUV MappingMode {}"), FbLayerElementUVMappingMode);
					return;
				}

				if (FbLayerElementUVReferenceMode == FbxLayerElement::eDirect)
				{
					FbxElementT FbxElement = FbLayerElements->GetDirectArray().GetAt(ElementIndex);
					Elements[PolygonVertexIndex] = Converter(FbxElement);
				}
				else if (FbLayerElementUVReferenceMode == FbxLayerElement::eIndexToDirect)
				{
					int32_t FbxElementIndex = FbLayerElements->GetIndexArray().GetAt(ElementIndex);
					FbxElementT FbxElement = FbLayerElements->GetDirectArray().GetAt(FbxElementIndex);
					Elements[PolygonVertexIndex] = Converter(FbxElement);
				}
				else
				{
					LogWarning(Str("Unsorported LayerElementUV ReferenceMode {}"), FbLayerElementUVReferenceMode);
					return;
				}
			}
		}
		
	public:
		TVector<FbxNode *> FbNodes;
		TVector<FbxNode *> FbMeshNodes;
		TVector<FbxNode *> FbSkeletonNodes;
		Engine::EVertexElements VertexElements = Engine::EVertexElements::None;
	};
}