#include "IEditorSpriteSheetDataCommand.h"

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/DX12System.h"

// logger
#include "logger/Logger.h"

// resources
#include "resources/ResourceAtlas.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// IEditorSpriteSheetDataCommand
	//---------------------------------------------------------------------
	void IEditorSpriteSheetDataCommand::Undo()
	{
		Execute(m_sID, m_OldSrcData);
	}

	//---------------------------------------------------------------------
	void IEditorSpriteSheetDataCommand::Redo()
	{
		Execute(m_sID, m_NewSrcData);
	}

	//---------------------------------------------------------------------
	void IEditorSpriteSheetDataCommand::Execute(const std::string& m_sID, const resources::SrcData& a_SrcData)
	{
		std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = GetDX12System().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

		if (std::shared_ptr<graphics::dx12::Texture> texture = GetResourceAtlas().LoadTexture(m_sID, cCommandQueue).lock())
		{
			DeserializeFields(texture.get(), a_SrcData);
		}
	}
}