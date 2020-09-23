#include "PCH.h"
#include "HairShaders.h"

HairVertexFactory::HairVertexFactory()
{
	VS = MakeShared<FShader>(EShaderType::Vertex, Str("../Content/shaders/BaseVertexShader.hlsl"), Str("VSMain"));
	uint32_t VertexOffset = 0;
	{
		FVertexElement Element;
		Element.Name = Str("POSITION");
		Element.StreamIndex = 0;
		Element.Offset = VertexOffset;
		Element.Stride = sizeof(float3);
		Element.Format = EFormat::Float3;
		VertexElements.Add(Element);
		VertexOffset += Element.Stride;
	}
	{
		FVertexElement Element;
		Element.Name = Str("TEXCOORD");
		Element.StreamIndex = 0;
		Element.Offset = VertexOffset;
		Element.Stride = sizeof(float3);
		Element.Format = EFormat::Float2;
		VertexElements.Add(Element);
		VertexOffset += Element.Stride;
	}
}

HairVertexFactory::~HairVertexFactory()
{
}

