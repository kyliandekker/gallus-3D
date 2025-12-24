#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "FilePickerModal.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core
#include "editor/core/EditorEngine.h"

// utils
#include "utils/string_extensions.h"

// graphics
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"

#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// graphics
#include "resources/FileResource.h"

// editor
#include "editor/graphics/imgui/selectables/FileEditorSelectable.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// FilePickerModal
			//---------------------------------------------------------------------
			FilePickerModal::FilePickerModal(ImGuiWindow& a_Window) : BaseModal(a_Window, std::string(font::ICON_FOLDER) + " File Picker", "FilePicker"), m_SearchBar(a_Window)
			{
				m_SearchBar.Initialize("");
			}

			//---------------------------------------------------------------------
			void FilePickerModal::LoadTexture(const std::string& a_sName)
			{
				auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				m_pPreviewTexture = core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(a_sName, cCommandQueue);
				if (auto tex = m_pPreviewTexture.lock())
				{
					tex->SetResourceCategory(gallus::resources::EngineResourceCategory::Editor);
				}
			}

			//---------------------------------------------------------------------
			void FilePickerModal::Render()
			{
				if (m_bNeedsRefresh)
				{
					m_aFilteredFileResources.clear();

					for (FileEditorSelectable& view : m_aResources)
					{
						if (m_SearchBar.GetString().empty() || string_extensions::StringToLower(view.GetFileResource().GetPath().filename().generic_string()).find(m_SearchBar.GetString()) != std::string::npos)
						{
							m_aFilteredFileResources.push_back(&view);
						}
					}

					m_aFilteredEngineResources.clear();

					size_t i = 0;
					for (auto& resource : m_aEngineResources)
					{
						if (auto pResource = resource.GetEngineResource().lock())
						{
							if (m_SearchBar.GetString().empty() || string_extensions::StringToLower(pResource->GetPath().filename().generic_string()).find(m_SearchBar.GetString()) != std::string::npos)
							{
								m_aFilteredEngineResources.push_back(&resource);
							}
						}
						i++;
					}

					m_bNeedsRefresh = false;
				}

				ImGui::SetCursorPos(ImVec2(
					ImGui::GetCursorPos().x + m_Window.GetFramePadding().x,
					ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));

				if (m_SearchBar.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "FILE_PICKER_MODAL").c_str(), ImVec2(ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x, m_Window.GetFramePadding().y), m_Window.GetFramePadding().x / 2))
				{
					m_bNeedsRefresh = true;
				}

				ImVec2 initialPos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(initialPos.x + m_Window.GetFramePadding().x, initialPos.y + m_Window.GetFramePadding().y));

				auto tex = m_pPreviewTexture.lock();
				float childSize = (tex && tex->CanBeDrawn()) ? (ImGui::GetContentRegionAvail().x / 2) : 0;

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_EXPLORER_FILE_PICKER_MODAL").c_str(),
					ImVec2(
					childSize - m_Window.GetFramePadding().x,
					ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
					))
				{
					for (EngineResourceEditorSelectable* resource : m_aFilteredEngineResources)
					{
						bool
							clicked = false,
							right_clicked = false,
							double_clicked = false;

						resource->RenderList(clicked, right_clicked, double_clicked, resource == m_pSelectedResource, false);

						if (clicked)
						{
							m_pSelectedResource = resource;
						}

						if (double_clicked && m_Callback)
						{
							m_Callback(1, resource->GetEngineResource().lock()->GetName());
							Hide();
							break;
						}
					}
					for (FileEditorSelectable* view : m_aFilteredFileResources)
					{
						bool
							clicked = false,
							right_clicked = false,
							double_clicked = false;

						view->RenderList(clicked, right_clicked, double_clicked, view == m_pSelectedResource, false);

						if (clicked)
						{
							m_pSelectedResource = view;
							m_iSelectedEngineResource = -1;

							if (view->GetFileResource().GetAssetType() == gallus::resources::AssetType::Sprite)
							{
								LoadTexture(view->GetFileResource().GetPath().filename().generic_string());
							}
						}

						if (double_clicked && m_Callback)
						{
							m_Callback(1, view->GetFileResource().GetPath().filename().generic_string());
							Hide();
							break;
						}
					}
				}
				ImGui::EndChild();

				if (tex && childSize > 0)
				{
					ImGui::SameLine();
					if (ImGui::BeginChild(
						ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_EXPLORER_FILE_PICKER_PREVIEW_MODAL").c_str(),
						ImVec2(
						childSize - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
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
			void RecursiveFind(gallus::resources::FileResource& a_File, resources::AssetType a_aFileType, std::vector<FileEditorSelectable>& a_aResources, ImGuiWindow& a_Window)
			{
				for (gallus::resources::FileResource& fileResource : a_File.GetChildren())
				{
					if (fileResource.GetAssetType() == a_aFileType)
					{
						a_aResources.emplace_back(a_Window, fileResource);
					}
					RecursiveFind(fileResource, a_aFileType, a_aResources, a_Window);
				}
			}

			//---------------------------------------------------------------------
			void FilePickerModal::Show()
			{
				m_aResources.clear();
				m_aResources.reserve(gallus::core::EDITOR_ENGINE->GetResourceAtlas().GetResourceFolder().GetChildren().size());
				RecursiveFind(gallus::core::EDITOR_ENGINE->GetResourceAtlas().GetResourceFolder(), m_AssetType, m_aResources, m_Window);

				BaseModal::Show();
			}

			//---------------------------------------------------------------------
			void FilePickerModal::SetData(const std::function<void(int, const std::string&)>& a_Callback, gallus::resources::AssetType a_aFileType)
			{
				m_pSelectedResource = nullptr;
				m_Callback = nullptr;

				m_Callback = a_Callback;
				m_AssetType = a_aFileType;

				m_aFilteredFileResources.clear();
				m_aResources.clear();

				m_aEngineResources.clear();
				m_aFilteredEngineResources.clear();
				for (auto resource : core::EDITOR_ENGINE->GetResourceAtlas().GetResourcesOfType(m_AssetType, resources::EngineResourceCategory::System))
				{
					m_aEngineResources.emplace_back(m_Window, resource);
				}

				m_bNeedsRefresh = true;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE