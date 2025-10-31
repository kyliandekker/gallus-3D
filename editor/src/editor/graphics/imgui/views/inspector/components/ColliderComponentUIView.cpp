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
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Offset: ");
					},
					[this]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						m_OffsetView.SetValue(m_Component.GetOffset());
						if (m_OffsetView.Render("COLLIDER_COMPONENT_OFFSET_INSPECTOR", 0.01f))
						{
							m_Component.SetOffset(m_OffsetView.GetValue());
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Size: ");
					},
					[this]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						m_SizeView.SetValue(m_Component.GetSize());
						if (m_SizeView.Render("COLLIDER_COMPONENT_SIZE_INSPECTOR", 0.01f))
						{
							m_Component.SetSize(m_SizeView.GetValue());
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);
			}

			constexpr float HANDLE_RADIUS = 6.0f;

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

				// get transform + camera
				auto& transform = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(m_Component.GetEntityID());
				graphics::dx12::Camera& cam = core::ENGINE->GetDX12().GetActiveCamera();

				const DirectX::XMFLOAT2& vWorldPos = transform.Transform().GetPosition();
				const DirectX::XMFLOAT2& vScale = transform.Transform().GetScale();
				const DirectX::XMFLOAT2& vPivot = transform.Transform().GetPivot();
				float fRotationDegrees = transform.Transform().GetRotation();

				// camera world position (center)
				const DirectX::XMFLOAT2& vCamPos = cam.Transform().GetPosition();

				// collider data
				DirectX::XMFLOAT2 vColliderOffset = m_Component.GetOffset();
				DirectX::XMFLOAT2 vColliderSize = m_Component.GetSize();

				// Build a precise world matrix for the entity (with pivot applied).
				// If your Transform class already has GetWorldMatrixWithPivot(), prefer that.
				DirectX::XMMATRIX matScale = DirectX::XMMatrixScaling(vScale.x, vScale.y, 1.0f);

				// rotation around Z (Degrees -> radians)
				float rotRad = DirectX::XMConvertToRadians(fRotationDegrees);
				DirectX::XMMATRIX matRot = DirectX::XMMatrixRotationZ(rotRad);

				// When pivot is used we usually apply translation by (-pivot * size) before scaling/rotation.
				// Build translation for pivot + collider offset + world position:
				// First translate by entity position.
				DirectX::XMMATRIX matTrans = DirectX::XMMatrixTranslation(vWorldPos.x, vWorldPos.y, 0.0f);

				// If your transform's pivot is already baked into GetWorldMatrixWithPivot(), use that instead.
				// Compose world matrix: scale -> rot -> translate (commonly used for 2D)
				DirectX::XMMATRIX worldMat = transform.Transform().GetWorldMatrixWithPivot();

				// If your engine treats pivot differently (pivot in local coords), you may need:
				// worldMat = XMMatrixTranslation(-vPivot.x * vScale.x * <width?>, -vPivot.y * vScale.y * <height?>, 0) * worldMat;
				// But since you have GetColliderWorldCorners that uses offset/pivot internally, we only need consistent handling below.

				// Translate world into camera space (camera is at vCamPos). We'll subtract camera translation
				DirectX::XMMATRIX cameraTranslation = DirectX::XMMatrixTranslation(-vCamPos.x, -vCamPos.y, 0.0f);
				DirectX::XMMATRIX worldToCamera = worldMat * cameraTranslation;

				// Compute local (object-space) collider corners (centered around the entity origin, respecting offset & size)
				// We compute corners relative to entity origin: half extents +/- plus offset
				DirectX::XMFLOAT2 half = { vColliderSize.x * 0.5f, vColliderSize.y * 0.5f };

				// Local-space corners in the same order you used previously (matching GetColliderWorldCorners)
				std::array<DirectX::XMFLOAT2, 4> localCorners = {
					DirectX::XMFLOAT2(-half.x + vColliderOffset.x, -half.y + vColliderOffset.y), // bottom-left
					DirectX::XMFLOAT2(half.x + vColliderOffset.x, -half.y + vColliderOffset.y), // bottom-right
					DirectX::XMFLOAT2(half.x + vColliderOffset.x,  half.y + vColliderOffset.y), // top-right
					DirectX::XMFLOAT2(-half.x + vColliderOffset.x,  half.y + vColliderOffset.y)  // top-left
				};

				// Transform local corners by worldToCamera to get camera-relative positions
				ImVec2 screenCorners[4];
				for (int i = 0; i < 4; ++i)
				{
					DirectX::XMVECTOR local = DirectX::XMVectorSet(localCorners[i].x, localCorners[i].y, 0.0f, 1.0f);
					DirectX::XMVECTOR camSpace = DirectX::XMVector4Transform(local, worldToCamera);

					DirectX::XMFLOAT4 out;
					DirectX::XMStoreFloat4(&out, camSpace);

					// camera-relative coordinates in world units (x,y)
					ImVec2 sceneLocal = ImVec2(out.x * a_fZoom, out.y * a_fZoom);

					// If your UI Y axis is inverted (screen Y down), flip here:
					// sceneLocal.y = -sceneLocal.y; // <-- enable only if required

					screenCorners[i] = a_vScenePos + a_vPanOffset + sceneLocal;
				}

				// Draw quad
				ImU32 handleCol = ImGui::ColorConvertFloat4ToU32(m_Window.GetAccentColor());
				drawList->AddPolyline(screenCorners, 4, handleCol, true, 2.0f);

				// handle centers (same semantics as your previous code)
				ImVec2 handles[4] = {
					(screenCorners[0] + screenCorners[3]) * 0.5f, // left
					(screenCorners[1] + screenCorners[2]) * 0.5f, // right
					(screenCorners[0] + screenCorners[1]) * 0.5f, // top
					(screenCorners[2] + screenCorners[3]) * 0.5f  // bottom
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

				// Dragging: use same conversion as before (mouse delta -> world delta)
				if (m_iSelectedHandle != -1)
				{
					if (ImGui::IsMouseDown(0))
					{
						ImVec2 delta = ImGui::GetIO().MouseDelta;

						float dx = delta.x / (vScale.x * a_fZoom);
						float dy = delta.y / (vScale.y * a_fZoom);

						const float minSize = 0.01f;

						if (m_iSelectedHandle == 0) // left
						{
							vColliderSize.x -= dx;
							vColliderOffset.x += dx * 0.5f;
							if (vColliderSize.x < minSize) vColliderSize.x = minSize;
						}
						else if (m_iSelectedHandle == 1) // right
						{
							vColliderSize.x += dx;
							vColliderOffset.x += dx * 0.5f;
							if (vColliderSize.x < minSize) vColliderSize.x = minSize;
						}
						else if (m_iSelectedHandle == 2) // top
						{
							vColliderSize.y -= dy;
							vColliderOffset.y += dy * 0.5f;
							if (vColliderSize.y < minSize) vColliderSize.y = minSize;
						}
						else if (m_iSelectedHandle == 3) // bottom
						{
							vColliderSize.y += dy;
							vColliderOffset.y += dy * 0.5f;
							if (vColliderSize.y < minSize) vColliderSize.y = minSize;
						}

						m_Component.SetSize(vColliderSize);
						m_Component.SetOffset(vColliderOffset);
						core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
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
