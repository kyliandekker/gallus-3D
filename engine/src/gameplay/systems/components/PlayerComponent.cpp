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

			if (left || right || up || down)
			{
				if (auto bulletPrefab = m_pBulletPrefab.lock())
				{
					gameplay::EntityID id = bulletPrefab->Instantiate();

					ProjectileSystem& projectileSystem = core::ENGINE->GetECS().GetSystem<ProjectileSystem>();

					CollisionSystem& collisionSys = core::ENGINE->GetECS().GetSystem<CollisionSystem>();
					collisionSys.GetComponent(id).IgnoreEntity(id);
					collisionSys.GetComponent(id).IgnoreEntity(m_EntityID);

					TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
					const DirectX::XMFLOAT2& pos = transformSys.GetComponent(m_EntityID).Transform().GetPosition();
					TransformComponent& transformComp = transformSys.GetComponent(id);
					transformComp.Transform().SetPosition(pos);

					if (left)
					{
						transformComp.Transform().SetRotation(-180);
						projectileSystem.GetComponent(id).SetMovementSpeed({ -bulletSpeed, 0 });
					}
					else if (right)
					{
						projectileSystem.GetComponent(id).SetMovementSpeed({ bulletSpeed, 0 });
					}
					else if (up)
					{
						transformComp.Transform().SetRotation(-90);
						projectileSystem.GetComponent(id).SetMovementSpeed({ 0, -bulletSpeed });
					}
					else if (down)
					{
						transformComp.Transform().SetRotation(90);
						projectileSystem.GetComponent(id).SetMovementSpeed({ 0, bulletSpeed });
					}
				}
			}

			float deltaSpeed = m_fSpeed * a_fDeltaTime;

			DirectX::XMFLOAT2 movement = { 0.0f, 0.0f };

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

			if (w && d)
			{
				animationComp.LoadAnimation("player_walk_up_right.anim");
				animationComp.Start();
			}
			else if (w && a)
			{
				animationComp.LoadAnimation("player_walk_up_left.anim");
				animationComp.Start();
			}
			else if (s && a)
			{
				animationComp.LoadAnimation("player_walk_down_left.anim");
				animationComp.Start();
			}
			else if (s && d)
			{
				animationComp.LoadAnimation("player_walk_down_right.anim");
				animationComp.Start();
			}
			else if (w)
			{
				animationComp.LoadAnimation("player_walk_up.anim");
				animationComp.Start();
			}
			else if (a)
			{
				animationComp.LoadAnimation("player_walk_left.anim");
				animationComp.Start();
			}
			else if (s)
			{
				animationComp.LoadAnimation("player_walk_down.anim");
				animationComp.Start();
			}
			else if (d)
			{
				animationComp.LoadAnimation("player_walk_right.anim");
				animationComp.Start();
			}
			
			if (movement.x == 0.0f && movement.y == 0.0f)
			{
				animationComp.LoadAnimation("player_walk_default.anim");
				animationComp.Start();
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
