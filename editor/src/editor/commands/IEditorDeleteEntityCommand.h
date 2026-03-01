#pragma once

#include "./IEditorCommand.h"

// resources
#include "resources/SrcData.h"

namespace gallus::editor
{
	class IEditorDeleteEntityCommand : public IEditorCommand
	{
	public:
		IEditorDeleteEntityCommand() = default;

		void SetData(const resources::SrcData& a_SrcData);

		void Undo() override;
		void Redo() override;
	protected:
		resources::SrcData m_SrcData;
	};
}