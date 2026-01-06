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

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/selectables/EntityEditorSelectable.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// Toolbar
			//---------------------------------------------------------------------
			ToolbarButton::ToolbarButton(std::function<void()> a_ButtonFunc, std::function<bool()> a_DisabledCondition) :
				m_ButtonFunc(a_ButtonFunc),
				m_DisabledCondition(a_DisabledCondition)
			{}

			//---------------------------------------------------------------------
			void ToolbarButton::Render()
			{
				bool wasDisabled = m_DisabledCondition();
				if (wasDisabled)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				m_ButtonFunc();
				if (wasDisabled)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}

			//---------------------------------------------------------------------
			void ToolbarBreak::Render()
			{
				ImGui::InvisibleButton("INVISIBLE_BUTTON", m_vBreakSize);
			}

			//---------------------------------------------------------------------
			void Toolbar::StartToolbar()
			{
				if (m_vToolbarSize.x == 0)
				{
					m_vToolbarSize.x = ImGui::GetContentRegionAvail().x;
				}
				if (m_vToolbarSize.y == 0)
				{
					m_vToolbarSize.y = ImGui::GetContentRegionAvail().y;
				}

				m_vToolbarStartScreenPos = ImGui::GetCursorScreenPos();
				m_vToolbarStartPos = ImGui::GetCursorPos();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				ImVec2 header_min = m_vToolbarStartScreenPos;
				ImVec2 header_max = ImVec2(header_min.x + m_vToolbarSize.x, header_min.y + m_vToolbarSize.y);

				draw_list->AddRectFilled(header_min, header_max, ImGui::GetColorU32(ImGuiCol_Button), 0);
			}

			//---------------------------------------------------------------------
			void Toolbar::EndToolbar()
			{
				ImGui::SetCursorScreenPos({
					m_vToolbarStartScreenPos.x,
					m_vToolbarStartScreenPos.y + m_vToolbarSize.y
					});

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			//---------------------------------------------------------------------
			void Toolbar::Render()
			{
				for (size_t i = 0; i < m_aButtons.size(); i++)
				{
					m_aButtons[i].Render();
					if (i < m_aButtons.size() - 1)
					{
						ImGui::SameLine();
					}
				}
			}

			//---------------------------------------------------------------------
			// SceneWindow
			//---------------------------------------------------------------------
			SceneWindow::SceneWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_SCENE) + " Scene", "Scene")
			{}

			bool SceneWindow::Initialize()
			{
				PopulateToolbar();

				ImGuizmo::Enable(true);

				return true;
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

				// Begin scene part.
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				if (ImGui::BeginChild("SceneScroll", windowSize, 0,
					ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
				{
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
				}
				ImGui::EndChild();

				//DrawViewportPanel();
			}

			//---------------------------------------------------------------------
			void SceneWindow::PopulateToolbar()
			{
				ImVec2 toolbarSize = ImVec2(0, m_Window.GetHeaderSize().y);
				m_Toolbar = Toolbar(toolbarSize);

				// Play button.
				m_Toolbar.m_aButtons.emplace_back(
					// TODO: This should probably be a global function.
					[this]()
					{
						bool isStarted = gameplay::GAME.IsStarted();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PLAY), BUTTON_ID, "PLAY_SCENE").c_str(), &isStarted, "Starts or stops the game simulation using the currently loaded scene. When stopping, the scene is reloaded to its original editor state.", ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)))
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
				);

				// Pause button.
				m_Toolbar.m_aButtons.emplace_back(
					[this]()
					{
						bool isPaused = gameplay::GAME.IsPaused();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PAUSE), BUTTON_ID, "PAUSE_SCENE").c_str(), &isPaused, "Pauses or resumes the game simulation while preserving the current runtime state.", ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)))
						{
							gameplay::GAME.SetIsPaused(isPaused);
						}
					},
					// Disable if not in game mode.
					[]()
					{
						return !gameplay::GAME.IsStarted();
					}
					
				);

				// Grid button.
				m_Toolbar.m_aButtons.emplace_back(
					[this]()
					{
						editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();

						bool showGrid = editorSettings.GetShowGrid();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SCENE), BUTTON_ID, "SHOW_GRID_SCENE").c_str(), &showGrid, "Toggles visibility of the scene grid to assist with spatial alignment and positioning.", ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)))
						{
							editorSettings.SetShowGrid(showGrid);
							editorSettings.Save();
						}
					}
				);

				// Camera mode button.
				m_Toolbar.m_aButtons.emplace_back(
					
					[this]()
					{
						editor::Editor& editor = core::EDITOR_ENGINE->GetEditor();

						bool inGameMode = editor.GetCameraMode() == editor::CameraMode::CAMERA_MODE_GAME;
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_CAMERA), BUTTON_ID, "CAMERA_MODE_GAME_SCENE").c_str(), &inGameMode, "Switches between the editor scene camera and the in-game camera view.", ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)))
						{
							editor.SetCameraMode(inGameMode ? editor::CameraMode::CAMERA_MODE_GAME : editor::CameraMode::CAMERA_MODE_SCENE);
						}
					}
					
				);

				// Full screen mode button.
				m_Toolbar.m_aButtons.emplace_back(
					
					[this]()
					{
						editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();

						bool inFullScreen = editorSettings.GetFullScreenPlayMode();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_GAMEMODE), BUTTON_ID, "FULL_SCREEN_PLAY_MODE_SCENE").c_str(), &inFullScreen, "Toggles fullscreen play mode, rendering the game view without editor UI overlays.", ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)))
						{
							editorSettings.SetFullScreenPlayMode(inFullScreen);
							editorSettings.Save();
						}
					}
					
				);

				// Camera isolation mode button.
				m_Toolbar.m_aButtons.emplace_back(
					
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
					
				);
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawToolbar()
			{
				// Start toolbar.
				m_Toolbar.StartToolbar();

				// Render toolbar.
				m_Toolbar.Render();

				// End toolbar.
				m_Toolbar.EndToolbar();
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

				bool changed = false;

				// Zoom with mouse wheel
				if (ImGui::IsWindowHovered())
				{
					float wheel = io.MouseWheel;
					if (wheel != 0.0f)
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

				// Reset keybind.
				if (ImGui::IsKeyPressed(ImGuiKey_R) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				{
					m_fZoom = 1.0f;
					m_vPanOffset = ImVec2(0, 0);
					changed = true;
				}

				if (changed)
				{
					editorSettings.SetSceneZoom(m_fZoom);
					editorSettings.SetScenePanOffset(glm::vec2(m_vPanOffset.x, m_vPanOffset.y));
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

				glm::ivec2 texSize = a_pTexture->GetSize();
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

				// Static state for camera movement
				static bool s_bMoving = false;
				static bool s_bRotating = false;
				static bool s_bFirstRotateFrame = false;

				// Determine if mouse is inside scene viewport and right button is held
				bool bHovered = ImGui::IsWindowHovered() &&
					mouse.x >= a_vSceneRect.Min.x && mouse.y >= a_vSceneRect.Min.y &&
					mouse.x <= a_vSceneRect.Max.x && mouse.y <= a_vSceneRect.Max.y;

				bool allowInteraction = bHovered && ImGui::IsMouseDown(ImGuiMouseButton_Right);

				// Reset movement/rotation if not interacting
				if (!allowInteraction)
				{
					s_bMoving = false;
					s_bRotating = false;
					s_bFirstRotateFrame = false;
					return;
				}

				static float s_fYawDegrees = 0.0f;
				static float s_fPitchDegrees = 0.0f;
				static POINT s_CenterPos = { 0, 0 };

				// Camera references
				graphics::dx12::Transform& transform = camera.GetTransform();
				DirectX::XMFLOAT3 position = transform.GetPosition();

				// Movement settings
				const float moveSpeed = 20.0f;
				const float rotationSpeed = 0.1f;
				float moveDistance = moveSpeed * static_cast<float>(a_fDeltaTime);

				int camIsolationMode = dx12System.GetCameraIsolationMode();

					// Compute viewport center in screen space
				ImVec2 viewportCenterLocal =
				{
					(a_vSceneRect.Min.x + a_vSceneRect.Max.x) * 0.5f,
					(a_vSceneRect.Min.y + a_vSceneRect.Max.y) * 0.5f
				};

				ImGuiViewport* viewport = ImGui::GetWindowViewport();
				ImVec2 windowPos = viewport->Pos;
				float dpiScale = viewport->DpiScale;

				s_CenterPos.x = static_cast<LONG>((windowPos.x + viewportCenterLocal.x) * dpiScale);
				s_CenterPos.y = static_cast<LONG>((windowPos.y + viewportCenterLocal.y) * dpiScale);

				// Initialize first frame of movement
				if (!s_bMoving && !s_bRotating)
				{
					s_bMoving = true;
					s_bRotating = true;
					s_bFirstRotateFrame = true;
					ImGui::SetMouseCursor(ImGuiMouseCursor_None);
				}

				// Lock cursor to center on first rotation frame
				if (s_bFirstRotateFrame)
				{
					s_bFirstRotateFrame = false;
					SetCursorPos(s_CenterPos.x, s_CenterPos.y);
				}

				// Calculate mouse delta
				POINT currentPos;
				GetCursorPos(&currentPos);
				float deltaX = static_cast<float>(currentPos.x - s_CenterPos.x);
				float deltaY = static_cast<float>(currentPos.y - s_CenterPos.y);
				SetCursorPos(s_CenterPos.x, s_CenterPos.y);

				// 2D camera movement
				if (camIsolationMode == graphics::dx12::CameraIsolationMode_2D)
				{
					position.x -= deltaX * moveSpeed * static_cast<float>(a_fDeltaTime);
					position.y -= deltaY * moveSpeed * static_cast<float>(a_fDeltaTime);
					transform.SetPosition(position);
				}
				// 3D camera movement and rotation
				else
				{
					// Update yaw/pitch
					s_fYawDegrees += -deltaX * rotationSpeed;
					s_fPitchDegrees += -deltaY * rotationSpeed;

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

					// Extract direction vectors
					DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
					DirectX::XMFLOAT4X4 m;
					DirectX::XMStoreFloat4x4(&m, rotMatrix);

					DirectX::XMFLOAT3 right(m.m[0][0], m.m[0][1], m.m[0][2]);
					DirectX::XMFLOAT3 up(m.m[1][0], m.m[1][1], m.m[1][2]);
					DirectX::XMFLOAT3 forward(m.m[2][0], m.m[2][1], m.m[2][2]);

					// Keyboard movement
					if (ImGui::IsKeyDown(ImGuiKey_W))
					{
						position.x += forward.x * moveDistance;
						position.y += forward.y * moveDistance;
						position.z += forward.z * moveDistance;
					}
					if (ImGui::IsKeyDown(ImGuiKey_S))
					{
						position.x -= forward.x * moveDistance;
						position.y -= forward.y * moveDistance;
						position.z -= forward.z * moveDistance;
					}
					if (ImGui::IsKeyDown(ImGuiKey_A))
					{
						position.x -= right.x * moveDistance;
						position.y -= right.y * moveDistance;
						position.z -= right.z * moveDistance;
					}
					if (ImGui::IsKeyDown(ImGuiKey_D))
					{
						position.x += right.x * moveDistance;
						position.y += right.y * moveDistance;
						position.z += right.z * moveDistance;
					}
					if (ImGui::IsKeyDown(ImGuiKey_Q))
					{
						position.x -= up.x * moveDistance;
						position.y -= up.y * moveDistance;
						position.z -= up.z * moveDistance;
					}
					if (ImGui::IsKeyDown(ImGuiKey_E))
					{
						position.x += up.x * moveDistance;
						position.y += up.y * moveDistance;
						position.z += up.z * moveDistance;
					}

					// Mouse wheel zoom
					if (io.MouseWheel != 0.0f)
					{
						position.x += forward.x * io.MouseWheel * moveDistance * 2.0f;
						position.y += forward.y * io.MouseWheel * moveDistance * 2.0f;
						position.z += forward.z * io.MouseWheel * moveDistance * 2.0f;
					}

					// Apply final position
					transform.SetPosition(position);
				}
			}

			//---------------------------------------------------------------------
			void SceneWindow::DrawViewportPanel()
			{
				//ImGuiWindowFlags toolbarFlags =
				//	ImGuiWindowFlags_NoTitleBar |
				//	ImGuiWindowFlags_NoResize |
				//	ImGuiWindowFlags_NoFocusOnAppearing |
				//	ImGuiWindowFlags_NoNav |
				//	ImGuiWindowFlags_NoScrollbar;

				//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
				//ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
				//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f));

				//// Fixed toolbar size
				//ImVec2 toolbarSize(m_Window.GetHeaderSize().x + (m_Window.GetWindowPadding().x * 2), m_Window.GetHeaderSize().x * 5); // width x height
				//ImGui::SetNextWindowSize(toolbarSize, ImGuiCond_Always);
				//if (ImGui::Begin(ImGui::IMGUI_FORMAT_ID("", WINDOW_ID, "SCENE_TOOLBAR").c_str(), nullptr, toolbarFlags))
				//{
				//	// Create a child for the actual content (with border)
				//	ImGuiWindowFlags childFlags =
				//		ImGuiWindowFlags_NoScrollbar |
				//		ImGuiWindowFlags_NoScrollWithMouse;

				//	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
				//	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));

				//	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
				//	if (ImGui::BeginChild(
				//		ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "SCENE_TOOLBAR_CHILD").c_str(),
				//		ImVec2(
				//		m_Window.GetHeaderSize().x,
				//		ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
				//		),
				//		ImGuiChildFlags_None
				//		))
				//	{
				//		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				//		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				//		bool isTranslate = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::TRANSLATE;
				//		if (ImGui::IconCheckboxButton(
				//			ImGui::IMGUI_FORMAT_ID(font::ICON_TRANSLATE, BUTTON_ID, "TRANSLATE").c_str(),
				//			&isTranslate, "Enables position manipulation of the selected object using translation gizmos.",
				//			m_Window.GetHeaderSize(),
				//			isTranslate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
				//		{
				//			core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::TRANSLATE);
				//			core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				//		}

				//		bool isRotate = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::ROTATE;
				//		if (ImGui::IconCheckboxButton(
				//			ImGui::IMGUI_FORMAT_ID(font::ICON_ROTATE, BUTTON_ID, "ROTATE").c_str(),
				//			&isRotate, "Enables rotation manipulation of the selected object using rotation gizmos.",
				//			m_Window.GetHeaderSize(),
				//			isRotate ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
				//		{
				//			core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::ROTATE);
				//			core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				//		}

				//		bool isScale = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetLastSceneOperation() == ImGuizmo::SCALE;
				//		if (ImGui::IconCheckboxButton(
				//			ImGui::IMGUI_FORMAT_ID(font::ICON_SCALE, BUTTON_ID, "SCALE").c_str(),
				//			&isScale, "Enables scale manipulation of the selected object using scaling gizmos.",
				//			m_Window.GetHeaderSize(),
				//			isScale ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
				//		{
				//			core::EDITOR_ENGINE->GetEditor().GetEditorSettings().SetLastSceneOperation((int) ImGuizmo::SCALE);
				//			core::EDITOR_ENGINE->GetEditor().GetEditorSettings().Save();
				//		}

				//		ImGui::PopStyleVar();
				//		ImGui::PopStyleVar();
				//	}
				//	ImGui::EndChild();

				//	ImGui::PopStyleVar();
				//	ImGui::PopStyleVar();
				//	ImGui::PopStyleVar();

				//	// --- DRAGGING ON BORDER ONLY ---
				//	// Border thickness from style
				//	float borderThickness = ImGui::GetStyle().FrameBorderSize + 3.0f;

				//	// Expand the whole toolbar rect
				//	ImVec2 winPos = ImGui::GetWindowPos();
				//	ImVec2 winSize = ImGui::GetWindowSize();

				//	ImRect borderRect(
				//		winPos,
				//		ImVec2(winPos.x + winSize.x, winPos.y + winSize.y)
				//	);

				//	// Shrink inside rect (where child lives)
				//	ImRect innerRect(
				//		winPos.x + borderThickness,
				//		winPos.y + borderThickness,
				//		winPos.x + winSize.x - borderThickness,
				//		winPos.y + winSize.y - borderThickness
				//	);

				//	// Border = full area - inner area
				//	if (ImGui::IsMouseHoveringRect(borderRect.Min, borderRect.Max) &&
				//		!ImGui::IsMouseHoveringRect(innerRect.Min, innerRect.Max))
				//	{
				//		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				//		{
				//			ImVec2 newPos = winPos + ImGui::GetIO().MouseDelta;
				//			ImGui::SetWindowPos(newPos);
				//		}
				//	}
				//}

				//ImGui::End();
				//ImGui::PopStyleVar();
				//ImGui::PopStyleColor();
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

				// Available region (minus padding)
				ImVec2 avail = ImGui::GetContentRegionAvail();
				ImVec2 padding = ImVec2(0, m_Window.GetFramePadding().y);
				avail.x -= padding.x * 2.0f;
				avail.y -= padding.y * 2.0f;

				// Fit inside available space (keep aspect ratio)
				glm::ivec2 size = tex->GetSize();
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

				DrawSceneBackground(sceneViewRect);
				DrawSceneTexture(tex, sceneViewRect);
				DrawSceneViewBorder(sceneViewRect);

				ImVec2 windowPadding = m_Window.GetWindowPadding();
				// Draw FPS.
				{
					std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(core::EDITOR_ENGINE->GetDX12().GetFPS().GetFPS()))) + " graphics fps";

					ImGui::PushFont(m_Window.GetCapitalFont());
					ImGui::SetCursorScreenPos(ImVec2(sceneViewRect.GetTR().x - (ImGui::CalcTextSize(fpsValue.c_str()).x + windowPadding.x), sceneViewRect.GetTR().y + windowPadding.y));
					ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
					ImGui::PopFont();
				}

				{
					std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(gameplay::GAME.GetFPS().GetFPS()))) + " game fps";

					ImGui::PushFont(m_Window.GetCapitalFont());
					ImGui::SetCursorScreenPos(ImVec2(sceneViewRect.GetTR().x - (ImGui::CalcTextSize(fpsValue.c_str()).x + windowPadding.x), sceneViewRect.GetTR().y + (m_Window.GetFontSize() * 2) + windowPadding.y));
					ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
					ImGui::PopFont();
				}
			}

			//---------------------------------------------------------------------
			void FullSceneWindow::PopulateToolbar()
			{
				ImVec2 toolbarSize = ImVec2(0, m_Window.GetHeaderSize().y);
				m_Toolbar = Toolbar(toolbarSize);

				// Play button.
				m_Toolbar.m_aButtons.emplace_back(
					// TODO: This should probably be a global function.
					[this]()
					{
						bool isStarted = gameplay::GAME.IsStarted();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PLAY), BUTTON_ID, "PLAY_SCENE").c_str(), &isStarted, "Starts or stops the game simulation using the currently loaded scene. When stopping, the scene is reloaded to its original editor state.", ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)))
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
				);

				// Pause button.
				m_Toolbar.m_aButtons.emplace_back(
					[this]()
					{
						bool isPaused = gameplay::GAME.IsPaused();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_PAUSE), BUTTON_ID, "PAUSE_SCENE").c_str(), &isPaused, "Pauses or resumes the game simulation while preserving the current runtime state.", ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)))
						{
							gameplay::GAME.SetIsPaused(isPaused);
						}
					},
					// Disable if not in game mode.
					[]()
					{
						return !gameplay::GAME.IsStarted();
					}

				);

				// Full screen mode button.
				m_Toolbar.m_aButtons.emplace_back(

					[this]()
					{
						editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();

						bool inFullScreen = editorSettings.GetFullScreenPlayMode();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_GAMEMODE), BUTTON_ID, "FULL_SCREEN_PLAY_MODE_SCENE").c_str(), &inFullScreen, "Toggles fullscreen play mode, rendering the game view without editor UI overlays.", ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)))
						{
							editorSettings.SetFullScreenPlayMode(inFullScreen);
							editorSettings.Save();
						}
					}

				);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE
