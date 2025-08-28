#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "graphics/imgui/windows/SceneWindow.h"

#include <imgui/imgui_helpers.h>

#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "core/Tool.h"
#include "graphics/dx12/Texture.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
				//---------------------------------------------------------------------
				// HierarchyWindow
				//---------------------------------------------------------------------
			SceneWindow::SceneWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_SCENE) + " Scene", "Scene")
			{}

			//---------------------------------------------------------------------
			SceneWindow::~SceneWindow()
			{}

			void CalculateUVs(float a_TextureAspect, const ImVec2& a_TextureSize, const ImVec2& a_Size, ImVec2& a_UV0, ImVec2& a_UV1)
			{
				float regionAspect = a_Size.x / a_Size.y;

				a_UV0 = ImVec2(0.0f, 0.0f);
				a_UV1 = ImVec2(1.0f, 1.0f);

				if (a_TextureAspect > regionAspect)
				{
					float newWidth = regionAspect / a_TextureAspect;
					a_UV0.x = (1.0f - newWidth) * 0.5f;
					a_UV1.x = 1.0f - a_UV0.x;
				}
				else
				{
					float newHeight = a_TextureAspect / regionAspect;
					a_UV0.y = (1.0f - newHeight) * 0.5f;
					a_UV1.y = 1.0f - a_UV0.y;
				}
			}

			constexpr bool CROP = false;
			//---------------------------------------------------------------------
			void SceneWindow::Render()
			{
				if (!core::TOOL->GetDX12().GetRenderTexture()->IsValid())
				{
					return;
				}

				ImVec2 availableSize = ImGui::GetContentRegionAvail();
				ImVec2 textureSize = ImVec2(static_cast<float>(core::TOOL->GetDX12().GetRenderTexture()->GetSize().x),
					static_cast<float>(core::TOOL->GetDX12().GetRenderTexture()->GetSize().y));

				// Calculate the UVs to make sure the scene does not stretch.
				float textureAspect = textureSize.x / textureSize.y;
				ImVec2 uv0, uv1;
				CalculateUVs(textureAspect, textureSize, availableSize, uv0, uv1);

				if (CROP)
				{
					ImGui::Image((ImTextureID) core::TOOL->GetDX12().GetRenderTexture()->GetGPUHandle().ptr,
						availableSize, uv0, uv1);
				}
				else
				{
					ImGui::Image((ImTextureID) core::TOOL->GetDX12().GetRenderTexture()->GetGPUHandle().ptr,
						availableSize);
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE