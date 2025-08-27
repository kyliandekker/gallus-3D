#include "ExplorerSpriteUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "graphics/imgui/ImGuiWindow.h"
#include "core/Tool.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/FileResource.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			namespace editor
			{
				ExplorerSpriteUIViewInfo::ExplorerSpriteUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView)
				{
					m_bShowPreview = true;

					auto cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
					auto cCommandList = cCommandQueue->GetCommandList();
					m_pPreviewTexture = core::TOOL->GetResourceAtlas().LoadTexture(m_ExplorerFileUIView.GetFileResource().GetPath().filename().generic_string(), cCommandList);
					cCommandQueue->ExecuteCommandList(cCommandList);
					cCommandQueue->Flush();
					m_pPreviewTexture->SetResourceCategory(gallus::core::EngineResourceCategory::Editor);
				}

				ExplorerSpriteUIViewInfo::~ExplorerSpriteUIViewInfo()
				{
					if (m_pPreviewTexture)
					{
						m_pPreviewTexture->Destroy();
					}
				}

				void ExplorerSpriteUIViewInfo::RenderSpecific()
				{}

				void ExplorerSpriteUIViewInfo::RenderPreview()
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
			}
		}
	}
}