// header
#include "Scene.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/Camera.h"
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/DirectionalLight.h"

// logger
#include "logger/Logger.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/Entity.h"
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/ECSBaseSystem.h"

namespace gallus::gameplay
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
    bool Scene::LoadData(const core::Data& a_Data)
    {
		m_AssetType = resources::AssetType::Scene;

		m_Data = a_Data;

        return true;
    }

    //---------------------------------------------------------------------
	bool Scene::LoadData()
	{
		// Clear all entities.
		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return false;
		}

		ecs->Clear();
		while (!ecs->GetEntities().empty())
		{
		}

		if (m_Data.empty())
		{
			return false;
		}

		resources::SrcData srcData(m_Data);
		if (!srcData.IsValid())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading scene data: Something went wrong when trying to load scene data.");
			return false;
		}

		resources::SrcData cameraSrcData;
		cameraSrcData.SetObject();
		
		graphics::dx12::DX12System& dx12 = GetDX12System();

		graphics::dx12::Camera& camera = dx12.GetCamera();
		camera.GetTransform().SetRotation(DirectX::XMQuaternionIdentity());
		camera.Init(graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
		if (!srcData.GetSrcObject(JSON_SCENE_CAMERA_VAR, cameraSrcData))
		{
			LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Failed loading scene data: Could not read camera settings in scene data.");
		}
		else
		{
			DeserializeFields(&camera, cameraSrcData);
		}
		
		resources::SrcData directionalLightSrcData;
		cameraSrcData.SetObject();

		if (!srcData.GetSrcObject(JSON_SCENE_DIRECTIONAL_LIGHT_VAR, directionalLightSrcData))
		{
			LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_GAME, "Failed loading scene data: Could not read directional light settings in scene data.");
		}
		else if (std::shared_ptr<graphics::dx12::DirectionalLight> directionalLight = dx12.GetDirectionalLight().lock())
		{
			DeserializeFields(directionalLight.get(), directionalLightSrcData);
		}
		
		resources::SrcData entitiesSrc;
		if (!srcData.GetSrcArray(JSON_SCENE_ENTITIES_VAR, entitiesSrc))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading scene data: Could not read entities array in scene data.");
			return false;
		}
		
		if (!entitiesSrc.IsValid())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading scene data: Invalid entities array in scene data.");
			return false;
		}

		size_t arraySize = 0;
		if (!entitiesSrc.GetArraySize(arraySize))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading scene data: Could not get size of entities array in scene data.");
			return false;
		}

		for (size_t i = 0; i < arraySize; i++)
		{
			resources::SrcData entitySrc;
			if (!entitiesSrc.GetSrcArrayElement(i, entitySrc))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading scene data: Something went wrong when trying to load entity index %i in scene data.", i);
				continue;
			}

			if (!entitySrc.IsValid())
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading scene data: Invalid entity data for entity index %i in scene data.", i);
				continue;
			}

			ecs->CreateEntity(entitySrc);
		}

		return true;
	}

	//---------------------------------------------------------------------
	const core::Data& Scene::GetData() const
	{
		return m_Data;
	}

	//---------------------------------------------------------------------
	const core::Data Scene::GetSceneData() const
	{
		resources::SrcData srcData = resources::SrcData();
		srcData.SetObject();

		graphics::dx12::DX12System& dx12 = GetDX12System();

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

		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return core::Data();
		}

		for (gameplay::EntityID entityID : ecs->GetEntities())
		{
			resources::SrcData entitySrc = resources::SrcData();
			entitySrc.SetObject();

			ecs->SerializeEntity(entityID, entitySrc);
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
}