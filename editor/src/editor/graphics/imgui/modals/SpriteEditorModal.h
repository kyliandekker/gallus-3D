#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/modals/BaseModal.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "graphics/imgui/views/DataTypes/StringTextInput.h"
#include "editor/AssetType.h"

namespace gallus
{
	namespace editor
	{
		class TextureMetaData;
	}
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
			class DX12Resource;
		}

		namespace imgui
		{
			class ImGuiWindow;

			class ExplorerFileUIView;

			class SpriteEditorModal : public BaseModal
			{
			public:
				/// <summary>
				/// Constructs a window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				SpriteEditorModal(ImGuiWindow& a_Window);

				void LoadTexture(const std::string& a_sName);

				/// <summary>
				/// Renders the element.
				/// </summary>
				void Render() override;

				/// <summary>
				/// Sets the data of the sprite editor modal.
				/// </summary>
				/// <param name="a_TextureMetaData">The texture meta data.</param>
				void SetData(editor::TextureMetaData& a_TextureMetaData);
			private:
				std::shared_ptr<graphics::dx12::Texture> m_pPreviewTexture = nullptr;

				editor::TextureMetaData* m_pTextureMetaData;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE