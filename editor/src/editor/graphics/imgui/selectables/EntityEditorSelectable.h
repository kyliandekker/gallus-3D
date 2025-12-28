#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <string>
#include <map>
#include <memory>

// editor
#include "editor/graphics/imgui/EditorSelectable.h"

namespace gallus
{
	namespace gameplay
	{
		class EntityID;
		class Entity;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class EntityEditorSelectable : public EditorSelectable
			{
			public:
				EntityEditorSelectable(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID);

				void RenderEntity(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected);

				void Render() override
				{}

				gameplay::EntityID& GetEntityID()
				{
					return m_EntityID;
				}

				const gameplay::EntityID& GetEntityID() const
				{
					return m_EntityID;
				}

				bool RenderGizmos(
					const ImVec2& a_vScenePos,
					const ImVec2& a_vSize,
					const ImVec2& a_vPanOffset,
					float a_fZoom) override;

				void OnRename(const std::string& a_sName) override;

				void OnDelete() override;

				std::string GetName() const override;

				std::string GetIcon() const override;

				void RenderEditorFields() override;
			private:
				gameplay::EntityID& m_EntityID;
				std::string m_sIcon;

				std::weak_ptr<gameplay::Entity> m_pEntity = {};

				std::map<std::string, bool> m_aExpanded; // A list of component booleans.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE