#pragma once

#include "./IEditorDataCommand.h"

// resources
#include "resources/SrcData.h"

namespace gallus::editor
{
	class IEditorEntityCommand : public IEditorDataCommand
	{
	public:
		IEditorEntityCommand() = default;

		void Undo() override;
		void Redo() override;
	private:
		void Execute(const resources::SrcData& a_SrcData);
	};
}