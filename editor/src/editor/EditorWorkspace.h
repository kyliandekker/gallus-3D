#pragma once

#include <memory>
#include <unordered_set>
#include <vector>
#include <string>

#include "core/Data.h"

namespace gallus::editor
{
	class IEditorCommand;

	struct EditorActionStack
	{
		std::vector<std::unique_ptr<IEditorCommand>> m_aUndoStack;
		std::vector<std::unique_ptr<IEditorCommand>> m_aRedoStack;

		void Undo();
		void Redo();
		void ClearActions();
	};

	enum EditorActionStackCategory
	{
		EditorActionStack_General,
		EditorActionStack_SpriteSheet,
		EditorActionStack_Animation,
		EditorActionStack_MAX
	};

	class EditorWorkspace
	{
	public:
		EditorWorkspace();

		void AddAction(std::unique_ptr<IEditorCommand> a_pCommand, EditorActionStackCategory a_EditorActionStackCategory = EditorActionStackCategory::EditorActionStack_General);
		void Execute(std::unique_ptr<IEditorCommand> a_pCommand, EditorActionStackCategory a_EditorActionStackCategory = EditorActionStackCategory::EditorActionStack_General);

		void Undo(EditorActionStackCategory a_EditorActionStackCategory = EditorActionStackCategory::EditorActionStack_General);
		void Redo(EditorActionStackCategory a_EditorActionStackCategory = EditorActionStackCategory::EditorActionStack_General);
		void ClearActions(EditorActionStackCategory a_EditorActionStackCategory = EditorActionStackCategory::EditorActionStack_General);

		bool CanUndo(EditorActionStackCategory a_EditorActionStackCategory = EditorActionStackCategory::EditorActionStack_General) const;
		bool CanRedo(EditorActionStackCategory a_EditorActionStackCategory = EditorActionStackCategory::EditorActionStack_General) const;

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

		std::vector<EditorActionStack> m_aEditorActionStacks;

		core::Data m_ClipboardData;
		std::string m_sClipboardType;
	};
}