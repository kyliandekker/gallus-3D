// header
#include "Prefab.h"

// external
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// core
#include "core/DataStream.h"

#include "resources/SrcData.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/ECSBaseSystem.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		bool Prefab::LoadData()
		{
#ifdef _EDITOR
			m_fIsDirty = false;
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
			m_fIsDirty = true;
#endif // _EDITOR
			return true;
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		const core::Data Prefab::GetSceneData() const
		{
			rapidjson::Document a_Document;
			a_Document.SetObject();
			rapidjson::Document::AllocatorType& allocator = a_Document.GetAllocator();

			auto& entities = core::ENGINE->GetECS().GetEntities();
			if (!entities.empty())
			{
				auto& entity = entities[0];

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
				a_Document.CopyFrom(entityDoc, a_Document.GetAllocator());
			}

			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			a_Document.Accept(writer);

			return core::DataStream(buffer.GetString(), buffer.GetSize());
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

			rapidjson::Document document;
			document.Parse(m_Data.dataAs<const char>(), m_Data.size());

			if (document.HasParseError())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return id;
			}

			if (!document.IsObject())
			{
				return id;
			}

			std::string name = core::ENGINE->GetECS().GetUniqueName("New GameObject");
			if (document.HasMember(JSON_SCENE_ENTITIES_VAR_NAME) && document[JSON_SCENE_ENTITIES_VAR_NAME].IsString())
			{
				name = document[JSON_SCENE_ENTITIES_VAR_NAME].GetString();
			}

			id = core::ENGINE->GetECS().CreateEntity(core::ENGINE->GetECS().GetUniqueName(name));
			gameplay::Entity* entity = core::ENGINE->GetECS().GetEntity(id);
			if (!entity)
			{
				return id;
			}

			bool isActive = true;
			if (document.HasMember(JSON_SCENE_ENTITIES_VAR_ACTIVE) && document[JSON_SCENE_ENTITIES_VAR_ACTIVE].IsBool())
			{
				isActive = document[JSON_SCENE_ENTITIES_VAR_ACTIVE].GetBool();
			}
			entity->SetIsActive(isActive);

			auto componentsDoc = document[JSON_SCENE_ENTITIES_VAR_COMPONENTS].GetObject();
			for (gameplay::AbstractECSSystem* system : core::ENGINE->GetECS().GetSystems())
			{
				if (componentsDoc.HasMember(system->GetPropertyName().c_str()))
				{
					gameplay::Component* component = system->GetBaseComponent(id);
					if (!component)
					{
						continue;
					}

					const rapidjson::Value& testMember = componentsDoc[system->GetPropertyName().c_str()];
					component->Deserialize(resources::SrcData(testMember));

					component->InitRealtime();
				}
			}

			return id;
		}
	}
}