#include "gameplay/systems/components/PlayerComponent.h"

#include <rapidjson/utils.h>

#include "core/Engine.h"

#include "gameplay/Entity.h"
#include "gameplay/systems/TransformSystem.h"

#define JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR "movementSpeed"

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
		}

		//---------------------------------------------------------------------
		void PlayerComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			if (a_Document.HasMember(JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR) && a_Document[JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR].IsFloat())
			{
				rapidjson::GetFloat(a_Document, JSON_PLAYER_COMPONENT_MOVEMENT_SPEED_VAR, m_fSpeed);
			}
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

		Key w('W'), a('A'), s('S'), d('D');
		//---------------------------------------------------------------------
		void PlayerComponent::Update(float a_fDeltaTime)
		{
			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

			bool wDown = w.isKey();
			bool aDown = a.isKey();
			bool sDown = s.isKey();
			bool dDown = d.isKey();

			const Entity* ent = core::ENGINE->GetECS().GetEntity(m_EntityID);

			if (ent)
			{
				TransformComponent& transformComp = transformSys.GetComponent(ent->GetEntityID());

				float deltaSpeed = m_fSpeed * a_fDeltaTime;
				auto pos = transformComp.Transform().GetPosition();
				if (wDown)
				{
					pos.y -= deltaSpeed;
					transformComp.Transform().SetPosition(pos);
				}
				if (aDown)
				{
					pos.x -= deltaSpeed;
					transformComp.Transform().SetPosition(pos);
				}
				if (sDown)
				{
					pos.y += deltaSpeed;
					transformComp.Transform().SetPosition(pos);
				}
				if (dDown)
				{
					pos.x += deltaSpeed;
					transformComp.Transform().SetPosition(pos);
				}
			}
		}
	}
}