#include "IEditorEntityCommand.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// IEditorEntityCommand
	//---------------------------------------------------------------------
	void IEditorEntityCommand::Undo()
	{
		Execute(m_OldSrcData);
	}

	//---------------------------------------------------------------------
	void IEditorEntityCommand::Redo()
	{
		Execute(m_NewSrcData);
	}

	//---------------------------------------------------------------------
	void IEditorEntityCommand::Execute(const resources::SrcData& a_SrcData)
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed executing editor command: ECS not found.");
			return;
		}

		std::string name;
		if (!a_SrcData.GetString(name, "name"))
		{
			LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Failed executing editor command: Could not read name of entity in src data. Defaulting to using name \"\".", name.c_str());
		}

		if (ecs->GetEntityByName(name).lock())
		{
			ecs->UpdateEntity(a_SrcData);
		}
		else
		{
			ecs->CreateEntity(a_SrcData);
		}
	}
}