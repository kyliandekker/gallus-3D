#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "SpriteEditorModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>
#include <algorithm>

// core includes
#include "editor/core/EditorEngine.h"

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

// graphics includes
#include "resources/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"
#include "resources/metadata/TextureMetaData.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			SpriteEditorModal::SpriteEditorModal(ImGuiWindow& a_Window) : BaseModal(a_Window, std::string(font::ICON_IMAGE) + " Sprite Editor", "SpriteEditor")
			{
			}

			void SpriteEditorModal::LoadTexture(const std::string& a_sName)
			{
				auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				m_pPreviewTexture = core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(a_sName, cCommandQueue);
				m_pPreviewTexture->SetResourceCategory(gallus::resources::EngineResourceCategory::Editor);
			}

			void SpriteEditorModal::Render()
			{
                if (!m_pTextureMetaData)
                {
                    return;
                }

                if (!m_pPreviewTexture || !m_pPreviewTexture->IsValid())
                {
                    return;
                }

                RenderToolbar();

                ImGui::Separator();

                ImVec2 textureSize(
                    static_cast<float>(m_pPreviewTexture->GetSize().x),
                    static_cast<float>(m_pPreviewTexture->GetSize().y)
                );

                ImVec2 windowSize = ImGui::GetContentRegionAvail();

                // --- Main scrollable view ---
                ImGui::BeginChild("SceneScroll", windowSize, false,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

                ImVec2 childPos = ImGui::GetCursorScreenPos();
                ImVec2 mouseScreen = ImGui::GetIO().MousePos;
                ImVec2 mouseLocal = mouseScreen - childPos;

                HandleGlobalControls(windowSize);

                // --- Draw texture ---
                ImVec2 imagePos = m_vPanOffset;
                ImVec2 imageSize(textureSize.x * m_fZoom, textureSize.y * m_fZoom);

                ImGui::SetCursorPos(imagePos);
                ImGui::Image((ImTextureID) m_pPreviewTexture->GetGPUHandle().ptr, imageSize);

                ImVec2 imgMin = ImGui::GetItemRectMin(); // screen coords
                ImVec2 imgMax = ImGui::GetItemRectMax();
                ImDrawList* dl = ImGui::GetWindowDrawList();

                dl->AddRect(imgMin, imgMax, IM_COL32(255, 255, 255, 255), 0, 0, 2.0f);

                bool handledRect = InteractWithRect(imgMin, imgMax);
                for (size_t i = 0; i < m_pTextureMetaData->GetSprites().size(); i++)
                {
                    RenderRect((int8_t) i, imgMin, imgMax);
                    if (!handledRect)
                    {
                        HandleRectSelection((int8_t) i, imgMin, imgMax);
                    }
                }
                DrawRectInteraction(imgMin, imgMax);

                ImGui::EndChild();
			}

            void SpriteEditorModal::RenderToolbar()
            {
                RenderFloatingToolbar();

                glm::ivec2 textureSize = m_pPreviewTexture->GetSize();

                if (ImGui::BeginChild("SpriteToolbar", ImVec2(0, m_Window.GetHeaderSize().y * 2.75f)))
                {
                    if (ImGui::IconButton(
                        ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_SPRITE_EDITOR_MODAL").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
                    {
                        m_pTextureMetaData->Save(m_pFileResource->GetPath());
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(ImGui::IMGUI_FORMAT_ID("Add Sprite", BUTTON_ID, "ADD_SPRITE_EDITOR_MODAL").c_str(), ImVec2(0, m_Window.GetHeaderSize().y)))
                    {
                        m_pTextureMetaData->GetSprites().push_back({ 10, 10, 50, 50 });
                        m_iCurrentSprite = (int8_t) m_pTextureMetaData->GetSprites().size() - 1;
                    }

                    static glm::ivec2 generateRects;
                    ImGui::PushItemWidth(75);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 7));
                    ImGui::AlignTextToFramePadding();
                    ImGui::DisplayHeader(m_Window.GetBoldFont(), "Size");
                    ImGui::SameLine();
                    if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "GENERATE_X_SPRITE_EDITOR_MODAL").c_str(), &generateRects.x, 1.0f, textureSize.x))
                    {
                        generateRects.x = std::clamp(generateRects.x, 0, textureSize.x);
                    }
                    ImGui::SameLine();
                    if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "GENERATE_Y_SPRITE_EDITOR_MODAL").c_str(), &generateRects.y, 1.0f, textureSize.y))
                    {
                        generateRects.y = std::clamp(generateRects.y, 0, textureSize.y);
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopItemWidth();
                    if (ImGui::Button(ImGui::IMGUI_FORMAT_ID("Generate", BUTTON_ID, "GENERATE_SPRITE_EDITOR_MODAL").c_str(), ImVec2(0, m_Window.GetHeaderSize().y)))
                    {
                        m_pTextureMetaData->GetSprites().clear();
                        m_iCurrentSprite = -1;

                        for (int y = 0; y < textureSize.y; y += generateRects.y)
                        {
                            for (int x = 0; x < textureSize.x; x += generateRects.x)
                            {
                                graphics::dx12::SpriteRect r;
                                r.x = x;
                                r.y = y;
                                r.width = generateRects.x;
                                r.height = generateRects.y;
                                m_pTextureMetaData->GetSprites().push_back(r);
                            }
                        }
                    }

                    ImGui::EndChild();
                }
            }

            void SpriteEditorModal::HandleGlobalControls(const ImVec2& a_vWindowSize)
            {
                ImVec2 childPos = ImGui::GetCursorScreenPos();
                ImVec2 mouseScreen = ImGui::GetIO().MousePos;
                ImVec2 mouseLocal = mouseScreen - childPos;

                ImVec2 textureSize(
                    static_cast<float>(m_pPreviewTexture->GetSize().x),
                    static_cast<float>(m_pPreviewTexture->GetSize().y)
                );

                // --- ZOOM ---
                if (ImGui::IsWindowHovered())
                {
                    float wheel = ImGui::GetIO().MouseWheel;
                    if (wheel != 0.0f)
                    {
                        ImVec2 beforeZoom = (mouseLocal - m_vPanOffset) / m_fZoom;
                        m_fZoom = std::clamp(m_fZoom * (wheel > 0 ? 1.1f : 0.9f), 0.5f, 8.0f);
                        m_vPanOffset = mouseLocal - beforeZoom * m_fZoom;
                    }
                }

                // --- PAN ---
                if (ImGui::IsWindowHovered() &&
                    (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || ImGui::IsMouseDragging(ImGuiMouseButton_Right)))
                {
                    m_vPanOffset += ImGui::GetIO().MouseDelta;
                }

                // --- Clamp pan ---
                ImVec2 viewSize = a_vWindowSize;
                ImVec2 imgSize(textureSize.x * m_fZoom, textureSize.y * m_fZoom);

                if (imgSize.x >= viewSize.x)
                {
                    m_vPanOffset.x = std::clamp(m_vPanOffset.x, viewSize.x - imgSize.x, 0.0f);
                }
                else
                {
                    m_vPanOffset.x = (viewSize.x - imgSize.x) * 0.5f;
                }
                if (imgSize.y >= viewSize.y)
                {
                    m_vPanOffset.y = std::clamp(m_vPanOffset.y, viewSize.y - imgSize.y, 0.0f);
                }
                else
                {
                    m_vPanOffset.y = (viewSize.y - imgSize.y) * 0.5f;
                }
            }

            void SpriteEditorModal::RenderRect(int8_t a_iIndex, const ImVec2& a_vImgMin, const ImVec2& a_vImgMax)
            {
                if (a_iIndex == -1 || a_iIndex >= m_pTextureMetaData->GetSprites().size())
                {
                    return;
                }

                ImVec2 rectMin;
                ImVec2 rectMax;
                GetSpriteMinMax(a_iIndex, rectMin, rectMax, a_vImgMin, a_vImgMax);

                // Draw rect
                ImU32 color = a_iIndex == m_iCurrentSprite ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 255, 255, 255);

                ImDrawList* dl = ImGui::GetWindowDrawList();
                dl->AddRect(rectMin, rectMax, color, 0, 0, 1.0f);
            }

            void SpriteEditorModal::HandleRectSelection(int8_t a_iIndex, const ImVec2& a_vImgMin, const ImVec2& a_vImgMax)
            {
                if (a_iIndex == -1 || a_iIndex >= m_pTextureMetaData->GetSprites().size())
                {
                    return;
                }

                ImVec2 textureSize(
                    static_cast<float>(m_pPreviewTexture->GetSize().x),
                    static_cast<float>(m_pPreviewTexture->GetSize().y)
                );

                graphics::dx12::SpriteRect& r = m_pTextureMetaData->GetSprites()[a_iIndex];

                ImVec2 rectMin;
                ImVec2 rectMax;
                GetSpriteMinMax(a_iIndex, rectMin, rectMax, a_vImgMin, a_vImgMax);

                ImVec2 selectedRectMin;
                ImVec2 selectedRectMax;
                if (m_iCurrentSprite != -1 && m_iCurrentSprite < m_pTextureMetaData->GetSprites().size())
                {
                    GetSpriteMinMax(m_iCurrentSprite, selectedRectMin, selectedRectMax, a_vImgMin, a_vImgMax);
                }

                ImVec2 mouseScreen = ImGui::GetIO().MousePos;
                bool mouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);

                if (!m_bResizing && !m_bDragging && ImGui::IsMouseHoveringRect(rectMin, rectMax) && !ImGui::IsMouseHoveringRect(selectedRectMin, selectedRectMax))
                {
                    if (mouseClicked &&
                        mouseScreen.x >= rectMin.x && mouseScreen.x <= rectMax.x &&
                        mouseScreen.y >= rectMin.y && mouseScreen.y <= rectMax.y)
                    {
                        m_iCurrentSprite = a_iIndex;
                    }
                }
            }

            void SpriteEditorModal::GetSpriteMinMax(int8_t a_iIndex, ImVec2& a_vMin, ImVec2& a_vMax, const ImVec2& a_vImgMin, const ImVec2& a_vImgMax)
            {
                graphics::dx12::SpriteRect& r = m_pTextureMetaData->GetSprites()[a_iIndex];

                // Texture-space rect
                ImVec2 rectMinTex(r.x, r.y);
                ImVec2 rectMaxTex(r.x + r.width, r.y + r.height);

                // Convert to screen space
                a_vMin = a_vImgMin + rectMinTex * m_fZoom;
                a_vMax = a_vImgMin + rectMaxTex * m_fZoom;
            }

            bool SpriteEditorModal::InteractWithRect(const ImVec2& a_vImgMin, const ImVec2& a_vImgMax)
            {
                if (m_iCurrentSprite == -1 || m_iCurrentSprite >= m_pTextureMetaData->GetSprites().size())
                {
                    return false;
                }

                ImVec2 textureSize(
                    static_cast<float>(m_pPreviewTexture->GetSize().x),
                    static_cast<float>(m_pPreviewTexture->GetSize().y)
                );

                graphics::dx12::SpriteRect& r = m_pTextureMetaData->GetSprites()[m_iCurrentSprite];

                ImVec2 mouseScreen = ImGui::GetIO().MousePos;
                bool mouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);

                ImVec2 selectedRectMin;
                ImVec2 selectedRectMax;
                GetSpriteMinMax(m_iCurrentSprite, selectedRectMin, selectedRectMax, a_vImgMin, a_vImgMax);

                float handleSize = 8.0f;

                ImVec2 handles[4] = {
                    selectedRectMin,
                    ImVec2(selectedRectMax.x, selectedRectMin.y),
                    selectedRectMax,
                    ImVec2(selectedRectMin.x, selectedRectMax.y)
                };

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                for (size_t i = 0; i < 4; i++)
                {
                    ImVec2& handleCenter = handles[i];
                    ImVec2 handleMin = handleCenter - ImVec2(handleSize * 0.5f, handleSize * 0.5f);
                    ImVec2 handleMax = handleCenter + ImVec2(handleSize * 0.5f, handleSize * 0.5f);

                    ImU32 col = IM_COL32(255, 255, 255, 255);
                    if (ImGui::IsMouseHoveringRect(handleMin, handleMax))
                    {
                        col = IM_COL32(255, 255, 0, 255);
                    }
                    drawList->AddCircleFilled(ImVec2(handleMin + handleMax) / 2, 6.f, col);

                    ImGui::SetCursorScreenPos(handleMin);
                    ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "HANDLE_" + std::to_string(i) + "SPRITE_EDITOR_MODAL").c_str(), ImVec2(handleSize, handleSize));

                    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    {
                        m_iDragIndex = i;
                        m_bResizing = true;
                    }
                    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                    {
                        m_iDragIndex = i;
                        m_bResizing = true;

                        ImVec2 delta = ImGui::GetIO().MouseDelta;

                        switch (i)
                        {
                            case 0: // top-left
                            {
                                r.x += delta.x;
                                r.y += delta.y;
                                r.width -= delta.x;
                                r.height -= delta.y;
                                break;
                            }
                            case 1: // top-right
                            {
                                r.y += delta.y;
                                r.width += delta.x;
                                r.height -= delta.y;
                                break;
                            }
                            case 2: // bottom-right
                            {
                                r.width += delta.x;
                                r.height += delta.y;
                                break;
                            }
                            case 3: // bottom-left
                            {
                                r.x += delta.x;
                                r.width -= delta.x;
                                r.height += delta.y;
                                break;
                            }
                        }
                    }
                }

                if (!m_bResizing)
                {
                    if (mouseDown &&
                        mouseScreen.x >= selectedRectMin.x && mouseScreen.x <= selectedRectMax.x &&
                        mouseScreen.y >= selectedRectMin.y && mouseScreen.y <= selectedRectMax.y)
                    {
                        m_bDragging = true;
                    }

                    if (m_bDragging)
                    {
                        ImVec2 deltaTex = ImGui::GetIO().MouseDelta;
                        r.x += deltaTex.x;
                        r.y += deltaTex.y;
                    }
                }

                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                {
                    m_bResizing = false;
                    m_bDragging = false;
                    m_iDragIndex = -1;
                }

                // Clamp inside image
                if (r.x < 0.0f)
                {
                    r.x = 0.0f;
                }
                if (r.y < 0.0f)
                {
                    r.y = 0.0f;
                }
                if (r.x + r.width > textureSize.x)
                {
                    r.width = textureSize.x - r.x;
                }
                if (r.y + r.height > textureSize.y)
                {
                    r.height = textureSize.y - r.y;
                }

                return m_bResizing || m_bDragging;
            }

            void SpriteEditorModal::DrawRectInteraction(const ImVec2& a_vImgMin, const ImVec2& a_vImgMax)
            {
                if (m_iCurrentSprite == -1 || m_iCurrentSprite >= m_pTextureMetaData->GetSprites().size())
                {
                    return;
                }

                ImVec2 textureSize(
                    static_cast<float>(m_pPreviewTexture->GetSize().x),
                    static_cast<float>(m_pPreviewTexture->GetSize().y)
                );

                graphics::dx12::SpriteRect& r = m_pTextureMetaData->GetSprites()[m_iCurrentSprite];

                ImVec2 mouseScreen = ImGui::GetIO().MousePos;
                bool mouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);

                ImVec2 selectedRectMin;
                ImVec2 selectedRectMax;
                GetSpriteMinMax(m_iCurrentSprite, selectedRectMin, selectedRectMax, a_vImgMin, a_vImgMax);

                float handleSize = 8.0f;

                ImVec2 handles[4] = {
                    selectedRectMin,
                    ImVec2(selectedRectMax.x, selectedRectMin.y),
                    selectedRectMax,
                    ImVec2(selectedRectMin.x, selectedRectMax.y)
                };

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                for (size_t i = 0; i < 4; i++)
                {
                    ImVec2& handleCenter = handles[i];
                    ImVec2 handleMin = handleCenter - ImVec2(handleSize * 0.5f, handleSize * 0.5f);
                    ImVec2 handleMax = handleCenter + ImVec2(handleSize * 0.5f, handleSize * 0.5f);

                    ImU32 col = IM_COL32(255, 255, 255, 255);
                    if (ImGui::IsMouseHoveringRect(handleMin, handleMax) || m_bResizing)
                    {
                        col = IM_COL32(255, 255, 0, 255);
                    }
                    drawList->AddCircleFilled(ImVec2(handleMin + handleMax) / 2, 6.f, col);
                }
            }

			void SpriteEditorModal::SetData(resources::FileResource& a_FileResource)
			{
                m_pFileResource = &a_FileResource;
				m_pTextureMetaData = a_FileResource.GetMetaData<resources::TextureMetaData>();
				LoadTexture(m_pFileResource->GetPath().filename().generic_string());
			}

            ImVec2 m_vFloatingPanelPos = ImVec2(50, 50);
            void SpriteEditorModal::RenderFloatingToolbar()
            {
                if (m_iCurrentSprite == -1 || m_iCurrentSprite >= m_pTextureMetaData->GetSprites().size())
                {
                    return;
                }

                graphics::dx12::SpriteRect& spriteUV = m_pTextureMetaData->GetSprites()[m_iCurrentSprite];
                glm::ivec2 textureSize = m_pPreviewTexture->GetSize();

                ImGui::SetNextWindowPos(m_vFloatingPanelPos, ImGuiCond_FirstUseEver);
                ImGui::SetNextWindowBgAlpha(0.9f);

                ImGuiWindowFlags windowFlags =
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_AlwaysAutoResize |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoScrollWithMouse;

                bool deleteRect = false;
                if (ImGui::Begin("Sprite Info", nullptr, windowFlags))
                {
                    // Allow dragging
                    if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                    {
                        m_vFloatingPanelPos += ImGui::GetIO().MouseDelta;
                    }

                    ImGui::Text("Sprite #%d", m_iCurrentSprite);

                    ImGui::Separator();

                    ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_EDITOR_MODAL_SPRITE_INFO"), m_Window.GetFramePadding());
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 7));
                    ImGuiWindow& window = m_Window;
                    ImGui::KeyValue([&window]
                        {
                            ImGui::AlignTextToFramePadding();
                            ImGui::DisplayHeader(window.GetBoldFont(), "Offset");
                        },
                        [&spriteUV, &textureSize]
                        {
                            ImGui::PushItemWidth(75);
                            if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "X_SPRITE_EDITOR_MODAL").c_str(), &spriteUV.x, 1.0f, textureSize.x))
                            {
                                spriteUV.x = std::clamp(spriteUV.x, 0, textureSize.x - spriteUV.height);
                            }
                            ImGui::SameLine();
                            if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "Y_SPRITE_EDITOR_MODAL").c_str(), &spriteUV.y, 1.0f, textureSize.y))
                            {
                                spriteUV.y = std::clamp(spriteUV.y, 0, textureSize.y - spriteUV.width);
                            }
                            ImGui::PopItemWidth();
                        });
                    ImGui::KeyValue([&window]
                        {
                            ImGui::AlignTextToFramePadding();
                            ImGui::DisplayHeader(window.GetBoldFont(), "Size");
                        },
                        [&spriteUV, &textureSize]
                        {
                            ImGui::PushItemWidth(75);
                            if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "WIDTH_SPRITE_EDITOR_MODAL").c_str(), &spriteUV.width, 1.0f, textureSize.x))
                            {
                                spriteUV.width = std::clamp(spriteUV.width, 1, textureSize.x - spriteUV.x);
                            }
                            ImGui::SameLine();
                            if (ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "HEIGHT_SPRITE_EDITOR_MODAL").c_str(), &spriteUV.height, 1.0f, textureSize.y))
                            {
                                spriteUV.height = std::clamp(spriteUV.height, 1, textureSize.y - spriteUV.y);
                            }
                            ImGui::PopItemWidth();
                        });
                    ImGui::PopStyleVar();
                    ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());

                    if (m_iCurrentSprite != -1 && m_iCurrentSprite < m_pTextureMetaData->GetSprites().size())
                    {
                        graphics::dx12::SpriteRect& spriteUV = m_pTextureMetaData->GetSprites()[m_iCurrentSprite];

                        if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_DELETE) + " Delete", BUTTON_ID, "DELETE_SPRITE_EDITOR_MODAL").c_str(), ImVec2(0, 0)))
                        {
                            deleteRect = true;
                        }

                        ImGui::SameLine();
                    }

                    ImGui::End();

                    if ((deleteRect || ImGui::IsKeyDown(ImGuiKey_Delete)) && m_iCurrentSprite != -1)
                    {
                        m_pTextureMetaData->GetSprites().erase(m_pTextureMetaData->GetSprites().begin() + m_iCurrentSprite);
                        m_iCurrentSprite = -1;
                    }
                }
            }
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE