#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "SpriteComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

#include "graphics/imgui/font_icon.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12ShaderBind.h"

#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

#include "graphics/imgui/modals/FilePickerModal.h"
#include "editor/AssetType.h"

#include "core/EditorTool.h"
#include "editor/FileResource.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			char m_PixelShaderName[128];
			char m_VertexShaderName[128];
			char m_TextureName[128];
			SpriteComponentUIView::SpriteComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::SpriteComponent& a_SpriteComponent, gameplay::SpriteSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_SpriteComponent, a_System), m_SizeView(a_Window)
			{
			}

			void SpriteComponentUIView::RenderInner()
			{
				memset(m_PixelShaderName, 0, sizeof(m_PixelShaderName));
				strncpy(m_PixelShaderName, m_Component.GetShader()->GetPixelShader()->GetName().c_str(), sizeof(m_PixelShaderName));
				m_PixelShaderName[sizeof(m_PixelShaderName) - 1] = '\0';

				memset(m_VertexShaderName, 0, sizeof(m_PixelShaderName));
				strncpy(m_VertexShaderName, m_Component.GetShader()->GetVertexShader()->GetName().c_str(), sizeof(m_VertexShaderName));
				m_VertexShaderName[sizeof(m_VertexShaderName) - 1] = '\0';

				memset(m_TextureName, 0, sizeof(m_PixelShaderName));
				strncpy(m_TextureName, m_Component.GetTexture()->GetName().c_str(), sizeof(m_TextureName));
				m_TextureName[sizeof(m_TextureName) - 1] = '\0';

				FilePickerModal* modal = dynamic_cast<FilePickerModal*>(m_Window.GetModal(0));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Pixel Shader: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_PIXEL_SHADER_NAME_INPUT").c_str(), m_PixelShaderName, sizeof(m_PixelShaderName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "MESH_COMPONENT_PIXEL_SHADER").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, gallus::editor::FileResource& resource)
							{
								if (success == 1)
								{
									m_Component.SetShader(
										core::TOOL->GetResourceAtlas().LoadShaderBind(
										core::TOOL->GetResourceAtlas().LoadPixelShader(resource.GetPath().filename().generic_string()).get(),
										m_Component.GetShader()->GetVertexShader()
									).get());
								}
							},
							std::vector<gallus::editor::AssetType>{ gallus::editor::AssetType::PixelShader }
						);
						modal->Show();
					}
				}

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Vertex Shader: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_VERTEX_SHADER_NAME_INPUT").c_str(), m_VertexShaderName, sizeof(m_VertexShaderName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "MESH_COMPONENT_VERTEX_SHADER").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, gallus::editor::FileResource& resource)
							{
								if (success == 1)
								{
									m_Component.SetShader(
										core::TOOL->GetResourceAtlas().LoadShaderBind(
											m_Component.GetShader()->GetPixelShader(), 
											core::TOOL->GetResourceAtlas().LoadVertexShader(resource.GetPath().filename().generic_string()).get()
									).get());
								}
							},
							std::vector<gallus::editor::AssetType>{ gallus::editor::AssetType::VertexShader }
						);
						modal->Show();
					}
				}

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Texture: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SPRITE_COMPONENT_TEXTURE_NAME_INPUT").c_str(), m_TextureName, sizeof(m_TextureName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_IMAGE, BUTTON_ID, "MESH_COMPONENT_TEXTURE").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, gallus::editor::FileResource& resource)
							{
								if (success == 1)
								{
									auto cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
									auto cCommandList = cCommandQueue->GetCommandList();
									m_Component.SetTexture(core::TOOL->GetResourceAtlas().LoadTexture(resource.GetPath().filename().generic_string(), cCommandList).get());
									cCommandQueue->ExecuteCommandList(cCommandList);
									cCommandQueue->Flush();

									//core::EDITOR_TOOL->GetEditor().SetDirty();
								}
							},
							std::vector<gallus::editor::AssetType>{ gallus::editor::AssetType::Texture }
						);
						modal->Show();
					}
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			std::string SpriteComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE