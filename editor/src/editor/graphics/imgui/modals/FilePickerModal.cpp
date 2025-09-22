#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "FilePickerModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core includes
#include "editor/core/EditorTool.h"

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

// graphics includes
#include "editor/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			FilePickerModal::FilePickerModal(ImGuiWindow& a_Window) : BaseModal(a_Window, std::string(font::ICON_FOLDER) + " File Explorer", "FileExplorer"), m_SearchBar(a_Window)
			{
				m_SearchBar.Initialize("");
			}

			void FilePickerModal::LoadTexture(const std::string& a_sName)
			{
				auto cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				auto cCommandList = cCommandQueue->GetCommandList();
				m_pPreviewTexture = core::TOOL->GetResourceAtlas().LoadTexture(a_sName, cCommandList);
				m_pPreviewTexture->SetResourceCategory(gallus::core::EngineResourceCategory::Editor);
				cCommandQueue->ExecuteCommandList(cCommandList);
				cCommandQueue->Flush();
			}

			void FilePickerModal::Render()
			{
				if (m_bNeedsRefresh)
				{
					m_aFilteredFileResources.clear();

					for (ExplorerFileUIView& view : m_aResources)
					{
						if (m_SearchBar.GetString().empty() || string_extensions::StringToLower(view.GetFileResource().GetPath().filename().generic_string()).find(m_SearchBar.GetString()) != std::string::npos)
						{
							m_aFilteredFileResources.push_back(&view);
						}
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

				float childSize = (m_pPreviewTexture && m_pPreviewTexture->CanBeDrawn()) ? (ImGui::GetContentRegionAvail().x / 2) : 0;

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
					for (ExplorerFileUIView* view : m_aFilteredFileResources)
					{
						bool
							clicked = false,
							right_clicked = false,
							double_clicked = false;

						view->RenderList(clicked, right_clicked, double_clicked, view == m_pSelectedFileResource, false);

						if (clicked)
						{
							m_pSelectedFileResource = view;

							if (view->GetFileResource().GetMetaData()->GetAssetType() == gallus::resources::AssetType::Sprite)
							{
								LoadTexture(view->GetFileResource().GetPath().filename().generic_string());
							}
						}

						if (double_clicked && m_Callback)
						{
							m_Callback(1, view->GetFileResource());
							Hide();
							break;
						}
					}
				}
				ImGui::EndChild();

				if (childSize > 0)
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
						if (m_pPreviewTexture && m_pPreviewTexture->CanBeDrawn())
						{
							const float spriteW = m_pPreviewTexture->GetResourceDesc().Width;
							const float spriteH = m_pPreviewTexture->GetResourceDesc().Height;

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
							ImGui::Image((ImTextureID) m_pPreviewTexture->GetGPUHandle().ptr, ImVec2(drawW, drawH));

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

			void RecursiveFind(gallus::editor::FileResource& a_File, std::vector<gallus::resources::AssetType>& a_aFileTypes, std::vector<ExplorerFileUIView>& a_aResources, ImGuiWindow& a_Window)
			{
				for (gallus::editor::FileResource& fileResource : a_File.GetChildren())
				{
					for (gallus::resources::AssetType assetType : a_aFileTypes)
					{
						if (fileResource.GetMetaData()->GetAssetType() == assetType)
						{
							a_aResources.emplace_back(a_Window, fileResource);
						}
						RecursiveFind(fileResource, a_aFileTypes, a_aResources, a_Window);
					}
				}
			}

			void FilePickerModal::Show()
			{
				m_aResources.clear();
				m_aResources.reserve(gallus::core::EDITOR_TOOL->GetEditor().GetAssetDatabase().GetRoot().GetChildren().size());
				RecursiveFind(gallus::core::EDITOR_TOOL->GetEditor().GetAssetDatabase().GetRoot(), m_aFileTypes, m_aResources, m_Window);

				BaseModal::Show();
			}

			void FilePickerModal::SetData(const std::function<void(int, gallus::editor::FileResource&)>& a_Callback, const std::vector<gallus::resources::AssetType>& a_aFileTypes)
			{
				m_pSelectedFileResource = nullptr;
				m_Callback = nullptr;

				m_Callback = a_Callback;
				m_aFileTypes = a_aFileTypes;

				m_bNeedsRefresh = true;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE