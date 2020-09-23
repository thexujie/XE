#include "PCH.h"
#include "FbxLoader.h"

#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")


namespace XE
{
	TSharedPtr<Engine::FStaticMeshPrimitive> FFbxLoader::LoadMeshFromFile(FStringView FileName, const FFbxLoadConfig & LoadConfig)
	{
		TUniquePtr<FbxManager, void(*)(FbxManager *)> FbManager = TUniquePtr<FbxManager, void(*)(FbxManager *)>(FbxManager::Create(), TFbxObjectDeleter);
		TUniquePtr<FbxScene, void(*)(FbxScene *)> FbScene = TUniquePtr<FbxScene, void(*)(FbxScene *)>(FbxScene::Create(FbManager.Get(), ""), TFbxObjectDeleter);
		TUniquePtr<FbxGeometryConverter> FbGeometryConverter = MakeUnique<FbxGeometryConverter>(FbManager.Get());
		TUniquePtr<FbxImporter, void(*)(FbxImporter *)> FbImper = TUniquePtr<FbxImporter, void(*)(FbxImporter *)>(FbxImporter::Create(FbManager.Get(), ""), TFbxObjectDeleter);

		FString FileNameStr = FileName;
		bool InitializeOK = FbImper->Initialize((const char *)(FileNameStr.Data), -1, nullptr);
		if (!InitializeOK)
			return nullptr;

		bool ImportOK = FbImper->Import(FbScene.Get());
		if (!ImportOK)
		{
			return nullptr;
		}

		FFbxVersion SdkVersin;
		FbxManager::GetFileFormatVersion(SdkVersin.Major, SdkVersin.Minor, SdkVersin.Revision);

		FFbxVersion FileVersin;
		FbImper->GetFileVersion(FileVersin.Major, FileVersin.Minor, FileVersin.Revision);

		FbGeometryConverter->Triangulate(FbScene.Get(), true);
		
		auto FunParseNode = [&](FbxNode * Node, auto & FunSelf)->void
		{
			FbNodes.Add(Node);

			FbxNodeAttribute * nodeattr = Node->GetNodeAttribute();
			if (nodeattr && nodeattr->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				//if (Node->GetMesh() && Node->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) > 0)
				FbMeshNodes.Add(Node);
			}

			if (Node->GetSkeleton())
				FbSkeletonNodes.Add(Node);

			for (int ChildIndex = 0; ChildIndex < Node->GetChildCount(); ++ChildIndex)
				FunSelf(Node->GetChild(ChildIndex), FunSelf);
		};

		FbxNode * RootNode = FbScene->GetRootNode();
		FunParseNode(RootNode, FunParseNode);

		TSharedPtr<Engine::FStaticMeshPrimitive> StaticMeshPrimitive = MakeShared<Engine::FStaticMeshPrimitive>(Engine::FEngine::Instance());
		if (!LoadConfig.BindToT0)
		{
			for (FbxNode * FbMeshNode : FbMeshNodes)
				LoadMesh(FbMeshNode, StaticMeshPrimitive->Vertices, StaticMeshPrimitive->Indices);
		}
		StaticMeshPrimitive->VertexElements = VertexElements;
		
		return StaticMeshPrimitive;
	}

	void FFbxLoader::LoadMesh(FbxNode * FbMeshNode, TVector<Engine::FMeshVertex> & Vertices, TVector<uint32_t> & Indices)
	{
		FbxMesh * FbMesh =  FbMeshNode->GetMesh();
		if (!FbMesh)
		{
			LogInfo(Str("No mesh in node {}"), FbMeshNode->GetName());
			return;
		}

		VertexElements = Engine::EVertexElements::Position;
		
		int32_t NumPoints = FbMesh->GetControlPointsCount();
		if (!NumPoints)
			return;
		
		FbxVector4 * FbPoints = FbMesh->GetControlPoints();
		assert(FbPoints);

		TVector<FVector3> Points(NumPoints, NumPoints);
		
		for (int32_t PointIndex = 0; PointIndex < NumPoints; ++PointIndex)
		{
			FbxVector4 & FbPoint = FbPoints[PointIndex];
			Points[PointIndex] = FVector3(float32_t(FbPoint[0]), float32_t(FbPoint[1]), float32_t(FbPoint[2]));
		}

		int32_t NumPolygons = FbMesh->GetPolygonCount();
		int32_t NumPolygonVertices = FbMesh->GetPolygonVertexCount();
		int32_t * PolygonPointIndices = FbMesh->GetPolygonVertices();

		TVector<int32_t> PointIndices(PolygonPointIndices, NumPolygonVertices);
		for (int32_t PolygonIndex = 0; PolygonIndex < NumPolygons; ++PolygonIndex)
		{
			int32_t PolygonSize = FbMesh->GetPolygonSize(PolygonIndex);
			if (PolygonSize != 3)
			{
				LogWarning(Str("Only triangles supported!"));
				return;
			}
		}
		
		auto ConvertUV = [](const FbxVector2 & FbUV)->FVector2
		{
			return FVector2(float32_t(FbUV[0]), float32_t(FbUV[1]));
		};

		TVector<FVector2> UVs;
		TVector<FVector2> UV2s;
		int32_t NumLayers = FbMesh->GetLayerCount();
		if (NumLayers < 1)
		{
			LogWarning(Str("Invalid layer count {}"), NumLayers);
			return;
		}
		
		//for (int32_t LayerIndex = 0; LayerIndex < NumLayers; ++LayerIndex)
		//{
		//}
		//
		FbxLayer * FbLayer0 = FbMesh->GetLayer(0);
		assert(FbLayer0);
		const FbxLayerElementUV * FbLayerUVs = FbLayer0->GetUVs(FbxLayerElement::eTextureDiffuse);
		if (FbLayerUVs)
		{
			UVs.Resize(NumPolygonVertices, NumPolygonVertices, EInitializeMode::Default);
			ExtractLayerElements<FbxVector2, FVector2, decltype(ConvertUV)>(FbLayerUVs, UVs, ConvertUV, NumPolygonVertices, PointIndices);
			VertexElements |= Engine::EVertexElements::UV;
		}

		FbxLayer * FbLayer2 = FbMesh->GetLayer(1);
		const FbxLayerElementUV * FbLayerUV2s = FbLayer2 ? FbLayer2->GetUVs(FbxLayerElement::eTextureDiffuse) : nullptr;
		if (FbLayerUV2s)
		{
			UV2s.Resize(NumPolygonVertices, NumPolygonVertices, EInitializeMode::Default);
			ExtractLayerElements<FbxVector2, FVector2, decltype(ConvertUV)>(FbLayerUV2s, UV2s, ConvertUV, NumPolygonVertices, PointIndices);
			VertexElements |= Engine::EVertexElements::UV2;
		}

		FbxLayer * FbMainLayer = FbMesh->GetLayer(0);

		// Normals
		auto ConvertNormal = [](const FbxVector4 & FbUV)->FVector3
		{
			return FVector3(float32_t(FbUV[0]), float32_t(FbUV[1]), float32_t(FbUV[2]));
		};

		TVector<FVector3> Normals;
		const FbxLayerElementNormal * FbLayerNormals = FbMainLayer->GetNormals();
		if (FbLayerNormals)
		{
			Normals.Resize(NumPolygonVertices, NumPolygonVertices, EInitializeMode::Default);
			ExtractLayerElements<FbxVector4, FVector3, decltype(ConvertNormal)>(FbLayerNormals, Normals, ConvertNormal, NumPolygonVertices, PointIndices);
			VertexElements |= Engine::EVertexElements::Normal;
		}

		TVector<FVector3> BiNormals;
		const FbxLayerElementBinormal * FbLayerBiNormals = FbMainLayer->GetBinormals();
		if (FbLayerBiNormals)
		{
			BiNormals.Resize(NumPolygonVertices, NumPolygonVertices, EInitializeMode::Default);
			ExtractLayerElements<FbxVector4, FVector3, decltype(ConvertNormal)>(FbLayerBiNormals, BiNormals, ConvertNormal, NumPolygonVertices, PointIndices);
			VertexElements |= Engine::EVertexElements::BiNormal;
		}

		// Vertex Colors
		auto ConvertColor= [](const FbxColor & FbUV)->FColor
		{
			return FColor(float32_t(FbUV[0]), float32_t(FbUV[1]), float32_t(FbUV[2]));
		};

		TVector<FColor> Colors;
		const FbxLayerElementVertexColor* FbLayerVertexColors = FbMainLayer->GetVertexColors();
		if (FbLayerVertexColors)
		{
			Colors.Resize(NumPolygonVertices, NumPolygonVertices, EInitializeMode::Default);
			ExtractLayerElements<FbxColor, FColor, decltype(ConvertColor)>(FbLayerVertexColors, Colors, ConvertColor, NumPolygonVertices, PointIndices);
			VertexElements |= Engine::EVertexElements::Color;
		}

		//float pi = 3.14159265f;
		//float normal_dot_angle = cos(1.0 / 360.0 * 2.0 * pi) - 0.001f;
		auto FunVertexEqual = [&](const Engine::FMeshVertex & VertexA, const Engine::FMeshVertex & VertexB, Engine::EVertexElements VertexElements)
		{
			if ((VertexElements * Engine::EVertexElements::UV) && (VertexA.UV - VertexB.UV).LengthSquare() > 0.01f)
				return false;
			
			if ((VertexElements * Engine::EVertexElements::UV2) && (VertexA.UV2 - VertexB.UV2).LengthSquare() > 0.01f)
				return false;
			
			if ((VertexElements * Engine::EVertexElements::Normal) && (VertexA.Normal - VertexB.Normal).LengthSquare() > 0.01f)
				return false;

			if ((VertexElements * Engine::EVertexElements::BiNormal) && (VertexA.BiNormal - VertexB.BiNormal).LengthSquare() > 0.01f)
				return false;

			if ((VertexElements * Engine::EVertexElements::Color) && VertexA.Color != VertexB.Color)
				return false;
			
			return true;
		};
		
		TMultiMap<int32_t, int32_t> PointVertexMapping;
		for (int32_t PolygonPointIndex = 0; PolygonPointIndex < PointIndices.GetSize(); ++PolygonPointIndex)
		{
			Engine::FMeshVertex Vertex = {};
			int32_t PointIndex = PointIndices[PolygonPointIndex];
			Vertex.Position = Points[PointIndex];
			
			if (VertexElements * Engine::EVertexElements::UV)
				Vertex.UV = UVs[PolygonPointIndex];

			if (VertexElements * Engine::EVertexElements::UV2)
				Vertex.UV2 = UV2s[PolygonPointIndex];

			if (VertexElements * Engine::EVertexElements::Normal)
				Vertex.Normal = Normals[PolygonPointIndex];

			if (VertexElements * Engine::EVertexElements::BiNormal)
				Vertex.BiNormal = BiNormals[PolygonPointIndex];

			if (VertexElements * Engine::EVertexElements::Color)
				Vertex.Color = Colors[PolygonPointIndex];

			auto PointVerticesIter =  PointVertexMapping.Find(PointIndex);
			if (PointVerticesIter.Key == PointVertexMapping.End())
			{
				Indices.Add(uint32_t(Vertices.GetSize()));
				PointVertexMapping.Insert(PointIndex, uint32_t(Vertices.GetSize()));
				Vertices.Add(Vertex);
			}
			else
			{
				bool Found = false;
				for (auto Iter = PointVerticesIter.Key; Iter != PointVerticesIter.Value; ++Iter)
				{
					if (FunVertexEqual(Vertex, Vertices[Iter->Value], VertexElements))
					{
						Indices.Add(uint32_t(Iter->Value));
						Found = true;
						break;
					}
				}

				if (!Found)
				{
					Indices.Add(uint32_t(Vertices.GetSize()));
					PointVertexMapping.Insert(PointIndex, uint32_t(Vertices.GetSize() - 1));
					Vertices.Add(Vertex);
				}
			}
		}
	}
}
