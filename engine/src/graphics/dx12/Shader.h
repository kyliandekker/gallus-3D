#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "resources/EngineResource.h"

#include <string>
#include <wrl.h>

// utils includes
#include "utils/file_abstractions.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
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
				/// <param name="a_sFilePath">The path to the shader source file.</param>
				/// <param name="a_sEntryPoint">The entry point function of the shader (e.g., "main").</param>
				/// <param name="a_sTarget">The shader model target (e.g., "vs_5_0", "ps_5_0").</param>
				/// <returns>A COM pointer to the compiled shader blob.</returns>
				static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const fs::path& a_sFilePath, const std::string& a_sEntryPoint, const std::string& a_sTarget);

				/// <summary>
				/// Loads and compiles shaders using logical resource names, resolving them through the engine�s resource system.
				/// </summary>
				/// <param name="a_sShader">The resource name of the shader.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				virtual bool LoadByName(const std::string& a_sShader);

				/// <summary>
				/// Loads and compiles shaders directly from file paths.
				/// </summary>
				/// <param name="a_ShaderPath">The file path to the shader source.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				virtual bool LoadByPath(const fs::path& a_ShaderPath);

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
#ifdef _EDITOR
				BEGIN_EXPOSE_FIELDS_PARENT(Shader, resources::EngineResource)
				END_EXPOSE_FIELDS(Shader)
				BEGIN_EXPOSE_GIZMOS(Shader)
				END_EXPOSE_GIZMOS(Shader)
				END_EXPOSE_TO_EDITOR(Shader)
#endif
			};

			class PixelShader : public Shader
			{
			public:
				/// <summary>
				/// Constructs an empty pixel shader resource.
				/// </summary>
				PixelShader() = default;

				/// <summary>
				/// Loads and compiles shaders using logical resource names, resolving them through the engine�s resource system.
				/// </summary>
				/// <param name="a_sShader">The resource name of the pixel shader.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByName(const std::string& a_sPixelShader) override;

				/// <summary>
				/// Loads and compiles shaders directly from file paths.
				/// </summary>
				/// <param name="a_PixelShaderPath">The file path to the pixel shader source.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByPath(const fs::path& a_PixelShaderPath) override;
			protected:
#ifdef _EDITOR
				BEGIN_EXPOSE_FIELDS_PARENT(PixelShader, Shader)
				END_EXPOSE_FIELDS(PixelShader)
				BEGIN_EXPOSE_GIZMOS(PixelShader)
				END_EXPOSE_GIZMOS(PixelShader)
				END_EXPOSE_TO_EDITOR(PixelShader)
#endif
			};

			class VertexShader : public Shader
			{
			public:
				/// <summary>
				/// Constructs an empty pixel shader resource.
				/// </summary>
				VertexShader() = default;

				/// <summary>
				/// Loads and compiles shaders using logical resource names, resolving them through the engine�s resource system.
				/// </summary>
				/// <param name="a_sShader">The resource name of the vertex shader.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByName(const std::string& a_sVertexShader) override;

				/// <summary>
				/// Loads and compiles shaders directly from file paths.
				/// </summary>
				/// <param name="a_VertexShaderPath">The file path to the vertex shader source.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByPath(const fs::path& a_VertexShaderPath) override;
			protected:
#ifdef _EDITOR
				BEGIN_EXPOSE_FIELDS_PARENT(VertexShader, Shader)
				END_EXPOSE_FIELDS(VertexShader)
				BEGIN_EXPOSE_GIZMOS(VertexShader)
				END_EXPOSE_GIZMOS(VertexShader)
				END_EXPOSE_TO_EDITOR(VertexShader)
#endif
			};
		}
	}
}