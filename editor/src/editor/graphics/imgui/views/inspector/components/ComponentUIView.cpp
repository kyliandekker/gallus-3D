#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>
#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "utils/string_extensions.h"

#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

#include "gameplay/ECSBaseSystem.h"
#include "gameplay/systems/components/Component.h"
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// ComponentBaseUIView
			//---------------------------------------------------------------------
			void ComponentBaseUIView::RenderBaseComponent(gameplay::Component& a_Component, gameplay::AbstractECSSystem& a_System, const gameplay::EntityID& a_EntityID)
			{
				ImVec2 size = m_Window.GetHeaderSize();

				float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
				width -= size.x;
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::FoldOutButton(
					ImGui::IMGUI_FORMAT_ID(
					std::string((m_bFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + GetName()),
					FOLDOUT_ID, string_extensions::StringToUpper(GetName()) + "_INSPECTOR"), &m_bFoldedOut, ImVec2(width, size.y));
				ImGui::SameLine();
				if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, string_extensions::StringToUpper(GetName()) + "_DELETE_HIERARCHY").c_str(), size, m_Window.GetIconFont()))
				{
					a_System.DeleteComponent(a_EntityID);
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				if (m_bFoldedOut)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetWindowPadding().y);

					ImGui::Indent(m_Window.GetFramePadding().x);
					RenderInner();
					ImGui::Unindent(m_Window.GetFramePadding().x);
				}
			}

			//---------------------------------------------------------------------
			void ComponentBaseUIView::DrawGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom)
			{
				ImGuizmo::BeginFrame();

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

				ImGuizmo::SetRect(a_vScenePos.x + a_vPanOffset.x, a_vScenePos.y + a_vPanOffset.y, a_vSize.x * a_fZoom, a_vSize.y * a_fZoom);
			}

			//---------------------------------------------------------------------
			void ComponentBaseUIView::DrawTransformGizmo(graphics::dx12::DX12Transform& a_Transform, const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom)
			{
				DirectX::XMMATRIX pivotOffset = DirectX::XMMatrixTranslation(a_Transform.GetPivot().x, a_Transform.GetPivot().y, 0.0f);
				DirectX::XMMATRIX objectMat = a_Transform.GetWorldMatrix();
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
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::TRANSLATE);
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_R))
				{
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::ROTATE_Z);
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_S))
				{
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::SCALE);
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}

				bool useSnap = ImGui::IsKeyDown(ImGuiKey_LeftShift);
				float snap = useSnap ? 1.0f : 0.0f;

				// Render the gizmo (check if manipulation occurred)
				if (ImGuizmo::Manipulate(
					viewFloat,
					projFloat,
					(ImGuizmo::OPERATION) core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation(),
					ImGuizmo::LOCAL,
					objectFloat, 0, &snap))
				{
					DirectX::XMMATRIX result = DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(objectFloat));
					result = result * DirectX::XMMatrixInverse(nullptr, pivotOffset);

					a_Transform.SetWorldMatrix(result);

					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE