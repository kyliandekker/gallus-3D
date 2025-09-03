#include "Scene.h"

#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// core includes
#include "core/Tool.h"
#include "core/DataStream.h"

// logger includes
#include "logger/Logger.h"

// gameplay includes
#include "gameplay/ECSBaseSystem.h"

#define JSON_SCENE_ENTITIES_VAR "entities"
#define JSON_SCENE_ENTITIES_VAR_NAME "name"
#define JSON_SCENE_ENTITIES_VAR_ACTIVE "isActive"
#define JSON_SCENE_ENTITIES_VAR_COMPONENTS "components"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Scene
		//---------------------------------------------------------------------
		Scene::~Scene()
		{
			// Clear all entities.
			core::TOOL->GetECS().Clear();
		}

		//---------------------------------------------------------------------
		bool Scene::LoadData()
		{
#ifdef _EDITOR
			m_fIsDirty = false;
#endif // _EDITOR

			rapidjson::Document document;
			document.Parse(m_Data.dataAs<const char>(), m_Data.size());

			if (document.HasParseError())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}

			// Clear all entities.
			core::TOOL->GetECS().Clear();
			while (!core::TOOL->GetECS().GetEntities().empty())
			{
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

					std::string name = core::TOOL->GetECS().GetUniqueName("New GameObject");
					if (entityDoc.HasMember(JSON_SCENE_ENTITIES_VAR_NAME) && entityDoc[JSON_SCENE_ENTITIES_VAR_NAME].IsString())
					{
						name = entityDoc[JSON_SCENE_ENTITIES_VAR_NAME].GetString();
					}

					gameplay::EntityID id = core::TOOL->GetECS().CreateEntity(core::TOOL->GetECS().GetUniqueName(name));
					gameplay::Entity* entity = core::TOOL->GetECS().GetEntity(id);
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
					for (gameplay::AbstractECSSystem* system : core::TOOL->GetECS().GetSystems())
					{
						if (componentsDoc.HasMember(system->GetPropertyName().c_str()))
						{
							gameplay::Component* component = system->GetBaseComponent(id);
							if (!component)
							{
								continue;
							}

							const rapidjson::Value& testMember = componentsDoc[system->GetPropertyName().c_str()];
							component->Deserialize(testMember, document.GetAllocator());
						}
					}
				}
			}

#ifdef _EDITOR
			m_fIsDirty = true;
#endif // _EDITOR
			return true;
		}

#ifdef _EDITOR
		//---------------------------------------------------------------------
		bool Scene::SaveData()
		{
			if (m_ScenePath.empty())
			{
				return false;
			}

			rapidjson::Document a_Document;
			a_Document.SetObject();
			rapidjson::Document::AllocatorType& allocator = a_Document.GetAllocator();

			a_Document.AddMember(JSON_SCENE_ENTITIES_VAR, rapidjson::Value().SetArray(), allocator);
			for (gameplay::Entity& entity : core::TOOL->GetECS().GetEntities())
			{
				rapidjson::Document entityDoc;
				entityDoc.SetObject();

				rapidjson::SetOrAddMember(entityDoc, JSON_SCENE_ENTITIES_VAR_NAME, entity.GetName().c_str(), allocator);
				entityDoc.AddMember(JSON_SCENE_ENTITIES_VAR_ACTIVE, entity.IsActive(), allocator);

				rapidjson::Document componentsDoc;
				componentsDoc.SetObject();

				for (gameplay::AbstractECSSystem* system : core::TOOL->GetECS().GetSystemsContainingEntity(entity))
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

			if (!file::SaveFile(m_ScenePath, core::DataStream(buffer.GetString(), buffer.GetSize())))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to save scene file \"%s\".", m_ScenePath.generic_string().c_str());
				return false;
			}

			m_fIsDirty = false;
			return true;
		}
#endif // _EDITOR

		//---------------------------------------------------------------------
		void Scene::SetData(const core::Data& a_Data)
		{
			m_Data = a_Data;
		}

		//---------------------------------------------------------------------
		const core::Data& Scene::GetData() const
		{
			return m_Data;
		}

#ifdef _EDITOR
		//---------------------------------------------------------------------
		const fs::path& Scene::GetScenePath() const
		{
			return m_ScenePath;
		}

		//---------------------------------------------------------------------
		void Scene::SetScenePath(const fs::path& a_ScenePath)
		{
			m_ScenePath = a_ScenePath;
		}

		//---------------------------------------------------------------------
		const core::Observable<bool>& Scene::IsDirty() const
		{
			return m_fIsDirty;
		}

		//---------------------------------------------------------------------
		void Scene::SetIsDirty(bool a_fIsDirty)
		{
			m_fIsDirty = a_fIsDirty;
		}
#endif // _EDITOR
	}
}