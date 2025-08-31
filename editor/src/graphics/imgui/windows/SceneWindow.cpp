#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "graphics/imgui/windows/SceneWindow.h"

#include <imgui/imgui_helpers.h>
#include <imgui/ImGuizmo.h>
#include <imgui/imgui_internal.h>

#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "core/EditorTool.h"
#include "graphics/dx12/Texture.h"
#include "graphics/imgui/views/HierarchyEntityUIView.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"

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
            {
                ImGuizmo::Enable(true);
            }

            //---------------------------------------------------------------------
            SceneWindow::~SceneWindow()
            {}

            //---------------------------------------------------------------------
            void SceneWindow::Render()
            {
                if (!core::EDITOR_TOOL)
                {
                    return;
                }

                std::shared_ptr<gallus::graphics::dx12::Texture> renderTexture = core::EDITOR_TOOL->GetDX12().GetRenderTexture();
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
                ImVec2 imagePosScreen = ImGui::GetCursorScreenPos();
                ImGui::Image(
                    (ImTextureID) renderTexture->GetGPUHandle().ptr,
                    imageSize
                );

                ImGui::SetCursorScreenPos(imagePosScreen);
                DrawGizmos(imagePosScreen, imageSize);

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

            void SceneWindow::DrawGizmos(const ImVec2& a_SceneStartPos, const ImVec2& a_Size)
            {
                ImGuizmo::BeginFrame();

                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

                ImGuizmo::SetRect(a_SceneStartPos.x, a_SceneStartPos.y, a_Size.x, a_Size.y);

                DrawTransformGizmo();
            }

            ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
            void SceneWindow::DrawTransformGizmo()
            {
                const HierarchyEntityUIView* entity = dynamic_cast<const HierarchyEntityUIView*>(core::EDITOR_TOOL->GetEditor().GetSelectable().get());
                if (entity)
                {
                    std::lock_guard<std::recursive_mutex> lock(core::TOOL->GetECS().m_EntityMutex);

                    ImGui::SetItemAllowOverlap();

                    DirectX::XMMATRIX objectMat;
                    if (core::TOOL->GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(entity->GetEntityID()))
                    {
                        auto& transformComponent = core::TOOL->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(entity->GetEntityID());
                        objectMat = transformComponent.Transform().GetWorldMatrix();
                    }
                    else
                    {
                        return;
                    }

                    // Get transformation matrices
                    DirectX::XMMATRIX viewMat = core::TOOL->GetDX12().GetCamera().GetViewMatrix();
                    DirectX::XMMATRIX projMat = core::TOOL->GetDX12().GetCamera().GetProjectionMatrix();

                    // Convert DirectX matrices to float[16] format for ImGuizmo
                    float gizmoMatrix[16];
                    DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(gizmoMatrix), objectMat);

                    if (ImGui::IsKeyPressed(ImGuiKey_T) || ImGui::IsKeyPressed(ImGuiKey_P)) currentOperation = ImGuizmo::TRANSLATE;
                    if (ImGui::IsKeyPressed(ImGuiKey_R)) currentOperation = ImGuizmo::ROTATE;
                    if (ImGui::IsKeyPressed(ImGuiKey_S)) currentOperation = ImGuizmo::SCALE;

                    bool useSnap = ImGui::IsKeyDown(ImGuiKey_LeftShift);
                    float snap = useSnap ? 1.0f : 0.0f;

                    // Render the gizmo (check if manipulation occurred)
                    if (ImGuizmo::Manipulate(reinterpret_cast<float*>(&viewMat),
                        reinterpret_cast<float*>(&projMat),
                        currentOperation,
                        ImGuizmo::LOCAL,
                        gizmoMatrix, 0, &snap))
                    {
                        auto& transformComponent = core::TOOL->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(entity->GetEntityID());
                        objectMat = XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(gizmoMatrix));
                        transformComponent.Transform().SetWorldMatrix(objectMat);

                        //core::TOOL->GetEditor().SetDirty();
                    }
                }
            }
        }
    }
}

#endif // _EDITOR
#endif // IMGUI_DISABLE