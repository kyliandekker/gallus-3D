#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ColliderComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay includes
#include "gameplay/Game.h"
#include "gameplay/systems/TransformSystem.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string ColliderComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void ColliderComponentUIView::RenderInner()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Offset: ");
				ImGui::Indent();

				m_OffsetView.SetValue(m_Component.GetOffset());
				if (m_OffsetView.Render("COLLIDER_OFFSET_INSPECTOR", 0.01f))
				{
					m_Component.SetOffset(m_OffsetView.GetValue());
					gameplay::GAME.GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Size: ");
				ImGui::Indent();

				m_SizeView.SetValue(m_Component.GetSize());
				if (m_SizeView.Render("COLLIDER_SIZE_INSPECTOR", 0.01f))
				{
					m_Component.SetSize(m_SizeView.GetValue());
					gameplay::GAME.GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

            void ColliderComponentUIView::RenderComponentGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom)
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 mouseScreen = ImGui::GetMousePos();
                std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

                if (!core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(m_Component.GetEntityID()))
                {
                    return;
                }

                auto& transform = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(m_Component.GetEntityID());

                const DirectX::XMFLOAT2& pos = transform.Transform().GetPosition();
                const DirectX::XMFLOAT2& scale = transform.Transform().GetScale();
                const DirectX::XMFLOAT2& pivot = transform.Transform().GetPivot();
                float rotation = transform.Transform().GetRotation(); // degrees

                DirectX::XMFLOAT2 colliderOffset = m_Component.GetOffset();
                DirectX::XMFLOAT2 colliderSize = m_Component.GetSize();

                ImVec2 objectPos = a_vScenePos + a_vPanOffset + ImVec2(pos.x * a_fZoom, pos.y * a_fZoom);

                ImVec2 finalSize = ImVec2(colliderSize.x * scale.x * a_fZoom, colliderSize.y * scale.y * a_fZoom);

                ImVec2 pivotShift = ImVec2(-pivot.x * scale.x * a_fZoom, -pivot.y * scale.y * a_fZoom);
                ImVec2 localOffset = ImVec2(colliderOffset.x * scale.x * a_fZoom, colliderOffset.y * scale.y * a_fZoom);
                ImVec2 localCenter = pivotShift + localOffset;

                float rotationRad = DirectX::XMConvertToRadians(rotation);
                float cosR = cosf(rotationRad);
                float sinR = sinf(rotationRad);

                ImVec2 rotatedCenter = ImVec2(localCenter.x * cosR - localCenter.y * sinR, localCenter.x * sinR + localCenter.y * cosR);
                ImVec2 colliderCenter = objectPos + rotatedCenter;

                ImVec2 corners[4] = {
                    ImVec2(-0.5f * finalSize.x, -0.5f * finalSize.y),
                    ImVec2(0.5f * finalSize.x, -0.5f * finalSize.y),
                    ImVec2(0.5f * finalSize.x, 0.5f * finalSize.y),
                    ImVec2(-0.5f * finalSize.x, 0.5f * finalSize.y)
                };

                for (int i = 0; i < 4; ++i)
                {
                    float x = corners[i].x;
                    float y = corners[i].y;
                    float rx = x * cosR - y * sinR;
                    float ry = x * sinR + y * cosR;
                    corners[i].x = rx + colliderCenter.x;
                    corners[i].y = ry + colliderCenter.y;
                }

                ImU32 handleCol = ImGui::ColorConvertFloat4ToU32(m_Window.GetAccentColor());

                // Draw collider bounds
                drawList->AddPolyline(corners, 4, handleCol, true, 2.0f);

                // === Edge handles ===
                ImVec2 handles[4] = {
                    (corners[0] + corners[3]) * 0.5f, // Left
                    (corners[1] + corners[2]) * 0.5f, // Right
                    (corners[0] + corners[1]) * 0.5f, // Top
                    (corners[2] + corners[3]) * 0.5f  // Bottom
                };

                static int activeHandle = -1;
                const float handleRadius = 6.0f;

                for (int i = 0; i < 4; ++i)
                {
                    ImVec4 defaultCol = m_Window.GetAccentColor();
                    defaultCol.w = 0.5f;

                    bool hovered = ImLengthSqr(mouseScreen - handles[i]) <= handleRadius * handleRadius;
                    drawList->AddCircleFilled(handles[i], handleRadius, hovered ? handleCol : ImGui::ColorConvertFloat4ToU32(defaultCol));
                    if (hovered && ImGui::IsMouseClicked(0))
                    {
                        activeHandle = i;
                    }
                }

                if (activeHandle != -1)
                {
                    if (ImGui::IsMouseDown(0))
                    {
                        ImVec2 delta = ImGui::GetIO().MouseDelta;

                        // Convert mouse delta back into local space (ignoring rotation for simplicity)
                        float dx = delta.x / (scale.x * a_fZoom);
                        float dy = delta.y / (scale.y * a_fZoom);

                        const float minSize = 0.01f;

                        if (activeHandle == 0)
                        { // Left
                            colliderSize.x -= dx;
                            colliderOffset.x += dx * 0.5f;          // <-- SIGN FIX (was -=)
                            if (colliderSize.x < minSize) colliderSize.x = minSize;
                        }
                        else if (activeHandle == 1)
                        { // Right
                            colliderSize.x += dx;
                            colliderOffset.x += dx * 0.5f;
                            if (colliderSize.x < minSize) colliderSize.x = minSize;
                        }
                        else if (activeHandle == 2)
                        { // Top
                            colliderSize.y -= dy;
                            colliderOffset.y += dy * 0.5f;          // <-- SIGN FIX (was -=)
                            if (colliderSize.y < minSize) colliderSize.y = minSize;
                        }
                        else if (activeHandle == 3)
                        { // Bottom
                            colliderSize.y += dy;
                            colliderOffset.y += dy * 0.5f;
                            if (colliderSize.y < minSize) colliderSize.y = minSize;
                        }

                        m_Component.SetSize(colliderSize);
                        m_Component.SetOffset(colliderOffset);
                    }
                    else
                    {
                        activeHandle = -1;
                    }
                }
            }
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE