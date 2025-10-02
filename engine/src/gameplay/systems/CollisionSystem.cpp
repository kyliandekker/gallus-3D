// header
#include "CollisionSystem.h"

// graphics
#include "graphics/imgui/font_icon.h"

// logger
#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// CollisionSystem
		//---------------------------------------------------------------------
		bool CollisionSystem::Initialize()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Collision system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string CollisionSystem::GetPropertyName() const
		{
			return "collision";
		}

		//---------------------------------------------------------------------
		std::string CollisionSystem::GetSystemName() const
		{
			std::string name = "Collision";
		#ifdef _EDITOR
			name = std::string(font::ICON_BOUNDS) + " " + name;
		#endif // _EDITOR
			return name;
		}

		//---------------------------------------------------------------------
		std::vector<CollisionEvent> CollisionSystem::GetCollisions(EntityID a_EntityID) const
		{
			std::vector<CollisionEvent> results;

			for (auto& [pair, type] : m_mCollision)
			{
				if (pair.a == a_EntityID)
				{
					results.push_back({ pair.a, pair.b, type });
				}
				if (pair.b == a_EntityID)
				{
					results.push_back({ pair.b, pair.a, type });
				}
			}

			return results;
		}

		//---------------------------------------------------------------------
		void CollisionSystem::Collide(ColliderComponent& a_ColliderA, ColliderComponent& a_ColliderB)
		{
			m_mNewCollision.insert({ a_ColliderA.GetEntityID(), a_ColliderB.GetEntityID() });
		}

		//---------------------------------------------------------------------
		void CollisionSystem::UpdateComponentsRealtime(float a_fDeltatime)
		{
			ECSBaseSystem::UpdateComponentsRealtime(a_fDeltatime);

			std::map<CollisionInfo, CollisionType> tempMap;

			// Remove all events that exited.
			for (auto it = m_mCollision.begin(); it != m_mCollision.end(); )
			{
				if (it->second == CollisionType::COLLISION_TYPE_EXIT)
				{
					it = m_mCollision.erase(it); // erase returns next iterator
				}
				else
				{
					++it;
				}
			}

			for (auto& newCollision : m_mNewCollision)
			{
				CollisionType collisionType;

				// If it is already in the map, it means it already started.
				if (m_mCollision.contains(newCollision))
				{
					collisionType = CollisionType::COLLISION_TYPE_STAY;
				}
				// If it was not present, set it to start.
				else
				{
					collisionType = CollisionType::COLLISION_TYPE_START;
				}
				tempMap.insert(std::make_pair(newCollision, collisionType));
			}

			// Set all the ones that previously had collision to exit.
			for (auto& prePair : m_mCollision)
			{
				if (!m_mNewCollision.contains(prePair.first))
				{
					tempMap.insert(std::make_pair(prePair.first, CollisionType::COLLISION_TYPE_EXIT));
				}
			}

			m_mCollision = std::move(tempMap);
			m_mNewCollision.clear();
		}
	}
}