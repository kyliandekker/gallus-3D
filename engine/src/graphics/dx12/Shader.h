#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "core/EngineResource.h"

#include <string>
#include <wrl.h>
#include <memory>

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
			class Shader : public core::EngineResource
			{
			public:
				/// <summary>
				/// Constructs an empty shader resource.
				/// </summary>
				Shader() = default;

				/// <summary>
				/// Binds the shader pipeline state to the given command list for rendering.
				/// </summary>
				/// <param name="a_pCommandList">The command list used to bind the shader.</param>
				void Bind(std::shared_ptr<CommandList> a_pCommandList);

				/// <summary>
				/// Compiles a shader from a source file using the specified entry point and target profile.
				/// </summary>
				/// <param name="a_sFilePath">The path to the shader source file.</param>
				/// <param name="a_sEntryPoint">The entry point function of the shader (e.g., "main").</param>
				/// <param name="a_sTarget">The shader model target (e.g., "vs_5_0", "ps_5_0").</param>
				/// <returns>A COM pointer to the compiled shader blob.</returns>
				static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const fs::path& a_sFilePath, const std::string& a_sEntryPoint, const std::string& a_sTarget);

				/// <summary>
				/// Checks if the shader has been successfully created and is valid for rendering.
				/// </summary>
				/// <returns>True if the shader has a valid pipeline state object, false otherwise.</returns>
				bool IsValid() const
				{
					return m_pPipelineState.Get();
				};

				/// <summary>
				/// Gets the file path of the currently bound pixel shader.
				/// </summary>
				/// <returns>Reference to the pixel shader path string.</returns>
				const std::string& GetPixelPath() const;

				/// <summary>
				/// Gets the file path of the currently bound vertex shader.
				/// </summary>
				/// <returns>Reference to the vertex shader path string.</returns>
				const std::string& GetVertexPath() const;

				/// <summary>
				/// Loads and compiles shaders using logical resource names, resolving them through the engine’s resource system.
				/// </summary>
				/// <param name="a_sVertexShader">The resource name of the vertex shader.</param>
				/// <param name="a_sPixelShader">The resource name of the pixel shader.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByName(const std::string& a_sVertexShader, const std::string& a_sPixelShader);

				/// <summary>
				/// Loads and compiles shaders directly from file paths, then creates the pipeline state object.
				/// </summary>
				/// <param name="a_VertexShaderPath">The file path to the vertex shader source.</param>
				/// <param name="a_PixelShaderPath">The file path to the pixel shader source.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByPath(const fs::path& a_VertexShaderPath, const fs::path& a_PixelShaderPath);
			private:
				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;
				std::string m_sPixelName;

				friend class ResourceAtlas;
			};
		}
	}
}