#include "AnimationWindow.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>
#include <imgui/imgui_internal.h>

// graphics
#include "imgui_system/font_icon.h"
#include "imgui_system/ImGuiSystem.h"

// animation
#include "animation/AnimationKeyFrameTextureComponent.h"
#include "animation/AnimationKeyFrameEventComponent.h"
#include "animation/AnimationKeyFrame.h"

// resources
#include "resources/SrcData.h"
#include "resources/ResourceAtlas.h"

// utils
#include "utils/string_extensions.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/Editor.h"
#include "editor/EditorWorkspace.h"
#include "editor/graphics/imgui/views/selectables/AnimationKeyFrameEditorSelectable.h"

// editor/file
#include "editor/file/IFileAssetSource.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// AnimationWindow
			//---------------------------------------------------------------------
			AnimationWindow::AnimationWindow(ImGuiSystem& a_System) : BaseWindow(a_System, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_ANIMATION) + " Animation", "Animation")
			{}

			//---------------------------------------------------------------------
			AnimationWindow::~AnimationWindow()
			{
			}

			//---------------------------------------------------------------------
			bool AnimationWindow::Initialize()
			{
				PopulateToolbar();

				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::PopulateToolbar()
			{
				ImVec2 toolbarSize = ImVec2(0, m_System.GetHeaderSize().y);
				m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_ANIMATION_EDITOR"), toolbarSize);

				// Save button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
					[this]()
					{
						if (ImGui::TextButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "TOOLBAR_ANIMATION_EDITOR_SAVE").c_str(), "Saves the current animation track.", m_System.GetHeaderSize(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
						{
							if (m_Animation.GetName().empty())
							{
								return;
							}

							editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
							if (!editorWorkspace)
							{
								return;
							}

							resources::SrcData srcData;
							srcData.SetObject();

							SerializeFields(&m_Animation, srcData, SerializationMethod::SerializationMethod_File);

							core::Data data;
							srcData.GetData(data);

							editorWorkspace->Save(m_Animation.GetName(), data, false);
						}
					},
						[this]()
					{
						editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
						if (!editorWorkspace)
						{
							return false;
						}

						return !editorWorkspace->IsDirty(m_Animation.GetName());
					}
				));

				// Add keyframe button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
					[this]()
					{
						if (ImGui::IconButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_ADD_KEYFRAME), BUTTON_ID, "TOOLBAR_ANIMATION_EDITOR_ADD_KEYFRAME").c_str(), "Adds a new keyframe at the currently selected frame.", m_System.GetHeaderSize()))
						{
							AddKeyFrame(m_iSelectedKeyFrame);
						}
					}
				));

				// Remove keyframe button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
					[this]()
					{
						if (ImGui::IconButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REMOVE_KEYFRAME), BUTTON_ID, "TOOLBAR_ANIMATION_EDITOR_REMOVE_KEYFRAME").c_str(), "Removes the currently selected keyframe from the track.", m_System.GetHeaderSize()))
						{
							RemoveKeyFrame(m_iSelectedKeyFrame);
						}
					},
					[this]()
					{
						return m_iSelectedKeyFrame == -1;
					}
				));

				// Looping button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
					[this]()
					{
						bool isLooping = m_Animation.IsLooping();
						if (ImGui::CheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REFRESH), BUTTON_ID, "TOOLBAR_ANIMATION_EDITOR_IS_LOOPING").c_str(), &isLooping, "Toggles looping for the animation track.", m_System.GetHeaderSize()))
						{
							m_Animation.SetIsLooping(isLooping);

							SetAnimationDirty();
						}
					}
				));

				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarBreak(m_System, ImVec2(m_Toolbar.GetToolbarSize().y, m_Toolbar.GetToolbarSize().y)));

				// Undo.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
					[this]()
					{
						editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
						if (!editorWorkspace)
						{
							return;
						}

						if (ImGui::TextButton(
							ImGui::IMGUI_FORMAT_ID(font::ICON_UNDO, BUTTON_ID, "TOOLBAR_ANIMATION_EDITOR_UNDO").c_str(), "Reverts the last action.", m_System.GetHeaderSize()))
						{
							editorWorkspace->Undo(editor::EditorActionStack_Animation);
						}
					},
						[]()
					{
						editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
						if (!editorWorkspace)
						{
							return true;
						}

						return !editorWorkspace->CanUndo(editor::EditorActionStack_Animation);
					}
				));

				// Redo.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
					[this]()
					{
						editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
						if (!editorWorkspace)
						{
							return;
						}

						if (ImGui::TextButton(
							ImGui::IMGUI_FORMAT_ID(font::ICON_REDO, BUTTON_ID, "TOOLBAR_ANIMATION_EDITOR_REDO").c_str(), "Reapplies the most recently undone action.", m_System.GetHeaderSize()))
						{
							editorWorkspace->Redo(editor::EditorActionStack_Animation);
						}
					},
					[]()
					{
						editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
						if (!editorWorkspace)
						{
							return true;
						}

						return !editorWorkspace->CanRedo(editor::EditorActionStack_Animation);
					}
				));
			}

			//---------------------------------------------------------------------
			void AnimationWindow::DrawToolbar()
			{
				// Start toolbar.
				m_Toolbar.StartToolbar();

				// Render toolbar.
				m_Toolbar.Render();

				// End toolbar.
				m_Toolbar.EndToolbar();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::RemoveKeyFrame(size_t a_iIndex)
			{
				m_Animation.RemoveKeyFrame(m_iSelectedKeyFrame);
				SetCurrentFrame(-1);

				SetAnimationDirty();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::AddKeyFrame(size_t a_iIndex)
			{
				m_Animation.AddKeyFrame(m_iSelectedFrame);

				SetAnimationDirty();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::SetAnimationDirty()
			{
				editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
				if (!editorWorkspace)
				{
					return;
				}

				editorWorkspace->MarkDirty(m_Animation.GetName());
			}

			//---------------------------------------------------------------------
			void AnimationWindow::Update()
			{
				editor::Editor* editor = GetEditorEngine().GetEditor();
				if (!editor)
				{
					return;
				}

				if (GetEditorEngine().GetEditor()->GetEditorSettings().GetEditorState() != editor::EditorState::EditorState_Workspace)
				{
					return;
				}
				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::Render()
			{
				DrawToolbar();

				ImGui::SetCursorPos({
					ImGui::GetCursorPos().x + m_System.GetFramePadding().x,
					ImGui::GetCursorPos().y + m_System.GetFramePadding().y,
				});

				int frameCount = m_Animation.GetFrameCount() == 0 ? 100 : m_Animation.GetFrameCount();
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "MAIN_ANIMATION").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_System.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_System.GetFramePadding().y
					),
					ImGuiChildFlags_Borders,
					ImGuiWindowFlags_AlwaysHorizontalScrollbar
				))
				{
					ImVec2 legendPadding = m_System.GetFramePadding();
					ImVec2 initialPos = ImGui::GetCursorScreenPos();
					
					ImRect legendRect = {
						initialPos,
						initialPos + ImVec2((frameCount * ANIMATION_FRAME_PIXEL_WIDTH) + (legendPadding.x * 2), LEGEND_HEIGHT)
					};

					ImRect legendBackground = legendRect;
					if (legendBackground.GetSize().x < ImGui::GetContentRegionAvail().x)
					{
						legendBackground.Max.x = legendBackground.Min.x + ImGui::GetContentRegionAvail().x;
					}

					ImDrawList* drawList = ImGui::GetWindowDrawList();

					ImVec2 timeLineStart = {
						legendRect.Min.x + legendPadding.x,
						legendRect.Min.y
					};

					// --- Frame setup ---
					int frameMin = 0;
					float fullWidth = legendRect.GetSize().x;

					// Modulo spacing for major ticks
					int modFrameCount = 5;
					int frameStep = 1;

					// --- Legend with ticks ---
					if (ImGui::BeginChild(
						ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "LEGEND_TICKS_ANIMATION").c_str(),
						legendRect.GetSize()
					))
					{
						// --- Background ---
						drawList->AddRectFilled(
							legendBackground.Min,
							legendBackground.Max,
							0xFF3D3837,
							0.0f
						);

						// --- Legend ticks ---
						{
							int halfModFrameCount = modFrameCount / 2;

							float majorLineHeight = LEGEND_HEIGHT - (m_System.GetFontSize() + legendPadding.y);
							float halfLineHeight = majorLineHeight / 2;
							float normalLineHeight = halfLineHeight / 2;
							int lineEnd = legendRect.Max.y - 4;

							for (int i = frameMin; i <= frameCount; i += frameStep)
							{
								bool baseIndex = ((i % modFrameCount) == 0) || (i == frameCount || i == frameMin);
								bool halfIndex = (i % halfModFrameCount) == 0;

								float px = timeLineStart.x + ((i - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);

								int lineStart = lineEnd;
								lineStart -= baseIndex ? majorLineHeight : (halfIndex ? halfLineHeight : normalLineHeight);
								ImRect lineRect = {
									ImVec2(px, lineEnd),
									ImVec2(px, lineStart),
								};

								drawList->AddLine(
									ImVec2(px, lineStart),
									ImVec2(px, lineEnd),
									0xFF606060,
									1.0f
								);

								if (baseIndex)
								{
									char tmp[32];
									ImFormatString(tmp, IM_ARRAYSIZE(tmp), "%d", i);
									ImVec2 textSize = ImGui::CalcTextSize(tmp);

									ImVec2 textPos = {
										px - (textSize.x / 2),
										lineStart - textSize.y
									};

									drawList->AddText(
										textPos,
										0xFFBBBBBB,
										tmp
									);
								}
							}
						}
					}
					ImGui::EndChild();

					ImRect keyFrameTimelineRect = {
						{ legendRect.Min.x, ImGui::GetCursorScreenPos().y },
						{ legendRect.Max.x, legendRect.Max.y + ImGui::GetContentRegionAvail().y }
					};

					static bool s_bDragging = false;
					static animation::AnimationKeyFrame* s_pDraggedKeyFrame = nullptr;
					static int s_iDraggedKeyFrameIndex = -1;
					bool bMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);

					// --- Key Frame timeline ---
					if (ImGui::BeginChild(
						ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "KEY_FRAME_TIMELINE_ANIMATION").c_str(),
						keyFrameTimelineRect.GetSize()
					))
					{
						float keyFrameY = keyFrameTimelineRect.Min.y - ImGui::GetScrollY();

						std::string keyFrameIcon = font::ICON_KEYFRAME;

						int i = 0;
						for (animation::AnimationKeyFrame* keyFrame : m_Animation.GetKeyFrames())
						{
							ImVec2 iconSize = ImGui::CalcTextSize(keyFrameIcon.c_str());
							float px = timeLineStart.x + (keyFrame->GetFrame() * ANIMATION_FRAME_PIXEL_WIDTH);
							px -= iconSize.x / 2;

							ImRect textRect = {
								{ px, keyFrameY },
								{ px + iconSize.x, keyFrameY + iconSize.y },
							};

							bool isSelected = m_iSelectedKeyFrame == i;
							ImU32 col = isSelected ? ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)) : 0xFFBBBBBB;

							drawList->AddText(
								textRect.Min,
								col,
								keyFrameIcon.c_str()
							);

							bool bHovered = ImGui::IsMouseHoveringRect(textRect.Min, textRect.Max, false);
							bool bMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
							bool bMouseDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

							// Start drag
							if (bHovered && bMouseClicked)
							{
								SetCurrentFrame(i);

								s_bDragging = true;
								s_pDraggedKeyFrame = keyFrame;
								s_iDraggedKeyFrameIndex = i;
							}

							// End drag
							if (!bMouseDown && s_bDragging && s_pDraggedKeyFrame == keyFrame)
							{
								s_bDragging = false;
								s_pDraggedKeyFrame = nullptr;
								s_iDraggedKeyFrameIndex = -1;
							}

							i++;
						}
					}
					ImGui::EndChild();
					
					// --- Tracker ---
					float selectedFramePos = timeLineStart.x + ((m_iSelectedFrame - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);

					drawList->AddLine(
						ImVec2(selectedFramePos, legendRect.Min.y),
						ImVec2(selectedFramePos, keyFrameTimelineRect.Max.y),
						0xA02A2AFF,
						1.0f
					);

					// Get the scroll offset of the child window that contains the timeline
					float scrollX = ImGui::GetScrollX();

					if (ImGui::IsMouseHoveringRect(legendRect.Min, keyFrameTimelineRect.Max) && bMouseDown)
					{
						ImVec2 mousePos = ImGui::GetMousePos();

						// Get the scroll offset of the child window that contains the timeline
						ImVec2 scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

						// Compute relative position in timeline space
						ImVec2 relativePos = mousePos - legendRect.Min + scroll;

						int index = std::floor(relativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
						index = std::clamp(index, 0, frameCount);

						m_iSelectedFrame = index;

						if (s_bDragging && s_pDraggedKeyFrame)
						{
							if (m_Animation.GetKeyFrameAtFrame(m_iSelectedFrame) == -1)
							{
								s_pDraggedKeyFrame->SetFrame(m_iSelectedFrame);
								SetAnimationDirty();
							}
						}
					}
				}

				ImGui::EndChild();
			}

			//---------------------------------------------------------------------
			void AnimationWindow::SetData(const std::string a_sID)
			{
				std::shared_ptr<resources::IAssetSource> assetSrc = GetResourceAtlas().GetAssetSource().lock();
				if (!assetSrc)
				{
					return;
				}

				core::Data data;
				assetSrc->LoadBinary(a_sID, data);

				m_Animation.LoadData(data);
				m_Animation.SetName(a_sID);

				m_iSelectedFrame = 0;
				m_iSelectedKeyFrame = -1;
			}

			//---------------------------------------------------------------------
			void AnimationWindow::SetCurrentFrame(int16_t a_iIndex)
			{
				editor::Editor* editor = GetEditorEngine().GetEditor();
				if (!editor)
				{
					return;
				}

				int index = std::clamp(a_iIndex, static_cast<int16_t>(-1), static_cast<int16_t>(m_Animation.GetFrameCount()));
				if (index == -1)
				{
					editor->ResetSelectable();
				}
				else
				{
					m_pKeyFrameSelectable = std::make_shared<AnimationKeyFrameEditorSelectable>(m_System, *m_Animation.GetKeyFrames()[a_iIndex], m_Animation);
					editor->SetSelectable(m_pKeyFrameSelectable);
				}

				m_iSelectedKeyFrame = a_iIndex;
			}
		}
	}
}