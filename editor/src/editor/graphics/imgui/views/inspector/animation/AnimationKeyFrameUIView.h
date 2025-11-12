//#pragma once
//
//#include "graphics/imgui/views/ImGuiUIView.h"
//
//#include <vector>
//
//#include "editor/graphics/imgui/views/inspector/animation/AnimationKeyFrameComponentBaseUIView.h"
//
//namespace gallus
//{
//	namespace animation
//	{
//		class AnimationKeyFrame;
//	}
//	namespace graphics
//	{
//		namespace imgui
//		{
//			class AnimationKeyFrameUIView : public ImGuiUIView
//			{
//			public:
//				AnimationKeyFrameUIView(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
//				{}
//
//				~AnimationKeyFrameUIView();
//
//				void SetKeyFrame(animation::AnimationKeyFrame& a_AnimationKeyFrame);
//
//				void Render() override;
//			protected:
//				animation::AnimationKeyFrame* m_pAnimationKeyFrame;
//				std::vector<AnimationKeyFrameComponentBaseUIView*> m_aAnimationKeyFrameUIViews;
//			};
//		}
//	}
//}