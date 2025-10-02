#ifndef IMGUI_DISABLE
#ifdef _EDITOR

// header
#include "ExplorerFileInspectorUIView.h"

// resource
#include "resources/metadata/MetaData.h"
#include "resources/AssetType.h"

// utils
#include "utils/file_abstractions.h"

// editor
#include "editor/core/EditorEngine.h"
#include "resources/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/graphics/imgui/views/inspector/fileinfo/ExplorerFileUIViewInfo.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			//---------------------------------------------------------------------
			// ExplorerFileInspectorUIView
			//---------------------------------------------------------------------
			ExplorerFileInspectorUIView::ExplorerFileInspectorUIView(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : InspectorUIView(a_Window), m_ExplorerFileUIView(a_ExplorerFileUIView)
			{
				m_bShowRename = true;
				m_bShowDelete = true;
				m_bShowShowInExplorer = true;

				resources::AssetType assetType = a_ExplorerFileUIView.GetFileResource().GetMetaData()->GetAssetType();
				
				if (auto it = GetExplorerUIFactoryRegistry().find(assetType); it != GetExplorerUIFactoryRegistry().end())
				{
					m_pExplorerFileUIViewInfo = it->second(m_Window, a_ExplorerFileUIView);
				}

				if (!m_pExplorerFileUIViewInfo)
				{
					return;
				}

				m_bShowPreview = m_pExplorerFileUIViewInfo->GetShowPreview();
			}

			//---------------------------------------------------------------------
			ExplorerFileInspectorUIView::~ExplorerFileInspectorUIView()
			{
				if (m_pExplorerFileUIViewInfo)
				{
					delete m_pExplorerFileUIViewInfo;
				}
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorUIView::OnRename(const std::string& a_sName)
			{
				m_ExplorerFileUIView.GetFileResource().Rename(a_sName);
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorUIView::OnDelete()
			{
				m_ExplorerFileUIView.GetFileResource().Delete();

				core::EDITOR_ENGINE->GetEditor().GetAssetDatabase().Rescan();
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorUIView::OnShowInExplorer()
			{
				file::OpenInExplorer(m_ExplorerFileUIView.GetFileResource().GetPath().parent_path().lexically_normal());
			}

			//---------------------------------------------------------------------
			std::string ExplorerFileInspectorUIView::GetName() const
			{
				return m_ExplorerFileUIView.GetFileResource().GetPath().filename().generic_string();
			}

			//---------------------------------------------------------------------
			std::string ExplorerFileInspectorUIView::GetIcon() const
			{
				return m_ExplorerFileUIView.GetIcon();
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorUIView::Render()
			{
				if (m_pExplorerFileUIViewInfo)
				{
					m_pExplorerFileUIViewInfo->Render();
				}
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorUIView::RenderPreview()
			{
				if (m_pExplorerFileUIViewInfo)
				{
					m_pExplorerFileUIViewInfo->RenderPreview();
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE