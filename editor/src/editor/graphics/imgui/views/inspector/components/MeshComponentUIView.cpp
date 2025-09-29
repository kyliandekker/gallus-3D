#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "MeshComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics includes
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12ShaderBind.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/imgui/font_icon.h"

// editor includes
#include "editor/core/EditorEngine.h"
#include "resources/AssetType.h"
#include "resources/FileResource.h"
#include "editor/graphics/imgui/modals/FilePickerModal.h"

// gameplay includes
#include "gameplay/Game.h"
#include "gameplay/systems/TransformSystem.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			MeshComponentUIView::MeshComponentUIView(ImGuiWindow& a_Window, gameplay::MeshComponent& a_MeshComponent, gameplay::MeshSystem& a_System) : ComponentUIView(a_Window, a_MeshComponent, a_System), m_SizeView(a_Window)
			{
			}

			void MeshComponentUIView::RenderInner()
			{
				memset(m_sPrefabName, 0, sizeof(m_sPrefabName));
				strncpy(m_sPrefabName, m_Component.GetShader()->GetPixelShader()->GetName().c_str(), sizeof(m_sPrefabName));
				m_sPrefabName[sizeof(m_sPrefabName) - 1] = '\0';

				memset(m_sVertexShaderName, 0, sizeof(m_sVertexShaderName));
				strncpy(m_sVertexShaderName, m_Component.GetShader()->GetVertexShader()->GetName().c_str(), sizeof(m_sVertexShaderName));
				m_sVertexShaderName[sizeof(m_sVertexShaderName) - 1] = '\0';

				memset(m_sTextureName, 0, sizeof(m_sTextureName));
				strncpy(m_sTextureName, m_Component.GetTexture()->GetName().c_str(), sizeof(m_sTextureName));
				m_sTextureName[sizeof(m_sTextureName) - 1] = '\0';

				memset(m_sMeshName, 0, sizeof(m_sMeshName));
				strncpy(m_sMeshName, m_Component.GetMesh()->GetName().c_str(), sizeof(m_sMeshName));
				m_sMeshName[sizeof(m_sMeshName) - 1] = '\0';

				FilePickerModal* modal = dynamic_cast<FilePickerModal*>(m_Window.GetModal((int) EDITOR_MODAL::EDITOR_MODAL_FILE_PICKER));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Pixel Shader: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "MESH_COMPONENT_PIXEL_SHADER_NAME_INPUT").c_str(), m_sPrefabName, sizeof(m_sPrefabName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "MESH_COMPONENT_PIXEL_SHADER").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, resources::FileResource& resource)
							{
								if (success == 1)
								{
									m_Component.SetShader(
										core::EDITOR_ENGINE->GetResourceAtlas().LoadShaderBind(
										core::EDITOR_ENGINE->GetResourceAtlas().LoadPixelShader(resource.GetPath().filename().generic_string()).get(),
										m_Component.GetShader()->GetVertexShader()
									).get());
								}
							},
							std::vector<resources::AssetType>{ resources::AssetType::PixelShader }
						);
						modal->Show();
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Vertex Shader: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "MESH_COMPONENT_VERTEX_SHADER_NAME_INPUT").c_str(), m_sVertexShaderName, sizeof(m_sVertexShaderName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "MESH_COMPONENT_VERTEX_SHADER").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, resources::FileResource& resource)
							{
								if (success == 1)
								{
									m_Component.SetShader(
										core::EDITOR_ENGINE->GetResourceAtlas().LoadShaderBind(
											m_Component.GetShader()->GetPixelShader(), 
											core::EDITOR_ENGINE->GetResourceAtlas().LoadVertexShader(resource.GetPath().filename().generic_string()).get()
									).get());
									
									core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
								}
							},
							std::vector<resources::AssetType>{ resources::AssetType::VertexShader }
						);
						modal->Show();
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Texture: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "MESH_COMPONENT_TEXTURE_NAME_INPUT").c_str(), m_sTextureName, sizeof(m_sTextureName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_IMAGE, BUTTON_ID, "MESH_COMPONENT_TEXTURE").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, resources::FileResource& resource)
							{
								if (success == 1)
								{
									auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
									auto cCommandList = cCommandQueue->GetCommandList();
									m_Component.SetTexture(core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(resource.GetPath().filename().generic_string(), cCommandList).get());
									cCommandQueue->ExecuteCommandList(cCommandList);
									cCommandQueue->Flush();

									core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
								}
							},
							std::vector<resources::AssetType>{ resources::AssetType::Sprite }
						);
						modal->Show();
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Mesh: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "MESH_COMPONENT_MESH_NAME_INPUT").c_str(), m_sMeshName, sizeof(m_sMeshName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL, BUTTON_ID, "MESH_COMPONENT_MESH").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, resources::FileResource& resource)
							{
								if (success == 1)
								{
									auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
									auto cCommandList = cCommandQueue->GetCommandList();

									m_Component.SetMesh(core::ENGINE->GetResourceAtlas().LoadMesh(resource.GetPath().filename().generic_string()).get());
									
									cCommandQueue->ExecuteCommandList(cCommandList);
									cCommandQueue->Flush();

									core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
								}
							},
							std::vector<resources::AssetType>{ resources::AssetType::Mesh }
						);
						modal->Show();
					}
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			std::string MeshComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE