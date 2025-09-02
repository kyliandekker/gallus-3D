#include "graphics/dx12/Shader.h"

// logger includes
#include "logger/Logger.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Shader
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

			// TODO: This does nothing right now.
			//---------------------------------------------------------------------
			bool Shader::LoadByName(const std::string& a_sShader)
			{
				return false;
			}

			//---------------------------------------------------------------------
			bool Shader::LoadByPath(const fs::path& a_ShaderPath)
			{
				m_Path = a_ShaderPath;
				m_sName = a_ShaderPath.filename().generic_string();

				return true;
			}

			//---------------------------------------------------------------------
			bool PixelShader::LoadByName(const std::string& a_sPixelShader)
			{
				if (Shader::LoadByName(a_sPixelShader))
				{
					m_ResourceType = core::ResourceType::ResourceType_PixelShader;

					m_pShaderBlob = CompileShader(a_sPixelShader, "main", "ps_5_1");

					return IsValid();
				}
				return false;
			}

			//---------------------------------------------------------------------
			bool PixelShader::LoadByPath(const fs::path& a_PixelShaderPath)
			{
				if (Shader::LoadByPath(a_PixelShaderPath))
				{
					m_ResourceType = core::ResourceType::ResourceType_PixelShader;

					m_pShaderBlob = CompileShader(a_PixelShaderPath, "main", "ps_5_1");
					return IsValid();
				}
				return false;
			}

			//---------------------------------------------------------------------
			bool VertexShader::LoadByName(const std::string& a_sVertexShader)
			{
				if (Shader::LoadByName(a_sVertexShader))
				{
					m_ResourceType = core::ResourceType::ResourceType_VertexShader;

					m_pShaderBlob = CompileShader(a_sVertexShader, "main", "vs_5_1");

					return IsValid();
				}
				return false;
			}

			//---------------------------------------------------------------------
			bool VertexShader::LoadByPath(const fs::path& a_VertexShaderPath)
			{
				if (Shader::LoadByPath(a_VertexShaderPath))
				{
					m_ResourceType = core::ResourceType::ResourceType_VertexShader;

					m_pShaderBlob = CompileShader(a_VertexShaderPath, "main", "vs_5_1");

					return IsValid();
				}
				return false;
			}
		}
	}
}