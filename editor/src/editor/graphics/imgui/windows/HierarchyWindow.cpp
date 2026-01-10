#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "HierarchyWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// utils
#include "utils/string_extensions.h"
#include "utils/file_abstractions.h"

// graphics
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay
#include "gameplay/Game.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/MeshSystem.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/EditorGlobalFunctions.h"
#include "editor/EditorInputScope.h"

#include "resources/FileResource.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// HierarchyWindow
			//---------------------------------------------------------------------
			HierarchyWindow::HierarchyWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_LIST) + " Hierarchy", "Hierarchy"), 
				m_CameraView(a_Window),
				m_DirectionalLightView(a_Window)
			{
			}

			//---------------------------------------------------------------------
			HierarchyWindow::~HierarchyWindow()
			{}

			//---------------------------------------------------------------------
			bool HierarchyWindow::Initialize()
			{
				core::EDITOR_ENGINE->GetECS().OnEntitiesUpdated() += std::bind(&HierarchyWindow::UpdateEntities, this);

				core::EDITOR_ENGINE->GetECS().OnEntityComponentsUpdated() += std::bind(&HierarchyWindow::UpdateEntityComponents, this);

				core::EDITOR_ENGINE->GetEditor().GetSelectable().OnChanged() += std::bind(&HierarchyWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);
				
				core::EDITOR_ENGINE->GetEditor().IsAnyDirty().OnChanged() += std::bind(&HierarchyWindow::OnSceneDirty, this, std::placeholders::_1, std::placeholders::_2);

				PopulateToolbar();

				RegisterKeybinds();

				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			bool HierarchyWindow::Destroy()
			{
				core::EDITOR_ENGINE->GetECS().OnEntitiesUpdated() -= std::bind(&HierarchyWindow::UpdateEntities, this);

				core::EDITOR_ENGINE->GetECS().OnEntityComponentsUpdated() -= std::bind(&HierarchyWindow::UpdateEntityComponents, this);

				core::EDITOR_ENGINE->GetEditor().GetSelectable().OnChanged() -= std::bind(&HierarchyWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

				return BaseWindow::Destroy();
			}

			//---------------------------------------------------------------------
			void HierarchyWindow::PopulateToolbar()
			{
				ImVec2 toolbarSize = ImVec2(0, m_Window.GetHeaderSize().y);
				m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_HIERARCHY"), toolbarSize);

				// Search bar.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarSearchbar(m_Window,
					ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SEARCHBAR_HIERARCHY"),
					200,
					[this](const std::string& a_sResult)
					{
						m_sSearchBarText = a_sResult;
						m_bNeedsRefresh = true;
					}
				));

				// Spawn entity button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						m_bSpawnEntity = false;
						if (ImGui::IconButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_MODEL), BUTTON_ID, "SPAWN_ENTITY_HIERARCHY").c_str(), "Spawns an entity in the currently opened scene.", m_Window.GetHeaderSize()))
						{
							m_bSpawnEntity = true;
						}
					}
				));

				// Save button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						if (ImGui::IconButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_HIERARCHY").c_str(), "Saves the currently opened scene data to its scene file.", m_Window.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
						{
							editor::g_SaveScene();
						}
					},
					[]()
					{
						return !(core::EDITOR_ENGINE->GetEditor().GetScene().IsDirty() && !gameplay::GAME.IsStarted());
					}
				));
			}

			//---------------------------------------------------------------------
			void HierarchyWindow::RegisterKeybinds()
			{
				editor::SetKeybindInputScope(editor::Keybind::Keybind_S,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_Ctrl));

				editor::SetKeybindInputScope(editor::Keybind::Keybind_Ctrl,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_Operations));

				editor::SetKeybindInputScope(editor::Keybind::Keybind_Shift,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_Operations));
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
				if (core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFullScreenPlayMode())
				{
					return;
				}
				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void HierarchyWindow::Render()
			{
				editor::ActivateInputScope(editor::EditorInputScope::EditorInputScope_Operations);

				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				if (editor::CanActivate(editor::Keybind::Keybind_Ctrl, editor::EditorInputScope::EditorInputScope_Operations, false))
				{
					editor::ActivateInputScope(editor::EditorInputScope::EditorInputScope_Ctrl);
				}
				else
				{
					editor::DeactivateInputScope(editor::EditorInputScope::EditorInputScope_Ctrl);
				}

				if (editor::CanActivate(editor::Keybind::Keybind_Shift, editor::EditorInputScope::EditorInputScope_Operations, false))
				{
					editor::ActivateInputScope(editor::EditorInputScope::EditorInputScope_Shift);
				}
				else
				{
					editor::DeactivateInputScope(editor::EditorInputScope::EditorInputScope_Shift);
				}

				if (editor::CanActivate(editor::Keybind::Keybind_S, editor::EditorInputScope::EditorInputScope_Ctrl, false))
				{
					if ((core::EDITOR_ENGINE->GetEditor().GetScene().IsDirty() && !gameplay::GAME.IsStarted()))
					{
						editor::g_SaveScene();
					}
				}

				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				// This needs to be done at the start of the frame to avoid errors.
				// We refresh the assets that show up based on the search bar and the root directory.
				if (m_bNeedsRefresh)
				{
					m_aEntities.clear();
					m_aFilteredEntities.clear();

					for (auto& entity : core::EDITOR_ENGINE->GetECS().GetEntities())
					{
						auto ent = entity.lock();
						if (!ent)
						{
							return;
						}

						m_aEntities.emplace_back(m_Window, ent->GetEntityID());
					}

					std::string searchString = string_extensions::StringToLower(m_sSearchBarText);
					for (EntityEditorSelectable& view : m_aEntities)
					{
						auto ent = core::EDITOR_ENGINE->GetECS().GetEntity(view.GetEntityID()).lock();
						if (!ent)
						{
							return;
						}

						if (searchString.empty() || string_extensions::StringToLower(ent->GetName()).find(searchString) != std::string::npos)
						{
							m_aFilteredEntities.push_back(&view);
						}
					}

					if (m_PreviousEntityID.IsValid())
					{
						auto prevEntityID = m_PreviousEntityID;
						auto it = std::find_if(
							m_aEntities.begin(),
							m_aEntities.end(),
							[prevEntityID](const EntityEditorSelectable& view)
							{
								return view.GetEntityID() == prevEntityID;
							}
						);
						if (it != m_aEntities.end())
						{
							SetSelectable(&(*it));
						}
						else
						{
							SetSelectable(nullptr);
						}
					}
					else
					{
						SetSelectable(nullptr);
					}

					m_bNeedsRefresh = false;
				}

				DrawToolbar();

				ImGui::SetCursorPos({
					ImGui::GetCursorPos().x + m_Window.GetFramePadding().x,
					ImGui::GetCursorPos().y + m_Window.GetFramePadding().y,
				});

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "ENTITIES_HIERARCHY").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
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
							IM_ASSERT(payload->DataSize == sizeof(resources::FileResource*));
							resources::FileResource* dropped = *(resources::FileResource**)payload->Data;
							if (dropped)
							{
								if (dropped->GetAssetType() == resources::AssetType::Prefab)
								{
									if (auto prefab = core::EDITOR_ENGINE->GetResourceAtlas().LoadPrefab(dropped->GetPath().filename().generic_string()).lock())
									{
										editor::g_InstantiatePrefab(*prefab);
									}
								}
								else
								{
									gameplay::EntityID entityID = editor::g_CreateEntity(dropped->GetPath().filename().generic_string());
									DragAction(entityID, dropped->GetAssetType(), dropped->GetPath().filename().generic_string());
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

						m_CameraView.RenderEntity(clicked, doubleClicked, core::EDITOR_ENGINE->GetEditor().GetSelectable().get() == &m_CameraView);

						if (clicked)
						{
							core::EDITOR_ENGINE->GetEditor().SetSelectable(&m_CameraView);
						}
					}

					{
						bool
							clicked = false,
							doubleClicked = false;

						m_DirectionalLightView.RenderEntity(clicked, doubleClicked, core::EDITOR_ENGINE->GetEditor().GetSelectable().get() == &m_DirectionalLightView);

						if (clicked)
						{
							core::EDITOR_ENGINE->GetEditor().SetSelectable(&m_DirectionalLightView);
						}
					}

					ImGui::Separator();

					for (EntityEditorSelectable* view : m_aFilteredEntities)
					{
						if (!view)
						{
							continue;
						}

						bool
							clicked = false,
							doubleClicked = false;

						view->RenderEntity(
							clicked,
							doubleClicked,
							core::EDITOR_ENGINE->GetEditor().GetSelectable().get() == view
						);
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXPLORER_ITEM"))
							{
								IM_ASSERT(payload->DataSize == sizeof(resources::FileResource*));
								resources::FileResource* dropped = *(resources::FileResource**)payload->Data;
								if (dropped)
								{
									DragAction(view->GetEntityID(), dropped->GetAssetType(), dropped->GetPath().filename().generic_string());
								}
							}
							ImGui::EndDragDropTarget();
						}

						if (clicked)
						{
							SetSelectable(view);
						}
						if (doubleClicked)
						{
							gameplay::TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<gameplay::TransformSystem>();
							if (transformSys.HasComponent(view->GetEntityID()))
							{
								gameplay::TransformComponent& transformComponent = transformSys.GetComponent(view->GetEntityID());
								DirectX::XMFLOAT3 pos = { 
									transformComponent.GetTransform().GetPosition().x - (graphics::dx12::RENDER_TEX_SIZE.x / 2), 
									transformComponent.GetTransform().GetPosition().y - (graphics::dx12::RENDER_TEX_SIZE.y / 2),
									transformComponent.GetTransform().GetPosition().z
								};
								core::EDITOR_ENGINE->GetEditor().GetEditorCamera().GetTransform().SetPosition(pos);
							}
						}
					}
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();

				if (m_bSpawnEntity)
				{
					const EntityEditorSelectable* derivedPtr = dynamic_cast<const EntityEditorSelectable*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get());
					if (derivedPtr)
					{
						core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr);
					}
					
					editor::g_CreateEntity();
				}
			}

			//---------------------------------------------------------------------
			void HierarchyWindow::DragAction(const gameplay::EntityID& a_EntityID, resources::AssetType a_AssetType, const std::string& a_sFileName)
			{
				switch (a_AssetType)
				{
					case resources::AssetType::None:
					{
						break;
					}
					case resources::AssetType::Folder:
					{
						break;
					}
					case resources::AssetType::Scene:
					{
						break;
					}
					case resources::AssetType::Sprite:
					{
						std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
						
						gameplay::MeshSystem& meshSystem = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::MeshSystem>();
						if (meshSystem.HasComponent(a_EntityID))
						{
							gameplay::MeshComponent* component = editor::g_CreateComponentOfType<gameplay::MeshComponent>(&meshSystem, a_EntityID);
							if (!component)
							{
								return;
							}
							
							component->SetTexture(
								core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(a_sFileName, cCommandQueue)
							);
						}
						else
						{
							gameplay::SpriteSystem& spriteSystem = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::SpriteSystem>();
							
							gameplay::SpriteComponent* component = editor::g_CreateComponentOfType<gameplay::SpriteComponent>(&spriteSystem, a_EntityID);
							if (!component)
							{
								return;
							}

							component->SetTexture(
								core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(a_sFileName, cCommandQueue)
							);
						}
						break;
					}
					case resources::AssetType::Sound:
					{
						break;
					}
					case resources::AssetType::Song:
					{
						break;
					}
					case resources::AssetType::VO:
					{
						break;
					}
					case resources::AssetType::Animation:
					{
						break;
					}
					case resources::AssetType::PixelShader:
					{
						break;
					}
					case resources::AssetType::VertexShader:
					{
						break;
					}
					//case resources::AssetType::Prefab:
					//{
					//	break;
					//}
					case resources::AssetType::ShaderBind:
					{
						break;
					}
					case resources::AssetType::Mesh:
					{
						std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
						
						gameplay::MeshSystem& meshSystem = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::MeshSystem>();

						gameplay::MeshComponent* component = editor::g_CreateComponentOfType<gameplay::MeshComponent>(&meshSystem, a_EntityID);
						if (!component)
						{
							return;
						}
						
						component->SetMesh(
							core::EDITOR_ENGINE->GetResourceAtlas().LoadMesh(a_sFileName, cCommandQueue)
						);
						break;
					}
					case resources::AssetType::Material:
					{
						std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

						gameplay::MeshSystem& meshSystem = core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::MeshSystem>();

						gameplay::MeshComponent* component = editor::g_CreateComponentOfType<gameplay::MeshComponent>(&meshSystem, a_EntityID);
						if (!component)
						{
							return;
						}

						component->SetMaterial(
							core::EDITOR_ENGINE->GetResourceAtlas().LoadMaterial(a_sFileName)
						);
						break;
					}
				}
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
			void HierarchyWindow::SetSelectable(EntityEditorSelectable* a_EntityView)
			{
				core::EDITOR_ENGINE->GetEditor().SetSelectable(a_EntityView);
			}

			//---------------------------------------------------------------------
			void HierarchyWindow::OnSelectableChanged(const EditorSelectable* oldVal, const EditorSelectable* newVal)
			{
				if (!newVal)
				{
					return;
				}

				const EntityEditorSelectable* derivedPtr = dynamic_cast<const EntityEditorSelectable*>(newVal);
				if (!derivedPtr) // New selectable is NOT an entity, so we must reset the previous folder path.
				{
					m_PreviousEntityID.SetInvalid();
				}
				else // New selectable is an entity, set folder path.
				{
					m_PreviousEntityID = derivedPtr->GetEntityID();
				}
			}

			//---------------------------------------------------------------------
			void HierarchyWindow::OnSceneDirty(const bool oldVal, const bool newVal)
			{
				std::string name = (newVal ? "*" : "") + core::EDITOR_ENGINE->GetEditor().GetScene().GetPath().filename().generic_string();
				std::string title = "";
				if (!name.empty())
				{
					title = " - (" + name + ")";
				}

				core::EDITOR_ENGINE->GetWindow().AddTitle(title);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE