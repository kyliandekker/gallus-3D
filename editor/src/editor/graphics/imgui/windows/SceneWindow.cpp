#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "SceneWindow.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/dx12/Texture.h"

// editor includes
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/views/HierarchyEntityUIView.h"

// gameplay includes
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/Game.h"

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
            void SceneWindow::Update()
            {
                if (gameplay::GAME.IsStarted() && !gameplay::GAME.IsPaused())
                {
                    return;
                }
                BaseWindow::Update();
            }

            //---------------------------------------------------------------------
            void SceneWindow::Render()
            {
                if (!core::EDITOR_ENGINE)
                {
                    return;
                }

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

                ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
                ImGui::BeginToolbar(toolbarSize);

                bool isStarted = gameplay::GAME.IsStarted();
                if (ImGui::CheckboxButton(
                    ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PLAY), BUTTON_ID, "PLAY_SCENE").c_str(), &isStarted, m_Window.GetHeaderSize()))
                {
                    core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr, nullptr);
                    gameplay::GAME.GetScene().LoadData();
                    gameplay::GAME.SetIsStarted(isStarted);
                }
                ImGui::SameLine();
                bool isPaused = gameplay::GAME.IsPaused();
                if (ImGui::CheckboxButton(
                    ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PAUSE), BUTTON_ID, "PAUSE_SCENE").c_str(), &isPaused, m_Window.GetHeaderSize()))
                {
                    gameplay::GAME.SetIsPaused(isPaused);
                }
                ImGui::SameLine();
                bool drawBounds = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetDrawBounds();
                if (ImGui::CheckboxButton(
                    ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_CUBE), BUTTON_ID, "DRAW_BOUNDS_SCENE").c_str(), &drawBounds, m_Window.GetHeaderSize()))
                {
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetDrawBounds(drawBounds);
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                }

                ImGui::EndToolbar(ImVec2(0, 0));

                ImGui::PopStyleVar();
                ImGui::PopStyleVar();

                DrawViewportPanel();

                std::shared_ptr<gallus::graphics::dx12::Texture> renderTexture = core::EDITOR_ENGINE->GetDX12().GetRenderTexture();
                if (!renderTexture || !renderTexture->IsValid())
                {
                    return;
                }

                ImVec2 windowSize = ImGui::GetContentRegionAvail();
                ImVec2 textureSize = ImVec2(
                    static_cast<float>(renderTexture->GetSize().x),
                    static_cast<float>(renderTexture->GetSize().y)
                );

                // Static variables for m_fZoom and pan
                m_fZoom = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetSceneZoom();
                m_vPanOffset = ImVec2(
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetScenePanOffset().x,
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetScenePanOffset().y
                );
                m_CurrentOperation = (ImGuizmo::OPERATION) core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation();

                // Begin scrollable child region
                ImGui::BeginChild("SceneScroll", windowSize, 0,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

                ImVec2 childPos = ImGui::GetCursorScreenPos(); // top-left corner of child

                ImVec2 mouseLocal = ImGui::GetIO().MousePos - childPos; // mouse relative to child

                // Zoom with mouse wheel
                if (ImGui::IsWindowHovered())
                {
                    float wheel = ImGui::GetIO().MouseWheel;
                    if (wheel != 0.0f)
                    {
                        ImVec2 beforeZoom = (mouseLocal - m_vPanOffset) / m_fZoom;
                        m_fZoom = std::clamp(m_fZoom * (wheel > 0 ? 1.1f : 0.9f), 0.05f, 4.0f);
                        core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetSceneZoom(m_fZoom);
                        m_vPanOffset = mouseLocal - beforeZoom * m_fZoom;
                        core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetScenePanOffset(glm::vec2(m_vPanOffset.x, m_vPanOffset.y));
                        core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                    }
                }

                // Pan with middle mouse button
                if ((ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) || (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Right)))
                {
                    m_vPanOffset += ImGui::GetIO().MouseDelta;
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetScenePanOffset(glm::vec2(m_vPanOffset.x, m_vPanOffset.y));
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                }

                // Draw the texture
                ImVec2 imageScreenPos = ImGui::GetCursorScreenPos();
                ImVec2 imagePos = m_vPanOffset;
                ImVec2 imageSize = ImVec2(textureSize.x * m_fZoom, textureSize.y * m_fZoom);
                ImGui::SetCursorPos(imagePos);
                ImGui::Image(
                    (ImTextureID) renderTexture->GetGPUHandle().ptr,
                    imageSize
                );

                if (drawBounds)
                {
                    ShowSpriteBounds(imageScreenPos, textureSize);
                }

                DrawGizmos(imageScreenPos, textureSize);

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
                if (ImGui::IsKeyPressed(ImGuiKey_R) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
                {
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetSceneZoom(1.0f);
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetScenePanOffset(glm::vec2(0));
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                }
            }

            //---------------------------------------------------------------------
            void SceneWindow::DrawGizmos(const ImVec2& a_vSceneStartPos, const ImVec2& a_vSize)
            {
                ImGuizmo::BeginFrame();

                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

                ImGuizmo::SetRect(a_vSceneStartPos.x + m_vPanOffset.x, a_vSceneStartPos.y + m_vPanOffset.y, a_vSize.x * m_fZoom, a_vSize.y * m_fZoom);

                DrawTransformGizmo();
                // DrawBoundsGizmo(a_vSceneStartPos, m_fZoom, m_vPanOffset);
            }

            //---------------------------------------------------------------------
            void SceneWindow::DrawBoundsGizmo(const ImVec2& a_vScenePos)
            {
                if (m_CurrentOperation != ImGuizmo::OPERATION::SPRITE_BOUNDS)
                {
                    return;
                }

                DirectX::XMFLOAT2 spritePos;
                DirectX::XMFLOAT2 spriteSize;
                const HierarchyEntityUIView* entity = dynamic_cast<const HierarchyEntityUIView*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get());
                if (!entity)
                {
                    return;
                }

                std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

                ImGui::SetItemAllowOverlap();

                if (!core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(entity->GetEntityID()))
                {
                    return;
                }
                if (!core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::SpriteSystem>().HasComponent(entity->GetEntityID()))
                {
                    return;
                }

                auto& transformComponent = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(entity->GetEntityID());
                spritePos = transformComponent.Transform().GetPosition();
                spriteSize = transformComponent.Transform().GetScale();

                ImDrawList* drawList = ImGui::GetWindowDrawList();

                ImVec2 m_fZoomedSpritePos = ImVec2(spritePos.x * m_fZoom, spritePos.y * m_fZoom);
                ImVec2 m_fZoomedSpriteSize = ImVec2(spriteSize.x * m_fZoom, spriteSize.y * m_fZoom);

                ImVec2 topLeft = a_vScenePos + m_fZoomedSpritePos + m_vPanOffset;
                ImVec2 bottomRight = topLeft + m_fZoomedSpriteSize;

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
                transformComponent.Transform().SetScale(spritePos);
            }

            //---------------------------------------------------------------------
            void SceneWindow::ShowSpriteBounds(const ImVec2& a_vScenePos, const ImVec2& a_vSize)
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 mouseScreen = ImGui::GetMousePos();

                std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

                for (auto entity : core::EDITOR_ENGINE->GetECS().GetEntities())
                {
                    if (!core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(entity.GetEntityID()))
                        continue;
                    if (!core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::SpriteSystem>().HasComponent(entity.GetEntityID()))
                        continue;

                    auto& transform = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(entity.GetEntityID());

                    const DirectX::XMFLOAT2& pos = transform.Transform().GetPosition();
                    const DirectX::XMFLOAT2& scale = transform.Transform().GetScale();
                    const DirectX::XMFLOAT2& pivot = transform.Transform().GetPivot(); // -0.5 .. 0.5
                    float rotation = transform.Transform().GetRotation(); // in radians or degrees?

                    // Convert position to screen space with pan and zoom
                    ImVec2 spritePos = a_vScenePos + m_vPanOffset + ImVec2(pos.x * m_fZoom, pos.y * m_fZoom);
                    ImVec2 scaledSize = ImVec2(scale.x * m_fZoom, scale.y * m_fZoom);

                    // Define corners centered on sprite origin
                    ImVec2 corners[4] = {
                        ImVec2(-0.5f * scaledSize.x, -0.5f * scaledSize.y), // top-left
                        ImVec2(0.5f * scaledSize.x, -0.5f * scaledSize.y), // top-right
                        ImVec2(0.5f * scaledSize.x, 0.5f * scaledSize.y), // bottom-right
                        ImVec2(-0.5f * scaledSize.x, 0.5f * scaledSize.y)  // bottom-left
                    };

                    // Precompute sin/cos for rotation
                    float rotationRad = DirectX::XMConvertToRadians(rotation); // assuming rotation is degrees
                    float cosR = cosf(rotationRad);
                    float sinR = sinf(rotationRad);

                    // Rotate corners around pivot and translate to screen space
                    for (int i = 0; i < 4; ++i)
                    {
                        // offset relative to pivot
                        float x = corners[i].x - (pivot.x * scaledSize.x);
                        float y = corners[i].y - (pivot.y * scaledSize.y);

                        // rotate
                        float rx = x * cosR - y * sinR;
                        float ry = x * sinR + y * cosR;

                        // translate to spritePos (pivot position in screen space)
                        corners[i].x = rx + spritePos.x;
                        corners[i].y = ry + spritePos.y;
                    }

                    // Draw bounding box polyline
                    drawList->AddPolyline(corners, 4, IM_COL32(255, 255, 0, 255), true, 2.0f);

                    // Draw pivot point (pivot position)
                    drawList->AddCircleFilled(spritePos, 4.0f, IM_COL32(0, 255, 0, 255));

                    // Mouse cursor debugging, picking logic can be added here similarly if needed...
                }
            }

            //---------------------------------------------------------------------
            void SceneWindow::DrawViewportPanel()
            {
                ImGuiWindowFlags toolbarFlags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoFocusOnAppearing |
                    ImGuiWindowFlags_NoNav |
                    ImGuiWindowFlags_NoScrollbar;

                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f));

                // Fixed toolbar size
                ImVec2 toolbarSize(m_Window.GetHeaderSize().x + (m_Window.GetWindowPadding().x * 2), m_Window.GetHeaderSize().x * 5); // width x height
                ImGui::SetNextWindowSize(toolbarSize, ImGuiCond_Always);
                if (ImGui::Begin(ImGui::IMGUI_FORMAT_ID("", WINDOW_ID, "SCENE_TOOLBAR").c_str(), nullptr, toolbarFlags))
                {
                    // Create a child for the actual content (with border)
                    ImGuiWindowFlags childFlags =
                        ImGuiWindowFlags_NoScrollbar |
                        ImGuiWindowFlags_NoScrollWithMouse;

                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));

                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
                    if (ImGui::BeginChild(
                        ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "SCENE_TOOLBAR_CHILD").c_str(),
                        ImVec2(
                        m_Window.GetHeaderSize().x,
                        ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
                        ),
                        ImGuiChildFlags_None
                        ))
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

                        bool isTranslate = m_CurrentOperation == ImGuizmo::TRANSLATE;
                        if (ImGui::IconCheckboxButton(
                            ImGui::IMGUI_FORMAT_ID(font::ICON_TRANSLATE, BUTTON_ID, "TRANSLATE").c_str(),
                            &isTranslate,
                            m_Window.GetHeaderSize(),
                            m_Window.GetIconFont(),
                            isTranslate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
                        {
                            m_CurrentOperation = ImGuizmo::TRANSLATE;
                            core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) m_CurrentOperation);
                            core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                        }

                        bool isRotate = m_CurrentOperation == ImGuizmo::ROTATE_Z;
                        if (ImGui::IconCheckboxButton(
                            ImGui::IMGUI_FORMAT_ID(font::ICON_ROTATE, BUTTON_ID, "ROTATE").c_str(),
                            &isRotate,
                            m_Window.GetHeaderSize(),
                            m_Window.GetIconFont(),
                            isRotate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
                        {
                            m_CurrentOperation = ImGuizmo::ROTATE_Z;
                            core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) m_CurrentOperation);
                            core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                        }

                        bool isScale = m_CurrentOperation == ImGuizmo::SCALE;
                        if (ImGui::IconCheckboxButton(
                            ImGui::IMGUI_FORMAT_ID(font::ICON_SCALE, BUTTON_ID, "SCALE").c_str(),
                            &isScale,
                            m_Window.GetHeaderSize(),
                            m_Window.GetIconFont(),
                            isScale ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
                        {
                            m_CurrentOperation = ImGuizmo::SCALE;
                            core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) m_CurrentOperation);
                            core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                        }

                        ImGui::PopStyleVar();
                        ImGui::PopStyleVar();
                    }
                    ImGui::EndChild();

                    ImGui::PopStyleVar();
                    ImGui::PopStyleVar();
                    ImGui::PopStyleVar();

                    // --- DRAGGING ON BORDER ONLY ---
                    // Border thickness from style
                    float borderThickness = ImGui::GetStyle().FrameBorderSize + 3.0f;

                    // Expand the whole toolbar rect
                    ImVec2 winPos = ImGui::GetWindowPos();
                    ImVec2 winSize = ImGui::GetWindowSize();

                    ImRect borderRect(
                        winPos,
                        ImVec2(winPos.x + winSize.x, winPos.y + winSize.y)
                    );

                    // Shrink inside rect (where child lives)
                    ImRect innerRect(
                        winPos.x + borderThickness,
                        winPos.y + borderThickness,
                        winPos.x + winSize.x - borderThickness,
                        winPos.y + winSize.y - borderThickness
                    );

                    // Border = full area - inner area
                    if (ImGui::IsMouseHoveringRect(borderRect.Min, borderRect.Max) &&
                        !ImGui::IsMouseHoveringRect(innerRect.Min, innerRect.Max))
                    {
                        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                        {
                            ImVec2 newPos = winPos + ImGui::GetIO().MouseDelta;
                            ImGui::SetWindowPos(newPos);
                        }
                    }
                }

                ImGui::End();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
            }

            //---------------------------------------------------------------------
            void SceneWindow::DrawTransformGizmo()
            {
                std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

                const HierarchyEntityUIView* entity = dynamic_cast<const HierarchyEntityUIView*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get());
                if (!entity)
                {
                    return;
                }

                if (!core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(entity->GetEntityID()))
                {
                    return;
                }

                ImGui::SetItemAllowOverlap();

                auto& transformComponent = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(entity->GetEntityID());
                DirectX::XMMATRIX pivotOffset = DirectX::XMMatrixTranslation(transformComponent.Transform().GetPivot().x, transformComponent.Transform().GetPivot().y, 0.0f);
                DirectX::XMMATRIX objectMat = transformComponent.Transform().GetWorldMatrix();
                objectMat = objectMat * pivotOffset;;

                // Get transformation matrices
                DirectX::XMMATRIX viewMat = core::EDITOR_ENGINE->GetDX12().GetCamera().GetViewMatrix();
                const DirectX::XMMATRIX& projMat = core::EDITOR_ENGINE->GetDX12().GetCamera().GetProjectionMatrix();

                // Convert DirectX matrices to float[16] format for ImGuizmo
                float objectFloat[16];
                DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(objectFloat), objectMat);
                float viewFloat[16];
                DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(viewFloat), viewMat);
                float projFloat[16];
                DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(projFloat), projMat);

                if (ImGui::IsKeyPressed(ImGuiKey_T) || ImGui::IsKeyPressed(ImGuiKey_P))
                {
                    m_CurrentOperation = ImGuizmo::TRANSLATE;
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) m_CurrentOperation);
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                }
                if (ImGui::IsKeyPressed(ImGuiKey_R))
                {
                    m_CurrentOperation = ImGuizmo::ROTATE;
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) m_CurrentOperation);
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                }
                if (ImGui::IsKeyPressed(ImGuiKey_S))
                {
                    m_CurrentOperation = ImGuizmo::SCALE;
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) m_CurrentOperation);
                    core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
                }

                bool useSnap = ImGui::IsKeyDown(ImGuiKey_LeftShift);
                float snap = useSnap ? 1.0f : 0.0f;

                // Render the gizmo (check if manipulation occurred)
                if (ImGuizmo::Manipulate(
                    viewFloat,
                    projFloat,
                    m_CurrentOperation,
                    ImGuizmo::LOCAL,
                    objectFloat, 0, &snap))
                {
                    DirectX::XMMATRIX result = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(objectFloat));
                    result = result * DirectX::XMMatrixInverse(nullptr, pivotOffset);

                    transformComponent.Transform().SetWorldMatrix(result);

					gameplay::GAME.GetScene().SetIsDirty(true);
                }
            }

            //---------------------------------------------------------------------
            FullSceneWindow::FullSceneWindow(ImGuiWindow& a_Window) : SceneWindow(a_Window)
            {
                m_sWindowID = "FullScene";
                m_sName = "FullScene";
                m_Flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
            }

            //---------------------------------------------------------------------
            void FullSceneWindow::Update()
            {
                m_bFullScreen = true;
                if (!gameplay::GAME.IsStarted() || gameplay::GAME.IsPaused())
                {
                    return;
                }

                BaseWindow::Update();
            }

            //---------------------------------------------------------------------
            void FullSceneWindow::Render()
            {
                if (!core::EDITOR_ENGINE)
                {
                    return;
                }

                if (ImGui::IsKeyDown(ImGuiKey_Escape))
                {
                    core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr, nullptr);
                    gameplay::GAME.GetScene().LoadData();
                    gameplay::GAME.SetIsStarted(false);
                }

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

                ImVec2 startToolbarPos = ImGui::GetCursorPos();
                ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
                ImGui::BeginToolbar(toolbarSize);

                bool isStarted = gameplay::GAME.IsStarted();
                if (ImGui::CheckboxButton(
                    ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PLAY), BUTTON_ID, "PLAY_FULL_SCENE").c_str(), &isStarted, m_Window.GetHeaderSize()))
                {
                    core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr, nullptr);
                    gameplay::GAME.GetScene().LoadData();
                    gameplay::GAME.SetIsStarted(isStarted);
                }
                ImGui::SameLine();
                bool isPaused = gameplay::GAME.IsPaused();
                if (ImGui::CheckboxButton(
                    ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PAUSE), BUTTON_ID, "PAUSE_FULL_SCENE").c_str(), &isPaused, m_Window.GetHeaderSize()))
                {
                    gameplay::GAME.SetIsPaused(isPaused);
                }

                ImGui::EndToolbar(ImVec2(0, 0));
                ImGui::SetCursorPos(ImVec2(startToolbarPos.x, startToolbarPos.y + toolbarSize.y + m_Window.GetFramePadding().y));

                ImGui::PopStyleVar();
                ImGui::PopStyleVar();

                std::shared_ptr<gallus::graphics::dx12::Texture> renderTexture = core::EDITOR_ENGINE->GetDX12().GetRenderTexture();
                if (!renderTexture || !renderTexture->IsValid())
                {
                    return;
                }

                // Available region (minus padding)
                ImVec2 avail = ImGui::GetContentRegionAvail() - ImVec2(0, m_Window.GetFramePadding().y);
                ImVec2 padding = ImVec2();
                avail.x -= padding.x * 2.0f;
                avail.y -= padding.y * 2.0f;

                // Fit inside available space (keep aspect ratio)
                float scale = std::min(avail.x / renderTexture->GetSize().x, avail.y / renderTexture->GetSize().y);
                float drawW = renderTexture->GetSize().x * scale;
                float drawH = renderTexture->GetSize().y * scale;

                // Center horizontally
                float cursorX = ImGui::GetCursorPosX() + (avail.x - drawW) * 0.5f;
                ImGui::SetCursorPosX(cursorX);

                // Center horizontally
                ImGui::SetCursorPosX(cursorX);

                // Draw the texture
                ImVec2 image_pos = ImGui::GetCursorScreenPos();
                ImGui::Image(
                    (ImTextureID) renderTexture->GetGPUHandle().ptr,
                    ImVec2(drawW, drawH)
                );

                // Draw border
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddRect(image_pos, ImVec2(image_pos.x + drawW, image_pos.y + drawH),
                    ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border]));
            }
        }
    }
}

#endif // _EDITOR
#endif // IMGUI_DISABLE