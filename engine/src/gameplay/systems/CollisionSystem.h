#pragma once

#include "gameplay/ECSBaseSystem.h"

#include <vector>
#include <map>
#include <set>

#include "core/Event.h"

// gameplay includes
#include "gameplay/systems/components/ColliderComponent.h"

namespace gallus
{
	namespace gameplay
	{
		enum class CollisionType
		{
			COLLISION_TYPE_NONE,
			COLLISION_TYPE_START,
			COLLISION_TYPE_STAY,
			COLLISION_TYPE_EXIT
		};

		struct CollisionInfo
		{
			EntityID a;
			EntityID b;

			bool operator==(const CollisionInfo& other) const noexcept
			{
				return a == other.a && b == other.b;
			}

			bool operator<(const CollisionInfo& other) const noexcept
			{
				return std::tie(a, b) < std::tie(other.a, other.b);
			}
		};

		struct CollisionEvent : public CollisionInfo
		{
			CollisionType m_CollisionType;
		};

		//---------------------------------------------------------------------
		// CollisionSystem
		//---------------------------------------------------------------------
		class CollisionSystem : public ECSBaseSystem<ColliderComponent>
		{
		public:
			/// <summary>
			/// Initializes the system, setting up necessary resources.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			bool Initialize() override;

			/// <summary>
			/// Retrieves the property name of the system (used in serialization).
			/// </summary>
			/// <returns>A string containing the property name of the system.</returns>
			std::string GetPropertyName() const override;

			/// <summary>
			/// Retrieves the name of the system.
			/// </summary>
			/// <returns>A string containing the name of the system.</returns>
			std::string GetSystemName() const override;

			std::vector<CollisionEvent> GetCollisions(EntityID a_EntityID) const;

			void Collide(ColliderComponent& a_ColliderA, ColliderComponent& a_ColliderB);

			void UpdateComponentsRealtime(float a_fDeltatime) override;
		private:
			std::map<CollisionInfo, CollisionType> m_mCollision;
			std::set<CollisionInfo> m_mNewCollision;
		};
	}
}