// header
#include "Prefab.h"

// core
#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/ECSBaseSystem.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		bool Prefab::LoadByName(const std::string& a_sName)
		{
			if (!Scene::LoadByName(a_sName))
			{
				return false;
			}
			m_AssetType = resources::AssetType::Prefab;

			return true;
		}

		//---------------------------------------------------------------------
		bool Prefab::LoadByPath(const fs::path& a_Path)
		{
			if (!Scene::LoadByPath(a_Path))
			{
				return false;
			}
			m_AssetType = resources::AssetType::Prefab;

			return true;
		}

		//---------------------------------------------------------------------
		bool Prefab::LoadData()
		{
#ifdef _EDITOR
			m_bIsDirty = false;
#endif // _EDITOR

			// Clear all entities.
			core::ENGINE->GetECS().Clear();
			while (!core::ENGINE->GetECS().GetEntities().empty())
			{
			}

			if (!Instantiate().IsValid())
			{
				return false;
			}

#ifdef _EDITOR
			m_bIsDirty = true;
#endif // _EDITOR
			return true;
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		const core::Data Prefab::GetSceneData() const
		{
			resources::SrcData srcData = resources::SrcData();
			srcData.SetObject();

			auto& entities = core::ENGINE->GetECS().GetEntities();
			if (!entities.empty())
			{
				gameplay::Entity& entity = entities[0];

				srcData.SetString(JSON_SCENE_ENTITIES_VAR_NAME, entity.GetName());
				srcData.SetBool(JSON_SCENE_ENTITIES_VAR_ACTIVE, entity.IsActive());

				resources::SrcData componentsSrc = resources::SrcData();
				componentsSrc.SetObject();

				for (gameplay::AbstractECSSystem* system : core::ENGINE->GetECS().GetSystemsContainingEntity(entity))
				{
					resources::SrcData componentSrc = resources::SrcData();
					componentSrc.SetObject();

					const gameplay::Component* component = system->GetBaseComponent(entity.GetEntityID());
					component->Serialize(componentSrc);

					componentsSrc.SetSrcObject(system->GetPropertyName(), componentSrc);
				}
				srcData.SetSrcObject(JSON_SCENE_ENTITIES_VAR_COMPONENTS, componentsSrc);
			}
			
			core::Data data;
			srcData.GetData(data);
			return data;
		}
#endif

		//---------------------------------------------------------------------
		gameplay::EntityID Prefab::Instantiate()
		{
			gameplay::EntityID id;
			id.SetInvalid();

			if (m_Data.empty())
			{
				return id;
			}
			
			resources::SrcData srcData(m_Data);
			if (!srcData.IsValid())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}

			std::string name = core::ENGINE->GetECS().GetUniqueName("New GameObject");
			if (!srcData.GetString(JSON_SCENE_ENTITIES_VAR_NAME, name))
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read name of entity in prefab data. Defaulting to using name \"\".", name.c_str());
			}

			id = core::ENGINE->GetECS().CreateEntity(core::ENGINE->GetECS().GetUniqueName(name));
			if (!id.IsValid())
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not spawn entity.");
				return id;
			}

			gameplay::Entity* entity = core::ENGINE->GetECS().GetEntity(id);

			bool isActive = true;
			if (!srcData.GetBool(JSON_SCENE_ENTITIES_VAR_ACTIVE, isActive))
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read active state of entity in prefab data. Defaulting to true.");
			}
			entity->SetIsActive(isActive);
			
			resources::SrcData componentsSrc;
			if (!srcData.GetSrcObject(JSON_SCENE_ENTITIES_VAR_COMPONENTS, componentsSrc))
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Entity did not have any components in prefab data.");
			}

			// Create all components.
			for (gameplay::AbstractECSSystem* system : core::ENGINE->GetECS().GetSystems())
			{
				if (componentsSrc.HasSrcObject(system->GetPropertyName()))
				{
					resources::SrcData componentSrc;
					if (!componentsSrc.GetSrcObject(system->GetPropertyName(), componentSrc))
					{
						continue;
					}

					const gameplay::Component* component = system->CreateBaseComponent(id, componentSrc);
				}
			}

			return id;
		}
	}
}