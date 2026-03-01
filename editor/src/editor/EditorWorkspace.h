#pragma once

#include <memory>
#include <unordered_set>
#include <vector>
#include <string>

#include "core/Data.h"

namespace gallus::editor
{
	class IEditorCommand;

	class EditorWorkspace
	{
	public:
		void AddAction(std::unique_ptr<IEditorCommand> a_pCommand);
		void Execute(std::unique_ptr<IEditorCommand> a_pCommand);

		void Undo();
		void Redo();
		void ClearActions();

		bool CanUndo() const;
		bool CanRedo() const;

		void MarkDirty(const std::string& a_sID);
		bool IsDirty(const std::string& a_sID) const;

		void Save(const std::string& a_sID, const core::Data& a_Data, bool a_bMetaData = true);

		void SetClipboardData(const core::Data& a_Data);
		const core::Data& GetClipboardData() const
		{
			return m_ClipboardData;
		}
	private:
		std::unordered_set<std::string> m_aDirtyAssets;

		std::vector<std::unique_ptr<IEditorCommand>> m_aUndoStack;
		std::vector<std::unique_ptr<IEditorCommand>> m_aRedoStack;

		core::Data m_ClipboardData;
		std::string m_sClipboardType;
	};
}