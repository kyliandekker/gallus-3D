#pragma once

#include <memory>

#include "utils/math.h"

#include "imgui_system/windows/BaseWindow.h"

// editor
#include "editor/graphics/imgui/views/Toolbar.h"

namespace gallus::editor
{
	class IEditorSpriteSheetDataCommand;
}
namespace gallus::graphics::dx12
{
	class Texture;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	//---------------------------------------------------------------------
	// SpriteSheetEditorWindow
	//---------------------------------------------------------------------
	/// <summary>
	/// A window that allows editing of sprite sheets.
	/// </summary>
	class SpriteSheetEditorWindow : public BaseWindow
	{
	public:
		/// <summary>
		/// Constructs a sprite sheet editor window.
		/// </summary>
		/// <param name="a_System">The ImGui System for rendering the view.</param>
		SpriteSheetEditorWindow(ImGuiSystem& a_System);
		
		/// <summary>
		/// Initializes all values and behaviours associated with the console window.
		/// </summary>
		/// <returns>True if initialization is successful, otherwise false.</returns>
		bool Initialize() override;

		/// <summary>
		/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
		/// </summary>
		void Update() override;

		/// <summary>
		/// Renders the sprite sheet editor window.
		/// </summary>
		void Render() override;

		void SetData(const std::string& a_sID);
	private:
		// Toolbar.
		void PopulateToolbar();
		void DrawToolbar();

		void RegisterKeybinds();

		void DrawViewportPanel();

		Toolbar m_Toolbar;

		std::weak_ptr<graphics::dx12::Texture> m_pTexture;
		int m_iSelectedRect = -1; // -1 = no selection

		IVector2 m_vCellSize = IVector2(32, 32);

		std::unique_ptr<editor::IEditorSpriteSheetDataCommand> m_pEditorCommand;
	};
}