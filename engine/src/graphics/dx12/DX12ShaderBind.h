#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "resources/EngineResource.h"

// external
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

			class DX12ShaderBind : public resources::EngineResource
			{
			public:
				/// <summary>
				/// Constructs an empty shader resource.
				/// </summary>
				DX12ShaderBind()
				{
					m_AssetType = resources::AssetType::ShaderBind;
				}

				/// <summary>
				/// Constructs a shader resource.
				/// </summary>
				bool LoadByName(const std::string& a_sName, std::shared_ptr<PixelShader> a_pPixelShader, std::shared_ptr<VertexShader> a_pVertexShader);

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
				/// <param name="a_pPixelShader">The pixel shader to check.</param>
				/// <returns>True if the resource has a pixel shader, false otherwise.</returns>
				bool HasPixelShader(std::weak_ptr<PixelShader> a_pPixelShader);

				/// <summary>
				/// Checks the resource has a given vertex shader.
				/// </summary>
				/// <param name="a_pVertexShader">The vertex shader to check.</param>
				/// <returns>True if the resource has a vertex shader, false otherwise.</returns>
				bool HasVertexShader(std::weak_ptr<VertexShader> a_pVertexShader);

				/// <summary>
				/// Returns the pixel shader used in the shader bind.
				/// </summary>
				/// <returns>Pointer to the pixel shader (can be nullptr).</returns>
				const std::weak_ptr<PixelShader> GetPixelShader();

				/// <summary>
				/// Returns the vertex shader used in the shader bind.
				/// </summary>
				/// <returns>Pointer to the vertex shader (can be nullptr).</returns>
				const std::weak_ptr<VertexShader> GetVertexShader();
			private:
				std::shared_ptr<PixelShader> m_pPixelShader = {};
				std::shared_ptr<VertexShader> m_pVertexShader = {};

				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;

#ifdef _EDITOR
				BEGIN_EXPOSE_FIELDS_PARENT(DX12ShaderBind, resources::EngineResource)
					EXPOSE_FIELD(DX12ShaderBind, m_pPixelShader, "Pixel Shader", (FieldOptions{ .type = EditorFieldWidgetType::AssetPickerPtr, .assetType = resources::AssetType::PixelShader, .description = "Pointer to the pixel shader asset used for rendering this object. Can be nullptr if no specific pixel shader is assigned." }))
					EXPOSE_FIELD(DX12ShaderBind, m_pVertexShader, "Vertex Shader", (FieldOptions{ .type = EditorFieldWidgetType::AssetPickerPtr, .assetType = resources::AssetType::VertexShader, .description = "Pointer to the vertex shader asset used for rendering this object. Can be nullptr if no specific vertex shader is assigned." }))
				END_EXPOSE_FIELDS(DX12ShaderBind)
				BEGIN_EXPOSE_GIZMOS(DX12ShaderBind)
				END_EXPOSE_GIZMOS(DX12ShaderBind)
				END_EXPOSE_TO_EDITOR(DX12ShaderBind)
#endif
			};
		}
	}
}