#include "PCH.h"
#include "SampleWorld.h"
#include "RHID3D12/RHID3D12.h"
#include "Core/Platform/Win32/Win32.h"

#include <iostream>

#include "HairShaders.h"
#include "SceneView_Window.h"
#include "Engine/Include.h"

FSampleWorld * SampleWorld = nullptr;

void FSampleWorld::Initialize(ptr_t WindowHandleIn)
{
	SampleWorld = this;

	WindowHandle = WindowHandleIn;
	RHIFactory = RHI::D3D12::CreateRHIFactory();
	RHIFactory->Load();

	Engine = MakeShared<FEngine>();
	Engine->Initialize(*RHIFactory);

	Scene = Engine->CreateScene();

	Plane = MakeShared<FPlanePrimitive>(*Engine);
	Material.SetTexture(MakeShared<FTexture>(*Engine, Str("../Content/Wolf_Body.xtex")));

	for(int32_t RowIndex = 0; RowIndex < MeshRows; ++RowIndex)
	{
		for (int32_t ColIndex = 0; ColIndex < MeshCols; ++ColIndex)
		{
			TSharedPtr<FStaticMeshPrimitive> Mesh = MakeShared<FStaticMeshPrimitive>(*Engine);
			Mesh->SetMaterial(0, &Material);
			Mesh->LoadAsset_any(Str("../Content/Wolf.xmsh"));
			Mesh->SetName(Format(Str("Mesh [{}][{}]"), RowIndex, ColIndex));
			Scene->AddPrimitive(Mesh.Get());

			FVector3 Translation = {};
			Translation.X = (ColIndex - MeshCols / 2) * MeshSpacing.X;
			Translation.Z = RowIndex * MeshSpacing.Y + 1;
			Mesh->SetWorldTransform(FTransform::Translate(Translation));
			
			Meshs.Add(Mesh);
		}
	}

	Plane->SetWorldTransform(FTransform::Scale(FVector3(10, 10, 10)));
	Scene->AddPrimitive(Plane.Get());

	SceneView_Window = MakeShared<FSceneView_Window>();
	Scene->AddView(SceneView_Window.Get());
	
	Engine->EnqueueCommand([this](FRenderFactory & RenderFactory, RHI::FRHICommandList & RHICommandList)
		{
			Material.Initialize_rt(RenderFactory, RHICommandList);
			Plane->Initialize_rt(RenderFactory, RHICommandList);
		});
}

void FSampleWorld::Finalize()
{
	Engine->Finalize();
}

void FSampleWorld::Game_Update()
{
	FTransform Transform = FTransform::Identity;
	float RotationTimeBeg = FDateTime::System();

	//Transform.Rotation = FQuat::Rotate(FVector3(0.f, 0.f, (FDateTime::System() - RotationTimeBeg) * 3.14159265f * 0.5f));
	Transform.Rotation = FQuat::Rotate(Rotate);
	//Transform.Scaling = FVector3(0.5f, 0.5f, 0.5f);
	Transform.Scaling = FVector3(10.0f, 10.0f, 10.0f);
	Engine->EnqueueCommand([&, Transform](FRenderFactory & RenderFactory, RHI::FRHICommandList & RHICommandList)
		{
			//Plane->SetLocalTransform(Transform);
			for (uint32_t RowIndex = 0; RowIndex < MeshRows; ++RowIndex)
			{
				for (uint32_t ColIndex = 0; ColIndex < MeshCols; ++ColIndex)
				{
					Meshs[RowIndex * MeshCols + ColIndex]->SetLocalTransform(Transform);
				}
			}
			SceneView_Window->Update(WindowHandle);
			//SceneView_Window->Update(WindowHandle);
			SceneView_Window->ViewMatrix = FMatrix::LookAtLH(CameraAt, FVector3(), FVector3(0.f, 1.f, 0.f));
			SceneView_Window->ProjectionMatrix = FMatrix::PerspectiveLH(3.14159265f * 0.5f, (float)SceneView_Window->ViewSize.Width / SceneView_Window->ViewSize.Height, 0.0001f, 1000.0f);
			SceneView_Window->ViewProjectionMatrix = SceneView_Window->ViewMatrix * SceneView_Window->ProjectionMatrix;
		});

	Engine->FlushScene(NullIndex);
}

void FSampleWorld::OnKeyDown(uint32_t vkey, uint32_t modify)
{
	switch (vkey)
	{
	default:
		break;
	}
}

void FSampleWorld::OnMouseDownL(const PointI & Point)
{
	DragPoint = Point;
	Draging = true;
	RotateLast = Rotate;
}

void FSampleWorld::OnMouseUpL(const PointI & Point)
{
	Draging = false;
}

void FSampleWorld::OnMouseMove(const PointI & Point)
{
	if (Draging)
	{
		Rotate.X = RotateLast.X -(Point.Y - DragPoint.Y) * RotateSpeed;
		Rotate.Y = RotateLast.Y -(Point.X - DragPoint.X) * RotateSpeed;
	}
}

void FSampleWorld::OnMouseWheel(const PointI & Wheel)
{
	CameraAt.Z += Wheel.Y * MoveSpeed;
}

