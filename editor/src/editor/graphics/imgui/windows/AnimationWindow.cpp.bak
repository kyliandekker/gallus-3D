#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationWindow.h"

#include <imgui/imgui_helpers.h>
#include <imgui/ImSequencer.h>
#include <imgui/imgui_internal.h> // for ImClamp

#include "graphics/imgui/font_icon.h"

#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/EditorSelectable.h"

#include "animation/AnimationTrack.h"
#include "animation/SpriteAnimationKeyFrame.h"

#include "gameplay/Game.h"

//---------------------------------------------------------------------
namespace gallus
{
    namespace graphics
    {
        namespace imgui
        {
            constexpr float ANIMATION_FRAME_PIXEL_WIDTH = 25.0f;
            constexpr float LEGEND_PADDING = 25.0f;
            constexpr float LEGEND_HEIGHT = 70.0f;
            constexpr float TRACK_SIZE = 75;
            int m_iSelectedKeyFrame = 0;

            class SpriteAnimationKeyFrameUIView
            {
            public:
                SpriteAnimationKeyFrameUIView(animation::SpriteAnimationKeyFrame& a_KeyFrame) :
                    m_KeyFrame(a_KeyFrame)
                {

                }
                animation::SpriteAnimationKeyFrame& m_KeyFrame;
            };

            class AnimationTrackUIView
            {
            public:
                AnimationTrackUIView(animation::AnimationTrack<animation::SpriteAnimationKeyFrame>& a_AnimationTrack) : a_AnimationTrack(a_AnimationTrack)
                {
                    for (size_t i = 0; i < a_AnimationTrack.GetKeyFrames().size(); i++)
                    {
                        m_aKeyFrames.push_back(SpriteAnimationKeyFrameUIView(a_AnimationTrack.GetKeyFrames()[i]));
                    }
                }

                void RenderTrack()
                {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    ImVec2 startPos = ImGui::GetCursorScreenPos();

                    for (size_t i = 0; i < a_AnimationTrack.GetKeyFrames().size(); i++)
                    {
                        int frame = a_AnimationTrack.GetKeyFrames()[i].GetFrame();

                        float px = startPos.x + (frame * ANIMATION_FRAME_PIXEL_WIDTH);

                        std::string keyFrameIcon = font::ICON_KEYFRAME;
                        ImVec2 iconSize = ImGui::CalcTextSize(keyFrameIcon.c_str());

                        float verticalOffset = (TRACK_SIZE - iconSize.y) / 2.0f;

                        ImVec2 contentStartPos = ImVec2(px - (iconSize.x / 2), startPos.y + verticalOffset);
                        ImGui::SetCursorScreenPos(contentStartPos);
                        if (m_iSelectedKeyFrame == i)
                        {
                            ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent), keyFrameIcon.c_str());
                        }
                        else
                        {
                            ImGui::Text(keyFrameIcon.c_str());
                        }

                        if (ImGui::IsMouseHoveringRect(contentStartPos, ImVec2(
                            contentStartPos.x + iconSize.x,
                            contentStartPos.y + iconSize.y
                        )) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                        {
                            m_iSelectedKeyFrame = i;
                        }

                    }

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + TRACK_SIZE);
                }

                std::vector<SpriteAnimationKeyFrameUIView> m_aKeyFrames;
                animation::AnimationTrack<animation::SpriteAnimationKeyFrame>& a_AnimationTrack;
            };

            animation::AnimationTrack<animation::SpriteAnimationKeyFrame> m_SpriteAnimatorTrack;
            AnimationTrackUIView* m_AnimationTrackUIView = nullptr;

            AnimationWindow::AnimationWindow(ImGuiWindow& a_Window)
                : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse,
                    std::string(font::ICON_ANIMATION) + " Animation", "Animation")
            {
                m_SpriteAnimatorTrack.GetKeyFrames().push_back(animation::SpriteAnimationKeyFrame(12, 0));
                m_SpriteAnimatorTrack.GetKeyFrames().push_back(animation::SpriteAnimationKeyFrame(25, 1));
                m_AnimationTrackUIView = new AnimationTrackUIView(m_SpriteAnimatorTrack);
            }

            AnimationWindow::~AnimationWindow() {}

            void AnimationWindow::Update()
            {
                if (gameplay::GAME.IsStarted() && !gameplay::GAME.IsPaused())
                {
                    return;
                }

                BaseWindow::Update();
            }

            int m_iCurrentFrame = 0;
            void AnimationWindow::Render()
            {
                if (!core::EDITOR_ENGINE)
                {
                    return;
                }

                ImGui::SetCursorPos(
                    ImVec2(
                        ImGui::GetCursorPos().x + m_Window.GetFramePadding().x,
                        ImGui::GetCursorPos().y + m_Window.GetFramePadding().y
                    )
                );

                if (ImGui::BeginChild(
                    ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "BOX_ANIMATION").c_str(),
                    ImVec2(
                        ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
                        ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
                    ),
                    ImGuiChildFlags_Borders,
                    ImGuiWindowFlags_AlwaysHorizontalScrollbar
                ))
                {
                    int frameMax = 300;

                    ImVec2 initialPos = ImGui::GetCursorScreenPos();
                    ImVec2 legendRectPos = initialPos + ImVec2(200, 0);
                    ImVec2 legendRectSize = ImVec2((frameMax * ANIMATION_FRAME_PIXEL_WIDTH) + LEGEND_PADDING + LEGEND_PADDING + LEGEND_PADDING, LEGEND_HEIGHT + LEGEND_PADDING);
                    ImVec2 legendPos = legendRectPos + ImVec2(LEGEND_PADDING, LEGEND_PADDING);
                    ImVec2 legendSize = ImVec2(legendRectSize.x - LEGEND_PADDING, legendRectSize.y - LEGEND_PADDING);

                    ImVec2 legendRectMax = ImVec2(legendRectPos.x + legendRectSize.x, legendRectPos.y + legendRectSize.y);

                    ImDrawList* drawList = ImGui::GetWindowDrawList();

                    // --- Background ---
                    drawList->AddRectFilled(
                        legendRectPos,
                        legendRectMax,
                        0xFF3D3837,
                        0.0f
                    );

                    // --- Frame setup ---
                    int frameMin = 0;
                    float visibleWidth = legendSize.x;

                    // Modulo spacing for major ticks
                    int modFrameCount = 10;
                    int frameStep = 1;

                    int halfModFrameCount = modFrameCount / 2;

                    // --- Draw ticks ---
                    for (int i = frameMin; i <= frameMax; i += frameStep)
                    {
                        bool baseIndex = ((i % modFrameCount) == 0) || (i == frameMax || i == frameMin);
                        bool halfIndex = (i % halfModFrameCount) == 0;

                        float px = legendPos.x + ((i - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);
                        int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
                        int tiretEnd = (int)legendSize.y - 4;

                        drawList->AddLine(
                            ImVec2(px, legendPos.y + (float)tiretStart),
                            ImVec2(px, legendPos.y + (float)tiretEnd),
                            0xFF606060,
                            1.0f
                        );

                        if (baseIndex)
                        {
                            char tmp[32];
                            ImFormatString(tmp, IM_ARRAYSIZE(tmp), "%d", i);
                            ImVec2 textSize = ImGui::CalcTextSize(tmp);
                            drawList->AddText(
                                ImVec2(px - (textSize.x / 2), legendPos.y - textSize.y),
                                0xFFBBBBBB,
                                tmp
                            );
                        }
                    }

                    float selectedFramePos = legendPos.x + ((m_iCurrentFrame - frameMin) * ANIMATION_FRAME_PIXEL_WIDTH);

                    drawList->AddLine(
                        ImVec2(selectedFramePos, legendPos.y),
                        ImVec2(selectedFramePos, legendPos.y + (float)ImGui::GetContentRegionAvail().y),
                        0xA02A2AFF,
                        1.0f
                    );

                    ImGui::SetCursorScreenPos(legendPos);
                    ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, "INVISIBLE_BUTTON_ANIMATION").c_str(), legendSize);
                    if (ImGui::IsMouseHoveringRect(legendPos, ImVec2(
                        legendPos.x + legendSize.x,
                        legendPos.y + ImGui::GetContentRegionAvail().y
                    )) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    {
                        ImVec2 mousePos = ImGui::GetMousePos();
                        ImVec2 relativePos = mousePos - legendPos;

                        m_iCurrentFrame = std::round(relativePos.x / ANIMATION_FRAME_PIXEL_WIDTH);
                    }

                    ImGui::SetCursorScreenPos(initialPos + m_Window.GetFramePadding());
                    ImGui::SetNextItemWidth(100);
                    ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "CURRENT_FRAME_ANIMATION").c_str(), &m_iCurrentFrame, 1, 0, frameMax);

                    ImGui::SetCursorScreenPos(ImVec2(legendPos.x, legendRectMax.y));

                    m_AnimationTrackUIView->RenderTrack();
                }

                ImGui::EndChild();
            }
        }
    }
}

#endif // _EDITOR
#endif // IMGUI_DISABLE
