#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "core/EngineResource.h"

#include <wrl.h>
#include <memory>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;
			class PixelShader;
			class VertexShader;

			class DX12ShaderBind : public core::EngineResource
			{
			public:
				/// <summary>
				/// Constructs an empty shader resource.
				/// </summary>
				DX12ShaderBind();

				/// <summary>
				/// Constructs a shader resource.
				/// </summary>
				DX12ShaderBind(const PixelShader* a_PixelShader, const VertexShader* a_VertexShader);

				/// <summary>
				/// Creates the pipe line state.
				/// </summary>
				bool CreatePipelineState();

				/// <summary>
				/// Binds the shader pipeline state to the given command list for rendering.
				/// </summary>
				/// <param name="a_pCommandList">The command list used to bind the shader.</param>
				void Bind(std::shared_ptr<CommandList> a_pCommandList);

				/// <summary>
				/// Checks if the shader has been successfully created and is valid for rendering.
				/// </summary>
				/// <returns>True if the shader has a valid pipeline state object, false otherwise.</returns>
				bool IsValid() const override
				{
					return m_pPipelineState.Get();
				};

				/// <summary>
				/// Returns the pipe line state.
				/// </summary>
				/// <returns>Pointer to the pipe line state (can be nullptr).</returns>
				ID3D12PipelineState* GetPipelineState()
				{
					return m_pPipelineState.Get();
				}

				/// <summary>
				/// Checks the resource has a given pixel shader.
				/// </summary>
				/// <param name="a_PixelShader">The pixel shader to check.</param>
				/// <returns>True if the resource has a pixel shader, false otherwise.</returns>
				bool HasPixelShader(const PixelShader* a_PixelShader);

				/// <summary>
				/// Checks the resource has a given vertex shader.
				/// </summary>
				/// <param name="a_PixelShader">The vertex shader to check.</param>
				/// <returns>True if the resource has a vertex shader, false otherwise.</returns>
				bool HasVertexShader(const VertexShader* a_VertexShader);

				/// <summary>
				/// Returns the pixel shader used in the shader bind.
				/// </summary>
				/// <returns>Pointer to the pixel shader (can be nullptr).</returns>
				const PixelShader* GetPixelShader();

				/// <summary>
				/// Returns the vertex shader used in the shader bind.
				/// </summary>
				/// <returns>Pointer to the vertex shader (can be nullptr).</returns>
				const VertexShader* GetVertexShader();
			private:
				const PixelShader* m_pPixelShader = nullptr;
				const VertexShader* m_pVertexShader = nullptr;

				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;
			};
		}
	}
}