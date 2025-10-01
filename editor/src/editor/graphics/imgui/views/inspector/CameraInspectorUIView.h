#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "editor/graphics/imgui/views/inspector/InspectorUIView.h"

#include <string>
#include <vector>

// graphics includes
#include "graphics/imgui/views/DataTypes/VectorView.h"
#include "graphics/dx12/DX12PCH.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class HierarchyUIView;

			/// <summary>
			/// Class that displays entities in the inspector.
			/// </summary>
			class CameraInspectorUIView : public InspectorUIView
			{
			public:
				~CameraInspectorUIView();

				/// <summary>
				/// Constructs an inspector view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_HierarchyUIView">The entity that will be shown in the view.</param>
				CameraInspectorUIView(ImGuiWindow& a_Window, HierarchyUIView& a_HierarchyUIView);

				std::string GetName() const override;

				std::string GetIcon() const override;

				void Render() override;
			protected:
				HierarchyUIView& m_HierarchyUIView;

				/// UI elements for displaying and editing the position, rotation, and scale as glm::vec3.
				Vector3View<DirectX::XMFLOAT3>
					m_PositionView,
					m_RotationView;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE