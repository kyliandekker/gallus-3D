#ifndef IMGUI_DISABLE

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			/// <summary>
			/// A UI view for rendering and interacting with RGB color values.
			/// Can be used for both glm-based types and ImColor types.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec3 or ImColor.</typeparam>
			template <class T>
			class ColorRGBView : public ImGuiUIView
			{
			public:
				ColorRGBView(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{}

				void SetValue(const T& a_Value)
				{
					m_Value = a_Value;
				}

				const T& GetValue() const
				{
					return m_Value;
				}

				void Render() override
				{}

				virtual bool Render(const char* a_Label)
				{
					return ImGui::ColorEdit3(
						ImGui::IMGUI_FORMAT_ID("", COLOR_WHEEL_ID, std::string(a_Label) + "COLOR_WHEEL").c_str(),
						reinterpret_cast<float*>(&this->m_Value)
					);
				}

			protected:
				T m_Value;
			};

			/// <summary>
			/// A UI view for rendering and interacting with RGB color values.
			/// Can be used for both glm-based types and ImColor types.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec3 or ImColor.</typeparam>
			template <class T>
			class ColorRGBAView : public ColorRGBView<T>
			{
			public:
				ColorRGBAView(ImGuiWindow& a_Window) : ColorRGBView<T>(a_Window)
				{}

				bool Render(const char* a_Label) override
				{
					return ImGui::ColorEdit4(
						ImGui::IMGUI_FORMAT_ID("", COLOR_WHEEL_ID, std::string(a_Label) + "COLOR_WHEEL").c_str(),
						reinterpret_cast<float*>(&this->m_Value)
					);
				}
			};
		}
	}
}

#endif // IMGUI_DISABLE
