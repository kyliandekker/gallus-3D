#include "IEditorCameraCommand.h"

// logger
#include "logger/Logger.h"

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/Camera.h"

// resources
#include "resources/ISerializableObject.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// IEditorCameraCommand
	//---------------------------------------------------------------------
	void IEditorCameraCommand::Undo()
	{
		Execute(m_OldSrcData);
	}

	//---------------------------------------------------------------------
	void IEditorCameraCommand::Redo()
	{
		Execute(m_NewSrcData);
	}

	//---------------------------------------------------------------------
	void IEditorCameraCommand::Execute(const resources::SrcData& a_SrcData)
	{
		graphics::dx12::DX12System& dx12 = GetDX12System();

		graphics::dx12::Camera& camera = dx12.GetCamera();
		DeserializeFields(&camera, a_SrcData);
	}
}