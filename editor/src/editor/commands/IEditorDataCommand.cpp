#include "./IEditorDataCommand.h"

// core
#include "core/Data.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// IEditorDataCommand
	//---------------------------------------------------------------------
	void IEditorDataCommand::SetOldData(const resources::SrcData& a_SrcData)
	{
		core::Data oldData;
		a_SrcData.GetData(oldData);

		m_OldSrcData = resources::SrcData(oldData);
	}

	//---------------------------------------------------------------------
	void IEditorDataCommand::SetNewData(const resources::SrcData& a_SrcData)
	{
		core::Data newData;
		a_SrcData.GetData(newData);

		m_NewSrcData = resources::SrcData(newData);
	}
}