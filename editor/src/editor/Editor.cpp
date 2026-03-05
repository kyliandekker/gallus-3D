#include "Editor.h"

// external
#include <iostream>

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/Camera.h"

// logger
#include "logger/Logger.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/views/selectables/EditorSelectable.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// Editor
	//---------------------------------------------------------------------
	Editor::Editor() : core::System(),
		m_EditorSettings("editorsettings.settings")
	{}

	//---------------------------------------------------------------------
	Editor::~Editor() = default;

	//---------------------------------------------------------------------
	bool Editor::Initialize()
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Initializing editor.");

		if (m_EditorSettings.Load())
		{
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Successfully loaded editor settings.");
		}

		m_pEditorCamera = std::make_unique<graphics::dx12::Camera>();

		m_pEditorCamera->Init(graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
		m_pEditorCamera->GetTransform().SetPosition({ 0.0f, 1.0f, -2.0f });

		graphics::dx12::DX12System& dx12System = GetDX12System();
		dx12System.SetDimensionDrawMode((graphics::dx12::DimensionDrawMode) m_EditorSettings.GetDimensionDrawMode());

		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Editor initialized.");

		return System::Initialize();
	}

	//---------------------------------------------------------------------
	bool Editor::Destroy()
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Editor destroyed.");
		return System::Destroy();
	}

	//---------------------------------------------------------------------
	graphics::dx12::Camera& Editor::GetEditorCamera()
	{
		return *m_pEditorCamera.get();
	}

	//---------------------------------------------------------------------
	void Editor::SetSelectable(std::weak_ptr<graphics::imgui::EditorSelectable> a_pSelectable)
	{
		std::shared_ptr<graphics::imgui::EditorSelectable> oldSelectable = m_pSelectable.lock();
		if (oldSelectable)
		{
			oldSelectable->OnDeselect();
		}
		m_pSelectable = a_pSelectable;
		std::shared_ptr<graphics::imgui::EditorSelectable> newSelectable = m_pSelectable.lock();
		if (newSelectable)
		{
			newSelectable->OnSelect();
		}

		m_eOnSelectableChanged(oldSelectable, newSelectable);
	}

	//---------------------------------------------------------------------
    bool Editor::IsSelectable(std::shared_ptr<graphics::imgui::EditorSelectable> a_pSelectable)
    {
		std::shared_ptr<graphics::imgui::EditorSelectable> newSelectable = m_pSelectable.lock();
		return newSelectable.get() == a_pSelectable.get();
	}

    //---------------------------------------------------------------------
	void Editor::ResetSelectable()
	{
		std::shared_ptr<graphics::imgui::EditorSelectable> sptr; // null
		m_pSelectable = sptr;
	}
}