#pragma once

#include "./IEditorDataCommand.h"

// resources
#include "resources/SrcData.h"

namespace gallus::editor
{
	class IEditorDirectionalLightCommand : public IEditorDataCommand
	{
	public:
		IEditorDirectionalLightCommand() = default;

		void Undo() override;
		void Redo() override;
	private:
		void Execute(const resources::SrcData& a_SrcData);
	};
}