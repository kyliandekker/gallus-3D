#include "EntityEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "imgui_system/font_icon.h"
#include "imgui_system/ImGuiSystem.h"
#include "graphics/dx12/Camera.h"

// utils
#include "utils/string_extensions.h"

// resources
#include "editor/graphics/imgui/RenderSerializableObject.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/Entity.h"
// systems
#include "gameplay/systems/MeshSystem.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/AnimationSystem.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/HealthSystem.h"
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/ProjectileSystem.h"
#include "gameplay/systems/RigidbodySystem.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/Editor.h"
#include "editor/GlobalEditorFunctions.h"
#include "editor/commands/IEditorEntityCommand.h"
#include "editor/EditorWorkspace.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	std::string GetSystemIcon(gameplay::AbstractECSSystem* a_pSystem)
	{
		if (dynamic_cast<gameplay::MeshSystem*>(a_pSystem))
		{
			return font::ICON_MODEL;
		}
		else if (dynamic_cast<gameplay::SpriteSystem*>(a_pSystem))
		{
			return font::ICON_IMAGE;
		}
		else if (dynamic_cast<gameplay::AnimationSystem*>(a_pSystem))
		{
			return font::ICON_ANIMATION;
		}
		else if (dynamic_cast<gameplay::TransformSystem*>(a_pSystem))
		{
			return font::ICON_AXIS;
		}
		else if (dynamic_cast<gameplay::HealthSystem*>(a_pSystem))
		{
			return font::ICON_HEART;
		}
		else if (dynamic_cast<gameplay::CollisionSystem*>(a_pSystem))
		{
			return font::ICON_GIZMO_BOUNDS;
		}
		else if (dynamic_cast<gameplay::ProjectileSystem*>(a_pSystem))
		{
			return font::ICON_PROJECTILE;
		}
		else if (dynamic_cast<gameplay::RigidbodySystem*>(a_pSystem))
		{
			return font::ICON_AXIS;
		}
		return font::ICON_MODEL;
	}

	//---------------------------------------------------------------------
	EntityEditorSelectable::EntityEditorSelectable(ImGuiSystem& a_System, gameplay::EntityID& a_EntityID) : BaseEntityEditorSelectable(a_System), m_EntityID(a_EntityID)
	{
		SetIcon();

		m_bShowRename = true;
		m_bShowDelete = true;
		m_bShowActivate = true;

		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		m_pEntity = ecs->GetEntity(m_EntityID);
		std::shared_ptr<gameplay::Entity> ent = m_pEntity.lock();
		if (!ent)
		{
			ent->Destroy();
		}

		gameplay::EntityID entityId = ent->GetEntityID();

		for (gameplay::AbstractECSSystem* sys : ecs->GetSystems())
		{
			if (sys->HasComponent(entityId))
			{
				const gameplay::Component* comp = sys->GetBaseComponent(entityId);

				std::string id = ImGui::IMGUI_FORMAT_ID("",
					FOLDOUT_ID, string_extensions::StringToUpper(comp->GetTypeName()) + "_INSPECTOR");

				ComponentEntrySelectable componentInfo = { false, GetSystemIcon(sys) };
				m_aComponentInfo.insert(std::make_pair(id, componentInfo));
			}
		}
	}

	//---------------------------------------------------------------------
	EntityEditorSelectable::~EntityEditorSelectable() = default;

	//---------------------------------------------------------------------
	void EntityEditorSelectable::RenderEntity(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected)
	{
		std::shared_ptr<gameplay::Entity> ent = m_pEntity.lock();
		if (!ent)
		{
			ent->Destroy();
		}

		m_bIsActive = ent->IsActive();

		BaseEntityEditorSelectable::RenderEntity(a_bClicked, a_bDoubleClicked, a_bSelected);
	}

	//---------------------------------------------------------------------
	void EntityEditorSelectable::OnRename(const std::string& a_sName)
	{
		editor::g_RenameEntity(m_EntityID, a_sName);
	}

	//---------------------------------------------------------------------
	void EntityEditorSelectable::OnDelete()
	{
		editor::g_DeleteEntity(m_EntityID);
	}

	//---------------------------------------------------------------------
	void EntityEditorSelectable::OnActivate(bool a_bActive)
	{
		editor::g_SetEntityActive(m_EntityID, a_bActive);
	}

	//---------------------------------------------------------------------
	const std::string& EntityEditorSelectable::GetName() const
	{
		std::shared_ptr<gameplay::Entity> ent = m_pEntity.lock();
		return ent->GetName();
	}

	//---------------------------------------------------------------------
	void EntityEditorSelectable::SetIcon()
	{
		if (GetEditorEngine().GetECS()->GetSystem<gameplay::MeshSystem>()->HasComponent(m_EntityID))
		{
			m_sIcon = font::ICON_MODEL;
		}
		else if (GetEditorEngine().GetECS()->GetSystem<gameplay::SpriteSystem>()->HasComponent(m_EntityID))
		{
			m_sIcon = font::ICON_IMAGE;
		}
		else if (GetEditorEngine().GetECS()->GetSystem<gameplay::AnimationSystem>()->HasComponent(m_EntityID))
		{
			m_sIcon = font::ICON_ANIMATION;
		}
		else if (GetEditorEngine().GetECS()->GetSystem<gameplay::TransformSystem>()->HasComponent(m_EntityID))
		{
			m_sIcon = font::ICON_AXIS;
		}
		else
		{
			m_sIcon = font::ICON_MODEL;
		}
	}

	//---------------------------------------------------------------------
	void EntityEditorSelectable::OnDoubleClicked()
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		gameplay::TransformSystem* transformSys = ecs->GetSystem<gameplay::TransformSystem>();
		if (!transformSys)
		{
			return;
		}

		if (!transformSys->HasComponent(m_EntityID))
		{
			return;
		}

		gameplay::TransformComponent* transformComponent = transformSys->TryGetComponent(m_EntityID);
		if (!transformComponent)
		{
			return;
		}

		DirectX::XMFLOAT3 pos = {
			transformComponent->GetTransform().GetPosition().x - (graphics::dx12::RENDER_TEX_SIZE.x / 2),
			transformComponent->GetTransform().GetPosition().y - (graphics::dx12::RENDER_TEX_SIZE.y / 2),
			transformComponent->GetTransform().GetPosition().z
		};
		GetEditorEngine().GetEditor()->GetEditorCamera().GetTransform().SetPosition(pos);
	}

	//---------------------------------------------------------------------
	bool EntityEditorSelectable::RenderEditorFields()
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return false;
		}

		bool changed = false;
		std::lock_guard<std::recursive_mutex> lock(ecs->m_EntityMutex);

		ImGui::SetCursorPosY(0);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		// TODO: Not good, gets reallocated every frame.
		resources::SrcData oldData;
		oldData.SetObject();
		ecs->SerializeEntity(m_EntityID, oldData);

		for (gameplay::AbstractECSSystem* sys : ecs->GetSystems())
		{
			bool deleteComp = false;

			ImGui::SetCursorPosX(0);
			float width = ImGui::GetContentRegionAvail().x + m_System.GetFramePadding().x;
			ImGui::SetNextItemWidth(width);
			if (sys->HasComponent(m_EntityID))
			{
				gameplay::Component* comp = sys->GetBaseComponent(m_EntityID);

				ImVec2 size = m_System.GetHeaderSize();

				float width = ImGui::GetContentRegionAvail().x + m_System.GetFramePadding().x;
				width -= size.x;
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

				ImVec2 foldOutButtonPos = ImGui::GetCursorScreenPos();

				std::string id = ImGui::IMGUI_FORMAT_ID("",
					FOLDOUT_ID, string_extensions::StringToUpper(comp->GetTypeName()) + "_INSPECTOR");

				ComponentEntrySelectable& componentInfo = m_aComponentInfo[id];
				ImGui::FoldOutButton(
					std::string((componentInfo.m_bExpanded ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + componentInfo.m_sIcon + " " + sys->GetSystemName() + id).c_str(), &componentInfo.m_bExpanded, ImVec2(width, size.y));
				ImGui::SameLine();
				if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, id + "_DELETE_INSPECTOR").c_str(), "Deletes the specific component from the selected entity.", size))
				{
					deleteComp = true;
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				if (componentInfo.m_bExpanded)
				{
					if (RenderObjectFields(comp))
					{
						changed = true;
					}
				}
				if (deleteComp)
				{
					editor::g_DeleteComponent(sys, m_EntityID);
				}
			}
		}
		ImGui::PopStyleVar();

		ImGui::SetCursorPosX(0 + m_System.GetFramePadding().x);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_System.GetFramePadding().y);

		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FOLDER + std::string(" Add Component"), BUTTON_ID, "ADD_COMPONENT_INSPECTOR").c_str(), ImVec2(width, 0)))
		{
			ImVec2 buttonPos = ImGui::GetItemRectMin();

			// Set the popup window position just below the button
			ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y));

			ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str());
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_System.GetFramePadding());
		ImGui::SetNextWindowSize(ImVec2(width, 0));
		if (ImGui::BeginPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str()))
		{
			for (gameplay::AbstractECSSystem* sys : ecs->GetSystems())
			{
				if (sys->HasComponent(m_EntityID))
				{
					continue;
				}

				if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(GetSystemIcon(sys) + " " + sys->GetSystemName(), MENU_ITEM_ID, "ADD_COMPONENT_MENU_INSPECTOR_" + sys->GetPropertyName()).c_str()))
				{
					editor::g_CreateComponent(sys, m_EntityID);
					changed = true;
				}
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		if (changed)
		{
			editor::g_SetEditorSceneDirty();
		}

		EditingInteractionState state = m_FieldState.Update(changed);

		if (state == EditingInteractionState::EditingInteractionState_StartInteraction)
		{
			m_pEditorCommand = std::make_unique<editor::IEditorEntityCommand>();
			m_pEditorCommand->SetOldData(oldData);
		}
		else if (state == EditingInteractionState::EditingInteractionState_EndInteraction)
		{
			if (m_pEditorCommand)
			{
				resources::SrcData newData;
				newData.SetObject();
				ecs->SerializeEntity(m_EntityID, newData);

				m_pEditorCommand->SetNewData(newData);

				GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));
				m_pEditorCommand = nullptr;
			}
		}

		return changed;
	}

	//---------------------------------------------------------------------
	bool EntityEditorSelectable::RenderGizmos(const ImRect& a_SceneViewRect)
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return false;
		}

		std::lock_guard<std::recursive_mutex> lock(ecs->m_EntityMutex);

		// TODO: Not good, gets reallocated every frame.
		resources::SrcData oldData;
		oldData.SetObject();
		ecs->SerializeEntity(m_EntityID, oldData);

		bool changed = false;
		for (gameplay::AbstractECSSystem* sys : ecs->GetSystems())
		{
			if (sys->HasComponent(GetEntityID()))
			{
				gameplay::Component* comp = sys->GetBaseComponent(GetEntityID());

				bool changedField = RenderObjectGizmos(a_SceneViewRect, comp);
				if (changedField)
				{
					changed = true;
				}
			}
		}

		if (changed)
		{
			editor::g_SetEditorSceneDirty();
		}

		EditingInteractionState state = m_GizmoState.Update(changed);

		if (state == EditingInteractionState::EditingInteractionState_StartInteraction)
		{
			m_pEditorCommand = std::make_unique<editor::IEditorEntityCommand>();
			m_pEditorCommand->SetOldData(oldData);
		}
		else if (state == EditingInteractionState::EditingInteractionState_EndInteraction)
		{
			if (m_pEditorCommand)
			{
				resources::SrcData newData;
				ecs->SerializeEntity(m_EntityID, newData);

				m_pEditorCommand->SetNewData(newData);

				GetEditorEngine().GetEditorWorkspace()->AddAction(std::move(m_pEditorCommand));
				m_pEditorCommand = nullptr;
			}
		}

		return changed;
	}
}