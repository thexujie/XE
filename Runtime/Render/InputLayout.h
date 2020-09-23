#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	RENDER_API TVector<FVertexElement> GetVertexElements(EVertexElements VertexElements);

	class IInputLayout : public IRenderObject
	{
	public:
		virtual TView<FVertexElement> GetVertexElements() const = 0;
	};

	class FInputLayout : public IInputLayout
	{
	public:
		FInputLayout(TView<FVertexElement> VertexElementsIn) : VertexElements(VertexElementsIn) {}

		TView<FVertexElement> GetVertexElements() const { return VertexElements; }
	private:
		TVector<FVertexElement> VertexElements;
	};
}
