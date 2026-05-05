#include "EditorWorkspace.h"

// graphics
#include "graphics/dx12/RenderView.h"
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/HeapAllocation.h"
#include "graphics/dx12/Camera.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/DX12UploadBufferAllocator.h"

// logger
#include "logger/Logger.h"

// resources
#include "resources/ResourceAtlas.h"

// utils
#include "utils/string_extensions.h"

// file
#include "file/file_abstractions.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/AssetDatabase.h"
#include "editor/commands/IEditorCommand.h"

// editor/file
#include "editor/file/FileResource.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	void EditorActionStack::Undo()
	{
		if (m_aUndoStack.empty())
		{
			return;
		}

		std::unique_ptr<IEditorCommand> command = std::move(m_aUndoStack.back());
		m_aUndoStack.pop_back();

		command->Undo();

		m_aRedoStack.push_back(std::move(command));
	}

	//---------------------------------------------------------------------
	void EditorActionStack::Redo()
	{
		if (m_aRedoStack.empty())
		{
			return;
		}

		std::unique_ptr<IEditorCommand> command = std::move(m_aRedoStack.back());
		m_aRedoStack.pop_back();

		command->Redo();

		m_aUndoStack.push_back(std::move(command));
	}

	//---------------------------------------------------------------------
	void EditorActionStack::ClearActions()
	{
		m_aUndoStack.clear();
		m_aRedoStack.clear();
	}

	//---------------------------------------------------------------------
	EditorWorkspace::EditorWorkspace()
	{
		for (size_t i = 0; i < EditorActionStack_MAX; i++)
		{
			m_aEditorActionStacks.emplace_back();
		}
	}

	//---------------------------------------------------------------------
	// EditorWorkspace
	//---------------------------------------------------------------------
	void EditorWorkspace::Initialize()
	{
		//std::shared_ptr<graphics::dx12::RenderView> renderView = GetDX12System().RegisterView(1);
		//if (renderView)
		//{
		//	D3D12_RESOURCE_DESC texDesc = {};
		//	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		//	texDesc.Width = graphics::dx12::RENDER_TEX_SIZE.x;
		//	texDesc.Height = graphics::dx12::RENDER_TEX_SIZE.y;
		//	texDesc.DepthOrArraySize = 1;
		//	texDesc.MipLevels = 1;
		//	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	texDesc.SampleDesc.Count = 1;
		//	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		//	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		//	renderView->m_iRTVIndex = GetDX12System().GetRTV().Allocate();
		//	renderView->m_iDSVIndex = GetDX12System().GetDSV().Allocate();
		//	GetDX12System().SetDepthBuffer(renderView->m_pDepthBuffer, renderView->m_iDSVIndex);

		//	renderView->m_pRenderTarget = GetResourceAtlas().LoadTextureByDescription("EditorRenderTexture", texDesc);

		//	if (std::shared_ptr<graphics::dx12::Texture> texture = renderView->m_pRenderTarget.lock())
		//	{
		//		texture->SetResourceCategory(resources::EngineResourceCategory::System);
		//		texture->SetIsDestroyable(false);

		//		GetDX12System().GetDevice()->CreateRenderTargetView(texture->GetResource().Get(), nullptr, GetDX12System().GetRTV().GetCPUHandle(renderView->m_iRTVIndex));
		//	}

		//	renderView->m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
		//	renderView->m_ScissorRect = CD3DX12_RECT(0, 0, graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);

		//	std::weak_ptr<graphics::dx12::Mesh> mesh = GetResourceAtlas().LoadMeshEmpty("EditorSphere");
		//	renderView->m_RenderFunc = [renderView](std::shared_ptr<graphics::dx12::CommandQueue> pCommandQueue, std::shared_ptr<graphics::dx12::CommandList> pCommandList)
		//	{
		//	};
		//}
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::AddAction(std::unique_ptr<IEditorCommand> a_pCommand, EditorActionStackCategory a_EditorActionStackCategory)
	{
		if (!a_pCommand)
		{
			return;
		}

		m_aEditorActionStacks[a_EditorActionStackCategory].m_aUndoStack.push_back(std::move(a_pCommand));
		m_aEditorActionStacks[a_EditorActionStackCategory].m_aRedoStack.clear();
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::Execute(std::unique_ptr<IEditorCommand> a_pCommand, EditorActionStackCategory a_EditorActionStackCategory)
	{
		if (!a_pCommand)
		{
			return;
		}

		a_pCommand->Redo();
		m_aEditorActionStacks[a_EditorActionStackCategory].m_aUndoStack.push_back(std::move(a_pCommand));
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::Undo(EditorActionStackCategory a_EditorActionStackCategory)
	{
		if (m_aEditorActionStacks[a_EditorActionStackCategory].m_aUndoStack.empty())
		{
			return;
		}

		std::unique_ptr<IEditorCommand> command = std::move(m_aEditorActionStacks[a_EditorActionStackCategory].m_aUndoStack.back());
		m_aEditorActionStacks[a_EditorActionStackCategory].m_aUndoStack.pop_back();

		command->Undo();

		m_aEditorActionStacks[a_EditorActionStackCategory].m_aRedoStack.push_back(std::move(command));
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::Redo(EditorActionStackCategory a_EditorActionStackCategory)
	{
		if (m_aEditorActionStacks[a_EditorActionStackCategory].m_aRedoStack.empty())
		{
			return;
		}

		std::unique_ptr<IEditorCommand> command = std::move(m_aEditorActionStacks[a_EditorActionStackCategory].m_aRedoStack.back());
		m_aEditorActionStacks[a_EditorActionStackCategory].m_aRedoStack.pop_back();

		command->Redo();

		m_aEditorActionStacks[a_EditorActionStackCategory].m_aUndoStack.push_back(std::move(command));
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::ClearActions(EditorActionStackCategory a_EditorActionStackCategory)
	{
		m_aEditorActionStacks[a_EditorActionStackCategory].m_aUndoStack.clear();
		m_aEditorActionStacks[a_EditorActionStackCategory].m_aRedoStack.clear();
	}

	//---------------------------------------------------------------------
	bool EditorWorkspace::CanUndo(EditorActionStackCategory a_EditorActionStackCategory) const
	{
		return !m_aEditorActionStacks[a_EditorActionStackCategory].m_aUndoStack.empty();
	}

	//---------------------------------------------------------------------
	bool EditorWorkspace::CanRedo(EditorActionStackCategory a_EditorActionStackCategory) const
	{
		return !m_aEditorActionStacks[a_EditorActionStackCategory].m_aRedoStack.empty();
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::MarkDirty(const std::string& a_sID)
	{
		if (a_sID.empty())
		{
			return;
		}
		m_aDirtyAssets.insert(a_sID);
	}

	//---------------------------------------------------------------------
	bool EditorWorkspace::IsDirty(const std::string& a_sID) const
	{
		if (a_sID.empty())
		{
			return false;
		}
		return m_aDirtyAssets.contains(a_sID);
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::Save(const std::string& a_sID, const core::Data& a_Data, bool a_bMetaData)
	{
		editor::AssetDatabase* assetDatabase = GetEditorEngine().GetAssetDatabase();
		if (!assetDatabase)
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed saving asset \"%s\" because asset database was null.", a_sID.c_str());
			return;
		}

		file::FileResource* fileResource = nullptr;

		std::string id = string_extensions::StringToLower(a_sID);
		if (!assetDatabase->GetResourceFolder()->Find(id, fileResource))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed saving asset \"%s\" because it could not find the asset.", a_sID.c_str());
			return;
		}

		fs::path path = fileResource->GetPath();
		if (a_bMetaData)
		{
			path += ".meta";
		}

		if (!file::SaveFile(path, a_Data))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed saving asset \"%s\" because it could not save for some reason.", a_sID.c_str());
			return;
		}

		if (IsDirty(a_sID))
		{
			m_aDirtyAssets.erase(a_sID);
		}
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::SetClipboardData(const core::Data& a_Data)
	{
		m_ClipboardData = a_Data;
	}
}