#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerFileInspectorView.h"

#include "graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/FileResource.h"
#include "utils/file_abstractions.h"
#include "editor/AssetType.h"

#include "graphics/imgui/views/inspector/fileinfo/ExplorerFileUIViewInfo.h"
#include "graphics/imgui/views/inspector/fileinfo/ExplorerSpriteUIViewInfo.h"
#include "graphics/imgui/views/inspector/fileinfo/ExplorerShaderUIViewInfo.h"
#include "graphics/imgui/views/inspector/fileinfo/ExplorerAudioUIViewInfo.h"
#include "core/EditorTool.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

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
					case gallus::editor::AssetType::PixelShader:
					case gallus::editor::AssetType::VertexShader:
					{
						m_pExplorerFileUIViewInfo = new ExplorerShaderUIViewInfo(m_Window, a_ExplorerFileUIView);
						break;
					}
					case gallus::editor::AssetType::Sound:
					case gallus::editor::AssetType::Song:
					case gallus::editor::AssetType::VO:
					{
						m_pExplorerFileUIViewInfo = new ExplorerAudioUIViewInfo(m_Window, a_ExplorerFileUIView);
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

				core::EDITOR_TOOL->GetEditor().GetAssetDatabase().Rescan();
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

#endif // _EDITOR
#endif // IMGUI_DISABLE