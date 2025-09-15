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

				ID3D12PipelineState* GetPipelineState()
				{
					return m_pPipelineState.Get();
				}

				bool HasPixelShader(const PixelShader* a_PixelShader);
				bool HasVertexShader(const VertexShader* a_VertexShader);

				const PixelShader* GetPixelShader();
				const VertexShader* GetVertexShader();
			private:
				const PixelShader* m_pPixelShader;
				const VertexShader* m_pVertexShader;

				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;
			};
		}
	}
}