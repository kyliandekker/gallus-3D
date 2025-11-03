#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "SceneWindow.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/dx12/Texture.h"

// editor includes
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/views/HierarchyEntityUIView.h"
#include "editor/graphics/imgui/views/inspector/EntityInspectorView.h"
#include "editor/graphics/imgui/views/inspector/components/ComponentUIView.h"

// gameplay includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// HierarchyWindow
			//---------------------------------------------------------------------
			SceneWindow::SceneWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_SCENE) + " Scene", "Scene")
			{
				ImGuizmo::Enable(true);
			}

			//---------------------------------------------------------------------
			SceneWindow::~SceneWindow()
			{}

			//---------------------------------------------------------------------
			void SceneWindow::Update()
			{
				bool isStarted = gameplay::GAME.IsStarted();
				bool isPaused = gameplay::GAME.IsPaused();

				graphics::dx12::Camera* cam = &core::ENGINE->GetDX12().GetCamera();
				if ((!isStarted || (isStarted && isPaused)) && core::EDITOR_ENGINE->GetEditor().GetCameraMode() == editor::CameraMode::CAMERA_MODE_SCENE)
				{
					cam = &core::EDITOR_ENGINE->GetEditor().GetEditorCamera();
				}
				core::EDITOR_ENGINE->GetDX12().SetActiveCamera(*cam);

				if (gameplay::GAME.IsStarted() && !gameplay::GAME.IsPaused())
				{
					return;
				}
				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void SceneWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				bool sceneMode = core::EDITOR_ENGINE->GetEditor().GetEditorMethod() == editor::EditorMethod::EDITOR_METHOD_SCENE;
				if (!sceneMode)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				bool isStarted = gameplay::GAME.IsStarted();
				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PLAY), BUTTON_ID, "PLAY_SCENE").c_str(), &isStarted, m_Window.GetHeaderSize()))
				{
					core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr, nullptr);
					if (isStarted)
					{
						gameplay::GAME.GetScene().Load();
						gameplay::GAME.GetScene().LoadData();
					}
					else
					{
						core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();
					}
					gameplay::GAME.SetIsStarted(isStarted);
				}
				ImGui::SameLine();

				bool isPaused = gameplay::GAME.IsPaused();
				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PAUSE), BUTTON_ID, "PAUSE_SCENE").c_str(), &isPaused, m_Window.GetHeaderSize()))
				{
					gameplay::GAME.SetIsPaused(isPaused);
				}
				ImGui::SameLine();

				if (!sceneMode)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				bool inGameMode = core::EDITOR_ENGINE->GetEditor().GetCameraMode() == editor::CameraMode::CAMERA_MODE_GAME;
				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_GAMEMODE), BUTTON_ID, "CAMERA_MODE_GAME").c_str(), &inGameMode, m_Window.GetHeaderSize()))
				{
					core::EDITOR_ENGINE->GetEditor().SetCameraMode(inGameMode ? editor::CameraMode::CAMERA_MODE_GAME : editor::CameraMode::CAMERA_MODE_SCENE);
				}

				ImGui::EndToolbar(ImVec2(0, 0));

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				DrawViewportPanel();

				std::shared_ptr<gallus::graphics::dx12::Texture> renderTexture = core::EDITOR_ENGINE->GetDX12().GetRenderTexture();
				if (!renderTexture || !renderTexture->IsValid())
				{
					return;
				}

				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				ImVec2 textureSize = ImVec2(
					static_cast<float>(renderTexture->GetSize().x),
					static_cast<float>(renderTexture->GetSize().y)
				);

				// Static variables for m_fZoom and pan
				m_fZoom = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetSceneZoom();
				m_vPanOffset = ImVec2(
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetScenePanOffset().x,
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetScenePanOffset().y
				);

				// Begin scrollable child region
				ImGui::BeginChild("SceneScroll", windowSize, 0,
					ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

				ImVec2 childPos = ImGui::GetCursorScreenPos(); // top-left corner of child

				ImVec2 mouseLocal = ImGui::GetIO().MousePos - childPos; // mouse relative to child

				// Zoom with mouse wheel
				if (ImGui::IsWindowHovered())
				{
					float wheel = ImGui::GetIO().MouseWheel;
					if (wheel != 0.0f)
					{
						ImVec2 beforeZoom = (mouseLocal - m_vPanOffset) / m_fZoom;
						m_fZoom = std::clamp(m_fZoom * (wheel > 0 ? 1.1f : 0.9f), 0.05f, 4.0f);
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetSceneZoom(m_fZoom);
						m_vPanOffset = mouseLocal - beforeZoom * m_fZoom;
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetScenePanOffset(glm::vec2(m_vPanOffset.x, m_vPanOffset.y));
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
					}
				}

				// Pan with middle mouse button
				if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
				{
					m_vPanOffset += ImGui::GetIO().MouseDelta;
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetScenePanOffset(glm::vec2(m_vPanOffset.x, m_vPanOffset.y));
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}

				// Draw the texture
				ImVec2 imageScreenPos = ImGui::GetCursorScreenPos();
				ImVec2 imagePos = m_vPanOffset;
				ImVec2 imageSize = ImVec2(textureSize.x * m_fZoom, textureSize.y * m_fZoom);
				ImGui::SetCursorPos(imagePos);
				ImGui::Image(
					(ImTextureID) renderTexture->GetGPUHandle().ptr,
					imageSize
				);

				if (core::EDITOR_ENGINE->GetEditor().GetCameraMode() == editor::CameraMode::CAMERA_MODE_SCENE)
				{
					HandleCameraInput(gameplay::GAME.GetDeltaTime(), imageScreenPos, textureSize);

					DrawComponentGizmos(imageScreenPos, textureSize);
				}

				ImU32 borderColor = IM_COL32(255, 255, 255, 255); // white border
				float borderThickness = 2.0f;
				ImGui::GetWindowDrawList()->AddRect(
					childPos + imagePos,
					childPos + imagePos + imageSize,
					borderColor,
					0.0f, // rounding
					0,    // flags
					borderThickness
				);

				ImGui::EndChild();

				// Optional reset
				if (ImGui::IsKeyPressed(ImGuiKey_R) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				{
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetSceneZoom(1.0f);
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetScenePanOffset(glm::vec2(0));
					core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				}
			}

			void SceneWindow::Draw2DGrid(
				const ImVec2& a_vScenePos,
				const ImVec2& a_vSize,
				const ImVec2& a_vPanOffset,
				float a_fZoom)
			{
				ImDrawList* drawList = ImGui::GetWindowDrawList();

				const float baseSpacing = 100.0f;
				const int majorEvery = 10;
				const ImU32 minorColor = IM_COL32(255, 255, 255, 25);
				const ImU32 majorColor = IM_COL32(255, 255, 255, 55);
				const ImU32 axisXColor = IM_COL32(255, 80, 80, 200);
				const ImU32 axisYColor = IM_COL32(80, 160, 255, 200);

				ImVec2 rectMin = ImVec2(a_vScenePos.x + a_vPanOffset.x, a_vScenePos.y + a_vPanOffset.y);
				ImVec2 rectMax = ImVec2(rectMin.x + a_vSize.x * a_fZoom, rectMin.y + a_vSize.y * a_fZoom);

				graphics::dx12::Camera& cam = core::ENGINE->GetDX12().GetActiveCamera();
				const DirectX::XMFLOAT2& cameraPos = cam.Transform().GetPosition();

				// Draw vertical grid lines
				float leftWorld = cameraPos.x + 0.5f;
				float rightWorld = cameraPos.x + a_vSize.x / a_fZoom;
				float firstX = floorf(leftWorld / baseSpacing) * baseSpacing;

				for (float x = firstX;; x += baseSpacing)
				{
					float screenX = rectMin.x + (x - cameraPos.x) * a_fZoom;
					if (screenX > rectMax.x + 0.5f) break; // stop when past rect
					if (screenX >= rectMin.x - 0.5f)      // only draw inside rect
					{
						bool major = (static_cast<int>(x / baseSpacing) % majorEvery == 0);
						ImU32 color = major ? majorColor : minorColor;
						drawList->AddLine(ImVec2(screenX, rectMin.y), ImVec2(screenX, rectMax.y), color);
					}
				}

				// Draw horizontal grid lines
				float topWorld = cameraPos.y;
				float bottomWorld = cameraPos.y + a_vSize.y / a_fZoom;
				float firstY = floorf(topWorld / baseSpacing) * baseSpacing;

				for (float y = firstY;; y += baseSpacing)
				{
					float screenY = rectMin.y + (y - cameraPos.y) * a_fZoom;
					if (screenY > rectMax.y + 0.5f) break;
					if (screenY >= rectMin.y - 0.5f)
					{
						bool major = (static_cast<int>(y / baseSpacing) % majorEvery == 0);
						ImU32 color = major ? majorColor : minorColor;
						drawList->AddLine(ImVec2(rectMin.x, screenY), ImVec2(rectMax.x, screenY), color);
					}
				}

				// Draw axes
				float originX = rectMin.x + (0.0f - cameraPos.x) * a_fZoom;
				float originY = rectMin.y + (0.0f - cameraPos.y) * a_fZoom;

				if (originX >= rectMin.x && originX <= rectMax.x)
					drawList->AddLine(ImVec2(originX, rectMin.y), ImVec2(originX, rectMax.y), axisYColor, 1.5f);

				if (originY >= rectMin.y && originY <= rectMax.y)
					drawList->AddLine(ImVec2(rectMin.x, originY), ImVec2(rectMax.x, originY), axisXColor, 1.5f);
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawComponentGizmos(const ImVec2& a_vSceneStartPos, const ImVec2& a_vSize)
			{
				DrawGizmos(a_vSceneStartPos, a_vSize, m_vPanOffset, m_fZoom);
				Draw2DGrid(a_vSceneStartPos, a_vSize, m_vPanOffset, m_fZoom);

				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				//auto test = core::EDITOR_ENGINE->GetEditor().GetSelectable().get();
				//if (!test)
				//{
				//	return;
				//}

				//const HierarchyEntityUIView* entity = dynamic_cast<const HierarchyEntityUIView*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get());
				//if (!entity)
				//{
				//	return;
				//}

				//EntityInspectorView* entityInspectorView = dynamic_cast<EntityInspectorView*>(core::EDITOR_ENGINE->GetEditor().GetInspectorView());
				//if (!entityInspectorView)
				//{
				//	return;
				//}

				//for (ComponentBaseUIView* component : entityInspectorView->GetComponents())
				//{
				//	component->RenderComponentGizmos(a_vSceneStartPos, a_vSize, m_vPanOffset, m_fZoom);
				//}
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom)
			{
				ImGuizmo::BeginFrame();

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

				ImGuizmo::SetRect(a_vScenePos.x + a_vPanOffset.x, a_vScenePos.y + a_vPanOffset.y, a_vSize.x * a_fZoom, a_vSize.y * a_fZoom);
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawViewportPanel()
			{
				ImGuiWindowFlags toolbarFlags =
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoFocusOnAppearing |
					ImGuiWindowFlags_NoNav |
					ImGuiWindowFlags_NoScrollbar;

				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f));

				// Fixed toolbar size
				ImVec2 toolbarSize(m_Window.GetHeaderSize().x + (m_Window.GetWindowPadding().x * 2), m_Window.GetHeaderSize().x * 5); // width x height
				ImGui::SetNextWindowSize(toolbarSize, ImGuiCond_Always);
				if (ImGui::Begin(ImGui::IMGUI_FORMAT_ID("", WINDOW_ID, "SCENE_TOOLBAR").c_str(), nullptr, toolbarFlags))
				{
					// Create a child for the actual content (with border)
					ImGuiWindowFlags childFlags =
						ImGuiWindowFlags_NoScrollbar |
						ImGuiWindowFlags_NoScrollWithMouse;

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
					if (ImGui::BeginChild(
						ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "SCENE_TOOLBAR_CHILD").c_str(),
						ImVec2(
						m_Window.GetHeaderSize().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
						),
						ImGuiChildFlags_None
						))
					{
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

						bool isTranslate = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::TRANSLATE;
						if (ImGui::IconCheckboxButton(
							ImGui::IMGUI_FORMAT_ID(font::ICON_TRANSLATE, BUTTON_ID, "TRANSLATE").c_str(),
							&isTranslate,
							m_Window.GetHeaderSize(),
							m_Window.GetIconFont(),
							isTranslate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
						{
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::TRANSLATE);
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
						}

						bool isRotate = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::ROTATE_Z;
						if (ImGui::IconCheckboxButton(
							ImGui::IMGUI_FORMAT_ID(font::ICON_ROTATE, BUTTON_ID, "ROTATE").c_str(),
							&isRotate,
							m_Window.GetHeaderSize(),
							m_Window.GetIconFont(),
							isRotate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
						{
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::ROTATE_Z);
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
						}

						bool isScale = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::SCALE;
						if (ImGui::IconCheckboxButton(
							ImGui::IMGUI_FORMAT_ID(font::ICON_SCALE, BUTTON_ID, "SCALE").c_str(),
							&isScale,
							m_Window.GetHeaderSize(),
							m_Window.GetIconFont(),
							isScale ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
						{
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::SCALE);
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
						}

						ImGui::PopStyleVar();
						ImGui::PopStyleVar();
					}
					ImGui::EndChild();

					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();

					// --- DRAGGING ON BORDER ONLY ---
					// Border thickness from style
					float borderThickness = ImGui::GetStyle().FrameBorderSize + 3.0f;

					// Expand the whole toolbar rect
					ImVec2 winPos = ImGui::GetWindowPos();
					ImVec2 winSize = ImGui::GetWindowSize();

					ImRect borderRect(
						winPos,
						ImVec2(winPos.x + winSize.x, winPos.y + winSize.y)
					);

					// Shrink inside rect (where child lives)
					ImRect innerRect(
						winPos.x + borderThickness,
						winPos.y + borderThickness,
						winPos.x + winSize.x - borderThickness,
						winPos.y + winSize.y - borderThickness
					);

					// Border = full area - inner area
					if (ImGui::IsMouseHoveringRect(borderRect.Min, borderRect.Max) &&
						!ImGui::IsMouseHoveringRect(innerRect.Min, innerRect.Max))
					{
						if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
						{
							ImVec2 newPos = winPos + ImGui::GetIO().MouseDelta;
							ImGui::SetWindowPos(newPos);
						}
					}
				}

				ImGui::End();
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}

			//---------------------------------------------------------------------
			FullSceneWindow::FullSceneWindow(ImGuiWindow& a_Window) : SceneWindow(a_Window)
			{
				m_sWindowID = "FullScene";
				m_sName = "FullScene";
				m_Flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
			}

			//---------------------------------------------------------------------
			void FullSceneWindow::Update()
			{
				m_bFullScreen = true;
				if (!gameplay::GAME.IsStarted() || gameplay::GAME.IsPaused())
				{
					return;
				}

				BaseWindow::Update();
			}

			void SceneWindow::HandleCameraInput(double a_fDeltaTime, const ImVec2& a_vSceneStartPos, const ImVec2& a_vSize)
			{
				ImGuiIO& io = ImGui::GetIO();
				graphics::dx12::Camera& camera = core::EDITOR_ENGINE->GetEditor().GetEditorCamera();

				DirectX::XMFLOAT2 position = camera.Transform().GetPosition();

				ImVec2 imageMin = { a_vSceneStartPos.x + m_vPanOffset.x, a_vSceneStartPos.y + m_vPanOffset.y };
				ImVec2 imageMax = imageMin + ImVec2(a_vSize.x * m_fZoom, a_vSize.y * m_fZoom);

				ImVec2 mouse = io.MousePos;

				// Persistent state for mouse look
				static bool moving = false;
				static POINT lastCursorPos;

				if (ImGui::IsWindowHovered() &&
					mouse.x >= imageMin.x && mouse.y >= imageMin.y &&
					mouse.x <= imageMax.x && mouse.y <= imageMax.y)
				{
					HWND hwnd = core::EDITOR_ENGINE->GetWindow().GetHWnd();

					// Right-click drag -> rotate
					if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
					{
						if (!moving)
						{
							moving = true;
							ImGui::SetMouseCursor(ImGuiMouseCursor_None);

							// Cache current cursor
							GetCursorPos(&lastCursorPos);
						}

						POINT currentPos;
						GetCursorPos(&currentPos);

						float deltaX = static_cast<float>(currentPos.x - lastCursorPos.x);
						float deltaY = static_cast<float>(currentPos.y - lastCursorPos.y);

						float moveSpeed = 20 * a_fDeltaTime;

						position.x -= deltaX * moveSpeed;
						position.y -= deltaY * moveSpeed;

						// Reset cursor so deltas stay per-frame
						SetCursorPos(lastCursorPos.x, lastCursorPos.y);
					}
					else
					{
						if (moving)
						{
							moving = false;
							ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
						}
					}
				}
				else
				{
					if (moving)
					{
						moving = false;
						ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
					}
				}

				// Always apply position after rotation
				camera.Transform().SetPosition(position);
			}

			//---------------------------------------------------------------------
			void FullSceneWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				bool isStarted = gameplay::GAME.IsStarted();
				if (ImGui::IsKeyDown(ImGuiKey_Escape))
				{
					core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr, nullptr);
					core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();
					gameplay::GAME.SetIsStarted(false);
				}

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				ImVec2 startToolbarPos = ImGui::GetCursorPos();
				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				bool prefabMode = core::EDITOR_ENGINE->GetEditor().GetEditorMethod() == editor::EditorMethod::EDITOR_METHOD_PREFAB;
				if (prefabMode)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PLAY), BUTTON_ID, "PLAY_FULL_SCENE").c_str(), &isStarted, m_Window.GetHeaderSize()))
				{
					core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr, nullptr);
					if (isStarted)
					{
						gameplay::GAME.GetScene().Load();
						gameplay::GAME.GetScene().LoadData();
					}
					else
					{
						core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();
					}
					gameplay::GAME.SetIsStarted(isStarted);
				}
				ImGui::SameLine();
				bool isPaused = gameplay::GAME.IsPaused();
				if (ImGui::CheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PAUSE), BUTTON_ID, "PAUSE_FULL_SCENE").c_str(), &isPaused, m_Window.GetHeaderSize()))
				{
					gameplay::GAME.SetIsPaused(isPaused);
				}

				if (prefabMode)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::EndToolbar(ImVec2(0, 0));
				ImGui::SetCursorPos(ImVec2(startToolbarPos.x, startToolbarPos.y + toolbarSize.y + m_Window.GetFramePadding().y));

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				std::shared_ptr<gallus::graphics::dx12::Texture> renderTexture = core::EDITOR_ENGINE->GetDX12().GetRenderTexture();
				if (!renderTexture || !renderTexture->IsValid())
				{
					return;
				}

				// Available region (minus padding)
				ImVec2 avail = ImGui::GetContentRegionAvail() - ImVec2(0, m_Window.GetFramePadding().y);
				ImVec2 padding = ImVec2();
				avail.x -= padding.x * 2.0f;
				avail.y -= padding.y * 2.0f;

				// Fit inside available space (keep aspect ratio)
				float scale = std::min(avail.x / renderTexture->GetSize().x, avail.y / renderTexture->GetSize().y);
				float drawW = renderTexture->GetSize().x * scale;
				float drawH = renderTexture->GetSize().y * scale;

				// Center horizontally
				float cursorX = ImGui::GetCursorPosX() + (avail.x - drawW) * 0.5f;
				ImGui::SetCursorPosX(cursorX);

				// Center horizontally
				ImGui::SetCursorPosX(cursorX);

				// Draw the texture
				ImVec2 image_pos = ImGui::GetCursorScreenPos();

				ImGui::SetCursorScreenPos(image_pos);
				ImGui::Image(
					(ImTextureID) renderTexture->GetGPUHandle().ptr,
					ImVec2(drawW, drawH)
				);

				// Draw border
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRect(image_pos, ImVec2(image_pos.x + drawW, image_pos.y + drawH),
					ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Border]));

				{
					std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(core::ENGINE->GetDX12().GetFPS().GetFPS()))) + " graphics fps";

					ImGui::PushFont(m_Window.GetCapitalFont());
					ImGui::SetCursorScreenPos(ImVec2(image_pos.x + drawW - (ImGui::CalcTextSize(fpsValue.c_str()).x + m_Window.GetWindowPadding().x), image_pos.y + m_Window.GetWindowPadding().y));
					ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
					ImGui::PopFont();
				}

				{
					std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(gameplay::GAME.GetFps()))) + " game fps";

					ImGui::PushFont(m_Window.GetCapitalFont());
					ImGui::SetCursorScreenPos(ImVec2(image_pos.x + drawW - (ImGui::CalcTextSize(fpsValue.c_str()).x + m_Window.GetWindowPadding().x), image_pos.y + (m_Window.GetFontSize() * 2) + m_Window.GetWindowPadding().y));
					ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
					ImGui::PopFont();
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE
