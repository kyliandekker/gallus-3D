// header
#include "Scene.h"

// core
#include "core/Engine.h"
#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/ECSBaseSystem.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Scene
		//---------------------------------------------------------------------
		bool Scene::Destroy()
		{
			if (!EngineResource::Destroy())
			{
				return false;
			}

			m_Data = core::Data();

			return true;
		}

		//---------------------------------------------------------------------
		bool Scene::LoadByName(const std::string& a_sName)
		{
			if (!EngineResource::LoadByName(a_sName))
			{
				return false;
			}
			m_AssetType = resources::AssetType::Scene;

			return true;
		}

		//---------------------------------------------------------------------
		bool Scene::LoadByPath(const fs::path& a_Path)
		{
			if (!EngineResource::LoadByPath(a_Path))
			{
				return false;
			}
			m_AssetType = resources::AssetType::Scene;
			
			return file::LoadFile(m_Path, m_Data);
		}

		//---------------------------------------------------------------------
		bool Scene::LoadData()
		{
			// Clear all entities.
			core::ENGINE->GetECS().Clear();
			while (!core::ENGINE->GetECS().GetEntities().empty())
			{
			}

			if (m_Data.empty())
			{
				return false;
			}

#ifdef _EDITOR
			m_bIsDirty = false;
#endif // _EDITOR

			resources::SrcData srcData(m_Data);
			if (!srcData.IsValid())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}
			
			if (!srcData.GetSrcArray(JSON_SCENE_ENTITIES_VAR, srcData))
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}
			
			if (!srcData.IsValid())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}

			size_t arraySize = 0;
			if (!srcData.GetArraySize(arraySize))
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}

			for (size_t i = 0; i < arraySize; i++)
			{
				resources::SrcData entitySrc;
				if (!srcData.GetSrcArrayElement(i, entitySrc))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load entity index %i in scene data.", i);
					continue;
				}

				if (!entitySrc.IsValid())
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load entity index %i in scene data.", i);
					continue;
				}

				std::string name = core::ENGINE->GetECS().GetUniqueName("New GameObject");
				if (!entitySrc.GetString(JSON_SCENE_ENTITIES_VAR_NAME, name))
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read name of entity index %i in scene data. Defaulting to using name \"\".", i, name.c_str());
				}

				gameplay::EntityID id = core::ENGINE->GetECS().CreateEntity(core::ENGINE->GetECS().GetUniqueName(name));
				if (!id.IsValid())
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not spawn entity index %i.", i);
					continue;
				}

				gameplay::Entity* entity = core::ENGINE->GetECS().GetEntity(id);

				bool isActive = true;
				if (!entitySrc.GetBool(JSON_SCENE_ENTITIES_VAR_ACTIVE, isActive))
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read active state of entity index %i in scene data. Defaulting to true.", i);
				}
				entity->SetIsActive(isActive);

				resources::SrcData componentsSrc;
				if (!entitySrc.GetSrcObject(JSON_SCENE_ENTITIES_VAR_COMPONENTS, componentsSrc))
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Entity index %i did not have any components in scene data.", i);
					continue;
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
			}

			return true;
		}

#ifdef _EDITOR
		//---------------------------------------------------------------------
		bool Scene::Save(const fs::path& a_Path)
		{
			m_Path = a_Path;
			return Save();
		}

		//---------------------------------------------------------------------
		bool Scene::Save()
		{
			if (m_Path.empty())
			{
				return false;
			}

			core::Data data = GetSceneData();

			if (!file::SaveFile(m_Path, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to save scene file \"%s\".", m_Path.generic_string().c_str());
				return false;
			}

#ifdef _EDITOR
			m_bIsDirty = false;
#endif // _EDITOR
			return true;
		}
#endif // _EDITOR

		//---------------------------------------------------------------------
		const core::Data& Scene::GetData() const
		{
			return m_Data;
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		const core::Data Scene::GetSceneData() const
		{
			resources::SrcData srcData = resources::SrcData();
			srcData.SetObject();

			resources::SrcData entitiesSrc = resources::SrcData();
			entitiesSrc.SetArray();

			for (gameplay::Entity& entity : core::ENGINE->GetECS().GetEntities())
			{
				resources::SrcData entitySrc = resources::SrcData();
				entitySrc.SetObject();

				entitySrc.SetString(JSON_SCENE_ENTITIES_VAR_NAME, entity.GetName());
				entitySrc.SetBool(JSON_SCENE_ENTITIES_VAR_ACTIVE, entity.IsActive());

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
				entitySrc.SetSrcObject(JSON_SCENE_ENTITIES_VAR_COMPONENTS, componentsSrc);

				entitiesSrc.PushArraySrcObject(entitySrc);
			}

			srcData.SetSrcObject(JSON_SCENE_ENTITIES_VAR, entitiesSrc);

			core::Data data;
			srcData.GetData(data);
			return data;
		}
#endif // _EDITOR
	}
}