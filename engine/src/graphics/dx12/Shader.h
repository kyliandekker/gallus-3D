#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "resources/EngineResource.h"

// external
#include <string>
#include <wrl.h>

namespace gallus::graphics::dx12
{
	class CommandList;

	//---------------------------------------------------------------------
	// Shader
	//---------------------------------------------------------------------
	/// <summary>
	/// Represents a GPU shader program resource in the engine.
	/// Wraps vertex and pixel shaders, compiling them and creating a pipeline state object (PSO).
	/// </summary>
	class Shader : public resources::EngineResource
	{
	public:
		/// <summary>
		/// Constructs an empty shader resource.
		/// </summary>
		Shader() = default;

		/// <summary>
		/// Compiles a shader from a source file using the specified entry point and target profile.
		/// </summary>
		/// <param name="a_Data">The shader data.</param>
		/// <param name="a_sEntryPoint">The entry point function of the shader (e.g., "main").</param>
		/// <param name="a_sTarget">The shader model target (e.g., "vs_5_0", "ps_5_0").</param>
		/// <returns>A COM pointer to the compiled shader blob.</returns>
		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const core::Data& a_Data, const std::string& a_sEntryPoint, const std::string& a_sTarget);

		/// <summary>
		/// Retrieves the shader blob data.
		/// </summary>
		/// <returns>Pointer to the shader blob data.</returns>
		const Microsoft::WRL::ComPtr<ID3DBlob> GetShaderBlob() const
		{
			return m_pShaderBlob;
		}

		/// <summary>
		/// Returns whether the resource is a valid resource.
		/// </summary>
		/// <returns>True if the resource was valid, false otherwise.</returns>
		bool IsValid() const override
		{
			return m_pShaderBlob;
		}
	protected:
		Microsoft::WRL::ComPtr<ID3DBlob> m_pShaderBlob = nullptr;

		BEGIN_SERIALIZE_PARENT(Shader, resources::EngineResource)
		END_SERIALIZE(Shader)
	};

	class PixelShader : public Shader
	{
	public:
		/// <summary>
		/// Constructs an empty pixel shader resource.
		/// </summary>
		PixelShader() : Shader()
		{
			m_AssetType = resources::AssetType::PixelShader;
		}

		/// <summary>
		/// Loads the shader data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data) override;
	protected:
		BEGIN_SERIALIZE_PARENT(PixelShader, Shader)
		END_SERIALIZE(PixelShader)
	};

	class VertexShader : public Shader
	{
	public:
		/// <summary>
		/// Constructs an empty pixel shader resource.
		/// </summary>
		VertexShader() : Shader()
		{
			m_AssetType = resources::AssetType::VertexShader;
		}

		/// <summary>
		/// Loads the shader data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data) override;
	protected:
		BEGIN_SERIALIZE_PARENT(VertexShader, Shader)
		END_SERIALIZE(VertexShader)
	};
}