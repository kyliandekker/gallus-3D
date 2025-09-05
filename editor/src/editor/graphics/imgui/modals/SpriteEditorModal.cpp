#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "SpriteEditorModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>
#include <algorithm>

// core includes
#include "editor/core/EditorTool.h"

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

// graphics includes
#include "editor/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/metadata/TextureMetaData.h"

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
				auto cCommandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				auto cCommandList = cCommandQueue->GetCommandList();
				m_pPreviewTexture = core::TOOL->GetResourceAtlas().LoadTexture(a_sName, cCommandList);
				m_pPreviewTexture->SetResourceCategory(gallus::core::EngineResourceCategory::Editor);
				cCommandQueue->ExecuteCommandList(cCommandList);
				cCommandQueue->Flush();
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

                // --- Static editor state ---
                static float m_fZoom = 1.0f;
                static ImVec2 m_vPanOffset = ImVec2(0, 0);
                static std::vector<ImVec4> spriteRects; // {x, y, w, h} in texture space
                static int selectedRect = -1;
                static int resizingHandle = -1; // -1 = none, 0..3 = which corner
                static bool draggingRect = false;

                ImVec2 textureSize(
                    static_cast<float>(m_pPreviewTexture->GetSize().x),
                    static_cast<float>(m_pPreviewTexture->GetSize().y)
                );

                if (ImGui::BeginChild("SpriteToolbar", ImVec2(0, 72), true))
                {
                    if (ImGui::Button("Add Rect"))
                    {
                        spriteRects.push_back(ImVec4(10, 10, 50, 50)); // default new rect
                        selectedRect = (int) spriteRects.size() - 1;
                    }
                    ImGui::SameLine();
                    if (selectedRect >= 0)
                    {
                        if (ImGui::Button("Delete"))
                        {
                            spriteRects.erase(spriteRects.begin() + selectedRect);
                            selectedRect = -1;
                            draggingRect = false;
                            resizingHandle = -1;
                        }

                        ImVec4& r = spriteRects[selectedRect];
                        float x = r.x;
                        float y = r.y;
                        float w = r.z;
                        float h = r.w;

                        ImGui::SameLine();
                        if (ImGui::InputFloat("X", &x, 1.0f, 10.0f))
                        {
                            r.x = std::clamp(x, 0.0f, textureSize.x - r.z);
                        }
                        ImGui::SameLine();
                        if (ImGui::InputFloat("Y", &y, 1.0f, 10.0f))
                        {
                            r.y = std::clamp(y, 0.0f, textureSize.y - r.w);
                        }
                        ImGui::SameLine();
                        if (ImGui::InputFloat("Width", &w, 1.0f, 10.0f))
                        {
                            r.z = std::clamp(w, 1.0f, textureSize.x - r.x);
                        }
                        ImGui::SameLine();
                        if (ImGui::InputFloat("Height", &h, 1.0f, 10.0f))
                        {
                            r.w = std::clamp(h, 1.0f, textureSize.y - r.y);
                        }
                    }
                    ImGui::EndChild();
                }

                ImVec2 windowSize = ImGui::GetContentRegionAvail();

                // --- Main scrollable view ---
                ImGui::BeginChild("SceneScroll", windowSize, true,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

                ImVec2 childPos = ImGui::GetCursorScreenPos();
                ImVec2 mouseScreen = ImGui::GetIO().MousePos;
                ImVec2 mouseLocal = mouseScreen - childPos;

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
                ImVec2 viewSize = windowSize;
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

                // --- Draw texture ---
                ImVec2 imagePos = m_vPanOffset;
                ImVec2 imageSize(textureSize.x * m_fZoom, textureSize.y * m_fZoom);

                ImGui::SetCursorPos(imagePos);
                ImGui::Image((ImTextureID) m_pPreviewTexture->GetGPUHandle().ptr, imageSize);

                ImVec2 imgMin = ImGui::GetItemRectMin(); // screen coords
                ImVec2 imgMax = ImGui::GetItemRectMax();
                ImDrawList* dl = ImGui::GetWindowDrawList();

                dl->AddRect(imgMin, imgMax, IM_COL32(255, 255, 255, 255), 0, 0, 2.0f);

                // --- Interaction ---
                bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
                bool mouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                bool mouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);

                // Handle resize/drag
                if (selectedRect >= 0 && selectedRect < spriteRects.size())
                {
                    ImVec4& r = spriteRects[selectedRect];
                    ImVec2 rectMinTex(r.x, r.y);
                    ImVec2 rectMaxTex(r.x + r.z, r.y + r.w);

                    ImVec2 rectMin = imgMin + rectMinTex * m_fZoom;
                    ImVec2 rectMax = imgMin + rectMaxTex * m_fZoom;

                    // Draw selected rect
                    dl->AddRect(rectMin, rectMax, IM_COL32(0, 255, 0, 255), 0, 0, 1.0f);

                    // Handle squares
                    ImVec2 handles[4] = {
                        rectMin,
                        ImVec2(rectMax.x, rectMin.y),
                        rectMax,
                        ImVec2(rectMin.x, rectMax.y)
                    };
                    float handleSize = 6.0f;

                    for (int i = 0; i < 4; i++)
                    {
                        ImVec2 hMin(handles[i].x - handleSize, handles[i].y - handleSize);
                        ImVec2 hMax(handles[i].x + handleSize, handles[i].y + handleSize);
                        dl->AddRectFilled(hMin, hMax, IM_COL32(0, 200, 0, 200));
                        dl->AddRect(hMin, hMax, IM_COL32(0, 255, 0, 255));

                        if (mouseClicked &&
                            mouseScreen.x >= hMin.x && mouseScreen.x <= hMax.x &&
                            mouseScreen.y >= hMin.y && mouseScreen.y <= hMax.y)
                        {
                            resizingHandle = i;
                        }
                    }

                    if (resizingHandle != -1 && mouseDown)
                    {
                        ImVec2 deltaTex = (ImGui::GetIO().MouseDelta) / m_fZoom;

                        if (resizingHandle == 0) // top-left
                        {
                            r.x += deltaTex.x;
                            r.y += deltaTex.y;
                            r.z -= deltaTex.x;
                            r.w -= deltaTex.y;
                        }
                        if (resizingHandle == 1) // top-right
                        {
                            r.y += deltaTex.y;
                            r.z += deltaTex.x;
                            r.w -= deltaTex.y;
                        }
                        if (resizingHandle == 2) // bottom-right
                        {
                            r.z += deltaTex.x;
                            r.w += deltaTex.y;
                        }
                        if (resizingHandle == 3) // bottom-left
                        {
                            r.x += deltaTex.x;
                            r.z -= deltaTex.x;
                            r.w += deltaTex.y;
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
                        if (r.x + r.z > textureSize.x)
                        {
                            r.z = textureSize.x - r.x;
                        }
                        if (r.y + r.w > textureSize.y)
                        {
                            r.w = textureSize.y - r.y;
                        }
                    }

                    if (mouseReleased)
                    {
                        resizingHandle = -1;
                    }

                    // If not resizing, allow dragging the whole rect
                    if (resizingHandle == -1)
                    {
                        if (mouseClicked &&
                            mouseScreen.x >= rectMin.x && mouseScreen.x <= rectMax.x &&
                            mouseScreen.y >= rectMin.y && mouseScreen.y <= rectMax.y)
                        {
                            draggingRect = true;
                        }

                        if (draggingRect && mouseDown)
                        {
                            ImVec2 deltaTex = (ImGui::GetIO().MouseDelta) / m_fZoom;
                            r.x += deltaTex.x;
                            r.y += deltaTex.y;

                            // Clamp inside image
                            if (r.x < 0.0f)
                            {
                                r.x = 0.0f;
                            }
                            if (r.y < 0.0f)
                            {
                                r.y = 0.0f;
                            }
                            if (r.x + r.z > textureSize.x)
                            {
                                r.x = textureSize.x - r.z;
                            }
                            if (r.y + r.w > textureSize.y)
                            {
                                r.y = textureSize.y - r.w;
                            }
                        }

                        if (mouseReleased)
                        {
                            draggingRect = false;
                        }
                    }
                }

                // Draw all other rects
                for (int i = 0; i < spriteRects.size(); i++)
                {
                    if (i == selectedRect)
                    {
                        continue;
                    }
                    ImVec4 r = spriteRects[i];
                    ImVec2 rectMinTex(r.x, r.y);
                    ImVec2 rectMaxTex(r.x + r.z, r.y + r.w);

                    ImVec2 rectMin = imgMin + rectMinTex * m_fZoom;
                    ImVec2 rectMax = imgMin + rectMaxTex * m_fZoom;

                    if (mouseClicked &&
                        mouseScreen.x >= rectMin.x && mouseScreen.x <= rectMax.x &&
                        mouseScreen.y >= rectMin.y && mouseScreen.y <= rectMax.y)
                    {
                        selectedRect = i;
                    }

                    dl->AddRect(rectMin, rectMax, IM_COL32(200, 200, 0, 255), 0, 0, 1.0f);
                }

                ImGui::EndChild();
			}

			void SpriteEditorModal::SetData(editor::TextureMetaData& a_TextureMetaData)
			{
				m_pTextureMetaData = &a_TextureMetaData;
				LoadTexture(m_pTextureMetaData->GetFileResource().GetPath().filename().generic_string());
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE