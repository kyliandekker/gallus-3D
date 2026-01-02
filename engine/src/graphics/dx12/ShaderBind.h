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

			class ShaderBind : public resources::EngineResource
			{
			public:
				/// <summary>
				/// Constructs an empty shader resource.
				/// </summary>
				ShaderBind() : resources::EngineResource()
				{
					m_AssetType = resources::AssetType::ShaderBind;
				}

				/// <summary>
				/// Constructs a shader resource.
				/// </summary>
				bool LoadByName(const std::string& a_sName, std::shared_ptr<PixelShader> a_pPixelShader, std::shared_ptr<VertexShader> a_pVertexShader, CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT a_DSVFormat = DXGI_FORMAT_D32_FLOAT);

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
				
				CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT m_DSVFormat = DXGI_FORMAT_UNKNOWN;

				Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;

				BEGIN_SERIALIZE_PARENT(ShaderBind, resources::EngineResource)
				// 	SERIALIZE_FIELD(m_pPixelShader, "Pixel Shader", "Pointer to the pixel shader asset used for rendering this object. Can be nullptr if no specific pixel shader is assigned.",
				// 		.type = FieldSerializationType::FieldSerializationType_EngineResource,
				// 		.assetType = resources::AssetType::PixelShader)
				// 	SERIALIZE_FIELD(m_pVertexShader, "Vertex Shader", "Pointer to the vertex shader asset used for rendering this object. Can be nullptr if no specific vertex shader is assigned.",
				// 		.type = FieldSerializationType::FieldSerializationType_EngineResource,
				// 		.assetType = resources::AssetType::VertexShader)
				END_SERIALIZE(ShaderBind)
			};
		}
	}
}