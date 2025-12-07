#include "PlayerComponent.h"

// external
#include <rapidjson/utils.h>

// core
#include "core/Engine.h"

// input
#include "input/InputSystem.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/Entity.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/ProjectileSystem.h"
#include "gameplay/systems/AnimationSystem.h"

#define JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR "movementSpeed"
#define JSON_PLAYER_COMPONENT_PREFAB_NAME "bulletPrefab"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// PlayerComponent
		//---------------------------------------------------------------------
#ifdef _EDITOR
		void PlayerComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR, m_fSpeed, a_Allocator);

			if (auto bulletPrefab = m_pBulletPrefab.lock())
			{
				a_Document.AddMember(
					JSON_PLAYER_COMPONENT_PREFAB_NAME,
					rapidjson::Value(bulletPrefab->GetPath().filename().generic_string().c_str(), a_Allocator),
					a_Allocator
				);
			}
		}
#endif

		//---------------------------------------------------------------------
		void PlayerComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_fSpeed = a_SrcData.GetFloat(JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR);
			std::string bulletPrefabName = a_SrcData.GetString(JSON_PLAYER_COMPONENT_PREFAB_NAME);
			if (!bulletPrefabName.empty())
			{
				m_pBulletPrefab = core::ENGINE->GetResourceAtlas().LoadPrefab(bulletPrefabName);
			}
		}

		float bulletSpeed = 5;
		void PlayerComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			AnimationSystem& animationSys = core::ENGINE->GetECS().GetSystem<AnimationSystem>();
			AnimationComponent& animationComp = animationSys.GetComponent(m_EntityID);

			input::InputSystem& inputSys = core::ENGINE->GetInputSystem();

			bool left = inputSys.IsDoubleDown(VK_LEFT);
			bool right = inputSys.IsDoubleDown(VK_RIGHT);
			bool up = inputSys.IsDoubleDown(VK_UP);
			bool down = inputSys.IsDoubleDown(VK_DOWN);

			bool w = inputSys.IsKey('W');
			bool a = inputSys.IsKey('A');
			bool s = inputSys.IsKey('S');
			bool d = inputSys.IsKey('D');

			float deltaSpeed = m_fSpeed * a_fDeltaTime;

			DirectX::XMFLOAT3 movement = { 0.0f, 0.0f, 0.0f };

			if (w)
			{
				movement.y -= 1.0f;
			}
			if (a)
			{
				movement.x -= 1.0f;
			}
			if (s)
			{
				movement.y += 1.0f;
			}
			if (d)
			{
				movement.x += 1.0f;
			}

			float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
			if (length > 0.0f)
			{
				movement.x = (movement.x / length) * deltaSpeed;
				movement.y = (movement.y / length) * deltaSpeed;
			}

			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

			TransformComponent& movementComp = transformSys.GetComponent(m_EntityID);
			movementComp.Translate(movement);
		}
	}
}
