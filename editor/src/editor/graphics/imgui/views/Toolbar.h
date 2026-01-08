#pragma once

#include <functional>
#include <vector>
#include <string>

#include <imgui/imgui.h>

#include "core/Data.h"

#include "graphics/imgui/views/DataTypes/StringTextInput.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ToolbarItem : public ImGuiUIView
			{
			public:
				ToolbarItem(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{}

				virtual void Render() = 0;
			};

			//---------------------------------------------------------------------
			// ToolbarButton
			//---------------------------------------------------------------------
			class ToolbarButton : public ToolbarItem
			{
			public:
				ToolbarButton(ImGuiWindow& a_Window, std::function<void()> a_ButtonFunc, std::function<bool()> a_DisabledCondition = []()
					{
						return false;
					});

				void Render() override;
			private:
				std::function<void()> m_ButtonFunc;
				std::function<bool()> m_DisabledCondition;
			};
			
			//---------------------------------------------------------------------
			// ToolbarSearchbar
			//---------------------------------------------------------------------
			class ToolbarSearchbar : public ToolbarItem
			{
			public:
				ToolbarSearchbar(ImGuiWindow& a_Window, const std::string& a_sID, float a_fWidth, std::function<void(const std::string&)> a_ButtonFunc);

				void Render() override;
			private:
				std::function<void(const std::string&)> m_ButtonFunc;
				std::string m_sID;
				float m_fWidth = 0;

				core::Data m_Data;
			};

			//---------------------------------------------------------------------
			// ToolbarBreak
			//---------------------------------------------------------------------
			class ToolbarBreak : public ToolbarItem
			{
			public:
				ToolbarBreak(ImGuiWindow& a_Window, const ImVec2& a_vBreakSize) : ToolbarItem(a_Window), m_vBreakSize(a_vBreakSize)
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
				Toolbar(const std::string& a_sID, const ImVec2& a_vSize) : m_sID(a_sID), m_vToolbarSize(a_vSize)
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
				std::string m_sID;

				ImVec2 m_vToolbarStartScreenPos = {};
				ImVec2 m_vToolbarStartPos = {};
				ImVec2 m_vToolbarSize = {};

				// temp
				ImVec2 m_vToolbarSizeFinal = {};
			};
		}
	}
}