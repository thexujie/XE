#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	class RENDER_API IRenderTarget : public IRenderObject
	{
	public:
		IRenderTarget() = default;

		virtual const IOutputLayout * GetOutputLayout() const = 0;
	};

	class IOutputLayout : public IRenderObject
	{
	public:
		virtual TView<EFormat> GetRenderTargetForamts() const = 0;
		virtual FMSAA GetMSAA() const = 0;
		virtual bool GetDepthTest() const = 0;
		virtual bool GetDepthWrite() const = 0;
		virtual EComparison GetDepthComparison() const = 0;
		virtual EFormat GetDepthStencilFormat() const = 0;
	};

	class FOutputLayout : public IOutputLayout
	{
	public:
		FOutputLayout(TView<EFormat> RenderTargetFormatsIn, EFormat DepthStencilFormatIn, const FMSAA & MSAAIn)
		: RenderTargetFormats(RenderTargetFormatsIn), DepthStencilFormat(DepthStencilFormatIn), MSAA(MSAAIn) {}

		TView<EFormat> GetRenderTargetForamts() const { return RenderTargetFormats; }
		bool GetDepthTest() const { return true; }
		bool GetDepthWrite() const { return true; }
		EComparison GetDepthComparison() const { return EComparison::Less; }
		EFormat GetDepthStencilFormat() const { return DepthStencilFormat; }
		FMSAA GetMSAA() const { return MSAA; }
		
	private:
		TVector<EFormat> RenderTargetFormats;
		EFormat DepthStencilFormat = EFormat::None;
		FMSAA MSAA;
	};
}
