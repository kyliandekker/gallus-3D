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
			gameplay::EntityComponentSystem& ecs = core::ENGINE->GetECS();
			ecs.Clear();
			while (!ecs.GetEntities().empty())
			{
			}

			if (m_Data.empty())
			{
				return false;
			}

			resources::SrcData srcData(m_Data);
			if (!srcData.IsValid())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}

			resources::SrcData cameraSrcData;
			cameraSrcData.SetObject();
			
			graphics::dx12::DX12System& dx12 = core::ENGINE->GetDX12();

			graphics::dx12::Camera& camera = dx12.GetCamera();
			camera.GetTransform().SetRotation(DirectX::XMQuaternionIdentity());
			camera.Init(graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
			if (!srcData.GetSrcObject(JSON_SCENE_CAMERA_VAR, cameraSrcData))
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read camera settings in scene data.");
			}
			else
			{
				DeserializeFields(&camera, cameraSrcData);
			}
			
			resources::SrcData directionalLightSrcData;
			cameraSrcData.SetObject();

			if (!srcData.GetSrcObject(JSON_SCENE_DIRECTIONAL_LIGHT_VAR, directionalLightSrcData))
			{
				LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read directional light settings in scene data.");
			}
			else if (auto directionalLight = dx12.GetDirectionalLight().lock())
			{
				DeserializeFields(directionalLight.get(), directionalLightSrcData);
			}
			
			resources::SrcData entitiesSrc;
			if (!srcData.GetSrcArray(JSON_SCENE_ENTITIES_VAR, entitiesSrc))
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}
			
			if (!entitiesSrc.IsValid())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}

			size_t arraySize = 0;
			if (!entitiesSrc.GetArraySize(arraySize))
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load scene data.");
				return false;
			}

			for (size_t i = 0; i < arraySize; i++)
			{
				resources::SrcData entitySrc;
				if (!entitiesSrc.GetSrcArrayElement(i, entitySrc))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load entity index %i in scene data.", i);
					continue;
				}

				if (!entitySrc.IsValid())
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load entity index %i in scene data.", i);
					continue;
				}

				std::string name = ecs.GetUniqueName("New GameObject");
				if (!entitySrc.GetString(JSON_SCENE_ENTITIES_VAR_NAME, name))
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read name of entity index %i in scene data. Defaulting to using name \"\".", i, name.c_str());
				}

				gameplay::EntityID id = ecs.CreateEntity(ecs.GetUniqueName(name));
				if (!id.IsValid())
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not spawn entity index %i.", i);
					continue;
				}

				std::weak_ptr<gameplay::Entity> entity = ecs.GetEntity(id);
				auto ent = entity.lock();
				if (!ent)
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not spawn entity.");
					continue;
				}

				bool isActive = true;
				if (!entitySrc.GetBool(JSON_SCENE_ENTITIES_VAR_ACTIVE, isActive))
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Could not read active state of entity index %i in scene data. Defaulting to true.", i);
				}
				ent->SetIsActive(isActive);

				resources::SrcData componentsSrc;
				if (!entitySrc.GetSrcObject(JSON_SCENE_ENTITIES_VAR_COMPONENTS, componentsSrc))
				{
					LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Entity index %i did not have any components in scene data.", i);
					continue;
				}

				// Create all components.
				for (gameplay::AbstractECSSystem* system : ecs.GetSystems())
				{
					if (componentsSrc.HasSrcObject(system->GetPropertyName()))
					{
						resources::SrcData componentSrc;
						if (!componentsSrc.GetSrcObject(system->GetPropertyName(), componentSrc))
						{
							continue;
						}

						system->CreateBaseComponent(id, componentSrc);
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

			graphics::dx12::DX12System& dx12 = core::ENGINE->GetDX12();
			{
				resources::SrcData cameraSrcData;
				cameraSrcData.SetObject();

				SerializeFields(&dx12.GetCamera(), cameraSrcData);
				srcData.SetSrcObject(JSON_SCENE_CAMERA_VAR, cameraSrcData);
			}

			{
				resources::SrcData directionalLightSrcData;
				directionalLightSrcData.SetObject();

				if (auto directionalLight = dx12.GetDirectionalLight().lock())
				{
					SerializeFields(directionalLight.get(), directionalLightSrcData);
					srcData.SetSrcObject(JSON_SCENE_DIRECTIONAL_LIGHT_VAR, directionalLightSrcData);
				}
			}

			resources::SrcData entitiesSrc = resources::SrcData();
			entitiesSrc.SetArray();

			gameplay::EntityComponentSystem& ecs = core::ENGINE->GetECS();
			for (std::weak_ptr<gameplay::Entity> entity : ecs.GetEntities())
			{
				auto ent = entity.lock();
				if (!ent)
				{
					continue;
				}

				resources::SrcData entitySrc = resources::SrcData();
				entitySrc.SetObject();

				entitySrc.SetString(JSON_SCENE_ENTITIES_VAR_NAME, ent->GetName());
				entitySrc.SetBool(JSON_SCENE_ENTITIES_VAR_ACTIVE, ent->IsActive());

				resources::SrcData componentsSrc = resources::SrcData();
				componentsSrc.SetObject();

				for (gameplay::AbstractECSSystem* system : ecs.GetSystemsContainingEntity(ent->GetEntityID()))
				{
					resources::SrcData componentSrc = resources::SrcData();
					componentSrc.SetObject();
					
					const gameplay::Component* component = system->GetBaseComponent(ent->GetEntityID());
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
		
		//---------------------------------------------------------------------
		void Scene::SetSceneData(const core::Data& a_Data)
		{
			m_Data = a_Data;
		}
#endif // _EDITOR
	}
}