#include "DX12ShaderBind.h"

// core includes
#include "core/Engine.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Shader.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			DX12ShaderBind::DX12ShaderBind()
			{
				m_ResourceType = core::ResourceType::ResourceType_ShaderBind;
			}

			//---------------------------------------------------------------------
			DX12ShaderBind::DX12ShaderBind(const PixelShader* a_PixelShader, const VertexShader* a_VertexShader) :
				m_pPixelShader(a_PixelShader),
				m_pVertexShader(a_VertexShader)
			{
				m_ResourceType = core::ResourceType::ResourceType_ShaderBind;
			}

			//---------------------------------------------------------------------
			bool DX12ShaderBind::CreatePipelineState()
			{
				CD3DX12_RASTERIZER_DESC rasterDesc(D3D12_DEFAULT);
				rasterDesc.CullMode = D3D12_CULL_MODE_NONE;

				// Build the raw blend desc
				D3D12_BLEND_DESC blendDesc = {};
				blendDesc.AlphaToCoverageEnable = FALSE;
				blendDesc.IndependentBlendEnable = FALSE;

				D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
				rtBlendDesc.BlendEnable = TRUE;
				rtBlendDesc.LogicOpEnable = FALSE;
				rtBlendDesc.SrcBlend = D3D12_BLEND_ONE;                  // premultiplied alpha
				rtBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
				rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
				rtBlendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
				rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
				rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

				blendDesc.RenderTarget[0] = rtBlendDesc;

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
				} pipelineStateStream;

				D3D12_RT_FORMAT_ARRAY rtvFormats = {};
				rtvFormats.NumRenderTargets = 1;
				rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

				pipelineStateStream.pRootSignature = core::ENGINE->GetDX12().GetRootSignature().Get();
				pipelineStateStream.InputLayout = { g_aInputLayout, _countof(g_aInputLayout) };
				pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(m_pPixelShader->GetShaderBlob().Get());
				pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(m_pVertexShader->GetShaderBlob().Get());
				pipelineStateStream.DSVFormat = DXGI_FORMAT_UNKNOWN;
				pipelineStateStream.RTVFormats = rtvFormats;
				pipelineStateStream.RasterizerState = rasterDesc;
				pipelineStateStream.BlendState = CD3DX12_BLEND_DESC(blendDesc);

				D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
					sizeof(PipelineStateStream), &pipelineStateStream
				};
				if (FAILED(core::ENGINE->GetDX12().GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating pipeline state.");
					return false;
				}

				return false;
			}

			//---------------------------------------------------------------------
			void DX12ShaderBind::Bind(std::shared_ptr<CommandList> a_pCommandList)
			{
				a_pCommandList->GetCommandList()->SetPipelineState(m_pPipelineState.Get());
				a_pCommandList->GetCommandList()->SetGraphicsRootSignature(core::ENGINE->GetDX12().GetRootSignature().Get()); // Set the existing root signature
			}

			//---------------------------------------------------------------------
			bool DX12ShaderBind::HasPixelShader(const PixelShader* a_PixelShader)
			{
				return m_pPixelShader == a_PixelShader;
			}

			//---------------------------------------------------------------------
			bool DX12ShaderBind::HasVertexShader(const VertexShader* a_VertexShader)
			{
				return m_pVertexShader == a_VertexShader;
			}

			//---------------------------------------------------------------------
			const PixelShader* DX12ShaderBind::GetPixelShader()
			{
				return m_pPixelShader;
			}

			//---------------------------------------------------------------------
			const VertexShader* DX12ShaderBind::GetVertexShader()
			{
				return m_pVertexShader;
			}
		}
	}
}