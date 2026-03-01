#include "CameraEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/Camera.h"

// imgui
#include "imgui_system/font_icon.h"
#include "imgui_system/ImGuiSystem.h"

// utils
#include "utils/string_extensions.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/RenderSerializableObject.h"
#include "editor/GlobalEditorFunctions.h"
#include "editor/commands/IEditorCameraCommand.h"
#include "editor/EditorWorkspace.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// CameraEditorSelectable
	//---------------------------------------------------------------------
	CameraEditorSelectable::CameraEditorSelectable(ImGuiSystem& a_System) : BaseEntityEditorSelectable(a_System)
	{
		m_sIcon = font::ICON_CAMERA;

		m_bIsActive = true;
	}

	//---------------------------------------------------------------------
	CameraEditorSelectable::~CameraEditorSelectable() = default;

	//---------------------------------------------------------------------
	const std::string& CameraEditorSelectable::GetName() const
	{
		return m_sName;
	}

	//---------------------------------------------------------------------
	bool CameraEditorSelectable::RenderEditorFields()
	{
		bool changed = false;

		ImGui::SetCursorPosY(0);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		ImGui::SetCursorPosX(0);
		float width = ImGui::GetContentRegionAvail().x + m_System.GetFramePadding().x;
		ImGui::SetNextItemWidth(width);
		
		ImVec2 size = m_System.GetHeaderSize();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		ImVec2 foldOutButtonPos = ImGui::GetCursorScreenPos();

		std::string id = ImGui::IMGUI_FORMAT_ID("",
			FOLDOUT_ID, string_extensions::StringToUpper(GetName()) + "_INSPECTOR");
		ImGui::FoldOutButton(
			std::string((m_bExpanded ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + GetIcon() + " " + GetName() + id).c_str(), &m_bExpanded, ImVec2(width, size.y));

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		graphics::dx12::DX12System& dx12System = GetDX12System();
		graphics::dx12::Camera& camera = dx12System.GetCamera();

		// TODO: Not good, gets reallocated every frame.
		resources::SrcData oldData;
		oldData.SetObject();
		SerializeFields(&camera, oldData);

		if (m_bExpanded)
		{
			if (RenderObjectFields(&camera))
			{
				changed = true;
			}
		}

		ImGui::PopStyleVar();

		ImGui::SetCursorPosX(0 + m_System.GetFramePadding().x);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_System.GetFramePadding().y);

		if (changed)
		{
			editor::g_SetEditorSceneDirty();
		}

		EditingInteractionState state = m_FieldState.Update(changed);

		if (state == EditingInteractionState::EditingInteractionState_StartInteraction)
		{
			m_pEditorCommand = std::make_unique<editor::IEditorCameraCommand>();
			m_pEditorCommand->SetOldData(oldData);
		}
		else if (state == EditingInteractionState::EditingInteractionState_EndInteraction)
		{
			if (m_pEditorCommand)
			{
				resources::SrcData newData;
				newData.SetObject();
				SerializeFields(&camera, newData);

				m_pEditorCommand->SetNewData(newData);

				GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));
				m_pEditorCommand = nullptr;
			}
		}

		return changed;
	}

	//---------------------------------------------------------------------
	bool CameraEditorSelectable::RenderGizmos(const ImRect& a_SceneViewRect)
	{
		graphics::dx12::DX12System& dx12System = GetDX12System();
		graphics::dx12::Camera& camera = dx12System.GetCamera();

		// TODO: Not good, gets reallocated every frame.
		resources::SrcData oldData;
		oldData.SetObject();
		SerializeFields(&camera, oldData);

		bool changed = false;
		if (RenderObjectGizmos(a_SceneViewRect, &camera))
		{
			changed = true;
		}

		if (changed)
		{
			editor::g_SetEditorSceneDirty();
		}

		EditingInteractionState state = m_GizmoState.Update(changed);

		if (state == EditingInteractionState::EditingInteractionState_StartInteraction)
		{
			m_pEditorCommand = std::make_unique<editor::IEditorCameraCommand>();
			m_pEditorCommand->SetOldData(oldData);
		}
		if (state == EditingInteractionState::EditingInteractionState_EndInteraction)
		{
			if (m_pEditorCommand)
			{
				resources::SrcData newData;
				newData.SetObject();
				SerializeFields(&camera, newData);

				m_pEditorCommand->SetNewData(newData);

				GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));
				m_pEditorCommand = nullptr;
			}
		}

		return false;
	}
}