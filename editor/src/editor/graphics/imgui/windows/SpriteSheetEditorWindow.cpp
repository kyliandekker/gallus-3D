#include "SpriteSheetEditorWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/Texture.h"

// imgui
#include "imgui_system/font_icon.h"
#include "imgui_system/ImGuiSystem.h"

// resources
#include "resources/ResourceAtlas.h"
#include "resources/SrcData.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/EditorWorkspace.h"
#include "editor/Editor.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// SpriteSheetEditorWindow
	//---------------------------------------------------------------------
	SpriteSheetEditorWindow::SpriteSheetEditorWindow(ImGuiSystem& a_System) : BaseWindow(a_System, 0, std::string(font::ICON_IMAGE) + " Sprite Sheet Editor", "SpriteSheetEditor")
	{
		m_Flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
		m_bHideCloseButton = false; // Since this is a full screen window, enable the close button.
	}

	//---------------------------------------------------------------------
	bool SpriteSheetEditorWindow::Initialize()
	{
		PopulateToolbar();

		return BaseWindow::Initialize();
	}

	//---------------------------------------------------------------------
	void SpriteSheetEditorWindow::Update()
	{
		m_bFullScreen = true;
		if (GetEditorEngine().GetEditor()->GetEditorSettings().GetEditorState() != editor::EditorState::EditorState_SpriteSheetEditor)
		{
			return;
		}
		BaseWindow::Update();

		// Check if the close button was pressed, if so return to workspace.
		if (!m_bEnabled)
		{
			GetEditorEngine().GetEditor()->GetEditorSettings().SetEditorState(editor::EditorState::EditorState_Workspace);
		}
	}

	//---------------------------------------------------------------------
	void SpriteSheetEditorWindow::PopulateToolbar()
	{
		ImVec2 toolbarSize = ImVec2(0, m_System.GetHeaderSize().y);
		m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_SPRITE_SHEET_EDITOR"), toolbarSize);

		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
			[this]()
			{
				if (ImGui::TextButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_SPRITE_SHEET_EDITOR").c_str(), "Saves the current sprite sheet.", m_System.GetHeaderSize()))
				{
					if (std::shared_ptr<graphics::dx12::Texture> texture = m_pTexture.lock())
					{
						editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
						if (!editorWorkspace)
						{
							return;
						}

						resources::SrcData srcData;
						srcData.SetObject();
						SerializeFields(texture.get(), srcData);

						core::Data data;
						srcData.GetData(data);

						editorWorkspace->Save(texture->GetName(), data);
					}
				}
			}
		));

		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
			[this]()
			{
				ImGui::SetNextItemWidth(300);
				if (ImGui::IVectorEdit2(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "TOOLBAR_SPRITE_SHEET_EDITOR_CELL_SIZE"), (int*) &m_vCellSize))
				{
					if (std::shared_ptr<graphics::dx12::Texture> texture = m_pTexture.lock())
					{
						texture->SetSpriteSheetCellSize(m_vCellSize);
					}
				}
			}
		));
	}

	//---------------------------------------------------------------------
	void SpriteSheetEditorWindow::DrawToolbar()
	{
		// Start toolbar.
		m_Toolbar.StartToolbar();

		// Render toolbar.
		m_Toolbar.Render();

		// End toolbar.
		m_Toolbar.EndToolbar();
	}

	//---------------------------------------------------------------------
	void SpriteSheetEditorWindow::DrawViewportPanel()
	{
		const int buttons = 4;

		ImVec2 windowSize = {
			300,
			ImGui::GetContentRegionAvail().y
		};

		if (ImGui::BeginChild("Operations", windowSize, 0,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::SetCursorScreenPos(ImVec2(
				ImGui::GetCursorScreenPos().x,
				ImGui::GetCursorScreenPos().y + (windowSize.y / 2) - ((buttons * m_System.GetHeaderSize().y) / 2)
			));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

			editor::Editor* editor = GetEditorEngine().GetEditor();
			if (!editor)
			{
				return;
			}

			if (std::shared_ptr<graphics::dx12::Texture> texture = m_pTexture.lock())
			{
				graphics::dx12::TextureRect* rect = texture->GetTextureRect(m_iSelectedRect);

				if (!rect)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				IVector2 
					currentRectPos,
					currentRectSize;

				if (rect)
				{
					currentRectPos = {
						static_cast<int>(rect->x),
						static_cast<int>(rect->y)
					};
					currentRectSize = {
						static_cast<int>(rect->width),
						static_cast<int>(rect->height)
					};
				}

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::IVectorEdit2(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "TOOLBAR_SPRITE_SHEET_EDITOR_CURRENT_RECT_POS"), (int*) &currentRectPos))
				{
					if (rect)
					{
						rect->x = currentRectPos.x;
						rect->y = currentRectPos.y;
					}
				}

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::IVectorEdit2(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "TOOLBAR_SPRITE_SHEET_EDITOR_CURRENT_RECT_SIZE"), (int*) &currentRectSize))
				{
					if (rect)
					{
						rect->width = currentRectSize.x;
						rect->height = currentRectSize.y;
					}
				}

				if (ImGui::TextButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_DELETE) + " Delete Rect", BUTTON_ID, "TOOLBAR_SPRITE_SHEET_EDITOR_DELETE").c_str(), "Deletes the current selected sprite rect.", ImVec2(ImGui::GetContentRegionAvail().x, m_System.GetHeaderSize().y)))
				{
					texture->GetTextureRects().erase(texture->GetTextureRects().begin() + m_iSelectedRect);
				}

				if (!rect)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				if (ImGui::TextButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_GIZMO_BOUNDS) + " Add Rect", BUTTON_ID, "TOOLBAR_SPRITE_SHEET_EDITOR_ADD").c_str(), "Adds a new sprite rect.", ImVec2(ImGui::GetContentRegionAvail().x, m_System.GetHeaderSize().y)))
				{
					graphics::dx12::TextureRect rect = graphics::dx12::TextureRect();
					rect.width = m_vCellSize.x;
					rect.height = m_vCellSize.y;

					m_iSelectedRect = texture->AddTextureRect(rect);
				}
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}

	//---------------------------------------------------------------------
	void SpriteSheetEditorWindow::Render()
	{
		DrawToolbar();

		ImVec2 startPos = ImGui::GetCursorScreenPos();

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_System.GetFramePadding().x,
			ImGui::GetCursorPos().y + m_System.GetFramePadding().y));

		static ImVec2 vOffset = ImVec2(0.0f, 0.0f);
		static float fZoom = 1.0f;

		ImDrawList* pDrawList = ImGui::GetWindowDrawList();
		ImVec2 vCanvasPos = ImGui::GetCursorScreenPos();
		ImVec2 vCanvasSize = ImGui::GetContentRegionAvail();

		if (vCanvasSize.x < 50.0f) vCanvasSize.x = 50.0f;
		if (vCanvasSize.y < 50.0f) vCanvasSize.y = 50.0f;

		ImVec2 vCanvasEnd = ImVec2(vCanvasPos.x + vCanvasSize.x, vCanvasPos.y + vCanvasSize.y);

		ImGui::InvisibleButton("canvas", vCanvasSize, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);
		bool bHovered = ImGui::IsItemHovered();
		ImVec2 vMouse = ImGui::GetIO().MousePos;

		// PAN
		if (bHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
		{
			ImVec2 vDelta = ImGui::GetIO().MouseDelta;
			vOffset.x += vDelta.x;
			vOffset.y += vDelta.y;
		}

		// ZOOM
		if (bHovered)
		{
			float fWheel = ImGui::GetIO().MouseWheel;
			if (fWheel != 0.0f)
			{
				float fOldZoom = fZoom;
				fZoom *= (fWheel > 0.0f) ? 1.1f : 0.9f;
				fZoom = std::clamp(fZoom, 0.2f, 5.0f);

				ImVec2 vMouseLocal = ImVec2(vMouse.x - vCanvasPos.x - vOffset.x, vMouse.y - vCanvasPos.y - vOffset.y);
				float fWorldX = vMouseLocal.x / fOldZoom;
				float fWorldY = vMouseLocal.y / fOldZoom;

				vOffset.x = vMouse.x - vCanvasPos.x - fWorldX * fZoom;
				vOffset.y = vMouse.y - vCanvasPos.y - fWorldY * fZoom;
			}
		}

		//-----------------------------------------------------------------
		// DRAW BACKGROUND
		//-----------------------------------------------------------------
		pDrawList->AddRectFilled(vCanvasPos, vCanvasEnd, IM_COL32(40, 40, 40, 255));

		//-----------------------------------------------------------------
		// DRAW GRID
		//-----------------------------------------------------------------
		float fStartX = fmodf(vOffset.x, m_vCellSize.x * fZoom);
		float fStartY = fmodf(vOffset.y, m_vCellSize.y * fZoom);

		for (float x = fStartX; x < vCanvasSize.x; x += m_vCellSize.x * fZoom)
		{
			pDrawList->AddLine(ImVec2(vCanvasPos.x + x, vCanvasPos.y),
				ImVec2(vCanvasPos.x + x, vCanvasEnd.y),
				IM_COL32(80, 80, 80, 255));
		}

		for (float y = fStartY; y < vCanvasSize.y; y += m_vCellSize.y * fZoom)
		{
			pDrawList->AddLine(ImVec2(vCanvasPos.x, vCanvasPos.y + y),
				ImVec2(vCanvasEnd.x, vCanvasPos.y + y),
				IM_COL32(80, 80, 80, 255));
		}

		//-----------------------------------------------------------------
		// HOVER CELL
		//-----------------------------------------------------------------
		if (bHovered)
		{
			ImVec2 vLocal = ImVec2(vMouse.x - vCanvasPos.x - vOffset.x,
				vMouse.y - vCanvasPos.y - vOffset.y);

			int iCellX = (int) floor(vLocal.x / (m_vCellSize.x * fZoom));
			int iCellY = (int) floor(vLocal.y / (m_vCellSize.y * fZoom));

			ImVec2 vMin = ImVec2(vCanvasPos.x + vOffset.x + iCellX * m_vCellSize.x * fZoom,
				vCanvasPos.y + vOffset.y + iCellY * m_vCellSize.y * fZoom);
			ImVec2 vMax = ImVec2(vMin.x + m_vCellSize.x * fZoom, vMin.y + m_vCellSize.y * fZoom);

			pDrawList->AddRectFilled(vMin, vMax, IM_COL32(200, 200, 80, 80, 80));
			pDrawList->AddRect(vMin, vMax, IM_COL32(255, 255, 120, 255));

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_iSelectedRect = -1;
			}
		}

		//-----------------------------------------------------------------
		// DRAW SPRITE SHEET
		//-----------------------------------------------------------------
		if (std::shared_ptr<graphics::dx12::Texture> texture = m_pTexture.lock())
		{
			ImVec2 vMin = ImVec2(vCanvasPos.x + vOffset.x, vCanvasPos.y + vOffset.y);
			ImVec2 vMax = ImVec2(vMin.x + texture->GetResourceDesc().Width * fZoom,
				vMin.y + texture->GetResourceDesc().Height * fZoom);

			pDrawList->AddImage(texture->GetGPUHandle().ptr, vMin, vMax);

			for (size_t i = 0; i < texture->GetTextureRectsSize(); i++)
			{
				graphics::dx12::TextureRect* textureRect = texture->GetTextureRect(i);
				if (textureRect)
				{
					ImVec2 rectMin = ImVec2(vMin.x + textureRect->x * fZoom,
						vMin.y + textureRect->y * fZoom);

					ImVec2 rectMax = ImVec2(rectMin.x + textureRect->width * fZoom,
						rectMin.y + textureRect->height * fZoom);

					// Check if mouse clicked inside this rect
					if (bHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						if (vMouse.x >= rectMin.x && vMouse.x <= rectMax.x &&
							vMouse.y >= rectMin.y && vMouse.y <= rectMax.y)
						{
							m_iSelectedRect = (int) i; // select this rect
						}
					}

					if ((int) i != m_iSelectedRect)
					{
						pDrawList->AddRect(rectMin, rectMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 1.0f); // normal = red
					}
				}
			}

			graphics::dx12::TextureRect* textureRect = texture->GetTextureRect(m_iSelectedRect);

			if (textureRect)
			{
				ImVec2 rectMin = ImVec2(vMin.x + textureRect->x * fZoom,
					vMin.y + textureRect->y * fZoom);

				ImVec2 rectMax = ImVec2(rectMin.x + textureRect->width * fZoom,
					rectMin.y + textureRect->height * fZoom);

				pDrawList->AddRect(rectMin, rectMax, ImColor(GetAccentColor()), 0.0f, 0, 3.0f);
			}
		}

		ImGui::SetCursorScreenPos(startPos);

		DrawViewportPanel();
	}

	//---------------------------------------------------------------------
	void SpriteSheetEditorWindow::SetData(const std::string& a_sID)
	{
		std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = GetDX12System().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

		m_pTexture = GetResourceAtlas().LoadTexture(a_sID, cCommandQueue);

		if (std::shared_ptr<graphics::dx12::Texture> texture = m_pTexture.lock())
		{
			IVector2 size = texture->GetSpriteSheetCellSize();
			m_vCellSize = size;
		}
	}
}