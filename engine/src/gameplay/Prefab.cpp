// header
#include "Prefab.h"

// core
#include "core/DataStream.h"
#include "core/Engine.h"

// logger
#include "logger/Logger.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
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
			// Clear all entities.
			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return false;
			}

			ecs->Clear();
			while (!ecs->GetEntities().empty())
			{
			}

			if (!Instantiate().IsValid())
			{
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		const core::Data Prefab::GetSceneData() const
		{
			resources::SrcData srcData = resources::SrcData();
			srcData.SetObject();

			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return core::Data();
			}

			auto entities = ecs->GetEntities();
			if (!entities.empty())
			{
				gameplay::EntityID entityID = entities[0];
				std::weak_ptr<gameplay::Entity> entity = ecs->GetEntity(entityID);
				std::shared_ptr<gameplay::Entity> ent = entity.lock();
				if (!entity.lock())
				{
					return core::Data();
				}

				srcData.SetString(JSON_SCENE_ENTITIES_VAR_NAME, ent->GetName());
				srcData.SetBool(JSON_SCENE_ENTITIES_VAR_ACTIVE, ent->IsActive());

				resources::SrcData componentsSrc = resources::SrcData();
				componentsSrc.SetObject();

				for (gameplay::AbstractECSSystem* system : ecs->GetSystemsContainingEntity(entityID))
				{
					resources::SrcData componentSrc = resources::SrcData();
					componentSrc.SetObject();

					const gameplay::Component* component = system->GetBaseComponent(entityID);
					component->Serialize(componentSrc);

					componentsSrc.SetSrcObject(system->GetPropertyName(), componentSrc);
				}
				srcData.SetSrcObject(JSON_SCENE_ENTITIES_VAR_COMPONENTS, componentsSrc);
			}
			
			core::Data data;
			srcData.GetData(data);
			return data;
		}
#endif // _EDITOR

		//---------------------------------------------------------------------
		gameplay::EntityID Prefab::Instantiate() const
		{
			resources::SrcData srcData(m_Data);
			if (!srcData.IsValid())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return EntityID();
			}

			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return EntityID();
			}

			std::string name = ecs->GetUniqueName("New GameObject");
			if (!srcData.GetString(name, JSON_SCENE_ENTITIES_VAR_NAME))
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read name of entity in prefab data. Defaulting to using name \"\".", name.c_str());
			}

			EntityID id = ecs->CreateEntity(ecs->GetUniqueName(name));
			if (!id.IsValid())
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not spawn entity.");
				return id;
			}

			std::weak_ptr<gameplay::Entity> entity = ecs->GetEntity(id);
			auto ent = entity.lock();
			if (!ent)
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not spawn entity.");
				return id;
			}

			bool isActive = true;
			if (!srcData.GetBool(isActive, JSON_SCENE_ENTITIES_VAR_ACTIVE))
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read active state of entity in prefab data. Defaulting to true.");
			}
			ent->SetIsActive(isActive);
			
			resources::SrcData componentsSrc;
			if (!srcData.GetSrcObject(JSON_SCENE_ENTITIES_VAR_COMPONENTS, componentsSrc))
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Entity did not have any components in prefab data.");
			}

			// Create all components.
			for (gameplay::AbstractECSSystem* system : ecs->GetSystems())
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