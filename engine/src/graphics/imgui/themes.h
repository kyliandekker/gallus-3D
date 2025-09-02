#pragma once

#include <imgui/imgui.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			void SetBessDark(ImGuiStyle& style, ImVec4* colors)
			{
				colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

				colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);  // #131318
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f); // #131318

				colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);

				colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
				colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);

				colors[ImGuiCol_Button] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.32f, 0.40f, 1.00f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.38f, 0.50f, 1.00f);

				colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);

				colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.50f, 1.00f);
				colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.38f, 1.00f);
				colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.17f, 1.00f);
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);

				colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

				colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.25f, 0.50f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

				colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.70f, 1.00f, 1.00f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.70f, 1.00f, 0.50f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.80f, 1.00f, 0.75f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.90f, 1.00f, 1.00f);

				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.55f, 1.00f);
			}

			void SetOwnDark(ImGuiStyle& style, ImVec4* colors)
			{
			}

			void SetSimplifiedDark(ImGuiStyle& style, ImVec4* colors)
			{
				// Base Colors
				ImVec4 bgColor = ImVec4(0.10f, 0.105f, 0.11f, 1.00f);
				ImVec4 lightBgColor = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
				ImVec4 panelColor = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
				ImVec4 panelHoverColor = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
				ImVec4 panelActiveColor = ImVec4(0.23f, 0.26f, 0.29f, 1.00f);
				ImVec4 textColor = ImVec4(0.86f, 0.87f, 0.88f, 1.00f);
				ImVec4 textDisabledColor = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
				ImVec4 borderColor = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);

				// Text
				colors[ImGuiCol_Text] = textColor;
				colors[ImGuiCol_TextDisabled] = textDisabledColor;

				// Windows
				colors[ImGuiCol_WindowBg] = bgColor;
				colors[ImGuiCol_ChildBg] = bgColor;
				colors[ImGuiCol_PopupBg] = bgColor;
				colors[ImGuiCol_Border] = borderColor;
				colors[ImGuiCol_BorderShadow] = borderColor;

				// Headers
				colors[ImGuiCol_Header] = panelColor;
				colors[ImGuiCol_HeaderHovered] = panelHoverColor;
				colors[ImGuiCol_HeaderActive] = panelActiveColor;

				// Buttons
				colors[ImGuiCol_Button] = panelColor;
				colors[ImGuiCol_ButtonHovered] = panelHoverColor;
				colors[ImGuiCol_ButtonActive] = panelActiveColor;

				// Frame BG
				colors[ImGuiCol_FrameBg] = lightBgColor;
				colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
				colors[ImGuiCol_FrameBgActive] = panelActiveColor;

				// Tabs
				colors[ImGuiCol_Tab] = panelColor;
				colors[ImGuiCol_TabHovered] = panelHoverColor;
				colors[ImGuiCol_TabActive] = panelActiveColor;
				colors[ImGuiCol_TabUnfocused] = panelColor;
				colors[ImGuiCol_TabUnfocusedActive] = panelHoverColor;

				// Title
				colors[ImGuiCol_TitleBg] = bgColor;
				colors[ImGuiCol_TitleBgActive] = bgColor;
				colors[ImGuiCol_TitleBgCollapsed] = bgColor;

				// Scrollbar
				colors[ImGuiCol_ScrollbarBg] = bgColor;
				colors[ImGuiCol_ScrollbarGrab] = panelColor;
				colors[ImGuiCol_ScrollbarGrabHovered] = panelHoverColor;
				colors[ImGuiCol_ScrollbarGrabActive] = panelActiveColor;

				// Checkmark
				colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

				// Slider
				colors[ImGuiCol_SliderGrab] = panelHoverColor;
				colors[ImGuiCol_SliderGrabActive] = panelActiveColor;

				// Resize Grip
				colors[ImGuiCol_ResizeGrip] = panelColor;
				colors[ImGuiCol_ResizeGripHovered] = panelHoverColor;
				colors[ImGuiCol_ResizeGripActive] = panelActiveColor;

				// Separator
				colors[ImGuiCol_Separator] = borderColor;
				colors[ImGuiCol_SeparatorHovered] = panelHoverColor;
				colors[ImGuiCol_SeparatorActive] = panelActiveColor;

				// Plot
				colors[ImGuiCol_PlotLines] = textColor;
				colors[ImGuiCol_PlotLinesHovered] = panelActiveColor;
				colors[ImGuiCol_PlotHistogram] = textColor;
				colors[ImGuiCol_PlotHistogramHovered] = panelActiveColor;

				// Text Selected BG
				colors[ImGuiCol_TextSelectedBg] = panelActiveColor;

				// Modal Window Dim Bg
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.105f, 0.11f, 0.5f);

				// Tables
				colors[ImGuiCol_TableHeaderBg] = panelColor;
				colors[ImGuiCol_TableBorderStrong] = borderColor;
				colors[ImGuiCol_TableBorderLight] = borderColor;
				colors[ImGuiCol_TableRowBg] = bgColor;
				colors[ImGuiCol_TableRowBgAlt] = lightBgColor;
			}

			void SetFuckingNerd(ImGuiStyle& style, ImVec4* colors)
			{
				colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
				colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
				colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
				colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
				colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
				colors[ImGuiCol_Border] = ImVec4(0.43f, 0.50f, 0.56f, 0.50f);
				colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
				colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
				colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
				colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
				colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
				colors[ImGuiCol_ButtonActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
				colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
				colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.50f, 0.56f, 0.50f);
				colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
				colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
				colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
				colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.56f, 1.00f, 0.80f);
				colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
				colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.22f, 0.36f, 1.00f);
				colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
				colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
				colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
			}

			namespace
			{ // Unnamed namespace, since we only use this here. 
				unsigned int Color(unsigned int c)
				{
					// add alpha.
					// also swap red and blue channel for some reason.
					// todo: figure out why, and fix it.
					const short a = 0xFF;
					const short r = (c >> 16) & 0xFF;
					const short g = (c >> 8) & 0xFF;
					const short b = (c >> 0) & 0xFF;
					return(a << 24)
						| (r << 0)
						| (g << 8)
						| (b << 16);
				}
			}

			void SetLabCoat(ImGuiStyle& style, ImVec4* colors)
			{
				const unsigned int GRAY50 = Color(0xFFFFFF);
				const unsigned int GRAY75 = Color(0xFAFAFA);
				const unsigned int GRAY100 = Color(0xF5F5F5);
				const unsigned int GRAY200 = Color(0xEAEAEA);
				const unsigned int GRAY300 = Color(0xE1E1E1);
				const unsigned int GRAY400 = Color(0xCACACA);
				const unsigned int GRAY500 = Color(0xB3B3B3);
				const unsigned int GRAY600 = Color(0x8E8E8E);
				const unsigned int GRAY700 = Color(0x707070);
				const unsigned int GRAY800 = Color(0x4B4B4B);
				const unsigned int GRAY900 = Color(0x2C2C2C);
				const unsigned int BLUE400 = Color(0x2680EB);
				const unsigned int BLUE500 = Color(0x1473E6);
				const unsigned int BLUE600 = Color(0x0D66D0);
				const unsigned int BLUE700 = Color(0x095ABA);
				const unsigned int RED400 = Color(0xE34850);
				const unsigned int RED500 = Color(0xD7373F);
				const unsigned int RED600 = Color(0xC9252D);
				const unsigned int RED700 = Color(0xBB121A);
				const unsigned int ORANGE400 = Color(0xE68619);
				const unsigned int ORANGE500 = Color(0xDA7B11);
				const unsigned int ORANGE600 = Color(0xCB6F10);
				const unsigned int ORANGE700 = Color(0xBD640D);
				const unsigned int GREEN400 = Color(0x2D9D78);
				const unsigned int GREEN500 = Color(0x268E6C);
				const unsigned int GREEN600 = Color(0x12805C);
				const unsigned int GREEN700 = Color(0x107154);
				const unsigned int INDIGO400 = Color(0x6767EC);
				const unsigned int INDIGO500 = Color(0x5C5CE0);
				const unsigned int INDIGO600 = Color(0x5151D3);
				const unsigned int INDIGO700 = Color(0x4646C6);
				const unsigned int CELERY400 = Color(0x44B556);
				const unsigned int CELERY500 = Color(0x3DA74E);
				const unsigned int CELERY600 = Color(0x379947);
				const unsigned int CELERY700 = Color(0x318B40);
				const unsigned int MAGENTA400 = Color(0xD83790);
				const unsigned int MAGENTA500 = Color(0xCE2783);
				const unsigned int MAGENTA600 = Color(0xBC1C74);
				const unsigned int MAGENTA700 = Color(0xAE0E66);
				const unsigned int YELLOW400 = Color(0xDFBF00);
				const unsigned int YELLOW500 = Color(0xD2B200);
				const unsigned int YELLOW600 = Color(0xC4A600);
				const unsigned int YELLOW700 = Color(0xB79900);
				const unsigned int FUCHSIA400 = Color(0xC038CC);
				const unsigned int FUCHSIA500 = Color(0xB130BD);
				const unsigned int FUCHSIA600 = Color(0xA228AD);
				const unsigned int FUCHSIA700 = Color(0x93219E);
				const unsigned int SEAFOAM400 = Color(0x1B959A);
				const unsigned int SEAFOAM500 = Color(0x16878C);
				const unsigned int SEAFOAM600 = Color(0x0F797D);
				const unsigned int SEAFOAM700 = Color(0x096C6F);
				const unsigned int CHARTREUSE400 = Color(0x85D044);
				const unsigned int CHARTREUSE500 = Color(0x7CC33F);
				const unsigned int CHARTREUSE600 = Color(0x73B53A);
				const unsigned int CHARTREUSE700 = Color(0x6AA834);
				const unsigned int PURPLE400 = Color(0x9256D9);
				const unsigned int PURPLE500 = Color(0x864CCC);
				const unsigned int PURPLE600 = Color(0x7A42BF);
				const unsigned int PURPLE700 = Color(0x6F38B1);

				colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(GRAY800); // text on hovered controls is gray900
				colors[ImGuiCol_TextDisabled] = ImGui::ColorConvertU32ToFloat4(GRAY500);
				colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(GRAY100);
				colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
				colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(GRAY50); // not sure about this. Note: applies to tooltips too.
				colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(GRAY300);
				colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0); // We don't want shadows. Ever.
				colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(GRAY75); // this isnt right, spectrum does not do this, but it's a good fallback
				colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(GRAY50);
				colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(GRAY200);
				colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(GRAY300); // those titlebar values are totally made up, spectrum does not have this.
				colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(GRAY200);
				colors[ImGuiCol_TitleBgCollapsed] = ImGui::ColorConvertU32ToFloat4(GRAY400);
				colors[ImGuiCol_MenuBarBg] = ImGui::ColorConvertU32ToFloat4(GRAY100);
				colors[ImGuiCol_ScrollbarBg] = ImGui::ColorConvertU32ToFloat4(GRAY100); // same as regular background
				colors[ImGuiCol_ScrollbarGrab] = ImGui::ColorConvertU32ToFloat4(GRAY400);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::ColorConvertU32ToFloat4(GRAY600);
				colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColorConvertU32ToFloat4(GRAY700);
				colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(BLUE500);
				colors[ImGuiCol_SliderGrab] = ImGui::ColorConvertU32ToFloat4(GRAY700);
				colors[ImGuiCol_SliderGrabActive] = ImGui::ColorConvertU32ToFloat4(GRAY800);
				colors[ImGuiCol_Button] = ImGui::ColorConvertU32ToFloat4(GRAY75); // match default button to Spectrum's 'Action Button'.
				colors[ImGuiCol_ButtonHovered] = ImGui::ColorConvertU32ToFloat4(GRAY50);
				colors[ImGuiCol_ButtonActive] = ImGui::ColorConvertU32ToFloat4(GRAY200);
				colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(BLUE400);
				colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(BLUE500);
				colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(BLUE600);
				colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(GRAY400);
				colors[ImGuiCol_SeparatorHovered] = ImGui::ColorConvertU32ToFloat4(GRAY600);
				colors[ImGuiCol_SeparatorActive] = ImGui::ColorConvertU32ToFloat4(GRAY700);
				colors[ImGuiCol_ResizeGrip] = ImGui::ColorConvertU32ToFloat4(GRAY400);
				colors[ImGuiCol_ResizeGripHovered] = ImGui::ColorConvertU32ToFloat4(GRAY600);
				colors[ImGuiCol_ResizeGripActive] = ImGui::ColorConvertU32ToFloat4(GRAY700);
				colors[ImGuiCol_PlotLines] = ImGui::ColorConvertU32ToFloat4(BLUE400);
				colors[ImGuiCol_PlotLinesHovered] = ImGui::ColorConvertU32ToFloat4(BLUE600);
				colors[ImGuiCol_PlotHistogram] = ImGui::ColorConvertU32ToFloat4(BLUE400);
				colors[ImGuiCol_PlotHistogramHovered] = ImGui::ColorConvertU32ToFloat4(BLUE600);
				colors[ImGuiCol_TextSelectedBg] = ImGui::ColorConvertU32ToFloat4((BLUE400 & 0x00FFFFFF) | 0x33000000);
				colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
				colors[ImGuiCol_NavHighlight] = ImGui::ColorConvertU32ToFloat4((GRAY900 & 0x00FFFFFF) | 0x0A000000);
				colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
				colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
				colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
			}
		}
	}
}