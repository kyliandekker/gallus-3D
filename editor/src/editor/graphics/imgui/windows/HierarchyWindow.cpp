#include "HierarchyWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// gameplay
#include "gameplay/Entity.h"
#include "gameplay/EntityComponentSystem.h"

// resources
#include "resources/AssetType.h"

// extensions
#include "utils/string_extensions.h"

// gameplay
#include "gameplay/Game.h"

// imgui
#include "imgui_system/font_icon.h"
#include "imgui_system/ImGuiSystem.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/EditorWorkspace.h"
#include "editor/GlobalEditorFunctions.h"
#include "editor/Editor.h"
#include "editor/graphics/imgui/EditorInputScope.h"

// editor/file
#include "editor/file/FileResource.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// HierarchyWindow
	//---------------------------------------------------------------------
	HierarchyWindow::HierarchyWindow(ImGuiSystem& a_System) : BaseWindow(a_System, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_LIST) + " Hierarchy", "Hierarchy")
	{
		m_pCameraView = std::make_shared<CameraEditorSelectable>(a_System);
		m_pDirectionalLightView = std::make_shared<DirectionalLightEditorSelectable>(a_System);
	}

	//---------------------------------------------------------------------
	HierarchyWindow::~HierarchyWindow()
	{}

	//---------------------------------------------------------------------
	bool HierarchyWindow::Initialize()
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return false;
		}

		GetEditorEngine().GetEditor()->OnSelectableChanged() += std::bind(&HierarchyWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

		ecs->OnEntitiesUpdated() += std::bind(&HierarchyWindow::UpdateEntities, this);
		ecs->OnEntityComponentsUpdated() += std::bind(&HierarchyWindow::UpdateEntityComponents, this);

		PopulateToolbar();

		RegisterKeybinds();

		return BaseWindow::Initialize();
	}

	//---------------------------------------------------------------------
	bool HierarchyWindow::Destroy()
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return false;
		}

		GetEditorEngine().GetEditor()->OnSelectableChanged() -= std::bind(&HierarchyWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

		ecs->OnEntitiesUpdated() -= std::bind(&HierarchyWindow::UpdateEntities, this);
		ecs->OnEntityComponentsUpdated() -= std::bind(&HierarchyWindow::UpdateEntityComponents, this);

		return BaseWindow::Destroy();
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::PopulateToolbar()
	{
		ImVec2 toolbarSize = ImVec2(0, m_System.GetHeaderSize().y);
		m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_HIERARCHY"), toolbarSize);

		// Search bar.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarSearchbar(m_System,
			ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SEARCHBAR_HIERARCHY"),
			200,
			[this](const std::string& a_sResult)
			{
				m_sSearchBarText = a_sResult;
				m_bNeedsRefresh = true;
			}
		));

		// Spawn entity button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
			[this]()
			{
				m_bSpawnEntity = false;
				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_MODEL), BUTTON_ID, "SPAWN_ENTITY_HIERARCHY").c_str(), "Spawns an entity in the currently opened scene.", m_System.GetHeaderSize()))
				{
					m_bSpawnEntity = true;
				}
			}
		));

		// Save button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
			[this]()
			{
				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_HIERARCHY").c_str(), "Saves the currently opened scene data to its scene file.", m_System.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					editor::g_SaveEditorScene();
				}
			},
			[]()
			{
				return !(editor::g_IsEditorSceneDirty() && !gameplay::GetGame().IsStarted());
			}
		));
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::RegisterKeybinds()
	{
		SetKeybindInputScope(Keybind::Keybind_S,
			static_cast<uint32_t>(EditorInputScope::EditorInputScope_Ctrl));

		SetKeybindInputScope(Keybind::Keybind_Ctrl,
			static_cast<uint32_t>(EditorInputScope::EditorInputScope_Operations));

		SetKeybindInputScope(Keybind::Keybind_Shift,
			static_cast<uint32_t>(EditorInputScope::EditorInputScope_Operations));
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::DrawToolbar()
	{
		// Start toolbar.
		m_Toolbar.StartToolbar();

		// Render toolbar.
		m_Toolbar.Render();

		// End toolbar.
		m_Toolbar.EndToolbar();
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::Update()
	{
		if (GetEditorEngine().GetEditor()->GetEditorSettings().GetEditorState() != editor::EditorState::EditorState_Workspace)
		{
			return;
		}
		BaseWindow::Update();
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::Render()
	{
		ActivateInputScope(EditorInputScope::EditorInputScope_Operations);

		bool canActivateCtrl = CanActivate(Keybind::Keybind_Ctrl, EditorInputScope::EditorInputScope_Operations, false);
		ActivateInputScope(EditorInputScope::EditorInputScope_Ctrl, canActivateCtrl);

		bool canActivateShift = CanActivate(Keybind::Keybind_Shift, EditorInputScope::EditorInputScope_Operations, false);
		ActivateInputScope(EditorInputScope::EditorInputScope_Shift, canActivateShift);

		if (CanActivate(Keybind::Keybind_S, EditorInputScope::EditorInputScope_Ctrl, false))
		{
			if (editor::g_IsEditorSceneDirty() && !gameplay::GetGame().IsStarted())
			{
				editor::g_SaveEditorScene();
			}
		}

		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		std::lock_guard<std::recursive_mutex> lock(ecs->m_EntityMutex);

		// This needs to be done at the start of the frame to avoid errors.
		// We refresh the assets that show up based on the search bar and the root directory.
		if (m_bNeedsRefresh)
		{
			m_aEntities.clear();
			m_aFilteredEntities.clear();

			for (gameplay::EntityID entity : ecs->GetEntities())
			{
				m_aEntities.push_back(std::make_shared<EntityEditorSelectable>(m_System, entity));
			}

			std::string searchString = string_extensions::StringToLower(m_sSearchBarText);
			for (std::shared_ptr<EntityEditorSelectable>& view : m_aEntities)
			{
				std::shared_ptr<gameplay::Entity> ent = ecs->GetEntity(view->GetEntityID()).lock();
				if (!ent)
				{
					return;
				}

				if (searchString.empty() || string_extensions::StringToLower(ent->GetName()).find(searchString) != std::string::npos)
				{
					m_aFilteredEntities.push_back(view);
				}
			}

			if (m_PreviousEntityID.IsValid())
			{
				gameplay::EntityID prevEntityID = m_PreviousEntityID;
				auto it = std::find_if(
					m_aEntities.begin(),
					m_aEntities.end(),
					[prevEntityID](const std::shared_ptr<EntityEditorSelectable>& view)
					{
						return view->GetEntityID() == prevEntityID;
					}
				);
				if (it != m_aEntities.end())
				{
					GetEditorEngine().GetEditor()->SetSelectable((*it));
				}
			}

			m_bNeedsRefresh = false;
		}

		resources::ResourceAtlas* resourceAtlas = GetEditorEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

		DrawToolbar();

		ImGui::SetCursorPos({
			ImGui::GetCursorPos().x + m_System.GetFramePadding().x,
			ImGui::GetCursorPos().y + m_System.GetFramePadding().y,
		});

		if (ImGui::BeginChild(
			ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "ENTITIES_HIERARCHY").c_str(),
			ImVec2(
				ImGui::GetContentRegionAvail().x - m_System.GetFramePadding().x,
				ImGui::GetContentRegionAvail().y - m_System.GetFramePadding().y
			),
			ImGuiChildFlags_Borders
			))
		{
			ImVec2 childSize = ImGui::GetContentRegionAvail();
			ImGui::InvisibleButton("HIERARCHY_DROP_TARGET", childSize,
				ImGuiButtonFlags_AllowOverlap | ImGuiButtonFlags_MouseButtonLeft);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXPLORER_ITEM"))
				{
					IM_ASSERT(payload->DataSize == sizeof(file::FileResource*));
					file::FileResource* dropped = *(file::FileResource**)payload->Data;
					if (dropped)
					{
						if (dropped->GetAssetType() == resources::AssetType::Prefab)
						{
							//if (auto prefab = resourceAtlas->LoadPrefab(dropped->GetPath().filename().generic_string()).lock())
							//{
							//	editor::g_InstantiatePrefab(*prefab);
							//}
						}
						else
						{
							//gameplay::EntityID entityID = editor::g_CreateEntity(dropped->GetPath().filename().generic_string());
							//DragAction(entityID, dropped->GetAssetType(), dropped->GetPath().filename().generic_string());
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SetCursorPos(ImVec2(0, 0));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			{
				bool
					clicked = false,
					doubleClicked = false;

				m_pCameraView->RenderEntity(clicked, doubleClicked, GetEditorEngine().GetEditor()->IsSelectable(m_pCameraView));

				if (clicked)
				{
					GetEditorEngine().GetEditor()->SetSelectable(m_pCameraView);
				}
			}

			{
				bool
					clicked = false,
					doubleClicked = false;

				m_pDirectionalLightView->RenderEntity(clicked, doubleClicked, GetEditorEngine().GetEditor()->IsSelectable(m_pDirectionalLightView));

				if (clicked)
				{
					GetEditorEngine().GetEditor()->SetSelectable(m_pDirectionalLightView);
				}
			}

			ImGui::Separator();

			for (std::weak_ptr<EntityEditorSelectable>& view : m_aFilteredEntities)
			{
				std::shared_ptr<EntityEditorSelectable> selectable = view.lock();
				if (!selectable)
				{
					continue;
				}

				bool
					clicked = false,
					doubleClicked = false;

				selectable->RenderEntity(
					clicked,
					doubleClicked,
					GetEditorEngine().GetEditor()->IsSelectable(selectable)
				);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXPLORER_ITEM"))
					{
						IM_ASSERT(payload->DataSize == sizeof(file::FileResource*));
						file::FileResource* dropped = *(file::FileResource**)payload->Data;
						if (dropped)
						{
							DragAction(selectable->GetEntityID(), dropped->GetAssetType(), dropped->GetPath().filename().generic_string());
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (clicked)
				{
					GetEditorEngine().GetEditor()->SetSelectable(view);
				}
				if (doubleClicked)
				{
					selectable->OnDoubleClicked();
				}
			}
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();

		if (m_bSpawnEntity)
		{
			editor::g_CreateEntity();
		}
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::DragAction(const gameplay::EntityID& a_EntityID, resources::AssetType a_AssetType, const std::string& a_sFileName)
	{
		//gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		//if (!ecs)
		//{
		//	return;
		//}

		//graphics::dx12::DX12System* dx12System = GetEditorEngine().GetDX12();
		//if (!dx12System)
		//{
		//	return;
		//}

		//resources::ResourceAtlas* resourceAtlas = GetEditorEngine().GetResourceAtlas();
		//if (!resourceAtlas)
		//{
		//	return;
		//}

		//switch (a_AssetType)
		//{
		//	case resources::AssetType::None:
		//	{
		//		break;
		//	}
		//	case resources::AssetType::Folder:
		//	{
		//		break;
		//	}
		//	case resources::AssetType::Scene:
		//	{
		//		break;
		//	}
		//	case resources::AssetType::Texture:
		//	{
		//		std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = dx12System->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		//		
		//		gameplay::MeshSystem* meshSystem = ecs->GetSystem<gameplay::MeshSystem>();
		//		if (meshSystem->HasComponent(a_EntityID))
		//		{
		//			meshSystem->TryGetComponent(a_EntityID)->SetTexture(
		//				resourceAtlas->LoadTexture(a_sFileName, cCommandQueue)
		//			);
		//		}
		//		else
		//		{
		//			gameplay::SpriteSystem* spriteSystem = ecs->GetSystem<gameplay::SpriteSystem>();
		//			
		//			gameplay::SpriteComponent* component = editor::g_CreateComponentOfType<gameplay::SpriteComponent>(spriteSystem, a_EntityID);
		//			if (!component)
		//			{
		//				return;
		//			}

		//			component->SetTexture(
		//				resourceAtlas->LoadTexture(a_sFileName, cCommandQueue)
		//			);
		//		}
		//		break;
		//	}
		//	case resources::AssetType::Sound:
		//	{
		//		break;
		//	}
		//	case resources::AssetType::Song:
		//	{
		//		break;
		//	}
		//	case resources::AssetType::VO:
		//	{
		//		break;
		//	}
		//	case resources::AssetType::Animation:
		//	{
		//		break;
		//	}
		//	case resources::AssetType::PixelShader:
		//	{
		//		break;
		//	}
		//	case resources::AssetType::VertexShader:
		//	{
		//		break;
		//	}
		//	//case resources::AssetType::Prefab:
		//	//{
		//	//	break;
		//	//}
		//	case resources::AssetType::Mesh:
		//	{
		//		std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = dx12System->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		//		
		//		gameplay::MeshSystem* meshSystem = ecs->GetSystem<gameplay::MeshSystem>();

		//		gameplay::MeshComponent* component = editor::g_CreateComponentOfType<gameplay::MeshComponent>(meshSystem, a_EntityID);
		//		if (!component)
		//		{
		//			return;
		//		}
		//		
		//		component->SetMesh(
		//			resourceAtlas->LoadMesh(a_sFileName, cCommandQueue)
		//		);
		//		break;
		//	}
		//	case resources::AssetType::Material:
		//	{
		//		std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = dx12System->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

		//		gameplay::MeshSystem* meshSystem = ecs->GetSystem<gameplay::MeshSystem>();

		//		gameplay::MeshComponent* component = editor::g_CreateComponentOfType<gameplay::MeshComponent>(meshSystem, a_EntityID);
		//		if (!component)
		//		{
		//			return;
		//		}

		//		component->SetMaterial(
		//			resourceAtlas->LoadMaterial(a_sFileName)
		//		);
		//		break;
		//	}
		//}
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::UpdateEntities()
	{
		m_bNeedsRefresh = true;
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::UpdateEntityComponents()
	{
		m_bNeedsRefresh = true;
	}

	//---------------------------------------------------------------------
	void HierarchyWindow::OnSelectableChanged(std::weak_ptr<EditorSelectable> a_pOldValue, std::weak_ptr<EditorSelectable> a_pNewValue)
	{
		std::shared_ptr<EditorSelectable> selectable = a_pNewValue.lock();
		
		if (!selectable)
		{
			return;
		}

		const EntityEditorSelectable* derivedPtr = dynamic_cast<const EntityEditorSelectable*>(selectable.get());
		if (!derivedPtr) // New selectable is NOT an entity, so we must reset the previous folder path.
		{
			m_PreviousEntityID = gameplay::EntityID();
		}
		else // New selectable is an entity, set folder path.
		{
			m_PreviousEntityID = derivedPtr->GetEntityID();
		}
	}
}