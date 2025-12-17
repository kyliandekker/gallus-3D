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
			HierarchyWindow::HierarchyWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_LIST) + " Hierarchy", "Hierarchy"), m_SearchBar(a_Window), m_CameraView(a_Window)
			{
				m_SearchBar.Initialize("");
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
				
				core::EDITOR_ENGINE->GetEditor().GetScene().IsDirty().OnChanged() += std::bind(&HierarchyWindow::OnSceneDirty, this, std::placeholders::_1, std::placeholders::_2);

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
				if (!core::EDITOR_ENGINE)
				{
					return;
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
						m_aEntities.emplace_back(m_Window, entity.GetEntityID());
					}

					for (EntityEditorSelectable& view : m_aEntities)
					{
						gameplay::Entity* entity = core::EDITOR_ENGINE->GetECS().GetEntity(view.GetEntityID());
						if (!entity)
						{
							continue;
						}
						if (m_SearchBar.GetString().empty() || string_extensions::StringToLower(entity->GetName()).find(m_SearchBar.GetString()) != std::string::npos)
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

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				float topPosY = ImGui::GetCursorPosY();

				bool prefabMode = core::EDITOR_ENGINE->GetEditor().GetEditorMethod() == editor::EditorMethod::EDITOR_METHOD_PREFAB;
				if (prefabMode)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				bool spawnEntity = false;
				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_MODEL), BUTTON_ID, "SPAWN_ENTITY_HIERARCHY").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont()))
				{
					spawnEntity = true;
				}
				ImGui::SameLine();

				bool isDelete = ImGui::IsKeyPressed(ImGuiKey_Delete);
				bool isDuplicate = (ImGui::IsKeyPressed(ImGuiKey_D) && ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl)) || (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D));
				if (isDelete || isDuplicate)
				{
					if (graphics::imgui::EntityEditorSelectable* ent = dynamic_cast<graphics::imgui::EntityEditorSelectable*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get()))
					{
						if (isDelete)
						{
							ent->OnDelete();
						}
						else if (isDuplicate)
						{
							// TODO: Duplicate functionality.
						}
					}
				}

				if (prefabMode)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				bool wasDirty = core::EDITOR_ENGINE->GetEditor().GetScene().IsDirty() && !gameplay::GAME.IsStarted();
				if (!wasDirty)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_HIERARCHY").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					editor::SaveScene();
				}

				ImVec2 endPos = ImGui::GetCursorPos();

				if (!wasDirty)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_S) && wasDirty)
				{
					editor::SaveScene();
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				float searchbarWidth = 200;
				float inputPadding = m_Window.GetFramePadding().x / 2;
				ImVec2 searchBarPos = ImVec2(
					ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (searchbarWidth + m_Window.GetWindowPadding().x),
					(topPosY + (toolbarSize.y / 2)) - (((inputPadding * 2) + m_Window.GetFontSize()) / 2)
				);
				ImGui::SetCursorPos(searchBarPos);
				if (m_SearchBar.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "HIERARCHY_CONSOLE").c_str(), ImVec2(searchbarWidth, toolbarSize.y), inputPadding))
				{
					m_bNeedsRefresh = true;
				}

				ImGui::SetCursorPos(endPos);

				ImGui::EndToolbar(ImVec2(0, 0));

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
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
					ImGui::InvisibleButton("HIERARCHY_DROP_TARGET", childSize);
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXPLORER_ITEM"))
						{
							IM_ASSERT(payload->DataSize == sizeof(resources::FileResource*));
							resources::FileResource* dropped = *(resources::FileResource**)payload->Data;
							if (dropped)
							{
								if (dropped->GetMetaData()->GetAssetType() == resources::AssetType::Prefab)
								{
									if (auto prefab = core::EDITOR_ENGINE->GetResourceAtlas().LoadPrefab(dropped->GetPath().filename().generic_string()).lock())
									{
										prefab->Instantiate();
									}
								}
								else
								{
									gameplay::EntityID entityID = core::EDITOR_ENGINE->GetECS().CreateEntity(dropped->GetPath().filename().generic_string());
									DragAction(entityID, dropped->GetMetaData()->GetAssetType(), dropped->GetPath().filename().generic_string());
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
									DragAction(view->GetEntityID(), dropped->GetMetaData()->GetAssetType(), dropped->GetPath().filename().generic_string());
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

				if (spawnEntity)
				{
					const EntityEditorSelectable* derivedPtr = dynamic_cast<const EntityEditorSelectable*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get());
					if (derivedPtr)
					{
						core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr);
					}

					core::EDITOR_ENGINE->GetECS().CreateEntity(core::EDITOR_ENGINE->GetECS().GetUniqueName("New GameObject"));
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
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
						if (core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::MeshSystem>().HasComponent(a_EntityID))
						{
							core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::MeshSystem>().CreateComponent(a_EntityID).SetMesh(
								core::EDITOR_ENGINE->GetResourceAtlas().LoadMesh(a_sFileName, cCommandQueue)
							);
						}
						else
						{
							core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::SpriteSystem>().CreateComponent(a_EntityID).SetTexture(
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
						core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::MeshSystem>().CreateComponent(a_EntityID).SetMesh(
							core::EDITOR_ENGINE->GetResourceAtlas().LoadMesh(a_sFileName, cCommandQueue)
						);
						break;
					}
					case resources::AssetType::Material:
					{
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
				std::string title = " - (" + name + ")";

				core::EDITOR_ENGINE->GetWindow().AddTitle(title);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE