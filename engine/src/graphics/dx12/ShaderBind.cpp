#include "ShaderBind.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Shader.h"

// logger
#include "logger/Logger.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			bool ShaderBind::LoadByName(const std::string& a_sName, std::shared_ptr<PixelShader> a_pPixelShader, std::shared_ptr<VertexShader> a_pVertexShader, CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT a_DSVFormat)
			{
				if (!EngineResource::LoadByName(a_sName))
				{
					return false;
				}

				m_DSVFormat = a_DSVFormat;
				m_pPixelShader = a_pPixelShader;
				m_pVertexShader = a_pVertexShader;
				m_AssetType = resources::AssetType::ShaderBind;

				return true;
			}

			//---------------------------------------------------------------------
			bool ShaderBind::CreatePipelineState()
			{
				CD3DX12_RASTERIZER_DESC rasterDesc(D3D12_DEFAULT);
				rasterDesc.CullMode = D3D12_CULL_MODE_BACK;

				D3D12_BLEND_DESC blendDesc = {};
				blendDesc.AlphaToCoverageEnable = FALSE;
				blendDesc.IndependentBlendEnable = FALSE;

				D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
				rtBlendDesc.BlendEnable = TRUE;
				rtBlendDesc.LogicOpEnable = FALSE;
				rtBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
				rtBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
				rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
				rtBlendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
				rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
				rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

				blendDesc.RenderTarget[0] = rtBlendDesc;

				CD3DX12_DEPTH_STENCIL_DESC depthDesc(D3D12_DEFAULT);
				depthDesc.DepthEnable = TRUE;
				depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
				depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
				depthDesc.StencilEnable = FALSE;

				D3D12_RT_FORMAT_ARRAY rtvFormats = {};
				rtvFormats.NumRenderTargets = 1;
				rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // Must match actual RTV

				struct PipelineStateStream
				{
					CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
					CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
					CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
					CD3DX12_PIPELINE_STATE_STREAM_VS VS;
					CD3DX12_PIPELINE_STATE_STREAM_PS PS;
					CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
					CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
					CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER RasterizerState;
					CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendState;
					CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
				} pipelineStateStream;

				pipelineStateStream.pRootSignature =
					core::ENGINE->GetDX12().GetRootSignature().Get();
				pipelineStateStream.InputLayout =
				{ g_aInputLayout, _countof(g_aInputLayout) };
				pipelineStateStream.PrimitiveTopologyType =
					D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				pipelineStateStream.VS =
					CD3DX12_SHADER_BYTECODE(m_pVertexShader->GetShaderBlob().Get());
				pipelineStateStream.PS =
					CD3DX12_SHADER_BYTECODE(m_pPixelShader->GetShaderBlob().Get());
				pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
				pipelineStateStream.RTVFormats = rtvFormats;
				pipelineStateStream.RasterizerState = rasterDesc;
				pipelineStateStream.BlendState = CD3DX12_BLEND_DESC(blendDesc);
				pipelineStateStream.DepthStencilState =
					CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL(depthDesc);

				D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
				desc.SizeInBytes = sizeof(pipelineStateStream);
				desc.pPipelineStateSubobjectStream = &pipelineStateStream;

				if (FAILED(core::ENGINE->GetDX12().GetDevice()->CreatePipelineState(
					&desc, IID_PPV_ARGS(&m_pPipelineState))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12,
						"Failed creating transparent sprite pipeline state.");
					return false;
				}

				return true;
			}

			//---------------------------------------------------------------------
			void ShaderBind::Bind(std::shared_ptr<CommandList> a_pCommandList)
			{
				a_pCommandList->GetCommandList()->SetPipelineState(m_pPipelineState.Get());
				a_pCommandList->GetCommandList()->SetGraphicsRootSignature(core::ENGINE->GetDX12().GetRootSignature().Get()); // Set the existing root signature
			}

			//---------------------------------------------------------------------
			bool ShaderBind::HasPixelShader(std::weak_ptr<PixelShader> a_pPixelShader)
			{
				if (auto pixelShader = a_pPixelShader.lock())
				{
					return m_pPixelShader == pixelShader;
				}
				return m_pPixelShader != nullptr;
			}

			//---------------------------------------------------------------------
			bool ShaderBind::HasVertexShader(std::weak_ptr<VertexShader> a_pVertexShader)
			{
				if (auto vertexShader = a_pVertexShader.lock())
				{
					return m_pVertexShader == vertexShader;
				}
				return m_pVertexShader != nullptr;
			}

			//---------------------------------------------------------------------
			const std::weak_ptr<PixelShader> ShaderBind::GetPixelShader()
			{
				return m_pPixelShader;
			}

			//---------------------------------------------------------------------
			const std::weak_ptr<VertexShader> ShaderBind::GetVertexShader()
			{
				return m_pVertexShader;
			}
		}
	}
}