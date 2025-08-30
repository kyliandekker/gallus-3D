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

            //---------------------------------------------------------------------
            void SceneWindow::Render()
            {
                std::shared_ptr<gallus::graphics::dx12::Texture> renderTexture = core::TOOL->GetDX12().GetRenderTexture();
                if (!renderTexture || !renderTexture->IsValid())
                    return;

                ImVec2 windowSize = ImGui::GetContentRegionAvail();
                ImVec2 textureSize = ImVec2(
                    static_cast<float>(renderTexture->GetSize().x),
                    static_cast<float>(renderTexture->GetSize().y)
                );

                // Static variables for zoom and pan
                static float zoom = 1.0f;
                static ImVec2 panOffset(0.0f, 0.0f);

                // Begin scrollable child region
                ImGui::BeginChild("SceneScroll", windowSize, true,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

                ImVec2 childPos = ImGui::GetCursorScreenPos(); // top-left corner of child

                ImVec2 mouseLocal = ImGui::GetIO().MousePos - childPos; // mouse relative to child

                // Zoom with mouse wheel
                if (ImGui::IsWindowHovered())
                {
                    float wheel = ImGui::GetIO().MouseWheel;
                    if (wheel != 0.0f)
                    {
                        ImVec2 beforeZoom = (mouseLocal - panOffset) / zoom;
                        zoom = std::clamp(zoom * (wheel > 0 ? 1.1f : 0.9f), 0.05f, 4.0f);
                        panOffset = mouseLocal - beforeZoom * zoom;
                    }
                }

                // Pan with middle mouse button
                if ((ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) || (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Right)))
                {
                    panOffset += ImGui::GetIO().MouseDelta;
                }

                // Draw the texture
                ImVec2 imagePos = panOffset;
                ImVec2 imageSize = ImVec2(textureSize.x * zoom, textureSize.y * zoom);
                ImGui::SetCursorPos(imagePos);
                ImGui::Image(
                    (ImTextureID) renderTexture->GetGPUHandle().ptr,
                    imageSize
                );

// Draw border around the texture
                ImU32 borderColor = IM_COL32(255, 255, 255, 255); // white border
                float borderThickness = 2.0f;
                ImGui::GetWindowDrawList()->AddRect(
                    childPos + imagePos,
                    childPos + imagePos + imageSize,
                    borderColor,
                    0.0f, // rounding
                    0,    // flags
                    borderThickness
                );

                ImGui::EndChild();

                // Optional reset
                if (ImGui::IsKeyPressed(ImGuiKey_R))
                {
                    zoom = 1.0f;
                    panOffset = ImVec2(0.0f, 0.0f);
                }
            }
        }
    }
}

#endif // _EDITOR
#endif // IMGUI_DISABLE