#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerFileInspectorView.h"

// utils includes
#include "utils/file_abstractions.h"

// resource includes
#include "resources/AssetType.h"

// editor includes
#include "editor/core/EditorTool.h"
#include "editor/FileResource.h"
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
			// ExplorerFileInspectorView
			//---------------------------------------------------------------------
			ExplorerFileInspectorView::ExplorerFileInspectorView(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : InspectorView(a_Window), m_ExplorerFileUIView(a_ExplorerFileUIView)
			{
				m_bShowRename = true;
				m_bShowDelete = true;
				m_bShowShowInExplorer = true;

				resources::AssetType assetType = a_ExplorerFileUIView.GetFileResource().GetMetaData()->GetAssetType();
				
				if (auto it = GetExplorerUIFactoryRegistry().find(assetType); it != GetExplorerUIFactoryRegistry().end())
				{
					m_pExplorerFileUIViewInfo = it->second(m_Window, a_ExplorerFileUIView);
				}

				/*switch (a_ExplorerFileUIView.GetFileResource().GetMetaData()->GetAssetType())
				{
					case gallus::resources::AssetType::Texture:
					{
						m_pExplorerFileUIViewInfo = new ExplorerSpriteUIViewInfo(m_Window, a_ExplorerFileUIView);
						break;
					}
					case gallus::resources::AssetType::PixelShader:
					case gallus::resources::AssetType::VertexShader:
					{
						m_pExplorerFileUIViewInfo = new ExplorerShaderUIViewInfo(m_Window, a_ExplorerFileUIView);
						break;
					}
					case gallus::resources::AssetType::Sound:
					case gallus::resources::AssetType::Song:
					case gallus::resources::AssetType::VO:
					{
						m_pExplorerFileUIViewInfo = new ExplorerAudioUIViewInfo(m_Window, a_ExplorerFileUIView);
						break;
					}
					case gallus::resources::AssetType::Scene:
					{
						m_pExplorerFileUIViewInfo = new ExplorerSceneUIViewInfo(m_Window, a_ExplorerFileUIView);
						break;
					}
					default:
					{
						m_pExplorerFileUIViewInfo = new ExplorerFileUIViewInfo(m_Window, a_ExplorerFileUIView);
						break;
					}
				}*/

				if (!m_pExplorerFileUIViewInfo)
				{
					return;
				}

				m_bShowPreview = m_pExplorerFileUIViewInfo->GetShowPreview();
			}

			//---------------------------------------------------------------------
			ExplorerFileInspectorView::~ExplorerFileInspectorView()
			{
				if (m_pExplorerFileUIViewInfo)
				{
					delete m_pExplorerFileUIViewInfo;
				}
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorView::OnRename(const std::string& a_sName)
			{
				m_ExplorerFileUIView.GetFileResource().Rename(a_sName);
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorView::OnDelete()
			{
				m_ExplorerFileUIView.GetFileResource().Delete();

				core::EDITOR_TOOL->GetEditor().GetAssetDatabase().Rescan();
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorView::OnShowInExplorer()
			{
				file::OpenInExplorer(m_ExplorerFileUIView.GetFileResource().GetPath().parent_path().lexically_normal());
			}

			//---------------------------------------------------------------------
			std::string ExplorerFileInspectorView::GetName() const
			{
				return m_ExplorerFileUIView.GetFileResource().GetPath().filename().generic_string();
			}

			//---------------------------------------------------------------------
			std::string ExplorerFileInspectorView::GetIcon() const
			{
				return m_ExplorerFileUIView.GetIcon();
			}

			//---------------------------------------------------------------------
			void ExplorerFileInspectorView::Render()
			{
				if (m_pExplorerFileUIViewInfo)
				{
					m_pExplorerFileUIViewInfo->Render();
				}
			}

			//---------------------------------------------------------------------
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