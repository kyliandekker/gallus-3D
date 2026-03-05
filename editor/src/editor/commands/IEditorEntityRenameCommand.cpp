#include "IEditorEntityRenameCommand.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/Entity.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// IEditorEntityRenameCommand
	//---------------------------------------------------------------------
	void IEditorEntityRenameCommand::Undo()
	{
		Execute(m_sNewName, m_sOldName);
	}

	//---------------------------------------------------------------------
	void IEditorEntityRenameCommand::Redo()
	{
		Execute(m_sOldName, m_sNewName);
	}

	void IEditorEntityRenameCommand::SetOldName(const std::string& a_sOldName)
	{
		m_sOldName = a_sOldName;
	}

	//---------------------------------------------------------------------
	void IEditorEntityRenameCommand::SetNewName(const std::string & a_sNewName)
	{
		m_sNewName = a_sNewName;
	}

	//---------------------------------------------------------------------
	void IEditorEntityRenameCommand::Execute(const std::string & a_sCurrentName, const std::string & a_sNewName)
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed executing editor command: ECS not found.");
			return;
		}

		std::shared_ptr<gameplay::Entity> entity = ecs->GetEntityByName(a_sCurrentName).lock();
		if (!entity)
		{
			return;
		}

		entity->SetName(a_sNewName);
	}
}