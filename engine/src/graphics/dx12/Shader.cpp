#include "Shader.h"

// core
#include "core/Data.h"

// logger
#include "logger/Logger.h"

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	// Shader
	//---------------------------------------------------------------------
	Microsoft::WRL::ComPtr<ID3DBlob> Shader::CompileShader(const core::Data& a_Data, const std::string& a_EntryPoint, const std::string& a_Target)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> blob;

		HRESULT hr = D3DCreateBlob(a_Data.size(), &blob);
		if (FAILED(hr))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to compile shader \"%s\": Could not create blob.", m_sName.c_str());
			return nullptr;
		}

		memcpy(blob->GetBufferPointer(), a_Data.data(), a_Data.size());
		return blob;
	}

	//---------------------------------------------------------------------
	bool PixelShader::LoadData(const core::Data& a_Data)
	{
		m_AssetType = resources::AssetType::PixelShader;
		
		m_pShaderBlob = CompileShader(a_Data, "main", "ps_6_0");
		
		return IsValid();
	}

	//---------------------------------------------------------------------
	bool VertexShader::LoadData(const core::Data& a_Data)
	{
		m_AssetType = resources::AssetType::PixelShader;
		
		m_pShaderBlob = CompileShader(a_Data, "main", "vs_6_0");
		
		return IsValid();
	}
}