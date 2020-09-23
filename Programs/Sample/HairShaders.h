#pragma once

#include "Inc.h"

class HairVertexFactory : public IMeshAssembler
{
public:
	HairVertexFactory();
	~HairVertexFactory();

	virtual EGeometry GetGeometry() const { return EGeometry::Triangle; }
	virtual FShader * GetVertexShader() const { return VS.Get(); }
	virtual FShader * GetHullShader() const { return HS.Get(); }
	virtual FShader * GetDomainShader() const { return DS.Get(); }
	virtual FShader * GetGeometryShader() const { return GS.Get(); }
	virtual TView<FVertexElement> GetVertexElements() const { return VertexElements; }

private:
	TSharedPtr<FShader> VS;
	TSharedPtr<FShader> HS;
	TSharedPtr<FShader> DS;
	TSharedPtr<FShader> GS;
	TVector<FVertexElement> VertexElements;
};

class FLocalVertexFactory : public IMeshAssembler
{
public:
	FLocalVertexFactory();
	~FLocalVertexFactory() = default;

	EGeometry GetGeometry() const { return EGeometry::Triangle; }
	FShader * GetVertexShader() const { return VS.Get(); }
	FShader * GetHullShader() const { return nullptr; }
	FShader * GetDomainShader() const { return nullptr; }
	FShader * GetGeometryShader() const { return nullptr; }
	TView<FVertexElement> GetVertexElements() const { return VertexElements; }

private:
	TSharedPtr<FShader> VS;
	TVector<FVertexElement> VertexElements;
};

class FLocalMaterial : public IMaterial
{
public:

private:
};
