#include "EditorWorkspace.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/string_extensions.h"

// file
#include "file/file_abstractions.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/AssetDatabase.h"
#include "editor/commands/IEditorCommand.h"

// editor/file
#include "editor/file/FileResource.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// EditorWorkspace
	//---------------------------------------------------------------------
	void EditorWorkspace::AddAction(std::unique_ptr<IEditorCommand> a_pCommand)
	{
		if (!a_pCommand)
		{
			return;
		}

		m_aUndoStack.push_back(std::move(a_pCommand));
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::Execute(std::unique_ptr<IEditorCommand> a_pCommand)
	{
		if (!a_pCommand)
		{
			return;
		}

		a_pCommand->Redo();
		m_aUndoStack.push_back(std::move(a_pCommand));
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::Undo()
	{
		if (m_aUndoStack.empty())
		{
			return;
		}

		std::unique_ptr<IEditorCommand> command = std::move(m_aUndoStack.back());
		m_aUndoStack.pop_back();

		command->Undo();

		m_aRedoStack.push_back(std::move(command));
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::Redo()
	{
		if (m_aRedoStack.empty())
		{
			return;
		}

		std::unique_ptr<IEditorCommand> command = std::move(m_aRedoStack.back());
		m_aRedoStack.pop_back();

		command->Redo();

		m_aUndoStack.push_back(std::move(command));
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::ClearActions()
	{
		m_aUndoStack.clear();
		m_aRedoStack.clear();
	}

	//---------------------------------------------------------------------
	bool EditorWorkspace::CanUndo() const
	{
		return !m_aUndoStack.empty();
	}

	//---------------------------------------------------------------------
	bool EditorWorkspace::CanRedo() const
	{
		return !m_aRedoStack.empty();
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::MarkDirty(const std::string& a_sID)
	{
		if (a_sID.empty())
		{
			return;
		}
		m_aDirtyAssets.insert(a_sID);
	}

	//---------------------------------------------------------------------
	bool EditorWorkspace::IsDirty(const std::string& a_sID) const
	{
		if (a_sID.empty())
		{
			return false;
		}
		return m_aDirtyAssets.contains(a_sID);
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::Save(const std::string& a_sID, const core::Data& a_Data, bool a_bMetaData)
	{
		editor::AssetDatabase* assetDatabase = GetEditorEngine().GetAssetDatabase();
		if (!assetDatabase)
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed saving asset \"%s\" because asset database was null.", a_sID.c_str());
			return;
		}

		file::FileResource* fileResource = nullptr;

		std::string id = string_extensions::StringToLower(a_sID);
		if (!assetDatabase->GetResourceFolder()->Find(id, fileResource))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed saving asset \"%s\" because it could not find the asset.", a_sID.c_str());
			return;
		}

		fs::path path = fileResource->GetPath();
		if (a_bMetaData)
		{
			path += ".meta";
		}

		if (!file::SaveFile(path, a_Data))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed saving asset \"%s\" because it could not save for some reason.", a_sID.c_str());
			return;
		}

		if (IsDirty(a_sID))
		{
			m_aDirtyAssets.erase(a_sID);
		}
	}

	//---------------------------------------------------------------------
	void EditorWorkspace::SetClipboardData(const core::Data& a_Data)
	{
		m_ClipboardData = a_Data;
	}
}