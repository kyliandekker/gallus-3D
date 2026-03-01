#include "./IEditorDeleteEntityCommand.h"

// core
#include "core/Data.h"

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
	// IEditorDeleteEntityCommand
	//---------------------------------------------------------------------
	void IEditorDeleteEntityCommand::SetData(const resources::SrcData& a_SrcData)
	{
		core::Data oldData;
		a_SrcData.GetData(oldData);

		m_SrcData = resources::SrcData(oldData);
	}

	//---------------------------------------------------------------------
	void IEditorDeleteEntityCommand::Undo()
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed executing editor command: ECS not found.");
			return;
		}

		ecs->CreateEntity(m_SrcData);
	}

	//---------------------------------------------------------------------
	void IEditorDeleteEntityCommand::Redo()
	{
		gameplay::EntityComponentSystem* ecs = GetEditorEngine().GetECS();
		if (!ecs)
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed executing editor command: ECS not found.");
			return;
		}

		std::string name;
		if (!m_SrcData.GetString(name, "name"))
		{
			LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Failed executing editor command: Could not read name of entity in src data. Defaulting to using name \"\".", name.c_str());
		}

		std::shared_ptr<gameplay::Entity> entity = ecs->GetEntityByName(name).lock();
		if (entity)
		{
			ecs->DeleteEntity(entity->GetEntityID());
		}
	}
}