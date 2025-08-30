#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "SpriteComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

#include "graphics/imgui/font_icon.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Shader.h"

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
			SpriteComponentUIView::SpriteComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::SpriteComponent& a_SpriteComponent, gameplay::SpriteSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_SpriteComponent, a_System), m_SizeView(a_Window)
			{
			}

			void SpriteComponentUIView::RenderInner()
			{
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Texture");

				if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_IMAGE, BUTTON_ID, "MESH_COMPONENT_TEXTURE").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont()))
				{
					FilePickerModal* modal = dynamic_cast<FilePickerModal*>(m_Window.GetModal(0));

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
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Text(m_Component.GetTexture() ? std::string(m_Component.GetTexture()->GetName().begin(), m_Component.GetTexture()->GetName().end()).c_str() : "None");
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();

				float fontSize = m_Window.GetFontSize();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(fontSize / 2, fontSize / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushItemWidth(75);

				m_SizeView.SetValue(m_Component.GetSize());
				if (m_SizeView.Render("SPRITE_COMPONENT_SIZE_INSPECTOR"))
				{
					m_Component.SetSize(m_SizeView.GetValue());
					//core::ENGINE.GetEditor().SetDirty();
				}

				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID("Set Size To Sprite Size", BUTTON_ID, "SPRITE_COMPONENT_SET_SPRITE_SIZE").c_str()))
				{
					m_Component.SetSizeToSpriteSize();
					//core::ENGINE.GetEditor().SetDirty();
				}

				ImGui::PopItemWidth();
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