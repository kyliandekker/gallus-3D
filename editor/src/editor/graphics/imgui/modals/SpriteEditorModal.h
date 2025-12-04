#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/modals/BaseModal.h"

// external
#include <string>
#include <vector>
#include <functional>
#include <memory>

// graphics
#include "graphics/imgui/views/DataTypes/StringTextInput.h"

// resources
#include "resources/AssetType.h"

namespace gallus
{
	namespace resources
	{
		class TextureMetaData;
	}
	namespace resources
	{
		class FileResource;
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

			class FileEditorSelectable;

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
				/// <param name="a_FileResource">The sprite file.</param>
				void SetData(resources::FileResource& a_FileResource);
				void RenderFloatingToolbar(std::shared_ptr<graphics::dx12::Texture> a_pTexture);
			private:
				void RenderToolbar(std::shared_ptr<graphics::dx12::Texture> a_pTexture);
				void HandleGlobalControls(const ImVec2& a_vWindowSize, std::shared_ptr<graphics::dx12::Texture> a_pTexture);
				void RenderRect(int8_t a_iIndex, const ImVec2& a_vImgMin, const ImVec2& a_vImgMax);
				void HandleRectSelection(int8_t a_iIndex, const ImVec2& a_vImgMin, const ImVec2& a_vImgMax, std::shared_ptr<graphics::dx12::Texture> a_pTexture);
				void GetSpriteMinMax(int8_t a_iIndex, ImVec2& a_vMin, ImVec2& a_vMax, const ImVec2& a_vImgMin, const ImVec2& a_vImgMax);
				bool InteractWithRect(const ImVec2& a_vImgMin, const ImVec2& a_vImgMax, std::shared_ptr<graphics::dx12::Texture> a_pTexture);
				void DrawRectInteraction(const ImVec2& a_vImgMin, const ImVec2& a_vImgMax, std::shared_ptr<graphics::dx12::Texture> a_pTexture);

				int8_t m_iCurrentSprite = -1;

				float m_fZoom = 1.0f;
				ImVec2 m_vPanOffset = ImVec2(0.0f, 0.0f);
				bool m_bDragging = false;
				bool m_bResizing = false;
				int8_t m_iDragIndex = -1;

				std::weak_ptr<graphics::dx12::Texture> m_pPreviewTexture = {};

				resources::TextureMetaData* m_pTextureMetaData = nullptr;
				resources::FileResource* m_pFileResource = nullptr;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE