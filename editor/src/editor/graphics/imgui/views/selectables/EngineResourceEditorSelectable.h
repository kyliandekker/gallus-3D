#pragma once

// external
#include <imgui/imgui.h>
#include <memory>

// editor
#include "editor/graphics/imgui/views/selectables/ListItemEditorSelectable.h"

namespace gallus::resources
{
	class EngineResource;
}
namespace gallus::graphics::imgui
{
	class ImGuiWindow;
	class FileEditorSelectables;

	class EngineResourceEditorSelectable : public ListItemEditorSelectable
	{
	public:
		EngineResourceEditorSelectable(ImGuiSystem& a_System, std::weak_ptr<resources::EngineResource> a_pResource);

		void SetEngineResourceProperties();

		std::weak_ptr<resources::EngineResource> GetEngineResource()
		{
			return m_pResource;
		}

		const std::weak_ptr<resources::EngineResource> GetEngineResource() const
		{
			return m_pResource;
		}
	private:
		std::weak_ptr<resources::EngineResource> m_pResource = {};
	};
}