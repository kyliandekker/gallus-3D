#include "graphics/dx12/Shader.h"

#include "logger/Logger.h"
#include "core/Tool.h"
#include "graphics/dx12/CommandList.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Shader
			//---------------------------------------------------------------------
			void Shader::Bind(std::shared_ptr<CommandList> a_CommandList)
			{
				a_CommandList->GetCommandList()->SetPipelineState(m_pPipelineState.Get());
				a_CommandList->GetCommandList()->SetGraphicsRootSignature(core::TOOL->GetDX12().GetRootSignature().Get()); // Set the existing root signature
			}

			//---------------------------------------------------------------------
			Microsoft::WRL::ComPtr<ID3DBlob> Shader::CompileShader(const fs::path& a_FilePath, const std::string& a_EntryPoint, const std::string& a_Target)
			{
				Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
				Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
				const std::wstring wFilePath = a_FilePath.generic_wstring();
				HRESULT hr = D3DCompileFromFile(
					wFilePath.c_str(),
					nullptr,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					a_EntryPoint.c_str(),
					a_Target.c_str(),
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
					0,
					&shaderBlob,
					&errorBlob
				);
				if (FAILED(hr))
				{
					if (errorBlob)
					{
						std::string errorMessage(static_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
						LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Shader Compilation Error: \"%s\".", errorMessage.c_str());
					}
					else
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading vertex shader.");
					}
					return nullptr;
				}

				LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Loaded shader: \"%s\".", a_FilePath.generic_string().c_str());
				return shaderBlob;
			}

			//---------------------------------------------------------------------
			const std::string& Shader::GetPixelPath() const
			{
				return m_sPixelName;
			}

			//---------------------------------------------------------------------
			const std::string& Shader::GetVertexPath() const
			{
				return m_sName;
			}

			// TODO: This does nothing right now.
			//---------------------------------------------------------------------
			bool Shader::LoadByName(const std::string& a_sVertexShader, const std::string& a_sPixelShader)
			{
				m_ResourceType = core::ResourceType::ResourceType_Shader;

				return false;
			}

			//---------------------------------------------------------------------
			bool Shader::LoadByPath(const fs::path& a_VertexShaderPath, const fs::path& a_PixelShaderPath)
			{
				m_Path = a_VertexShaderPath;
				m_sName = a_VertexShaderPath.filename().generic_string();
				m_sPixelName = a_PixelShaderPath.filename().generic_string();

				const Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob = CompileShader(a_VertexShaderPath, "main", "vs_5_1");
				const Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob = CompileShader(a_PixelShaderPath, "main", "ps_5_1");

				if (!vertexShaderBlob || !pixelShaderBlob)
				{
					return false;
				}

				// Create the vertex input layout
				const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				};

				CD3DX12_RASTERIZER_DESC rasterDesc(D3D12_DEFAULT);
				rasterDesc.CullMode = D3D12_CULL_MODE_NONE;

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
				} pipelineStateStream;

				D3D12_RT_FORMAT_ARRAY rtvFormats = {};
				rtvFormats.NumRenderTargets = 1;
				rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

				pipelineStateStream.pRootSignature = core::TOOL->GetDX12().GetRootSignature().Get();
				pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
				pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
				pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
				pipelineStateStream.DSVFormat = DXGI_FORMAT_UNKNOWN;
				pipelineStateStream.RTVFormats = rtvFormats;
				pipelineStateStream.RasterizerState = rasterDesc;

				D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
					sizeof(PipelineStateStream), &pipelineStateStream
				};
				if (FAILED(core::TOOL->GetDX12().GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating pipeline state.");
					return false;
				}

				m_ResourceType = core::ResourceType::ResourceType_Shader;
				return true;
			}
		}
	}
}