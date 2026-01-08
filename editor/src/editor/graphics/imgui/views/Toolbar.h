#pragma once

#include <functional>
#include <vector>

#include <imgui/imgui.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ToolbarItem
			{
			public:
				virtual void Render() = 0;
			};

			//---------------------------------------------------------------------
			// ToolbarButton
			//---------------------------------------------------------------------
			class ToolbarButton : public ToolbarItem
			{
			public:
				ToolbarButton(std::function<void()> a_ButtonFunc, std::function<bool()> a_DisabledCondition = []()
					{
						return false;
					});

				void Render() override;
			private:
				std::function<void()> m_ButtonFunc;
				std::function<bool()> m_DisabledCondition;
			};

			//---------------------------------------------------------------------
			// ToolbarBreak
			//---------------------------------------------------------------------
			class ToolbarBreak : public ToolbarItem
			{
			public:
				ToolbarBreak(const ImVec2& a_vBreakSize) : m_vBreakSize(a_vBreakSize)
				{}

				void Render() override;
			private:
				ImVec2 m_vBreakSize = {};
			};

			//---------------------------------------------------------------------
			// Toolbar
			//---------------------------------------------------------------------
			class Toolbar
			{
			public:
				Toolbar() = default;
				~Toolbar()
				{
					for (size_t i = 0; i < m_aToolbarItems.size(); i++)
					{
						delete m_aToolbarItems[i];
					}
				}
				Toolbar(const ImVec2& a_vSize) : m_vToolbarSize(a_vSize)
				{}

				void StartToolbar();
				void EndToolbar();

				void Render();

				const ImVec2& GetToolbarSize() const
				{
					return m_vToolbarSize;
				}

				std::vector<ToolbarItem*> m_aToolbarItems = {};
			private:
				ImVec2 m_vToolbarStartScreenPos = {};
				ImVec2 m_vToolbarStartPos = {};
				ImVec2 m_vToolbarSize = {};
			};
		}
	}
}