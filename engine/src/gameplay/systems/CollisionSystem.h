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

		struct CollisionEntry
		{
			EntityID a;
			EntityID b;

			bool operator==(const CollisionEntry& other) const noexcept
			{
				return a == other.a && b == other.b;
			}

			bool operator<(const CollisionEntry& other) const noexcept
			{
				return std::tie(a, b) < std::tie(other.a, other.b);
			}
		};

		struct CollisionInfo : public CollisionEntry
		{
			CollisionType m_CollisionType;
			DirectX::XMFLOAT2 m_vNormal;
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

			std::vector<CollisionInfo> GetCollisions(EntityID a_EntityID) const;

			void Collide(ColliderComponent& a_ColliderA, ColliderComponent& a_ColliderB, const DirectX::XMFLOAT2& a_vNormal);

			/// <summary>
			/// Updates the system's components.
			/// </summary>
			// <param name="a_fDeltaTime">The time it took since last frame.</param>
			void UpdateComponentsRealtime(float a_fDeltatime, UpdateTime a_UpdateTime) override;
		private:
			std::map<CollisionEntry, CollisionInfo> m_mCollisions;
			std::set<CollisionInfo> m_mNewCollisions;
		};
	}
}