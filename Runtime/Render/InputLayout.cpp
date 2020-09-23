#include "PCH.h"
#include "InputLayout.h"

#include "RenderFactory.h"

namespace XE::Render
{
	TVector<FVertexElement> GetVertexElements(EVertexElements VertexElements)
	{
		TVector<FVertexElement> Elements;
		uint32_t VertexOffset = 0;
		if (!!(VertexElements & EVertexElements::Position))
		{
			FVertexElement Element;
			Element.Name = Str("POSITION");
			Element.StreamIndex = 0;
			Element.Offset = VertexOffset;
			Element.Stride = sizeof(float3);
			Element.Format = EFormat::Float3;
			Elements.Add(Element);
			VertexOffset += Element.Stride;
		}

		if (!!(VertexElements & EVertexElements::UV))
		{
			FVertexElement Element;
			Element.Name = Str("TEXCOORD");
			Element.StreamIndex = 0;
			Element.Offset = VertexOffset;
			Element.Stride = sizeof(float3);
			Element.Format = EFormat::Float2;
			Elements.Add(Element);
			VertexOffset += Element.Stride;
		}

		if (!!(VertexElements & EVertexElements::UV2))
		{
			FVertexElement Element;
			Element.Name = Str("TEXCOORD2");
			Element.StreamIndex = 0;
			Element.Offset = VertexOffset;
			Element.Stride = sizeof(float3);
			Element.Format = EFormat::Float2;
			Elements.Add(Element);
			VertexOffset += Element.Stride;
		}
		return Elements;
	}
}
