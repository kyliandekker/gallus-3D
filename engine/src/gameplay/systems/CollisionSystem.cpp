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
			m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_RESOLVE);
			m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_END);

			LOG_ICON(font::ICON_GIZMO_BOUNDS, LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Collision system initialized.");
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
			name = std::string(font::ICON_GIZMO_BOUNDS) + " " + name;
		#endif // _EDITOR
			return name;
		}

		//---------------------------------------------------------------------
		//std::vector<CollisionInfo> CollisionSystem::GetCollisions(EntityID a_EntityID) const
		//{
		//	std::vector<CollisionInfo> results;

		//	for (auto& [pair, info] : m_mCollisions)
		//	{
		//		if (pair.a == a_EntityID)
		//		{
		//			results.push_back({ info.a, info.b, info.m_CollisionType, info.m_vNormal });
		//		}
		//	}

		//	return results;
		//}

		//---------------------------------------------------------------------
		void CollisionSystem::Collide(ColliderComponent& a_ColliderA, ColliderComponent& a_ColliderB, const DirectX::XMFLOAT2& a_vNormal)
		{
			//m_mNewCollisions.insert({ a_ColliderA.GetEntityID(), a_ColliderB.GetEntityID(), CollisionType::COLLISION_TYPE_NONE, a_vNormal });
		}

		//---------------------------------------------------------------------
		void CollisionSystem::UpdateComponentsRealtime(float a_fDeltatime, UpdateTime a_UpdateTime)
		{
			ECSBaseSystem::UpdateComponentsRealtime(a_fDeltatime, a_UpdateTime);
			//
			//if (a_UpdateTime == UpdateTime::UPDATE_TIME_FRAME_END)
			//{
			//	std::map<CollisionEntry, CollisionInfo> tempMap;

			//	// Remove all events that exited.
			//	for (auto it = m_mCollisions.begin(); it != m_mCollisions.end(); )
			//	{
			//		if (it->second.m_CollisionType == CollisionType::COLLISION_TYPE_EXIT)
			//		{
			//			it = m_mCollisions.erase(it); // erase returns next iterator
			//		}
			//		else
			//		{
			//			++it;
			//		}
			//	}

			//	for (auto& newCollision : m_mNewCollisions)
			//	{
			//		CollisionInfo info = newCollision;

			//		// If it is already in the map, it means it already started.
			//		if (m_mCollisions.contains(newCollision))
			//		{
			//			info.m_CollisionType = CollisionType::COLLISION_TYPE_STAY;
			//		}
			//		// If it was not present, set it to start.
			//		else
			//		{
			//			info.m_CollisionType = CollisionType::COLLISION_TYPE_START;
			//		}
			//		tempMap.insert(std::make_pair(newCollision, info));
			//	}

			//	// Set all the ones that previously had collision to exit.
			//	for (auto& prePair : m_mCollisions)
			//	{
			//		if (!m_mNewCollisions.contains(prePair.second))
			//		{
			//			CollisionInfo info = m_mCollisions[prePair.second];
			//			info.m_CollisionType = CollisionType::COLLISION_TYPE_EXIT;
			//			tempMap.insert(std::make_pair(prePair.first, info));
			//		}
			//	}

			//	m_mCollisions = std::move(tempMap);
			//	m_mNewCollisions.clear();
			//}
		}
	}
}
