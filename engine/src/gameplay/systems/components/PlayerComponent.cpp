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

			a_Document.AddMember(
				JSON_PLAYER_COMPONENT_PREFAB_NAME,
				rapidjson::Value(m_pBulletPrefab.GetPath().filename().generic_string().c_str(), a_Allocator),
				a_Allocator
			);
		}
#endif

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

		float bulletSpeed = 5;
		Key w('W'), a('A'), s('S'), d('D'), left(VK_LEFT), right(VK_RIGHT), up(VK_UP), down(VK_DOWN);
		void PlayerComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			AnimationSystem& animationSys = core::ENGINE->GetECS().GetSystem<AnimationSystem>();
			AnimationComponent& animationComp = animationSys.GetComponent(m_EntityID);

			bool leftDown = left.isKeyDown();
			bool rightDown = right.isKeyDown();
			if (leftDown || rightDown)
			{
				gameplay::EntityID id = m_pBulletPrefab.Instantiate();

				ProjectileSystem& projectileSystem = core::ENGINE->GetECS().GetSystem<ProjectileSystem>();

				CollisionSystem& collisionSys = core::ENGINE->GetECS().GetSystem<CollisionSystem>();
				collisionSys.GetComponent(id).IgnoreEntity(id);
				collisionSys.GetComponent(id).IgnoreEntity(m_EntityID);

				TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
				const DirectX::XMFLOAT2& pos = transformSys.GetComponent(m_EntityID).Transform().GetPosition();
				TransformComponent& transformComp = transformSys.GetComponent(id);
				transformComp.Transform().SetPosition(pos);
				
				if (leftDown)
				{
					transformComp.Transform().SetRotation(-180);
					projectileSystem.GetComponent(id).SetMovementSpeed({ -bulletSpeed, 0 });
				}
				else if (rightDown)
				{
					projectileSystem.GetComponent(id).SetMovementSpeed({ bulletSpeed, 0 });
				}
			}

			float deltaSpeed = m_fSpeed * a_fDeltaTime;

			DirectX::XMFLOAT2 movement = { 0.0f, 0.0f };

			bool wDown = w.isKey();
			bool aDown = a.isKey();
			bool sDown = s.isKey();
			bool dDown = d.isKey();
			if (wDown)
			{
				movement.y -= 1.0f;
				animationComp.LoadAnimation("player_walk_up.anim");
			}
			if (sDown)
			{
				movement.y += 1.0f;
				animationComp.LoadAnimation("player_walk_down.anim");
			}
			if (aDown)
			{
				movement.x -= 1.0f;
				animationComp.LoadAnimation("player_walk_left.anim");
			}
			if (dDown)
			{
				animationComp.LoadAnimation("player_walk_right.anim");
				movement.x += 1.0f;
			}
			if (wDown && dDown)
			{
				animationComp.LoadAnimation("player_walk_up_right.anim");
			}
			if (wDown && aDown)
			{
				animationComp.LoadAnimation("player_walk_up_left.anim");
			}
			if (sDown && aDown)
			{
				animationComp.LoadAnimation("player_walk_down_left.anim");
			}
			if (sDown && dDown)
			{
				animationComp.LoadAnimation("player_walk_down_right.anim");
			}
			
			if (movement.x == 0.0f && movement.y == 0.0f)
			{
				animationComp.LoadAnimation("player_walk_default.anim");
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
