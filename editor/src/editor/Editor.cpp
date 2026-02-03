#include "Editor.h"

// external
#include <iostream>

// graphics
#include "graphics/dx12/DX12System.h"

// logger
#include "logger/Logger.h"

// editor
#include "editor/graphics/imgui/EditorSelectable.h"
#include "editor/core/EditorEngine.h"

namespace gallus
{
	namespace editor
	{
		//---------------------------------------------------------------------
		// Editor
		//---------------------------------------------------------------------
		Editor::Editor() : core::ThreadedSystem::ThreadedSystem(), 
			m_EditorSettings("editorsettings.settings")
		{
			m_pSelectable = nullptr;
		}

		//---------------------------------------------------------------------
		bool Editor::Initialize(bool a_bWait)
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Initializing editor.");

			return ThreadedSystem::Initialize(a_bWait);
		}

		//---------------------------------------------------------------------
		void Editor::Loop()
		{
			std::lock_guard<std::mutex> lock(m_EditorMutex);
			m_AssetDatabase.CheckAssetDatabase();
		}

		//---------------------------------------------------------------------
		bool Editor::Destroy()
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Destroying editor.");
			return ThreadedSystem::Destroy();
		}

		//---------------------------------------------------------------------
		void Editor::SetSelectable(graphics::imgui::EditorSelectable* a_pSelectable)
		{
			if (m_pSelectable)
			{
				m_pSelectable.get()->Deselect();
			}
			m_pSelectable = a_pSelectable;
			if (m_pSelectable)
			{
				m_pSelectable.get()->Select();
			}
		}
		
		//---------------------------------------------------------------------
		void Editor::SceneOrPrefabDirtyChanged(bool a_bPrev, bool a_bNew)
		{
			m_bIsAnyDirty.OnChanged().invoke(a_bPrev, a_bNew);
		}
		
		//---------------------------------------------------------------------
		void Editor::CopyGameFoV()
		{
			graphics::dx12::DX12System* dx12System = core::ENGINE->GetDX12();
			if (!dx12System)
			{
				return;
			}

			graphics::dx12::Camera& gameCamera = dx12System->GetCamera();
			if (m_EditorCamera.GetFoV() == gameCamera.GetFoV())
			{
				return;
			}

			m_EditorCamera.SetFoV(gameCamera.GetFoV());
			m_EditorCamera.SetProjection();
		}

		//---------------------------------------------------------------------
		bool Editor::InitThreadWorker()
		{
			m_AssetDatabase.Initialize();

			m_EditorSettings.Load();

			m_CurrentScene.SetResourceCategory(resources::EngineResourceCategory::Editor);
			m_EditorCamera.Init(graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
			m_EditorCamera.GetTransform().SetPosition({ 0.0f, 1.0f, -2.0f });

			m_CurrentScene.IsDirty().OnChanged() += std::bind(&Editor::SceneOrPrefabDirtyChanged, this, std::placeholders::_1, std::placeholders::_2);
			m_Prefab.IsDirty().OnChanged() += std::bind(&Editor::SceneOrPrefabDirtyChanged, this, std::placeholders::_1, std::placeholders::_2);

			graphics::dx12::DX12System* dx12System = core::ENGINE->GetDX12();
			if (!dx12System)
			{
				return false;
			}

			dx12System->SetDimensionDrawMode((graphics::dx12::DimensionDrawMode) m_EditorSettings.GetDimensionDrawMode());

			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Initialized editor.");

			return true;
		}

		//---------------------------------------------------------------------
		void Editor::Finalize()
		{
			m_AssetDatabase.Destroy();

			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Destroyed editor.");
		}
	}
}