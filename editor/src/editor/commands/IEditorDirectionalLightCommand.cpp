#include "IEditorDirectionalLightCommand.h"

// logger
#include "logger/Logger.h"

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/DirectionalLight.h"

// resources
#include "resources/ISerializableObject.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// IEditorDirectionalLightCommand
	//---------------------------------------------------------------------
	void IEditorDirectionalLightCommand::Undo()
	{
		Execute(m_OldSrcData);
	}

	//---------------------------------------------------------------------
	void IEditorDirectionalLightCommand::Redo()
	{
		Execute(m_NewSrcData);
	}

	//---------------------------------------------------------------------
	void IEditorDirectionalLightCommand::Execute(const resources::SrcData& a_SrcData)
	{
		graphics::dx12::DX12System& dx12 = GetDX12System();

		if (std::shared_ptr<graphics::dx12::DirectionalLight> directionalLight = dx12.GetDirectionalLight().lock())
		{
			DeserializeFields(directionalLight.get(), a_SrcData);
		}
	}
}