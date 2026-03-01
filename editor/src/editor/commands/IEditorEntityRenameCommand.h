#pragma once

#include "./IEditorCommand.h"

// resources
#include "resources/SrcData.h"

namespace gallus::editor
{
	class IEditorEntityRenameCommand : public IEditorCommand
	{
	public:
		IEditorEntityRenameCommand() = default;

		void Undo() override;
		void Redo() override;

		void SetOldName(const std::string& a_sOldName);
		void SetNewName(const std::string& a_sNewName);
	protected:
		void Execute(const std::string& a_sCurrentName, const std::string& a_sNewName);

		std::string
			m_sOldName,
			m_sNewName;
	};
}