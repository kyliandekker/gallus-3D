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
			namespace editor
			{
				void SpriteComponentUIView::RenderInner()
				{
					//ImGui::DisplayHeader(m_Window.GetBoldFont(), "Shader");

					//if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_IMAGE, BUTTON_ID, "MESH_COMPONENT_SHADER").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont()))
					//{
					//	//m_Window.m_FilePickerModal.SetData(
					//	//	[this](int success, std::shared_ptr<graphics::dx12::DX12Resource> resource)
					//	//	{
					//	//		if (success == 1)
					//	//		{
					//	//			m_Component.SetShader(core::ENGINE.GetDX12().GetResourceAtlas().LoadShaderByPath(resource->GetPath(), resource->GetPath()));
					//	//		}
					//	//	}, std::vector<std::shared_ptr<graphics::dx12::DX12Resource>>(core::ENGINE.GetDX12().GetResourceAtlas().GetShaders().begin(), core::ENGINE.GetDX12().GetResourceAtlas().GetShaders().end())
					//	//		);
					//	//m_Window.m_FilePickerModal.Show();
					//}
					//ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					//ImGui::Text(m_Component.GetShader() ? std::string(m_Component.GetShader()->GetName().begin(), m_Component.GetShader()->GetName().end()).c_str() : "None");
					//ImGui::PopItemFlag();
					//ImGui::PopStyleVar();

					//ImGui::NewLine();

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
										m_Component.SetTexture(core::TOOL->GetResourceAtlas().LoadTexture(resource.GetPath().filename().generic_string(), cCommandList));
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
				}

				std::string SpriteComponentUIView::GetName() const
				{
					return m_System.GetSystemName();
				}
			}
		}
	}
}