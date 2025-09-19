#include "gameplay/systems/PlayerSystem.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

// gameplay includes
#include "gameplay/systems/TransformSystem.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// PlayerSystem
		//---------------------------------------------------------------------
		bool PlayerSystem::Initialize()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Player system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string PlayerSystem::GetPropertyName() const
		{
			return "player";
		}

		//---------------------------------------------------------------------
		std::string PlayerSystem::GetSystemName() const
		{
			std::string name = "Player";
		#ifdef _EDITOR
			name = std::string(font::ICON_SETTINGS) + " " + name;
		#endif // _EDITOR
			return name;
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

		//---------------------------------------------------------------------
		Key w('W'), a('A'), s('S'), d('D');
		float speed = 225.0f;
		void PlayerSystem::UpdateComponentsRealtime(float a_fDeltaTime)
		{
			TransformSystem& transformSys = core::TOOL->GetECS().GetSystem<TransformSystem>();

			bool wDown = w.isKey();
			bool aDown = a.isKey();
			bool sDown = s.isKey();
			bool dDown = d.isKey();

			for (auto& component : m_mComponents)
			{
				const Entity* ent = core::TOOL->GetECS().GetEntity(component.first);

				if (ent)
				{
					TransformComponent& transformComp = transformSys.GetComponent(ent->GetEntityID());

					float deltaSpeed = speed * a_fDeltaTime;
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
}