#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "SceneWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>

// graphics
#include "graphics/dx12/Texture.h"

#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

#include "utils/math.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/selectables/EntityEditorSelectable.h"
#include "editor/EditorInputScope.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// SceneWindow
			//---------------------------------------------------------------------
			SceneWindow::SceneWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_SCENE) + " Scene", "Scene")
			{}
			
			//---------------------------------------------------------------------
			bool SceneWindow::Initialize()
			{
				PopulateBaseToolbar();
				PopulateToolbar();

				RegisterKeybinds();

				ImGuizmo::Enable(true);

				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			SceneWindow::~SceneWindow()
			{}

			//---------------------------------------------------------------------
			void SceneWindow::Update()
			{
				gameplay::Game& game = gameplay::GAME;
				bool isStarted = game.IsStarted();
				bool isPaused = game.IsPaused();

				graphics::dx12::DX12System& dx12System = core::EDITOR_ENGINE->GetDX12();
				graphics::dx12::Camera* cam = &dx12System.GetCamera();
				editor::Editor& editor = core::EDITOR_ENGINE->GetEditor();
				if ((!isStarted || (isStarted && isPaused)) && editor.GetCameraMode() == editor::CameraMode::CAMERA_MODE_SCENE)
				{
					cam = &editor.GetEditorCamera();
				}
				dx12System.SetActiveCamera(*cam);

				if (editor.GetEditorSettings().GetFullScreenPlayMode())
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

				// Not the cleanest, but this copies the game camera FoV to the editor camera.
				core::EDITOR_ENGINE->GetEditor().CopyGameFoV();

				// Draw the toolbar first.
				DrawToolbar();

				ImVec2 startPos = ImGui::GetCursorScreenPos();

				// Begin scene part.
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				if (ImGui::BeginChild("SceneScroll", windowSize, 0,
					ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
				{
					ImVec2 viewportPos = ImGui::GetCursorScreenPos();

					// Handle zooming, panning, etc.
					HandleViewportControls();

					// Get the texture and determine the position and size of the scene view (based on panning and zoom).
					if (graphics::dx12::Texture* tex = GetRenderTexture())
					{
						ImRect sceneViewRect = GetRenderTextureRect(tex);

						// Draw the scene view.
						DrawSceneView(tex, sceneViewRect);

						// Handle specific controls like camera.
						HandleSceneViewControls(core::EDITOR_ENGINE->GetDX12().GetFPS().GetDeltaTime(), sceneViewRect);
					}

					ImGui::SetCursorScreenPos(viewportPos);
				}
				ImGui::EndChild();

				ImGui::SetCursorScreenPos(startPos);

				DrawViewportPanel();
			}
			
			//---------------------------------------------------------------------
			void SceneWindow::PopulateBaseToolbar()
			{
				// ---- Top toolbar ----

				ImVec2 toolbarSize = ImVec2(0, m_Window.GetHeaderSize().y);
				m_TopToolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_SCENE"), toolbarSize);

				// Play button.
				m_TopToolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					// TODO: This should probably be a global function.
					[this]()
					{
						bool isStarted = gameplay::GAME.IsStarted();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PLAY), BUTTON_ID, "PLAY_SCENE").c_str(), &isStarted, "Starts or stops the game simulation using the currently loaded scene. When stopping, the scene is reloaded to its original editor state.", ImVec2(m_TopToolbar.GetToolbarSize().y, m_TopToolbar.GetToolbarSize().y)))
						{
							editor::Editor& editor = core::EDITOR_ENGINE->GetEditor();

							editor.SetSelectable(nullptr);
							
							// If we start the game, set the game scene to the scene that is currently opened in the editor.
							if (isStarted)
							{
								const fs::path scenePath = editor.GetScene().GetPath();

								gameplay::Scene& gameScene = gameplay::GAME.GetScene();
								gameScene.LoadByPath(scenePath);
								gameScene.LoadData();
							}
							// If we stop the game, we want to load back the initial scene.
							// TODO: We should not do this, we should go back to a (possibly) edited version of the scene.
							else
							{
								gameplay::Scene& editorScene = editor.GetScene();
								const fs::path scenePath = editorScene.GetPath();

								editorScene.LoadByPath(scenePath);
								editorScene.LoadData();
							}
							gameplay::GAME.SetIsStarted(isStarted);
						}
					},
					// Disable if the editor is in prefab mode.
					[]()
					{
						return core::EDITOR_ENGINE->GetEditor().GetEditorMethod() != editor::EditorMethod::EDITOR_METHOD_SCENE;
					}
				));

				// Pause button.
				m_TopToolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						bool isPaused = gameplay::GAME.IsPaused();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PAUSE), BUTTON_ID, "PAUSE_SCENE").c_str(), &isPaused, "Pauses or resumes the game simulation while preserving the current runtime state.", ImVec2(m_TopToolbar.GetToolbarSize().y, m_TopToolbar.GetToolbarSize().y)))
						{
							gameplay::GAME.SetIsPaused(isPaused);
						}
					},
					// Disable if not in game mode.
					[]()
					{
						return !gameplay::GAME.IsStarted();
					}
				));

				// Full screen mode button.
				m_TopToolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					
					[this]()
					{
						editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();

						bool inFullScreen = editorSettings.GetFullScreenPlayMode();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_GAMEMODE), BUTTON_ID, "FULL_SCREEN_PLAY_MODE_SCENE").c_str(), &inFullScreen, "Toggles fullscreen play mode, rendering the game view without editor UI overlays.", ImVec2(m_TopToolbar.GetToolbarSize().y, m_TopToolbar.GetToolbarSize().y)))
						{
							editorSettings.SetFullScreenPlayMode(inFullScreen);
							editorSettings.Save();
						}
					}
				));
			}

			//---------------------------------------------------------------------
			void SceneWindow::PopulateToolbar()
			{
				// Grid button.
				m_TopToolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();

						bool showGrid = editorSettings.GetShowGrid();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SCENE), BUTTON_ID, "SHOW_GRID_SCENE").c_str(), &showGrid, "Toggles visibility of the scene grid to assist with spatial alignment and positioning.", ImVec2(m_TopToolbar.GetToolbarSize().y, m_TopToolbar.GetToolbarSize().y)))
						{
							editorSettings.SetShowGrid(showGrid);
							editorSettings.Save();
						}
					}
				));

				// Camera mode button.
				m_TopToolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					
					[this]()
					{
						editor::Editor& editor = core::EDITOR_ENGINE->GetEditor();

						bool inGameMode = editor.GetCameraMode() == editor::CameraMode::CAMERA_MODE_GAME;
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_CAMERA), BUTTON_ID, "CAMERA_MODE_GAME_SCENE").c_str(), &inGameMode, "Switches between the editor scene camera and the in-game camera view.", ImVec2(m_TopToolbar.GetToolbarSize().y, m_TopToolbar.GetToolbarSize().y)))
						{
							editor.SetCameraMode(inGameMode ? editor::CameraMode::CAMERA_MODE_GAME : editor::CameraMode::CAMERA_MODE_SCENE);
						}
					}
				));

				// Camera isolation mode button.
				m_TopToolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					
					[this]()
					{
						graphics::dx12::DX12System& dx12System = core::EDITOR_ENGINE->GetDX12();

						int camIsolationMode = dx12System.GetCameraIsolationMode();
						std::string camIsolationModeIcon = camIsolationMode == 0 ? font::ICON_2D3D : (camIsolationMode == 1 ? font::ICON_3D : font::ICON_2D);
						if (ImGui::TextButton(
							ImGui::IMGUI_FORMAT_ID(camIsolationModeIcon, BUTTON_ID, "CAMERA_ISOLATION_MODE_SCENE").c_str(), "Cycles camera rendering between combined 2D and 3D, 3D-only, and 2D-only modes.", m_Window.GetHeaderSize()))
						{
							camIsolationMode = ++camIsolationMode % 3;
							dx12System.SetCameraIsolationMode((graphics::dx12::CameraIsolationMode) camIsolationMode);
						}
					}
				));
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawToolbar()
			{
				// Start toolbar.
				m_TopToolbar.StartToolbar();

				// Render toolbar.
				m_TopToolbar.Render();

				// End toolbar.
				m_TopToolbar.EndToolbar();
			}

			//---------------------------------------------------------------------
			void SceneWindow::RegisterKeybinds()
			{
				editor::SetKeybindInputScope(editor::Keybind::Keybind_Zoom,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHover) |
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_Viewport) | static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick));

				editor::SetKeybindInputScope(editor::Keybind::Keybind_W,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick));
				editor::SetKeybindInputScope(editor::Keybind::Keybind_A,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick));
				editor::SetKeybindInputScope(editor::Keybind::Keybind_S,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick) | static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHover) | static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_Viewport));
				editor::SetKeybindInputScope(editor::Keybind::Keybind_D,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick));
				editor::SetKeybindInputScope(editor::Keybind::Keybind_Q,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick));
				editor::SetKeybindInputScope(editor::Keybind::Keybind_E,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick));
				editor::SetKeybindInputScope(editor::Keybind::Keybind_Shift,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick));

				editor::SetKeybindInputScope(editor::Keybind::Keybind_T,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHover) | static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_Viewport));
				editor::SetKeybindInputScope(editor::Keybind::Keybind_P,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHover) | static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_Viewport));
				editor::SetKeybindInputScope(editor::Keybind::Keybind_R,
					static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_SceneHover) | static_cast<uint32_t>(editor::EditorInputScope::EditorInputScope_Viewport));
			}

			//---------------------------------------------------------------------
			void SceneWindow::HandleViewportControls()
			{
				ImGuiIO& io = ImGui::GetIO();
				ImVec2 childPos = ImGui::GetCursorScreenPos();
				ImVec2 mouseLocal = io.MousePos - childPos;

				gallus::editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();
				m_fZoom = editorSettings.GetSceneZoom();
				m_vPanOffset = ImVec2(editorSettings.GetScenePanOffset().x, editorSettings.GetScenePanOffset().y);

				if (!ImGui::IsWindowHovered())
				{
					editor::DeactivateInputScope(editor::EditorInputScope::EditorInputScope_Viewport);
					return;
				}
				bool changed = false;

				// Zoom with mouse wheel
				if (ImGui::IsWindowHovered())
				{
					editor::ActivateInputScope(editor::EditorInputScope::EditorInputScope_Viewport);

					float wheel = io.MouseWheel;
					if (editor::CanActivate(editor::Keybind::Keybind_Zoom, editor::EditorInputScope::EditorInputScope_Viewport))
					{
						ImVec2 beforeZoom = (mouseLocal - m_vPanOffset) / m_fZoom;
						m_fZoom = std::clamp(m_fZoom * (wheel > 0 ? 1.1f : 0.9f), 0.05f, 4.0f);
						m_vPanOffset = mouseLocal - beforeZoom * m_fZoom;
						changed = true;
					}

					// Pan with middle mouse button
					if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
					{
						m_vPanOffset += io.MouseDelta;
						changed = true;
					}
				}

				if (changed)
				{
					editorSettings.SetSceneZoom(m_fZoom);
					editorSettings.SetScenePanOffset(m_vPanOffset);
					editorSettings.Save();
				}
			}

			//---------------------------------------------------------------------
			graphics::dx12::Texture* SceneWindow::GetRenderTexture() const
			{
				// Get the scene render texture.
				std::weak_ptr<gallus::graphics::dx12::Texture> renderTexture = core::EDITOR_ENGINE->GetDX12().GetRenderTexture();
				auto tex = renderTexture.lock();
				if (!tex || !tex->IsValid())
				{
					return nullptr;
				}

				return tex.get();
			}

			//---------------------------------------------------------------------
			ImRect SceneWindow::GetRenderTextureRect(gallus::graphics::dx12::Texture* a_pTexture) const
			{
				if (!a_pTexture)
				{
					return ImRect();
				}

				Vector2 texSize = a_pTexture->GetSize();
				ImVec2 textureSize = ImVec2(
					static_cast<float>(texSize.x),
					static_cast<float>(texSize.y)
				);

				ImVec2 imageScreenPos = ImGui::GetCursorScreenPos() + m_vPanOffset;
				ImVec2 imageSize = ImVec2(textureSize.x * m_fZoom, textureSize.y * m_fZoom);

				ImRect imageScreenRect = {
					{ imageScreenPos },
					{ imageScreenPos.x + imageSize.x, imageScreenPos.y + imageSize.y },
				};

				return imageScreenRect;
			}

			void SceneWindow::DrawSceneView(gallus::graphics::dx12::Texture* a_pTexture, const ImRect& a_SceneViewRect) const
			{
				if (!a_pTexture)
				{
					return;
				}

				// TODO: This will be tricky with a skybox... How will I display the grid??

				// First draw the (black) background, otherwise if there is no skybox, the game will be rendered transparent.
				DrawSceneBackground(a_SceneViewRect);

				// Setup ImGuizmo.
				SetupSceneGizmos(a_SceneViewRect);

				// Draw the grid below the scene.
				DrawSceneGrid(a_SceneViewRect);

				// Draw the actual scene with all the objects.
				DrawSceneTexture(a_pTexture, a_SceneViewRect);
				
				// Draw the gizmos on top of the scene.
				DrawSceneGizmos(a_SceneViewRect);

				// Finally draw the border around the whole scene view.
				DrawSceneViewBorder(a_SceneViewRect);
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawSceneBackground(const ImRect& a_SceneViewRect) const
			{
				// Draw background.
				ImU32 backgroundColor = IM_COL32(0, 0, 0, 255);
				ImGui::GetWindowDrawList()->AddRectFilled(
					a_SceneViewRect.Min,
					a_SceneViewRect.Max,
					backgroundColor,
					0.0f
				);
			}

			//---------------------------------------------------------------------
			void SceneWindow::SetupSceneGizmos(const ImRect& a_SceneViewRect) const
			{
				ImGuizmo::BeginFrame();

				int camIsolationMode = core::EDITOR_ENGINE->GetDX12().GetCameraIsolationMode();
				if (camIsolationMode == graphics::dx12::CameraIsolationMode_2D)
				{
					ImGuizmo::SetOrthographic(true);
				}
				else
				{
					ImGuizmo::SetOrthographic(false);
				}
				ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList); // vital because with other draw lists it will go outside of the view rect.

				ImGuizmo::SetRect(a_SceneViewRect.Min.x, a_SceneViewRect.Min.y, a_SceneViewRect.GetWidth(), a_SceneViewRect.GetHeight());
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawSceneGrid(const ImRect& a_SceneViewRect) const
			{
				if (!core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetShowGrid())
				{
					return;
				}

				graphics::dx12::DX12System& dx12System = core::EDITOR_ENGINE->GetDX12();
				int camIsolationMode = dx12System.GetCameraIsolationMode();
				if (camIsolationMode == graphics::dx12::CameraIsolationMode_2D)
				{
					ImDrawList* drawList = ImGui::GetWindowDrawList();

					const float baseSpacing = 100.0f;
					const int majorEvery = 10;
					const ImU32 minorColor = IM_COL32(255, 255, 255, 25);
					const ImU32 majorColor = IM_COL32(255, 255, 255, 55);
					const ImU32 axisXColor = IM_COL32(255, 80, 80, 200);
					const ImU32 axisYColor = IM_COL32(80, 160, 255, 200);

					graphics::dx12::Camera& cam = dx12System.GetActiveCamera();
					const DirectX::XMFLOAT3& cameraPos = cam.GetTransform().GetPosition();

					// Draw vertical grid lines
					float leftWorld = cameraPos.x + 0.5f;
					float rightWorld = cameraPos.x + a_SceneViewRect.GetWidth() / m_fZoom;
					float firstX = floorf(leftWorld / baseSpacing) * baseSpacing;

					for (float x = firstX;; x += baseSpacing)
					{
						float screenX = a_SceneViewRect.Min.x + (x - cameraPos.x) * m_fZoom;
						if (screenX > a_SceneViewRect.Max.x + 0.5f) break; // stop when past rect
						if (screenX >= a_SceneViewRect.Min.x - 0.5f)      // only draw inside rect
						{
							bool major = (static_cast<int>(x / baseSpacing) % majorEvery == 0);
							ImU32 color = major ? majorColor : minorColor;
							drawList->AddLine(ImVec2(screenX, a_SceneViewRect.Min.y), ImVec2(screenX, a_SceneViewRect.Max.y), color);
						}
					}

					// Draw horizontal grid lines
					float topWorld = cameraPos.y;
					float bottomWorld = cameraPos.y + a_SceneViewRect.GetHeight() / m_fZoom;
					float firstY = floorf(topWorld / baseSpacing) * baseSpacing;

					for (float y = firstY;; y += baseSpacing)
					{
						float screenY = a_SceneViewRect.Min.y + (y - cameraPos.y) * m_fZoom;
						if (screenY > a_SceneViewRect.Max.y + 0.5f) break;
						if (screenY >= a_SceneViewRect.Min.y - 0.5f)
						{
							bool major = (static_cast<int>(y / baseSpacing) % majorEvery == 0);
							ImU32 color = major ? majorColor : minorColor;
							drawList->AddLine(ImVec2(a_SceneViewRect.Min.x, screenY), ImVec2(a_SceneViewRect.Max.x, screenY), color);
						}
					}

					// Draw axes
					float originX = a_SceneViewRect.Min.x + (0.0f - cameraPos.x) * m_fZoom;
					float originY = a_SceneViewRect.Min.y + (0.0f - cameraPos.y) * m_fZoom;

					if (originX >= a_SceneViewRect.Min.x && originX <= a_SceneViewRect.Max.x)
					{
						drawList->AddLine(ImVec2(originX, a_SceneViewRect.Min.y), ImVec2(originX, a_SceneViewRect.Max.y), axisYColor, 1.5f);
					}

					if (originY >= a_SceneViewRect.Min.y && originY <= a_SceneViewRect.Max.y)
					{
						drawList->AddLine(ImVec2(a_SceneViewRect.Min.x, originY), ImVec2(a_SceneViewRect.Max.x, originY), axisXColor, 1.5f);
					}
				}
				else
				{
					const graphics::dx12::Camera& editorCamera = core::EDITOR_ENGINE->GetDX12().GetActiveCamera();

					// Get the camera projection and view matrices
					DirectX::XMMATRIX camProj = editorCamera.GetProjectionMatrix(graphics::dx12::CameraType_World);
					DirectX::XMMATRIX camView = editorCamera.GetViewMatrix(graphics::dx12::CameraType_World);

					// Draw the grid with the adjusted projection matrix
					ImGuizmo::DrawGrid(reinterpret_cast<float*>(&camView), reinterpret_cast<float*>(&camProj), reinterpret_cast<const float*>(&IDENTITY), 100.f);
				}
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawSceneTexture(gallus::graphics::dx12::Texture* a_pTexture, const ImRect& a_SceneViewRect) const
			{
				ImGui::SetCursorScreenPos(a_SceneViewRect.Min);
				ImGui::Image(
					(ImTextureID) a_pTexture->GetGPUHandle().ptr,
					a_SceneViewRect.GetSize()
				);
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawSceneGizmos(const ImRect& a_SceneViewRect) const
			{
				graphics::imgui::EditorSelectable* currentSelectable = core::EDITOR_ENGINE->GetEditor().GetSelectable().get();
				if (!currentSelectable)
				{
					return;
				}

				if (currentSelectable->RenderGizmos(a_SceneViewRect))
				{
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawSceneViewBorder(const ImRect& a_SceneViewRect) const
			{
				ImU32 borderColor = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Border));
				ImGui::GetWindowDrawList()->AddRect(
					a_SceneViewRect.Min,
					a_SceneViewRect.Max,
					borderColor,
					0.0f,
					0,
					2
				);
			}

			//---------------------------------------------------------------------
			void SceneWindow::HandleSceneViewControls(double a_fDeltaTime, const ImRect& a_vSceneRect)
			{
				graphics::dx12::DX12System& dx12System = core::EDITOR_ENGINE->GetDX12();
				graphics::dx12::Camera& camera = core::EDITOR_ENGINE->GetEditor().GetEditorCamera();
				if (&dx12System.GetActiveCamera() != &camera)
				{
					return;
				}

				ImGuiIO& io = ImGui::GetIO();
				ImVec2 mouse = io.MousePos;

				// Check if mouse is inside the scene rect
				bool bHovered = ImGui::IsWindowHovered() &&
					mouse.x >= a_vSceneRect.Min.x && mouse.y >= a_vSceneRect.Min.y &&
					mouse.x <= a_vSceneRect.Max.x && mouse.y <= a_vSceneRect.Max.y;

				bool mouseRightDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);
				if (bHovered && mouseRightDown)
				{
					editor::ActivateInputScope(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick);
				}
				else
				{
					editor::DeactivateInputScope(editor::EditorInputScope::EditorInputScope_SceneHoverRightClick);
				}

				if (bHovered)
				{
					editor::ActivateInputScope(editor::EditorInputScope::EditorInputScope_SceneHover);
				}
				else
				{
					editor::DeactivateInputScope(editor::EditorInputScope::EditorInputScope_SceneHover);
				}

				if (!bHovered)
				{
					return;
				}

				graphics::dx12::Transform& transform = camera.GetTransform();
				DirectX::XMFLOAT3 position = transform.GetPosition();

				const float rotationSpeed = 0.1f;

				// Increase speed.
				if (editor::CanActivate(editor::Keybind::Keybind_Zoom, editor::EditorInputScope::EditorInputScope_SceneHoverRightClick, false))
				{
					m_fMoveSpeed *= (io.MouseWheel > 0 ? 1.1f : 0.9f);
					m_fMoveSpeed = std::clamp(m_fMoveSpeed, 0.0f, 200.0f);
				}

				float totalMoveSpeed = m_fMoveSpeed;
				// Extra speed when pressing shift.
				if (editor::CanActivate(editor::Keybind::Keybind_Shift, editor::EditorInputScope::EditorInputScope_SceneHoverRightClick, false))
				{
					totalMoveSpeed += (m_fMoveSpeed / 2);
				}
				float moveDistance = totalMoveSpeed * static_cast<float>(a_fDeltaTime);
				int camIsolationMode = dx12System.GetCameraIsolationMode();

				static float s_fYawDegrees = 0.0f;
				static float s_fPitchDegrees = 0.0f;
				static POINT s_CenterPos = { 0, 0 };
				static bool s_bFirstFrame = true;

				// Only handle movement and rotation if right mouse is held
				if (mouseRightDown)
				{
					// Initialize first frame
					if (s_bFirstFrame)
					{
						s_bFirstFrame = false;
						ImVec2 viewportCenterLocal = {
							(a_vSceneRect.Min.x + a_vSceneRect.Max.x) * 0.5f,
							(a_vSceneRect.Min.y + a_vSceneRect.Max.y) * 0.5f
						};
						ImGuiViewport* viewport = ImGui::GetWindowViewport();
						ImVec2 windowPos = viewport->Pos;
						float dpiScale = viewport->DpiScale;
						s_CenterPos.x = static_cast<LONG>((windowPos.x + viewportCenterLocal.x) * dpiScale);
						s_CenterPos.y = static_cast<LONG>((windowPos.y + viewportCenterLocal.y) * dpiScale);
						ImGui::SetMouseCursor(ImGuiMouseCursor_None);
						SetCursorPos(s_CenterPos.x, s_CenterPos.y);
					}

					// Calculate mouse delta for rotation
					POINT currentPos;
					GetCursorPos(&currentPos);
					float deltaX = static_cast<float>(currentPos.x - s_CenterPos.x);
					float deltaY = static_cast<float>(currentPos.y - s_CenterPos.y);
					SetCursorPos(s_CenterPos.x, s_CenterPos.y);

					if (camIsolationMode != graphics::dx12::CameraIsolationMode_2D)
					{
						// Update yaw/pitch
						s_fYawDegrees += deltaX * rotationSpeed;
						s_fPitchDegrees += deltaY * rotationSpeed;

						if (s_fPitchDegrees > 89.0f) s_fPitchDegrees = 89.0f;
						if (s_fPitchDegrees < -89.0f) s_fPitchDegrees = -89.0f;

						// Compute rotation quaternion
						DirectX::XMVECTOR yawQuat = DirectX::XMQuaternionRotationAxis(
							DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
							DirectX::XMConvertToRadians(s_fYawDegrees)
						);
						DirectX::XMVECTOR pitchQuat = DirectX::XMQuaternionRotationAxis(
							DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
							DirectX::XMConvertToRadians(s_fPitchDegrees)
						);
						DirectX::XMVECTOR rotation = DirectX::XMQuaternionNormalize(
							DirectX::XMQuaternionMultiply(pitchQuat, yawQuat)
						);
						transform.SetRotation(rotation);

						// Extract directions for movement
						DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
						DirectX::XMFLOAT4X4 m;
						DirectX::XMStoreFloat4x4(&m, rotMatrix);
						DirectX::XMFLOAT3 right(m.m[0][0], m.m[0][1], m.m[0][2]);
						DirectX::XMFLOAT3 up(m.m[1][0], m.m[1][1], m.m[1][2]);
						DirectX::XMFLOAT3 forward(m.m[2][0], m.m[2][1], m.m[2][2]);

						// Keyboard movement (WASD + QE)
						if (editor::CanActivate(editor::Keybind::Keybind_W, editor::EditorInputScope::EditorInputScope_SceneHoverRightClick, false))
						{
							position.x += forward.x * moveDistance;
							position.y += forward.y * moveDistance;
							position.z += forward.z * moveDistance;
						}
						if (editor::CanActivate(editor::Keybind::Keybind_S, editor::EditorInputScope::EditorInputScope_SceneHoverRightClick, false))
						{
							position.x -= forward.x * moveDistance;
							position.y -= forward.y * moveDistance;
							position.z -= forward.z * moveDistance;
						}
						if (editor::CanActivate(editor::Keybind::Keybind_A, editor::EditorInputScope::EditorInputScope_SceneHoverRightClick, false))
						{
							position.x -= right.x * moveDistance;
							position.y -= right.y * moveDistance;
							position.z -= right.z * moveDistance;
						}
						if (editor::CanActivate(editor::Keybind::Keybind_D, editor::EditorInputScope::EditorInputScope_SceneHoverRightClick, false))
						{
							position.x += right.x * moveDistance;
							position.y += right.y * moveDistance;
							position.z += right.z * moveDistance;
						}
						if (editor::CanActivate(editor::Keybind::Keybind_Q, editor::EditorInputScope::EditorInputScope_SceneHoverRightClick, false))
						{
							position.x -= up.x * moveDistance;
							position.y -= up.y * moveDistance;
							position.z -= up.z * moveDistance;
						}
						if (editor::CanActivate(editor::Keybind::Keybind_E, editor::EditorInputScope::EditorInputScope_SceneHoverRightClick, false))
						{
							position.x += up.x * moveDistance;
							position.y += up.y * moveDistance;
							position.z += up.z * moveDistance;
						}
					}
				}
				else
				{
					// Reset first-frame flag when right button is released
					s_bFirstFrame = true;
				}

				// Mouse wheel zoom always works
				if (editor::CanActivate(editor::Keybind::Keybind_Zoom, editor::EditorInputScope::EditorInputScope_SceneHover))
				{
					DirectX::XMVECTOR rotation = transform.GetRotationQ();
					DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
					DirectX::XMFLOAT4X4 m;
					DirectX::XMStoreFloat4x4(&m, rotMatrix);
					DirectX::XMFLOAT3 forward(m.m[2][0], m.m[2][1], m.m[2][2]);

					position.x += forward.x * io.MouseWheel * moveDistance * 2.0f;
					position.y += forward.y * io.MouseWheel * moveDistance * 2.0f;
					position.z += forward.z * io.MouseWheel * moveDistance * 2.0f;
				}

				transform.SetPosition(position);
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawViewportPanel()
			{
				ImVec2 windowSize = {
					m_Window.GetHeaderSize().x,
					ImGui::GetContentRegionAvail().y
				};

				const int buttons = 4;

				if (ImGui::BeginChild("Operations", windowSize, 0,
					ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
				{
					ImGui::SetCursorScreenPos(ImVec2(
						ImGui::GetCursorScreenPos().x,
						ImGui::GetCursorScreenPos().y + (windowSize.y / 2) - ((buttons * m_Window.GetHeaderSize().y) / 2)
					));

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

					bool isTranslate = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::TRANSLATE;
					if (ImGui::IconCheckboxButton(
						ImGui::IMGUI_FORMAT_ID(font::ICON_GIZMO_TRANSLATE, BUTTON_ID, "TRANSLATE").c_str(),
						&isTranslate, "Enables position manipulation of the selected object using translation gizmos.",
						m_Window.GetHeaderSize(),
						isTranslate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
					{
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::TRANSLATE);
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
					}

					bool isRotate = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::ROTATE;
					if (ImGui::IconCheckboxButton(
						ImGui::IMGUI_FORMAT_ID(font::ICON_GIZMO_ROTATE, BUTTON_ID, "ROTATE").c_str(),
						&isRotate, "Enables rotation manipulation of the selected object using rotation gizmos.",
						m_Window.GetHeaderSize(),
						isRotate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
					{
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::ROTATE);
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
					}

					bool isScale = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::SCALE;
					if (ImGui::IconCheckboxButton(
						ImGui::IMGUI_FORMAT_ID(font::ICON_GIZMO_SCALE, BUTTON_ID, "SCALE").c_str(),
						&isScale, "Enables scale manipulation of the selected object using scaling gizmos.",
						m_Window.GetHeaderSize(),
						isScale ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
					{
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::SCALE);
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
					}

					bool isAll = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::UNIVERSAL;
					if (ImGui::IconCheckboxButton(
						ImGui::IMGUI_FORMAT_ID(font::ICON_GIZMO_ALL, BUTTON_ID, "ALL").c_str(),
						&isAll, "Enables all manipulations of the selected object.",
						m_Window.GetHeaderSize(),
						isAll ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
					{
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::UNIVERSAL);
						core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
					}

					editor::EditorInputScope inputScopesToCheck[2] = {
						editor::EditorInputScope::EditorInputScope_Viewport,
						editor::EditorInputScope::EditorInputScope_SceneHover,
					};
					for (editor::EditorInputScope inputScope : inputScopesToCheck)
					{
						if (editor::CanActivate(editor::Keybind::Keybind_T, inputScope)|| editor::CanActivate(editor::Keybind::Keybind_P, inputScope))
						{
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::TRANSLATE);
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
						}
						if (editor::CanActivate(editor::Keybind::Keybind_R, inputScope))
						{
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::ROTATE);
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
						}
						if (editor::CanActivate(editor::Keybind::Keybind_S, inputScope))
						{
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::SCALE);
							core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
						}
					}

					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();
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
				if (!core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFullScreenPlayMode())
				{
					return;
				}

				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void FullSceneWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				DrawToolbar();

				graphics::dx12::Texture* tex = GetRenderTexture();
				if (!tex)
				{
					return;
				}

				ImRect sceneViewRect = GetRenderTextureRect(tex);
				DrawSceneBackground(sceneViewRect);
				DrawSceneTexture(tex, sceneViewRect);
				DrawSceneViewBorder(sceneViewRect);
				DrawFPS(sceneViewRect);
			}

			//---------------------------------------------------------------------
			void FullSceneWindow::PopulateToolbar()
			{}
			
			//---------------------------------------------------------------------
			ImRect FullSceneWindow::GetRenderTextureRect(gallus::graphics::dx12::Texture * a_pTexture) const
			{
				// Available region (minus padding)
				ImVec2 avail = ImGui::GetContentRegionAvail();
				ImVec2 padding = ImVec2(0, m_Window.GetFramePadding().y);
				avail.x -= padding.x * 2.0f;
				avail.y -= padding.y * 2.0f;

				// Fit inside available space (keep aspect ratio)
				Vector2 size = a_pTexture->GetSize();
				float scale = std::min(avail.x / size.x, avail.y / size.y);
				float drawW = size.x * scale;
				float drawH = size.y * scale;

				// Center horizontally
				ImGui::SetCursorPos({
					ImGui::GetCursorPosX() + (avail.x - drawW) * 0.5f,
					ImGui::GetCursorPosY() + padding.y
				});

				ImVec2 sceneViewPosition = ImGui::GetCursorScreenPos();
				ImRect sceneViewRect = {
					sceneViewPosition,
					{ sceneViewPosition.x + drawW, sceneViewPosition.y + drawH }
				};
				return sceneViewRect;
			}

			//---------------------------------------------------------------------
			void FullSceneWindow::DrawFPS(const ImRect& a_SceneViewRect)
			{
				ImVec2 windowPadding = m_Window.GetWindowPadding();
				// Draw FPS.
				{
					std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(core::EDITOR_ENGINE->GetDX12().GetFPS().GetFPS()))) + " graphics fps";

					ImGui::PushFont(m_Window.GetCapitalFont());
					ImGui::SetCursorScreenPos(ImVec2(a_SceneViewRect.GetTR().x - (ImGui::CalcTextSize(fpsValue.c_str()).x + windowPadding.x), a_SceneViewRect.GetTR().y + windowPadding.y));
					ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
					ImGui::PopFont();
				}

				{
					std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(gameplay::GAME.GetFPS().GetFPS()))) + " game fps";

					ImGui::PushFont(m_Window.GetCapitalFont());
					ImGui::SetCursorScreenPos(ImVec2(a_SceneViewRect.GetTR().x - (ImGui::CalcTextSize(fpsValue.c_str()).x + windowPadding.x), a_SceneViewRect.GetTR().y + (m_Window.GetFontSize() * 2) + windowPadding.y));
					ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
					ImGui::PopFont();
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE
