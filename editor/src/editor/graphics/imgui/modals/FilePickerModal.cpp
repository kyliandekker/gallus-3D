#include "FilePickerModal.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core
#include "editor/core/EditorEngine.h"

// utils
#include "utils/string_extensions.h"

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"

#include "imgui_system/font_icon.h"
#include "imgui_system/ImGuiSystem.h"

// graphics
#include "resources/ResourceAtlas.h"
#include "editor/file/FileResource.h"

// editor
#include "editor/graphics/imgui/views/selectables/FileEditorSelectable.h"
#include "editor/graphics/imgui/views/selectables/EngineResourceEditorSelectable.h"
#include "editor/AssetDatabase.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// FilePickerModal
			//---------------------------------------------------------------------
			FilePickerModal::FilePickerModal(ImGuiSystem& a_System) : BaseModal(a_System, std::string(font::ICON_FOLDER) + " File Picker", "FilePicker"), m_SearchBar(a_System)
			{
				m_SearchBar.Initialize("");
			}

			//---------------------------------------------------------------------
			void FilePickerModal::LoadTexture(const std::string& a_sName)
			{
				graphics::dx12::DX12System* dx12System = GetEditorEngine().GetDX12();
				if (!dx12System)
				{
					return;
				}

				resources::ResourceAtlas* resourceAtlas = GetEditorEngine().GetResourceAtlas();
				if (!resourceAtlas)
				{
					return;
				}

				std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = dx12System->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				m_pPreviewTexture = resourceAtlas->LoadTexture(a_sName, cCommandQueue);
				if (std::shared_ptr<graphics::dx12::Texture> tex = m_pPreviewTexture.lock())
				{
					tex->SetResourceCategory(resources::EngineResourceCategory::Editor);
				}
			}

			//---------------------------------------------------------------------
			void FilePickerModal::Render()
			{
				if (m_bNeedsRefresh)
				{
					m_aFilteredListFileItems.clear();

					for (std::shared_ptr<FileEditorSelectable>& view : m_aListFileItems)
					{
						if (m_SearchBar.GetString().empty() || string_extensions::StringToLower(view->GetName()).find(m_SearchBar.GetString()) != std::string::npos)
						{
							m_aFilteredListFileItems.push_back(view);
						}
					}

					m_aFilteredListEngineItems.clear();

					size_t i = 0;
					for (std::shared_ptr<EngineResourceEditorSelectable>& resource : m_aListEngineItems)
					{
						if (std::shared_ptr<resources::EngineResource> pResource = resource->GetEngineResource().lock())
						{
							if (m_SearchBar.GetString().empty() || string_extensions::StringToLower(pResource->GetName()).find(m_SearchBar.GetString()) != std::string::npos)
							{
								m_aFilteredListEngineItems.push_back(resource);
							}
						}
						i++;
					}

					m_bNeedsRefresh = false;
				}

				ImGui::SetCursorPos(ImVec2(
					ImGui::GetCursorPos().x + m_System.GetFramePadding().x,
					ImGui::GetCursorPos().y + m_System.GetFramePadding().y));

				if (m_SearchBar.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "FILE_PICKER_MODAL").c_str(), ImVec2(ImGui::GetContentRegionAvail().x - m_System.GetFramePadding().x, m_System.GetFramePadding().y), m_System.GetFramePadding().x / 2))
				{
					m_bNeedsRefresh = true;
				}

				ImVec2 initialPos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(initialPos.x + m_System.GetFramePadding().x, initialPos.y + m_System.GetFramePadding().y));

				std::shared_ptr<graphics::dx12::Texture> tex = m_pPreviewTexture.lock();
				float childSize = (tex && tex->CanBeDrawn()) ? (ImGui::GetContentRegionAvail().x / 2) : 0;

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_System.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_System.GetFramePadding().x * 2, m_System.GetFramePadding().y * 2));
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_EXPLORER_FILE_PICKER_MODAL").c_str(),
					ImVec2(
					childSize - m_System.GetFramePadding().x,
					ImGui::GetContentRegionAvail().y - m_System.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
				))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, m_System.GetFramePadding());
					bool showTable = ImGui::BeginTable(
						ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "EXPLORER_LIST_ITEMS").c_str(),
						5,
						ImGuiTableFlags_RowBg |
						ImGuiTableFlags_SizingFixedFit |
						ImGuiTableFlags_BordersInnerH |
						ImGuiTableFlags_BordersInnerV
					);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 0);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("AssetType", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);

					ImGui::TableHeadersRow();

					ListEngineResources();
					ListFileResources();

					ImGui::EndTable();
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();

				if (tex && childSize > 0)
				{
					ImGui::SameLine();
					if (ImGui::BeginChild(
						ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_EXPLORER_FILE_PICKER_PREVIEW_MODAL").c_str(),
						ImVec2(
						childSize - m_System.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_System.GetFramePadding().y
						),
						ImGuiChildFlags_Borders
						))
					{
						if (tex->CanBeDrawn())
						{
							const float spriteW = tex->GetResourceDesc().Width;
							const float spriteH = tex->GetResourceDesc().Height;

							ImVec2 avail = ImGui::GetContentRegionAvail();
							ImVec2 padding = ImVec2();
							avail.x -= padding.x * 2.0f;
							avail.y -= padding.y * 2.0f;

							// Fit inside available space (keep aspect ratio)
							float scale = std::min(avail.x / spriteW, avail.y / spriteH);
							float drawW = spriteW * scale;
							float drawH = spriteH * scale;

							// Center horizontally
							float cursorX = ImGui::GetCursorPosX() + (avail.x - drawW) * 0.5f;
							ImGui::SetCursorPosX(cursorX);

							// Center horizontally
							ImGui::SetCursorPosX(cursorX);

							ImVec2 image_pos = ImGui::GetCursorScreenPos();
							ImGui::Image((ImTextureID) tex->GetGPUHandle().ptr, ImVec2(drawW, drawH));

							// Draw border
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							draw_list->AddRect(image_pos, ImVec2(image_pos.x + drawW, image_pos.y + drawH),
								ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border]));
						}
					}
					ImGui::EndChild();
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			//---------------------------------------------------------------------
			void RecursiveFind(file::FileResource& a_File, resources::AssetType a_aFileType, std::vector<std::shared_ptr<FileEditorSelectable>>& a_aResources, ImGuiSystem& a_System)
			{
				for (file::FileResource& fileResource : a_File.GetChildren())
				{
					if (fileResource.GetAssetType() == a_aFileType)
					{
						a_aResources.push_back(std::make_shared<FileEditorSelectable>(a_System, fileResource));
					}
					RecursiveFind(fileResource, a_aFileType, a_aResources, a_System);
				}
			}

			//---------------------------------------------------------------------
			void FilePickerModal::ListEngineResources()
			{
				std::shared_ptr<EditorSelectable> currentSelectable = m_pSelectedResource.lock();

				int count = 0;
				for (std::weak_ptr<EngineResourceEditorSelectable>& view : m_aFilteredListEngineItems)
				{
					bool
						clicked = false,
						rightClicked = false,
						doubleClicked = false;

					std::shared_ptr<EngineResourceEditorSelectable> explorerItem = view.lock();
					if (!explorerItem)
					{
						continue;
					}

					ImGui::TableNextRow();

					bool selected = currentSelectable.get() == explorerItem.get();
					if (selected)
					{
						ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
							ImGui::GetColorU32(ImGuiCol_HeaderActive));
					}

					ImGui::TableNextColumn();

					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));          // disables background
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));   // disables hover highlight
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));    // disables pressed highlight

					std::shared_ptr<resources::EngineResource> resource = explorerItem->GetEngineResource().lock();
					if (resource)
					{
						// This spans the entire row internally.
						if (ImGui::Selectable(
							ImGui::IMGUI_FORMAT_ID("", SELECTABLE_ID, "ENGINE_RESOURCE_" + resource->GetName()).c_str(),
							selected,
							ImGuiSelectableFlags_SpanAllColumns |
							ImGuiSelectableFlags_AllowItemOverlap,
							ImVec2(0.0f, 0.0f)
						))
						{
							clicked = true;
						}

						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
						{
							doubleClicked = true;
						}

						if (clicked)
						{
							m_pSelectedResource = view;
						}

						if (doubleClicked && m_Callback)
						{
							m_Callback(1, resource->GetName());
							Hide();
							break;
						}

						ImGui::TableNextColumn();
						explorerItem->RenderList(selected);
						ImGui::TableNextColumn();
						ImGui::Text("ENGINE/MISSING");
					}
				}
			}

			//---------------------------------------------------------------------
			void FilePickerModal::ListFileResources()
			{
				std::shared_ptr<EditorSelectable> currentSelectable = m_pSelectedResource.lock();

				int count = 0;
				for (std::weak_ptr<FileEditorSelectable>& view : m_aFilteredListFileItems)
				{
					bool
						clicked = false,
						rightClicked = false,
						doubleClicked = false;

					std::shared_ptr<FileEditorSelectable> explorerItem = view.lock();
					if (!explorerItem)
					{
						continue;
					}

					ImGui::TableNextRow();

					bool selected = currentSelectable.get() == explorerItem.get();
					if (selected)
					{
						ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
							ImGui::GetColorU32(ImGuiCol_HeaderActive));
					}

					ImGui::TableNextColumn();

					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));          // disables background
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));   // disables hover highlight
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));    // disables pressed highlight

					// This spans the entire row internally.
					if (ImGui::Selectable(
						ImGui::IMGUI_FORMAT_ID("", SELECTABLE_ID, explorerItem->GetFileResource().GetPath().generic_string()).c_str(),
						selected,
						ImGuiSelectableFlags_SpanAllColumns |
						ImGuiSelectableFlags_AllowItemOverlap,
						ImVec2(0.0f, 0.0f)
					))
					{
						clicked = true;
					}

					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						doubleClicked = true;
					}

					if (clicked)
					{
						m_pSelectedResource = view;
					}

					if (doubleClicked && m_Callback)
					{
						m_Callback(1, explorerItem->GetFileResource().GetName());
						Hide();
						break;
					}

					ImGui::TableNextColumn();
					explorerItem->RenderList(selected);
				}
			}

			//---------------------------------------------------------------------
			void FilePickerModal::SetData(const std::function<void(int, const std::string&)>& a_Callback, resources::AssetType a_aFileType)
			{
				m_Callback = nullptr;

				m_Callback = a_Callback;
				m_AssetType = a_aFileType;

				resources::ResourceAtlas* resourceAtlas = GetEditorEngine().GetResourceAtlas();
				if (!resourceAtlas)
				{
					return;
				}

				editor::AssetDatabase* assetDatabase = GetEditorEngine().GetAssetDatabase();
				if (!assetDatabase)
				{
					return;
				}

				m_aListFileItems.clear();
				m_aFilteredListFileItems.clear();
				m_aListEngineItems.clear();
				m_aFilteredListEngineItems.clear();

				RecursiveFind(*assetDatabase->GetResourceFolder(), m_AssetType, m_aListFileItems, m_System);

				for (std::weak_ptr<resources::EngineResource>& resource : resourceAtlas->GetResourcesOfType(m_AssetType, resources::EngineResourceCategory::Engine))
				{
					m_aListEngineItems.push_back(std::make_shared<EngineResourceEditorSelectable>(m_System, resource));
				}
				for (std::weak_ptr<resources::EngineResource>& resource : resourceAtlas->GetResourcesOfType(m_AssetType, resources::EngineResourceCategory::Missing))
				{
					m_aListEngineItems.push_back(std::make_shared<EngineResourceEditorSelectable>(m_System, resource));
				}

				m_bNeedsRefresh = true;
			}
		}
	}
}