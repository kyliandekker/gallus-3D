#include "gameplay/systems/components/PlayerComponent.h"

#include <rapidjson/utils.h>

// engine includes
#include "core/Engine.h"

#include "resources/SrcData.h"

// gameplay includes
#include "gameplay/Entity.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/ProjectileSystem.h"

#define JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR "movementSpeed"
#define JSON_PLAYER_COMPONENT_PREFAB_NAME "bulletPrefab"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// PlayerComponent
		//---------------------------------------------------------------------
		void PlayerComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR, m_fSpeed, a_Allocator);

			a_Document.AddMember(
				JSON_PLAYER_COMPONENT_PREFAB_NAME,
				rapidjson::Value(m_pBulletPrefab.GetPath().filename().generic_string().c_str(), a_Allocator),
				a_Allocator
			);
		}

		//---------------------------------------------------------------------
		void PlayerComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_fSpeed = a_SrcData.GetFloat(JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR);
			core::ENGINE->GetResourceAtlas().LoadPrefab(a_SrcData.GetString(JSON_PLAYER_COMPONENT_PREFAB_NAME), m_pBulletPrefab);
			m_pBulletPrefab.Load();
		}

#include <Windows.h>

		struct Key
		{
			char key;
			bool pressed = false;

			Key(char key) : key(key)
			{

			}

			bool isKeyDown()
			{
				if (GetKeyState(key) & 0x8000)
				{
					if (!pressed)
					{
						pressed = true;
						return true;
					}
					return false;
				}
				else
				{
					pressed = false;
				}
				return false;
			}

			bool isKey()
			{
				return GetKeyState(key) & 0x8000;
			}
		};

		float bulletSpeed = 50;
		Key w('W'), a('A'), s('S'), d('D'), left(VK_LEFT), right(VK_RIGHT), up(VK_UP), down(VK_DOWN);
		void PlayerComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			bool leftDown = left.isKeyDown();
			bool rightDown = right.isKeyDown();
			if (leftDown || rightDown)
			{
				gameplay::EntityID id = m_pBulletPrefab.Instantiate();

				ProjectileSystem& projectileSystem = core::ENGINE->GetECS().GetSystem<ProjectileSystem>();
				projectileSystem.GetComponent(id).SetMovementSpeed({ bulletSpeed, 0 });

				CollisionSystem& collisionSys = core::ENGINE->GetECS().GetSystem<CollisionSystem>();
				collisionSys.GetComponent(id).IgnoreEntity(id);
				collisionSys.GetComponent(id).IgnoreEntity(m_EntityID);

				TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
				const DirectX::XMFLOAT2& pos = transformSys.GetComponent(m_EntityID).Transform().GetPosition();
				TransformComponent& transformComp = transformSys.GetComponent(id);
				transformComp.Transform().SetPosition(pos);

				if (leftDown)
				{
					transformComp.Transform().SetRotation(180);
				}
			}

			float deltaSpeed = m_fSpeed * a_fDeltaTime;

			DirectX::XMFLOAT2 movement = { 0.0f, 0.0f };
			if (w.isKey())
			{
				movement.y -= 1.0f;
			}
			if (s.isKey())
			{
				movement.y += 1.0f;
			}
			if (a.isKey())
			{
				movement.x -= 1.0f;
			}
			if (d.isKey())
			{
				movement.x += 1.0f;
			}

			if (movement.x == 0.0f && movement.y == 0.0f)
			{
				return; 
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
