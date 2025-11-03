#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "EntityInspectorView.h"

#include <typeindex>
#include <unordered_map>
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>
#include <imgui/imgui_internal.h>

// core includes
#include "editor/core/EditorEngine.h"
#include "editor/EditorExpose.h"
#include "editor/graphics/imgui/modals/FilePickerModal.h"
#include <editor/graphics/imgui/EditorWindowsConfig.h>

// graphics includes
#include "graphics/imgui/font_icon.h"

// editor includes
#include "editor/graphics/imgui/views/HierarchyEntityUIView.h"

// game includes
#include "gameplay/Game.h"
#include "gameplay/ECSBaseSystem.h"

#include "utils/string_extensions.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			template<typename T>
			void ShowEditorFieldFromObject(T* obj, const EditorFieldInfo& field)
			{
				void* ptr = reinterpret_cast<char*>(obj) + field.m_iOffset;

				std::function<void()> func = [] {};

				bool showTable = true;
				std::string fieldId = ImGui::IMGUI_FORMAT_ID("", INPUT_ID, string_extensions::StringToUpper(field.m_sUIName) + "_INSPECTOR");
				switch (field.m_Options.type)
				{
				case EditorWidgetType::DragFloat:
				{
					float* value = reinterpret_cast<float*>(ptr);
					func = [value, &field, &fieldId]
						{
							float max = 0.0f;
							float min = 0.0f;
							if (!field.m_Options.max.empty())
							{
								max = std::stof(field.m_Options.max);
							}
							if (!field.m_Options.min.empty())
							{
								min = std::stof(field.m_Options.min);
							}
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
							ImGui::DragFloat(fieldId.c_str(), value, 1, min, max);
						};
					break;
				}
				case EditorWidgetType::DragInt8:
				case EditorWidgetType::DragInt16:
				case EditorWidgetType::DragInt32:
				case EditorWidgetType::DragInt64:
				{
					int8_t* value = reinterpret_cast<int8_t*>(ptr);
					func = [value, &field, &fieldId]
						{
							int max = 0;
							int min = 0;

							if (!field.m_Options.max.empty())
							{
								max = std::stoi(field.m_Options.max);
							}
							if (!field.m_Options.min.empty())
							{
								min = std::stoi(field.m_Options.min);
							}

							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

							int temp = static_cast<int>(*value); // promote to int
							if (ImGui::DragInt(fieldId.c_str(), &temp, 1.0f, min, max))
							{
								if (field.m_Options.type == EditorWidgetType::DragInt8)
								{
									*value = static_cast<int8_t>(temp);
								}
								else if (field.m_Options.type == EditorWidgetType::DragInt16)
								{
									*value = static_cast<int16_t>(temp);
								}
								else if (field.m_Options.type == EditorWidgetType::DragInt32)
								{
									*value = static_cast<int32_t>(temp);
								}
								else if (field.m_Options.type == EditorWidgetType::DragInt64)
								{
									*value = static_cast<int64_t>(temp);
								}
							}
						};
					break;
				}
				case EditorWidgetType::Checkbox:
				{
					bool* value = reinterpret_cast<bool*>(ptr);
					func = [&field, &fieldId, value]
						{
							ImGui::Checkbox(fieldId.c_str(), value);
						};
					break;
				}
				case EditorWidgetType::Toggle:
				{
					bool* value = reinterpret_cast<bool*>(ptr);
					func = [&field, &fieldId, value]
						{
							ImGui::Toggle(fieldId.c_str(), value);
						};
					break;
				}
				case EditorWidgetType::Vector2Field:
				{
					DirectX::XMFLOAT2* value = reinterpret_cast<DirectX::XMFLOAT2*>(ptr);
					func = [&field, &fieldId, value]
						{
							float val[2] = {
								value->x,
								value->y,
							};
							float max = 0.0f;
							float min = 0.0f;
							if (!field.m_Options.max.empty())
							{
								max = std::stof(field.m_Options.max);
							}
							if (!field.m_Options.min.empty())
							{
								min = std::stof(field.m_Options.min);
							}
							float availW = ImGui::GetContentRegionAvail().x;
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
							if (ImGui::VectorEdit2(fieldId.c_str(), val, 0.1f, min, max))
							{
								value->x = val[0];
								value->y = val[1];
							}
						};
					break;
				}
				case EditorWidgetType::AssetPicker:
				{
					resources::EngineResource** pValuePtr = reinterpret_cast<resources::EngineResource**>(ptr);
					resources::EngineResource* value = (pValuePtr ? *pValuePtr : nullptr);
					func = [&field, &fieldId, value]
						{
							std::string name = value ? value->GetName() : "<null>";
							char buf[256];
							strncpy_s(buf, sizeof(buf), name.c_str(), sizeof(buf) - 1);
							buf[sizeof(buf) - 1] = '\0';

							ImVec2 buttonSize = ImVec2(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() * 2, core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() * 2);

							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() / 2, core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFontSize() / 2));
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
							ImGui::InputText(ImGui::IMGUI_FORMAT_ID(fieldId, INPUT_ID, "").c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
							ImGui::PopItemFlag();
							ImGui::SameLine();
							if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE + fieldId, BUTTON_ID, "").c_str(), buttonSize))
							{
								FilePickerModal& filePickerModal = core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetWindowsConfig<EditorWindowsConfig>().GetFilePickerModal();
								filePickerModal.SetData(
									[value](int success, gallus::resources::FileResource& resource)
									{
										if (success == 1)
										{
											core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
										}
									},
									std::vector<gallus::resources::AssetType>{ value->GetResourceType() });
								filePickerModal.Show();
							}
							ImGui::PopStyleVar();
						};
					break;
				}
				case EditorWidgetType::Object:
				{
					showTable = false;
					IExposableToEditor* editorObject = reinterpret_cast<IExposableToEditor*>(ptr);

					ImGui::Spacing();
					ImGui::SeparatorText(field.m_sUIName);
					std::string nestedId = ImGui::IMGUI_FORMAT_ID("", TABLE_ID, string_extensions::StringToUpper(field.m_sUIName) + "_NESTED_INSPECTOR");

					for (const EditorFieldInfo& subField : editorObject->GetEditorFields())
					{
						ShowEditorFieldFromObject(editorObject, subField);
					}

					break;
				}
				default:
				{
					break;
				}
				}

				if (showTable)
				{
					ImGui::KeyValue([&field]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetBoldFont(), field.m_sUIName);
						}, func);
				}
			}

			template<typename T>
			void RenderEditorForObject(T* obj)
			{
				const auto& fields = obj->GetEditorFields();

				std::string id = ImGui::IMGUI_FORMAT_ID("", TABLE_ID, string_extensions::StringToUpper(obj->GetName()) + "_INSPECTOR");
				bool tableActive = ImGui::StartInspectorKeyVal(id, core::EDITOR_ENGINE->GetDX12().GetImGuiWindow().GetFramePadding());

				if (tableActive)
				{
					for (const EditorFieldInfo& field : fields)
					{
						ShowEditorFieldFromObject(obj, field);
					}
					ImGui::EndInspectorKeyVal(ImVec2());
				}
			}
			//---------------------------------------------------------------------
			// EntityInspectorView
			//---------------------------------------------------------------------
			EntityInspectorView::~EntityInspectorView()
			{
			}

			//---------------------------------------------------------------------
			EntityInspectorView::EntityInspectorView(ImGuiWindow& a_Window, HierarchyEntityUIView& a_HierarchyEntityUIView) : InspectorView(a_Window), m_HierarchyEntityUIView(a_HierarchyEntityUIView)
			{
				m_bShowRename = true;
				m_bShowDelete = true;

				m_pEntity = core::EDITOR_ENGINE->GetECS().GetEntity(m_HierarchyEntityUIView.GetEntityID());
				if (!m_pEntity)
				{
					return;
				}

				gameplay::EntityID& entityId = m_pEntity->GetEntityID();

				for (auto* sys : core::EDITOR_ENGINE->GetECS().GetSystems())
				{
					if (sys->HasComponent(entityId))
					{
						auto* comp = sys->GetBaseComponent(entityId);

						std::string id = ImGui::IMGUI_FORMAT_ID("",
							FOLDOUT_ID, string_extensions::StringToUpper(comp->GetName()) + "_INSPECTOR");

						m_aExpanded.insert(std::make_pair(id, false));
					}
				}
			}

			//---------------------------------------------------------------------
			void EntityInspectorView::OnRename(const std::string& a_sName)
			{
				if (m_pEntity)
				{
					m_pEntity->SetName(a_sName);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
			}

			//---------------------------------------------------------------------
			void EntityInspectorView::OnDelete()
			{
				core::EDITOR_ENGINE->GetECS().DeleteEntity(m_HierarchyEntityUIView.GetEntityID());
				core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
			}

			//---------------------------------------------------------------------
			std::string EntityInspectorView::GetName() const
			{
				return m_pEntity ? m_pEntity->GetName() : "";
			}

			//---------------------------------------------------------------------
			std::string EntityInspectorView::GetIcon() const
			{
				return m_HierarchyEntityUIView.GetIcon();
			}

			//---------------------------------------------------------------------
			void EntityInspectorView::Render()
			{
				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				m_pEntity = core::EDITOR_ENGINE->GetECS().GetEntity(m_HierarchyEntityUIView.GetEntityID());
				if (!m_pEntity)
				{
					return;
				}

				gameplay::EntityID& entityId = m_pEntity->GetEntityID();

				ImGui::SetCursorPosY(0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				for (auto* sys : core::EDITOR_ENGINE->GetECS().GetSystems())
				{
					ImGui::SetCursorPosX(0);
					float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
					ImGui::SetNextItemWidth(width);
					if (sys->HasComponent(entityId))
					{
						auto* comp = sys->GetBaseComponent(entityId);

						ImVec2 size = m_Window.GetHeaderSize();

						float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
						width -= size.x;
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

						ImVec2 foldOutButtonPos = ImGui::GetCursorScreenPos();

						std::string id = ImGui::IMGUI_FORMAT_ID("",
							FOLDOUT_ID, string_extensions::StringToUpper(comp->GetName()) + "_INSPECTOR");
						ImGui::FoldOutButton(
							std::string((m_aExpanded[id] ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + sys->GetSystemName() + id).c_str(), &m_aExpanded[id], ImVec2(width, size.y));
						ImGui::SameLine();
						if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, id + "_DELETE_INSPECTOR").c_str(), size, m_Window.GetIconFont()))
						{
							sys->DeleteComponent(entityId);
						}

						ImGui::PopStyleVar();
						ImGui::PopStyleVar();

						if (m_aExpanded[id])
						{
							ImGui::Indent(m_Window.GetFramePadding().x * 4);
							RenderEditorForObject(comp);
							ImGui::Unindent(m_Window.GetFramePadding().x * 4);
						}
					}
				}
				ImGui::PopStyleVar();

				ImGui::SetCursorPosX(0 + m_Window.GetFramePadding().x);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);
				
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FOLDER + std::string(" Add Component"), BUTTON_ID, "ADD_COMPONENT_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					ImVec2 buttonPos = ImGui::GetItemRectMin();

					// Set the popup window position just below the button
					ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y));

					ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str());
				}

				gameplay::EntityComponentSystem& ecs = core::EDITOR_ENGINE->GetECS();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_Window.GetFramePadding());
				ImGui::SetNextWindowSize(ImVec2(width, 0));
				if (ImGui::BeginPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str()))
				{
					for (gameplay::AbstractECSSystem* system : core::EDITOR_ENGINE->GetECS().GetSystems())
					{
						if (system->HasComponent(entityId))
						{
							continue;
						}

						if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(system->GetSystemName(), MENU_ITEM_ID, "ADD_COMPONENT_MENU_INSPECTOR_" + system->GetPropertyName()).c_str()))
						{
							system->CreateBaseComponent(entityId);
						}
					}

					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();
			}

			//---------------------------------------------------------------------
			void EntityInspectorView::RenderPreview()
			{
				//ComponentBaseUIView* view = nullptr;
				//for (ComponentBaseUIView* component : m_aComponents)
				//{
				//	if (component->ShowPreview() && (!view || component->GetPreviewPriority() > view->GetPreviewPriority()))
				//	{
				//		view = component;
				//	}
				//}

				//if (view)
				//{
				//	view->RenderPreview();
				//}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE