// D3D11RenderView.cpp
// KlayGE D3D11渲染视图类 实现文件
// Ver 3.8.0
// 版权所有(C) 龚敏敏, 2009
// Homepage: http://www.klayge.org
//
// 3.8.0
// 初次建立 (2009.1.30)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Util.hpp>
#include <KlayGE/COMPtr.hpp>
#include <KlayGE/ThrowErr.hpp>
#include <KlayGE/Math.hpp>
#include <KlayGE/RenderFactory.hpp>
#include <KlayGE/Context.hpp>
#include <KlayGe/FrameBuffer.hpp>

#include <boost/assert.hpp>

#include <KlayGE/D3D11/D3D11RenderEngine.hpp>
#include <KlayGE/D3D11/D3D11Mapping.hpp>
#include <KlayGE/D3D11/D3D11Texture.hpp>
#include <KlayGE/D3D11/D3D11RenderView.hpp>

namespace KlayGE
{
	D3D11RenderView::D3D11RenderView()
	{
		D3D11RenderEngine& renderEngine(*checked_cast<D3D11RenderEngine*>(&Context::Instance().RenderFactoryInstance().RenderEngineInstance()));
		d3d_device_ = renderEngine.D3DDevice();
		d3d_imm_ctx_ = renderEngine.D3DDeviceImmContext();
	}

	D3D11RenderView::~D3D11RenderView()
	{
	}


	D3D11RenderTargetRenderView::D3D11RenderTargetRenderView(Texture& texture, int array_index, int level)
	{
		rt_view_ = checked_cast<D3D11Texture*>(&texture)->RetriveD3DRenderTargetView(array_index, level);

		width_ = texture.Width(0);
		height_ = texture.Height(0);
		pf_ = texture.Format();
	}

	D3D11RenderTargetRenderView::D3D11RenderTargetRenderView(Texture& texture_3d, int array_index, uint32_t first_slice, uint32_t num_slices, int level)
	{
		rt_view_ = checked_cast<D3D11Texture*>(&texture_3d)->RetriveD3DRenderTargetView(array_index, first_slice, num_slices, level);

		width_ = texture_3d.Width(0);
		height_ = texture_3d.Height(0);
		pf_ = texture_3d.Format();
    }

	D3D11RenderTargetRenderView::D3D11RenderTargetRenderView(Texture& texture_cube, int array_index, Texture::CubeFaces face, int level)
	{
		rt_view_ = checked_cast<D3D11Texture*>(&texture_cube)->RetriveD3DRenderTargetView(array_index, face, level);

		width_ = texture_cube.Width(0);
		height_ = texture_cube.Width(0);
		pf_ = texture_cube.Format();
	}

	D3D11RenderTargetRenderView::D3D11RenderTargetRenderView(GraphicsBuffer& gb, uint32_t width, uint32_t height, ElementFormat pf)
	{
		BOOST_ASSERT(gb.AccessHint() & EAH_GPU_Write);

		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = D3D11Mapping::MappingFormat(pf);
		desc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
		desc.Buffer.ElementOffset = 0;
		desc.Buffer.ElementWidth = std::min(width * height, gb.Size() / NumFormatBytes(pf));

		ID3D11RenderTargetView* rt_view;
		TIF(d3d_device_->CreateRenderTargetView(checked_cast<D3D11GraphicsBuffer*>(&gb)->D3DBuffer().get(), &desc, &rt_view));
		rt_view_ = MakeCOMPtr(rt_view);

		width_ = width * height;
		height_ = 1;
		pf_ = pf;
	}

	D3D11RenderTargetRenderView::D3D11RenderTargetRenderView(ID3D11RenderTargetViewPtr const & view, uint32_t width, uint32_t height, ElementFormat pf)
		: rt_view_(view)
	{
		width_ = width;
		height_ = height;
		pf_ = pf;
	}

	void D3D11RenderTargetRenderView::ClearColor(Color const & clr)
	{
		d3d_imm_ctx_->ClearRenderTargetView(rt_view_.get(), &clr.r());
	}

	void D3D11RenderTargetRenderView::ClearDepth(float /*depth*/)
	{
		BOOST_ASSERT(false);
	}

	void D3D11RenderTargetRenderView::ClearStencil(int32_t /*stencil*/)
	{
		BOOST_ASSERT(false);
	}

	void D3D11RenderTargetRenderView::ClearDepthStencil(float /*depth*/, int32_t /*stencil*/)
	{
		BOOST_ASSERT(false);
	}

	void D3D11RenderTargetRenderView::OnAttached(FrameBuffer& /*fb*/, uint32_t /*att*/)
	{
	}

	void D3D11RenderTargetRenderView::OnDetached(FrameBuffer& /*fb*/, uint32_t /*att*/)
	{
	}


	D3D11DepthStencilRenderView::D3D11DepthStencilRenderView(Texture& texture, int array_index, int level)
	{
		ds_view_ = checked_cast<D3D11Texture*>(&texture)->RetriveD3DDepthStencilView(array_index, level);

		width_ = texture.Width(0);
		height_ = texture.Height(0);
		pf_ = texture.Format();
	}

	D3D11DepthStencilRenderView::D3D11DepthStencilRenderView(Texture& texture_3d, int array_index, uint32_t first_slice, uint32_t num_slices, int level)
	{
		ds_view_ = checked_cast<D3D11Texture*>(&texture_3d)->RetriveD3DDepthStencilView(array_index, first_slice, num_slices, level);

		width_ = texture_3d.Width(0);
		height_ = texture_3d.Height(0);
		pf_ = texture_3d.Format();
    }

	D3D11DepthStencilRenderView::D3D11DepthStencilRenderView(Texture& texture_cube, int array_index, Texture::CubeFaces face, int level)
	{
		ds_view_ = checked_cast<D3D11Texture*>(&texture_cube)->RetriveD3DDepthStencilView(array_index, face, level);

		width_ = texture_cube.Width(0);
		height_ = texture_cube.Width(0);
		pf_ = texture_cube.Format();
	}

	D3D11DepthStencilRenderView::D3D11DepthStencilRenderView(ID3D11DepthStencilViewPtr const & view, uint32_t width, uint32_t height, ElementFormat pf)
		: ds_view_(view)
	{
		width_ = width;
		height_ = height;
		pf_ = pf;
	}

	D3D11DepthStencilRenderView::D3D11DepthStencilRenderView(uint32_t width, uint32_t height,
											ElementFormat pf, uint32_t sample_count, uint32_t sample_quality)
	{
		BOOST_ASSERT(IsDepthFormat(pf));

		D3D11_TEXTURE2D_DESC tex_desc;
		tex_desc.Width = width;
		tex_desc.Height = height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = D3D11Mapping::MappingFormat(pf);
		tex_desc.SampleDesc.Count = sample_count;
		tex_desc.SampleDesc.Quality = sample_quality;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = 0;
		ID3D11Texture2D* depth_tex;
		TIF(d3d_device_->CreateTexture2D(&tex_desc, NULL, &depth_tex));

		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		desc.Format = tex_desc.Format;
		if (sample_count > 1)
		{
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		}
		desc.Flags = 0;
		desc.Texture2D.MipSlice = 0;

		ID3D11DepthStencilView* ds_view;
		TIF(d3d_device_->CreateDepthStencilView(depth_tex, &desc, &ds_view));
		ds_view_ = MakeCOMPtr(ds_view);

		depth_tex->Release();

		width_ = width;
		height_ = height;
		pf_ = pf;
	}

	void D3D11DepthStencilRenderView::ClearColor(Color const & /*clr*/)
	{
		BOOST_ASSERT(false);
	}

	void D3D11DepthStencilRenderView::ClearDepth(float depth)
	{
		d3d_imm_ctx_->ClearDepthStencilView(ds_view_.get(), D3D11_CLEAR_DEPTH, depth, 0);
	}

	void D3D11DepthStencilRenderView::ClearStencil(int32_t stencil)
	{
		d3d_imm_ctx_->ClearDepthStencilView(ds_view_.get(), D3D11_CLEAR_STENCIL, 1, static_cast<uint8_t>(stencil));
	}

	void D3D11DepthStencilRenderView::ClearDepthStencil(float depth, int32_t stencil)
	{
		d3d_imm_ctx_->ClearDepthStencilView(ds_view_.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, static_cast<uint8_t>(stencil));
	}

	void D3D11DepthStencilRenderView::OnAttached(FrameBuffer& /*fb*/, uint32_t att)
	{
		UNREF_PARAM(att);

		BOOST_ASSERT(FrameBuffer::ATT_DepthStencil == att);
	}

	void D3D11DepthStencilRenderView::OnDetached(FrameBuffer& /*fb*/, uint32_t att)
	{
		UNREF_PARAM(att);

		BOOST_ASSERT(FrameBuffer::ATT_DepthStencil == att);
	}
}
