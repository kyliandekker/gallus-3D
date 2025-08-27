#include "ExplorerFileInspectorView.h"

#include "graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/FileResource.h"
#include "utils/file_abstractions.h"
#include "editor/AssetType.h"

#include "graphics/imgui/views/inspector/fileinfo/ExplorerFileUIViewInfo.h"
#include "graphics/imgui/views/inspector/fileinfo/ExplorerSpriteUIViewInfo.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			namespace editor
			{
				ExplorerFileInspectorView::ExplorerFileInspectorView(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : InspectorView(a_Window), m_ExplorerFileUIView(a_ExplorerFileUIView)
				{
					m_bShowRename = true;
					m_bShowDelete = true;
					m_bShowShowInExplorer = true;

					switch (a_ExplorerFileUIView.GetFileResource().GetAssetType())
					{
						case gallus::editor::AssetType::Texture:
						{
							m_pExplorerFileUIViewInfo = new ExplorerSpriteUIViewInfo(m_Window, a_ExplorerFileUIView);
							break;
						}
						default:
						{
							m_pExplorerFileUIViewInfo = new ExplorerFileUIViewInfo(m_Window, a_ExplorerFileUIView);
							break;
						}
					}
					m_bShowPreview = m_pExplorerFileUIViewInfo->GetShowPreview();
				}

				ExplorerFileInspectorView::~ExplorerFileInspectorView()
				{
					if (m_pExplorerFileUIViewInfo)
					{
						delete m_pExplorerFileUIViewInfo;
					}
				}

				void ExplorerFileInspectorView::OnRename(const std::string& a_sName)
				{
					m_ExplorerFileUIView.GetFileResource().Rename(a_sName);
				}

				void ExplorerFileInspectorView::OnDelete()
				{
					m_ExplorerFileUIView.GetFileResource().Delete();
				}

				void ExplorerFileInspectorView::OnShowInExplorer()
				{
					file::OpenInExplorer(m_ExplorerFileUIView.GetFileResource().GetPath().parent_path().lexically_normal());
				}

				std::string ExplorerFileInspectorView::GetName() const
				{
					return m_ExplorerFileUIView.GetFileResource().GetPath().filename().generic_string();
				}


				std::string ExplorerFileInspectorView::GetIcon() const
				{
					return m_ExplorerFileUIView.GetIcon();
				}

				void ExplorerFileInspectorView::Render()
				{
					if (m_pExplorerFileUIViewInfo)
					{
						m_pExplorerFileUIViewInfo->Render();
					}
				}

				void ExplorerFileInspectorView::RenderPreview()
				{
					if (m_pExplorerFileUIViewInfo)
					{
						m_pExplorerFileUIViewInfo->RenderPreview();
					}
				}
			}
		}
	}
}