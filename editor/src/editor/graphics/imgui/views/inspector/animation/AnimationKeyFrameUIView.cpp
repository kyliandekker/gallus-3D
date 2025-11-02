//#include "AnimationKeyFrameUIView.h"
//
//namespace gallus
//{
//	namespace graphics
//	{
//		namespace imgui
//		{
//			AnimationKeyFrameUIView::~AnimationKeyFrameUIView()
//			{}
//
//			void AnimationKeyFrameUIView::SetKeyFrame(animation::AnimationKeyFrame & a_AnimationKeyFrame)
//			{
//				m_aAnimationKeyFrameUIViews.clear();
//
//				m_pAnimationKeyFrame = &a_AnimationKeyFrame;
//
//				for (auto& [type, factory] : GetKeyFrameComponentUIFactoryRegistry())
//				{
//					if (AnimationKeyFrameComponentBaseUIView* view = factory(m_Window, *m_pAnimationKeyFrame))
//					{
//						m_aAnimationKeyFrameUIViews.push_back(view);
//					}
//				}
//			}
//
//			void AnimationKeyFrameUIView::Render()
//			{
//				for (AnimationKeyFrameComponentBaseUIView* keyFrameComponentUIView : m_aAnimationKeyFrameUIViews)
//				{
//					keyFrameComponentUIView->Render();
//				}
//			}
//		}
//	}
//}