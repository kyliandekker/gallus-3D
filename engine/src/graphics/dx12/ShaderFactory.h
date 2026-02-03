#pragma once

#include "graphics/dx12/DX12PCH.h"

// external
#include <string>
#include <unordered_map>

#include "core/Engine.h"

#include "graphics/dx12/Shader.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			struct PSOKey
			{
				std::weak_ptr<PixelShader> pixelShader;
				std::weak_ptr<VertexShader> vertexShader;

				bool operator==(const PSOKey& other) const
				{
					return pixelShader.lock().get() == other.pixelShader.lock().get() &&
						vertexShader.lock().get() == other.vertexShader.lock().get();
				}
			};

			struct PSOKeyHash
			{
				size_t operator()(const PSOKey& key) const
				{
					std::shared_ptr<PixelShader> ps = key.pixelShader.lock();
					std::shared_ptr<VertexShader> vs = key.vertexShader.lock();
					return std::hash<PixelShader*>()(ps.get()) ^ std::hash<VertexShader*>()(vs.get());
				}
			};

			class PipelineStateCache
			{
				inline static std::unordered_map<PSOKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PSOKeyHash> m_mCache = {};
			public:
				static ID3D12PipelineState* GetOrCreate(std::weak_ptr<PixelShader> a_pPixelShader, std::weak_ptr<VertexShader> a_pVertexShader, DXGI_FORMAT a_DSVFormat)
				{
					PSOKey key{ a_pPixelShader, a_pVertexShader };
					auto it = m_mCache.find(key);
					if (it != m_mCache.end())
					{
						return it->second.Get();
					}

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

					D3D12_RT_FORMAT_ARRAY rtvFormats = {};
					rtvFormats.NumRenderTargets = 1;
					rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

					CD3DX12_DEPTH_STENCIL_DESC depthDesc(D3D12_DEFAULT);
					if (a_DSVFormat != DXGI_FORMAT_UNKNOWN)
					{
						depthDesc.DepthEnable = TRUE;
						depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
						depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
						depthDesc.StencilEnable = FALSE;
					}
					else
					{
						depthDesc.DepthEnable = FALSE;
					}

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
					};

					std::shared_ptr<PixelShader> ps = key.pixelShader.lock();
					std::shared_ptr<VertexShader> vs = key.vertexShader.lock();

					if (!ps || !vs)
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating pipeline state because pixel shader or vertex shader were null.");
						return nullptr;
					}

					graphics::dx12::DX12System* dx12System = core::ENGINE->GetDX12();
					if (!dx12System)
					{
						return nullptr;
					}

					PipelineStateStream pipelineStateStream;
					pipelineStateStream.pRootSignature = dx12System->GetRootSignature().Get();
					pipelineStateStream.InputLayout = { g_aInputLayout, _countof(g_aInputLayout) };
					pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
					pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(ps->GetShaderBlob().Get());
					pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vs->GetShaderBlob().Get());
					pipelineStateStream.DSVFormat = a_DSVFormat;
					pipelineStateStream.RTVFormats = rtvFormats;
					pipelineStateStream.RasterizerState = rasterDesc;
					pipelineStateStream.BlendState = CD3DX12_BLEND_DESC(blendDesc);
					if (a_DSVFormat != DXGI_FORMAT_UNKNOWN)
					{
						pipelineStateStream.DepthStencilState = CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL(depthDesc);
					}

					D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
					desc.SizeInBytes = sizeof(pipelineStateStream);
					desc.pPipelineStateSubobjectStream = &pipelineStateStream;

					ID3D12PipelineState* pso = nullptr;
					if (FAILED(dx12System->GetDevice()->CreatePipelineState(&desc, IID_PPV_ARGS(&pso))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating pipeline state.");
						return nullptr;
					}

					m_mCache[key] = pso;
					return pso;
				}
			};
		}
	}
}