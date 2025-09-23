#pragma once

#ifdef _EDITOR

#include "graphics/imgui/views/ImGuiUIView.h"

#include <cstdint>
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// Vector2View
			//---------------------------------------------------------------------
			/// <summary>
			/// A UI view for rendering and interacting with vectors.
			/// Can be used for both glm-based types and ImVec types.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec2 or ImVec.</typeparam>
			template <class T>
			class Vector2View : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs a Vector2View that renders a vector.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				Vector2View(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{}

				/// <summary>
				/// Sets the value of the 2D vector. Updates the value if it's different from the current one.
				/// </summary>
				/// <param name="a_Value">The new vector value to set.</param>
				void SetValue(const T& a_Value)
				{
					m_Value = a_Value;
				}

				/// <summary>
				/// Retrieves the current vector value.
				/// </summary>
				/// <returns>The current vector value.</returns>
				const T& GetValue() const
				{
					return m_Value;
				}

				/// <summary>
				/// Empty render method (implemented because base class is pure).
				/// </summary>
				void Render() override
				{}

				/// <summary>
				/// Renders the 2D vector as draggable float inputs in ImGui (X and Y components).
				/// </summary>
				/// <param name="a_sLabel">The label displayed next to the input fields.</param>
				virtual uint8_t Render(const char* a_sLabel, float a_fSpeed = 1.0f, float a_fMin = -999999999999999, float a_fMax = 9999999999999999)
				{
					ImGui::AlignTextToFramePadding();
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					bool changedValueX = ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, std::string("X_") + a_sLabel).c_str(), &m_Value.x, a_fSpeed, a_fMin, a_fMax);
					
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					bool changedValueY = ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, std::string("Y_") + a_sLabel).c_str(), &m_Value.y, a_fSpeed, a_fMin, a_fMax);

					return changedValueX ? 1 : (changedValueY ? 2 : 0);
				}
			protected:
				T m_Value; /// The current value of the vector.
			};

			//---------------------------------------------------------------------
			// IVector2View
			//---------------------------------------------------------------------
			/// <summary>
			/// A UI view for rendering and interacting with vectors.
			/// Can be used for both glm-based types and ImVec types.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec2 or ImVec.</typeparam>
			template <class T>
			class IVector2View : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs a IVector2View that renders a vector.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				IVector2View(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{}

				/// <summary>
				/// Sets the value of the 2D vector. Updates the value if it's different from the current one.
				/// </summary>
				/// <param name="a_Value">The new vector value to set.</param>
				void SetValue(const T& a_Value)
				{
					m_Value = a_Value;
				}

				/// <summary>
				/// Retrieves the current vector value.
				/// </summary>
				/// <returns>The current vector value.</returns>
				const T& GetValue() const
				{
					return m_Value;
				}

				/// <summary>
				/// Empty render method (implemented because base class is pure).
				/// </summary>
				void Render() override
				{}

				/// <summary>
				/// Renders the 2D vector as draggable float inputs in ImGui (X and Y components).
				/// </summary>
				/// <param name="a_Label">The label displayed next to the input fields.</param>
				virtual uint8_t Render(const char* a_Label)
				{
					ImGui::AlignTextToFramePadding();
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					bool changedValueX = ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, std::string("X_") + a_Label).c_str(), &m_Value.x, 1.0f);
			
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					bool changedValueY = ImGui::DragInt(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, std::string("Y_") + a_Label).c_str(), &m_Value.y, 1.0f);

					return changedValueX ? 1 : (changedValueY ? 2 : 0);
				}
			protected:
				T m_Value; /// The current value of the vector.
			};

			//---------------------------------------------------------------------
			// Vector3View
			//---------------------------------------------------------------------
			/// <summary>
			/// A UI view for rendering and interacting with vectors.
			/// Can be used for both glm-based types and ImVec types.
			/// </summary>
			/// <typeparam name="T">The type of the color, such as glm::vec3 or ImVec.</typeparam>
			template <class T>
			class Vector3View : public Vector2View<T>
			{
			public:
				/// <summary>
				/// Constructor for Vector3View.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				Vector3View(ImGuiWindow& a_Window) : Vector2View<T>(a_Window)
				{}

				/// <summary>
				/// Empty render method (implemented because base class is pure).
				/// </summary>
				void Render() override
				{}

				/// <summary>
				/// Renders the 3D vector as draggable float inputs in ImGui (X, Y and Z components).
				/// </summary>
				/// <param name="a_Label">The label displayed next to the input fields.</param>
				uint8_t Render(const char* a_Label) override
				{
					uint8_t changedValueXY = Vector2View<T>::Render(a_Label);
					ImGui::SameLine();
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Z");
					ImGui::SameLine();
					bool changedValueZ = ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, std::string("Z_") + a_Label).c_str(), &this->m_Value.z, 0.1f);

					return changedValueZ ? 3 : changedValueXY;
				}
			};
		}
	}
}

#endif // _EDITOR