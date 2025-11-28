// header
#include "Scene.h"

// external
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// core
#include "core/Engine.h"
#include "core/DataStream.h"

#include "resources/SrcData.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

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

			rapidjson::Document document;
			document.Parse(m_Data.dataAs<const char>(), m_Data.size());

			if (document.HasParseError())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}

			// Load entities and systems.
			if (document.HasMember(JSON_SCENE_ENTITIES_VAR) && document[JSON_SCENE_ENTITIES_VAR].IsArray())
			{
				for (auto& entityDoc : document[JSON_SCENE_ENTITIES_VAR].GetArray())
				{
					if (!entityDoc.IsObject())
					{
						continue;
					}

					std::string name = core::ENGINE->GetECS().GetUniqueName("New GameObject");
					if (entityDoc.HasMember(JSON_SCENE_ENTITIES_VAR_NAME) && entityDoc[JSON_SCENE_ENTITIES_VAR_NAME].IsString())
					{
						name = entityDoc[JSON_SCENE_ENTITIES_VAR_NAME].GetString();
					}

					gameplay::EntityID id = core::ENGINE->GetECS().CreateEntity(core::ENGINE->GetECS().GetUniqueName(name));
					gameplay::Entity* entity = core::ENGINE->GetECS().GetEntity(id);
					if (!entity)
					{
						continue;
					}

					bool isActive = true;
					if (entityDoc.HasMember(JSON_SCENE_ENTITIES_VAR_ACTIVE) && entityDoc[JSON_SCENE_ENTITIES_VAR_ACTIVE].IsBool())
					{
						isActive = entityDoc[JSON_SCENE_ENTITIES_VAR_ACTIVE].GetBool();
					}
					entity->SetIsActive(isActive);

					auto componentsDoc = entityDoc[JSON_SCENE_ENTITIES_VAR_COMPONENTS].GetObject();
					for (gameplay::AbstractECSSystem* system : core::ENGINE->GetECS().GetSystems())
					{
						if (componentsDoc.HasMember(system->GetPropertyName().c_str()))
						{
							const rapidjson::Value& srcData = componentsDoc[system->GetPropertyName().c_str()];
							const gameplay::Component* component = system->CreateBaseComponent(id, srcData);
							if (!component)
							{
								continue;
							}
						}
					}
				}
			}

			return true;
		}

#ifdef _EDITOR
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
			rapidjson::Document a_Document;
			a_Document.SetObject();
			rapidjson::Document::AllocatorType& allocator = a_Document.GetAllocator();

			a_Document.AddMember(JSON_SCENE_ENTITIES_VAR, rapidjson::Value().SetArray(), allocator);
			for (gameplay::Entity& entity : core::ENGINE->GetECS().GetEntities())
			{
				rapidjson::Document entityDoc;
				entityDoc.SetObject();

				rapidjson::SetOrAddMember(entityDoc, JSON_SCENE_ENTITIES_VAR_NAME, entity.GetName().c_str(), allocator);
				entityDoc.AddMember(JSON_SCENE_ENTITIES_VAR_ACTIVE, entity.IsActive(), allocator);

				rapidjson::Document componentsDoc;
				componentsDoc.SetObject();

				for (gameplay::AbstractECSSystem* system : core::ENGINE->GetECS().GetSystemsContainingEntity(entity))
				{
					rapidjson::Value key(system->GetPropertyName().c_str(), allocator);

					rapidjson::Document componentDoc;
					componentDoc.SetObject();

					const gameplay::Component* component = system->GetBaseComponent(entity.GetEntityID());
					componentsDoc.AddMember(key, rapidjson::Value().SetObject(), allocator);
					component->Serialize(componentsDoc[system->GetPropertyName().c_str()], allocator);
				}

				entityDoc.AddMember(JSON_SCENE_ENTITIES_VAR_COMPONENTS, componentsDoc, allocator);
				a_Document[JSON_SCENE_ENTITIES_VAR].PushBack(entityDoc, allocator);
			}

			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			a_Document.Accept(writer);

			return core::DataStream(buffer.GetString(), buffer.GetSize());
		}
#endif // _EDITOR
	}
}