#pragma once

#include "Inc.h"
#include "PlanePrimitive.h"
#include "SceneView_Window.h"
#include "Engine/Materials/ColorMaterial.h"


class FSampleWorld/* : public FWorld*/
{
public:
	FSampleWorld() = default;
	~FSampleWorld() = default;

	void Initialize(ptr_t WindowHandleIn);
	void Finalize();

	void Game_Update();

public:
	void OnKeyDown(uint32_t vkey, uint32_t modify);
	void OnMouseDownL(const PointI & Point);
	void OnMouseUpL(const PointI & Point);
	void OnMouseMove(const PointI & Point);
	void OnMouseWheel(const PointI & Wheel);
	
private:
	ptr_t WindowHandle = nullptr;

	TSharedPtr<FRHIFactory> RHIFactory;
	TSharedPtr<FEngine> Engine;
	
	TSharedPtr<FScene> Scene;

	TSharedPtr<FPlanePrimitive> Plane;

	intx_t MeshRows = 10;
	intx_t MeshCols = 10;
	Vec2F MeshSpacing = { 1, 1 };
	TVector<TSharedPtr<FStaticMeshPrimitive>> Meshs;

	FTextureMaterial Material;
	TSharedPtr<FSceneView_Window> SceneView_Window;

	FColorMaterial ColorMaterial;

	FVector3 CameraAt = FVector3(0.f, 1.f, -1.f);
	// Intersect
	PointI DragPoint;
	bool Draging = false;
	FVector3 RotateLast = {};
	FVector3 Rotate = {};
	float RotateSpeed = 0.01f;
	float MoveSpeed = 0.01f;
};

