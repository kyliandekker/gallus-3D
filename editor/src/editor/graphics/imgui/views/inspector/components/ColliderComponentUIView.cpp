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

                ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "COLLIDER_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding());
                ImGuiWindow& window = m_Window;
                gameplay::ColliderComponent& colliderComp = m_Component;
                Vector2View<DirectX::XMFLOAT2>& offsetView = m_OffsetView;
                ImGui::KeyValue([&window]
                    {
                        ImGui::AlignTextToFramePadding();
                        ImGui::DisplayHeader(window.GetBoldFont(), "Offset: ");
                    },
                    [&offsetView, &colliderComp]
                    {
                        offsetView.SetValue(colliderComp.GetOffset());
                        if (offsetView.Render("COLLIDER_COMPONENT_OFFSET_INSPECTOR", 0.01f))
                        {
                            colliderComp.SetOffset(offsetView.GetValue());
                            core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
                        }
                    });
                Vector2View<DirectX::XMFLOAT2>& sizeView = m_SizeView;
                ImGui::KeyValue([&window]
                    {
                        ImGui::AlignTextToFramePadding();
                        ImGui::DisplayHeader(window.GetBoldFont(), "Size: ");
                    },
                    [&sizeView, &colliderComp]
                    {
                        sizeView.SetValue(colliderComp.GetSize());
                        if (sizeView.Render("COLLIDER_COMPONENT_SIZE_INSPECTOR", 0.01f))
                        {
                            colliderComp.SetSize(sizeView.GetValue());
                            core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
                        }
                    });
                ImGui::EndInspectorKeyVal();

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);
			}

            constexpr float HANDLE_RADIUS = 6;
            void ColliderComponentUIView::RenderComponentGizmos(
                const ImVec2& a_vScenePos,
                const ImVec2& a_vSize,
                const ImVec2& a_vPanOffset,
                float a_fZoom)
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
                float rotation = transform.Transform().GetRotation();

                DirectX::XMFLOAT2 colliderOffset = m_Component.GetOffset();
                DirectX::XMFLOAT2 colliderSize = m_Component.GetSize();

                std::array<DirectX::XMFLOAT2, 4> worldCorners =
                    m_Component.GetColliderWorldCorners(pos, scale, pivot, rotation);

                ImVec2 screenCorners[4];
                for (int i = 0; i < 4; ++i)
                {
                    screenCorners[i] = a_vScenePos + a_vPanOffset +
                        ImVec2(worldCorners[i].x * a_fZoom,
                            worldCorners[i].y * a_fZoom);
                }

                ImU32 handleCol = ImGui::ColorConvertFloat4ToU32(m_Window.GetAccentColor());

                drawList->AddPolyline(screenCorners, 4, handleCol, true, 2.0f);

                ImVec2 handles[4] = {
                    (screenCorners[0] + screenCorners[3]) * 0.5f, 
                    (screenCorners[1] + screenCorners[2]) * 0.5f, 
                    (screenCorners[0] + screenCorners[1]) * 0.5f, 
                    (screenCorners[2] + screenCorners[3]) * 0.5f  
                };

                for (int i = 0; i < 4; ++i)
                {
                    ImVec4 defaultCol = m_Window.GetAccentColor();
                    defaultCol.w = 0.5f;

                    bool hovered = ImLengthSqr(mouseScreen - handles[i]) <= HANDLE_RADIUS * HANDLE_RADIUS;
                    drawList->AddCircleFilled(handles[i], HANDLE_RADIUS, hovered ? handleCol : ImGui::ColorConvertFloat4ToU32(defaultCol));
                    if (hovered && ImGui::IsMouseClicked(0))
                    {
                        m_iSelectedHandle = i;
                    }
                }

                if (m_iSelectedHandle != -1)
                {
                    if (ImGui::IsMouseDown(0))
                    {
                        ImVec2 delta = ImGui::GetIO().MouseDelta;

                        float dx = delta.x / (scale.x * a_fZoom);
                        float dy = delta.y / (scale.y * a_fZoom);

                        const float minSize = 0.01f;

                        if (m_iSelectedHandle == 0)
                        {
                            colliderSize.x -= dx;
                            colliderOffset.x += dx * 0.5f;
                            if (colliderSize.x < minSize) colliderSize.x = minSize;
                        }
                        else if (m_iSelectedHandle == 1)
                        {
                            colliderSize.x += dx;
                            colliderOffset.x += dx * 0.5f;
                            if (colliderSize.x < minSize) colliderSize.x = minSize;
                        }
                        else if (m_iSelectedHandle == 2)
                        {
                            colliderSize.y -= dy;
                            colliderOffset.y += dy * 0.5f;
                            if (colliderSize.y < minSize) colliderSize.y = minSize;
                        }
                        else if (m_iSelectedHandle == 3)
                        {
                            colliderSize.y += dy;
                            colliderOffset.y += dy * 0.5f;
                            if (colliderSize.y < minSize) colliderSize.y = minSize;
                        }

                        m_Component.SetSize(colliderSize);
                        m_Component.SetOffset(colliderOffset);
                    }
                    else
                    {
                        m_iSelectedHandle = -1;
                    }
                }
            }
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE