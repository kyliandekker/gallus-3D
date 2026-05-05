#pragma once

#include "./IEditorDataCommand.h"

// external
#include <string>

// resources
#include "resources/SrcData.h"

namespace gallus::editor
{
	class IEditorSpriteSheetDataCommand : public IEditorDataCommand
	{
	public:
		IEditorSpriteSheetDataCommand() = default;

		void Undo() override;
		void Redo() override;

		void SetID(const std::string& a_sID)
		{
			m_sID = a_sID;
		}
	protected:
		void Execute(const std::string& m_sID, const resources::SrcData& a_SrcData);

		std::string m_sID;
	};
}