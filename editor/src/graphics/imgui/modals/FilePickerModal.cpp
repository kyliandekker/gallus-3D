#ifdef _EDITOR

#include "FilePickerModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "editor/FileResource.h"
#include "core/Tool.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"
#include "utils/string_extensions.h"
#include "graphics/imgui/views/ExplorerFileUIView.h"
#include "core/EditorTool.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			namespace editor
			{
				FilePickerModal::FilePickerModal(ImGuiWindow& a_Window) : BaseModal(a_Window, std::string(font::ICON_FOLDER) + " File Explorer", "FileExplorer"), m_SearchBar(a_Window)
				{
					m_SearchBar.Initialize("");
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

					float childSize = ImGui::GetContentRegionAvail().x / 2;

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

								if (view->GetFileResource().GetAssetType() == gallus::editor::AssetType::Texture)
								{
									if (!m_pPreviewTexture)
									{
										m_pPreviewTexture = core::TOOL->GetResourceAtlas().LoadTextureEmpty("Explorer Preview Texture");
									}

									m_pPreviewTexture->SetResourceCategory(gallus::core::EngineResourceCategory::Editor);

									if (m_pPreviewTexture->IsValid())
									{
										m_pPreviewTexture->Destroy();
									}

									auto cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
									auto cCommandList = cCommandQueue->GetCommandList();
									m_pPreviewTexture->LoadByPath(view->GetFileResource().GetPath().lexically_normal(), cCommandList);
									cCommandQueue->ExecuteCommandList(cCommandList);
									cCommandQueue->Flush();
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
						if (m_pPreviewTexture && m_pPreviewTexture->IsValid())
						{
							const float height_new = ImGui::GetContentRegionAvail().y;
							const float width = (m_pPreviewTexture->GetResourceDesc().Width * (1.0f / m_pPreviewTexture->GetResourceDesc().Height * height_new));

							float offset = (ImGui::GetContentRegionAvail().x - width) / 2;
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
							ImVec2 image_pos = ImGui::GetCursorScreenPos();
							ImGui::Image((ImTextureID) m_pPreviewTexture->GetGPUHandle().ptr, ImVec2(width, height_new));

							// Draw border
							ImDrawList* draw_list = ImGui::GetForegroundDrawList();
							draw_list->AddRect(image_pos, ImVec2(image_pos.x + width, image_pos.y + height_new), ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border])); // White border
						}
					}
					ImGui::EndChild();

					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}

				void RecursiveFind(gallus::editor::FileResource& a_File, std::vector<gallus::editor::AssetType>& a_aFileTypes, std::vector<ExplorerFileUIView>& a_aResources, ImGuiWindow& a_Window)
				{
					for (gallus::editor::FileResource& fileResource : a_File.GetChildren())
					{
						for (gallus::editor::AssetType assetType : a_aFileTypes)
						{
							if (fileResource.GetAssetType() == assetType)
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

				void FilePickerModal::SetData(const std::function<void(int, gallus::editor::FileResource&)>& a_Callback, const std::vector<gallus::editor::AssetType>& a_aFileTypes)
				{
					m_pSelectedFileResource = nullptr;
					m_pPreviewTexture = nullptr;
					m_Callback = nullptr;

					m_Callback = a_Callback;
					m_aFileTypes = a_aFileTypes;

					m_bNeedsRefresh = true;
				}
			}
		}
	}
}

#endif // _EDITOR