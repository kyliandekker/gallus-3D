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
            ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
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

                DrawViewportPanel();

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
                ImVec2 imageScreenPos = ImGui::GetCursorScreenPos();
                ImVec2 imagePos = panOffset;
                ImVec2 imageSize = ImVec2(textureSize.x * zoom, textureSize.y * zoom);
                ImGui::SetCursorPos(imagePos);
                ImGui::Image(
                    (ImTextureID) renderTexture->GetGPUHandle().ptr,
                    imageSize
                );

                DrawGizmos(imageScreenPos, textureSize, zoom, panOffset);

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

            void SceneWindow::DrawGizmos(const ImVec2& a_vSceneStartPos, const ImVec2& a_vSize, float a_fZoom, const ImVec2& a_vPanOffset)
            {
                ImGuizmo::BeginFrame();

                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

                ImGuizmo::SetRect(a_vSceneStartPos.x + a_vPanOffset.x, a_vSceneStartPos.y + a_vPanOffset.y, a_vSize.x * a_fZoom, a_vSize.y * a_fZoom);

                DrawTransformGizmo();
                DrawBoundsGizmo(a_vSceneStartPos, a_fZoom, a_vPanOffset);
            }

            void SceneWindow::DrawBoundsGizmo(const ImVec2& a_vScenePos, float a_fZoom, const ImVec2& a_vPanOffset)
            {
                if (currentOperation != ImGuizmo::OPERATION::SPRITE_BOUNDS)
                {
                    return;
                }

                DirectX::XMFLOAT2 spritePos;
                DirectX::XMINT2 spriteSize;
                const HierarchyEntityUIView* entity = dynamic_cast<const HierarchyEntityUIView*>(core::EDITOR_TOOL->GetEditor().GetSelectable().get());
                if (!entity)
                {
                    return;
                }

                std::lock_guard<std::recursive_mutex> lock(core::TOOL->GetECS().m_EntityMutex);

                ImGui::SetItemAllowOverlap();

                if (!core::TOOL->GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(entity->GetEntityID()))
                {
                    return;
                }
                if (!core::TOOL->GetECS().GetSystem<gameplay::SpriteSystem>().HasComponent(entity->GetEntityID()))
                {
                    return;
                }

                auto& transformComponent = core::TOOL->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(entity->GetEntityID());
                spritePos = transformComponent.Transform().GetPosition();

                auto& spriteComponent = core::TOOL->GetECS().GetSystem<gameplay::SpriteSystem>().GetComponent(entity->GetEntityID());
                spriteSize = spriteComponent.GetSize();

                ImDrawList* drawList = ImGui::GetWindowDrawList();

                ImVec2 zoomedSpritePos = ImVec2(spritePos.x * a_fZoom, spritePos.y * a_fZoom);
                ImVec2 zoomedSpriteSize = ImVec2(spriteSize.x * a_fZoom, spriteSize.y * a_fZoom);

                ImVec2 topLeft = a_vScenePos + zoomedSpritePos + a_vPanOffset;
                ImVec2 bottomRight = topLeft + zoomedSpriteSize;

                ImU32 color = IM_COL32(255, 255, 255, 100); 
                float thickness = 2.0f;

                // Draw rectangle
                drawList->AddRect(topLeft, bottomRight, color, 0.0f, 0, thickness);

                // Handle size
                float handleSize = 8.0f;

                // Four corner handles
                ImVec2 handles[4] = {
                    topLeft,
                    ImVec2(bottomRight.x, topLeft.y),
                    bottomRight,
                    ImVec2(topLeft.x, bottomRight.y)
                };

                for (int i = 0; i < 4; ++i)
                {
                    ImVec2 handleCenter = handles[i];
                    ImVec2 handleMin = handleCenter - ImVec2(handleSize * 0.5f, handleSize * 0.5f);
                    ImVec2 handleMax = handleCenter + ImVec2(handleSize * 0.5f, handleSize * 0.5f);

                    ImU32 col = IM_COL32(255, 255, 255, 255);
                    if (ImGui::IsMouseHoveringRect(handleMin, handleMax))
                    {
                        col = IM_COL32(255, 255, 0, 255);
                    }
                    drawList->AddCircleFilled(ImVec2(handleMin + handleMax) / 2, 6.f, col);

                    ImGui::SetCursorScreenPos(handleMin);
                    ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "HandleBounds_" + std::to_string(i)).c_str(), ImVec2(handleSize, handleSize));

                    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                    {
                        ImVec2 delta = ImGui::GetIO().MouseDelta;

                        switch (i)
                        {
                            case 0: // top-left
                            {
                                spritePos.x += delta.x;
                                spritePos.y += delta.y;
                                spriteSize.x -= delta.x;
                                spriteSize.y -= delta.y;
                                break;
                            }
                            case 1: // top-right
                            {
                                spritePos.y += delta.y;
                                spriteSize.x += delta.x;
                                spriteSize.y -= delta.y;
                                break;
                            }
                            case 2: // bottom-right
                            {
                                spriteSize.x += delta.x;
                                spriteSize.y += delta.y;
                                break;
                            }
                            case 3: // bottom-left
                            {
                                spritePos.x += delta.x;
                                spriteSize.x -= delta.x;
                                spriteSize.y += delta.y;
                                break;
                            }
                        }
                    }
                }

                transformComponent.Transform().SetPosition(spritePos);
                spriteComponent.SetSize(spriteSize);
            }

            void SceneWindow::DrawViewportPanel()
            {
                // Get the scene window's top-left position
                ImVec2 scenePos = ImGui::GetCursorScreenPos();

                // Fixed toolbar size
                ImVec2 toolbarSize(m_Window.GetHeaderSize().x, m_Window.GetHeaderSize().x * 5); // width x height

                ImGuiWindowFlags toolbarFlags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoFocusOnAppearing |
                    ImGuiWindowFlags_NoNav |
                    ImGuiWindowFlags_NoScrollbar; // remove vertical scrollbar

                ImGui::SetNextWindowSize(toolbarSize, ImGuiCond_Always);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f));

                if (ImGui::Begin(ImGui::IMGUI_FORMAT_ID("", WINDOW_ID, "ViewportToolbar").c_str(), nullptr, toolbarFlags))
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

                    bool isTranslate = currentOperation == ImGuizmo::TRANSLATE;
                    if (ImGui::IconCheckboxButton(
                        ImGui::IMGUI_FORMAT_ID(font::ICON_TRANSLATE, BUTTON_ID, "TRANSLATE").c_str(), &isTranslate, m_Window.GetHeaderSize(), m_Window.GetIconFont(), isTranslate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
                    {
                        currentOperation = ImGuizmo::TRANSLATE;
                    }

                    bool isRotate = currentOperation == ImGuizmo::ROTATE_Z;
                    if (ImGui::IconCheckboxButton(
                        ImGui::IMGUI_FORMAT_ID(font::ICON_ROTATE, BUTTON_ID, "ROTATE").c_str(), &isRotate, m_Window.GetHeaderSize(), m_Window.GetIconFont(), isRotate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
                    {
                        currentOperation = ImGuizmo::ROTATE_Z;
                    }

                    bool isScale = currentOperation == ImGuizmo::SCALE;
                    if (ImGui::IconCheckboxButton(
                        ImGui::IMGUI_FORMAT_ID(font::ICON_SCALE, BUTTON_ID, "SCALE").c_str(), &isScale, m_Window.GetHeaderSize(), m_Window.GetIconFont(), isScale ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
                    {
                        currentOperation = ImGuizmo::SCALE;
                    }

                    bool isBounds = currentOperation == ImGuizmo::SPRITE_BOUNDS;
                    if (ImGui::IconCheckboxButton(
                        ImGui::IMGUI_FORMAT_ID(font::ICON_SCALE, BUTTON_ID, "BOUNDS").c_str(), &isBounds, m_Window.GetHeaderSize(), m_Window.GetIconFont(), isBounds ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
                    {
                        currentOperation = ImGuizmo::SPRITE_BOUNDS;
                    }

                    ImGui::PopStyleVar();
                    ImGui::PopStyleVar();

                    // Drag handle for moving the toolbar
                    ImGui::InvisibleButton("##dragHandle", toolbarSize);
                    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                    {
                        ImVec2 toolbarPos = ImGui::GetWindowPos() + ImGui::GetIO().MouseDelta;
                        ImGui::SetWindowPos(toolbarPos);
                    }
                }

                ImGui::End();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
            }

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