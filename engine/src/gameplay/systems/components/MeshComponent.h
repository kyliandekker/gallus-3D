#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <memory>
#include <glm/vec4.hpp>

// graphics
#include "graphics/dx12/DX12PCH.h"

// resources
#include "resources/AssetType.h"

#include "graphics/dx12/Material.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class CommandList;

			class Transform;
			class Texture;
			class ShaderBind;
			class Mesh;
			class Camera;
		}
	}
	namespace gameplay
	{
		struct EntityID;

		//---------------------------------------------------------------------
		// MeshComponent
		//---------------------------------------------------------------------
		class MeshComponent : public Component
		{
		public:
			/// <summary>
			/// Initializes the component.
			/// </summary>
			void SetDefaults(const gameplay::EntityID& a_EntityID) override;

			/// <summary>
			/// Sets the mesh used by the mesh component.
			/// </summary>
			/// <param name="a_Mesh">Reference to the mesh that the mesh component will use.</param>
			void SetMesh(std::weak_ptr<graphics::dx12::Mesh> a_pMesh);

			/// <summary>
			/// Sets the shader used by the mesh component.
			/// </summary>
			/// <param name="a_Shader">Reference to the shader bind that the mesh component will use.</param>
			void SetShader(std::weak_ptr < graphics::dx12::ShaderBind> a_pShaderBind);

			/// <summary>
			/// Sets the texture used by the mesh component.
			/// </summary>
			/// <param name="a_Texture">Reference to the texture that the mesh component will use.</param>
			void SetTexture(std::weak_ptr < graphics::dx12::Texture> a_pTexture);

			/// <summary>
			/// Sets the texture index.
			/// </summary>
			/// <param name="a_iTextureIndex">The index the texture should have.</param>
			void SetTextureIndex(int8_t a_iTextureIndex);

			/// <summary>
			/// Renders the mesh.
			/// </summary>
			/// <param name="a_pCommandList">The command list used for rendering.</param>
			/// <param name="a_EntityID">The entity ID to get the data from.</param>
			/// <param name="a_Camera">The camera.</param>
			void Render(std::shared_ptr<graphics::dx12::CommandList> a_pCommandList, const EntityID& a_EntityID, const graphics::dx12::Camera& a_Camera);
		private:
			std::weak_ptr<graphics::dx12::Mesh> m_pMesh = {};
			std::weak_ptr<graphics::dx12::ShaderBind> m_pShaderBind = {};
			std::weak_ptr<graphics::dx12::Texture> m_pTexture = {};
			int8_t m_iTextureIndex = 0;
			std::weak_ptr<graphics::dx12::Material> m_pMaterial = {};
			DirectX::XMFLOAT4 m_vColor = { 1, 1, 1, 1 };

			BEGIN_EXPOSE_FIELDS_PARENT(MeshComponent, Component)
				EXPOSE_FIELD(MeshComponent, m_pShaderBind, "Shader Bind", "",
					(FieldOptions{ 
						.type = EditorFieldWidgetType::EditorFieldWidgetType_ObjectPtr 
					}))
				EXPOSE_FIELD(MeshComponent, m_pMaterial, "Material", "",
					(FieldOptions{ 
						.type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource, 
						.assetType = resources::AssetType::Material,
					}))
				EXPOSE_FIELD(MeshComponent, m_pMaterial, "MaterialView", "",
					(FieldOptions{ 
						.type = EditorFieldWidgetType::EditorFieldWidgetType_ObjectPtr, 
						.assetType = resources::AssetType::Material,
						.serialize = false,
						.indent = 1
					}))
				EXPOSE_FIELD(MeshComponent, m_pMesh, "Mesh", "Pointer to the texture asset used by this Mesh. Can be nullptr if no texture is assigned. Determines the visual appearance of the Mesh.",
					(FieldOptions{ 
						.type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource, 
						.assetType = resources::AssetType::Mesh, 
					}))
				EXPOSE_FIELD(MeshComponent, m_pTexture, "Texture", "Pointer to the mesh asset used by this Mesh. Can be nullptr if no texture is assigned. Determines the visual appearance of the Mesh.",
					(FieldOptions{ 
						.type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource, 
						.assetType = resources::AssetType::Sprite, 
					}))
				EXPOSE_FIELD(MeshComponent, m_iTextureIndex, "Texture Index", "Index of the Mesh within a texture atlas. Used when the texture contains multiple rects to select which one is displayed.",
					(FieldOptions{ 
						.type = EditorFieldWidgetType::EditorFieldWidgetType_Int8, 
					}))
				EXPOSE_FIELD(MeshComponent, m_pTexture, "Texture Preview", "",
					(FieldOptions{
						.type = EditorFieldWidgetType::EditorFieldWidgetType_TexturePreview,
						.relatedIndexFieldOffset = offsetof(MeshComponent, m_iTextureIndex)
					}))
			END_EXPOSE_FIELDS(MeshComponent)
			BEGIN_EXPOSE_GIZMOS(MeshComponent)
			END_EXPOSE_GIZMOS(MeshComponent)
			END_EXPOSE_TO_EDITOR(MeshComponent)
		};
	}
}