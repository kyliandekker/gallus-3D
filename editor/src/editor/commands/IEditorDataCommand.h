#pragma once

#include "./IEditorCommand.h"

// resources
#include "resources/SrcData.h"

namespace gallus::editor
{
	class IEditorDataCommand : public IEditorCommand
	{
	public:
		IEditorDataCommand() = default;

		void SetOldData(const resources::SrcData& a_SrcData);
		void SetNewData(const resources::SrcData& a_SrcData);
	protected:
		resources::SrcData m_OldSrcData;
		resources::SrcData m_NewSrcData;
	};
}