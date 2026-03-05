#pragma once

#include "resources/ISerializableObject.h"

// external
#include <string>

// gameplay
#include "gameplay/EntityID.h"

namespace gallus::resources
{
	class SrcData;
}
namespace gallus::gameplay
{
	enum class UpdateTime : uint32_t;
	class EntityComponentSystem;

	//---------------------------------------------------------------------
	// Component
	//---------------------------------------------------------------------
	class Component : public ISerializableObject
	{
	public:
		/// <summary>
		/// Destroys the component.
		/// </summary>
		virtual ~Component() = default;

		/// <summary>
		/// Initializes the component.
		/// </summary>
		virtual void SetDefaults(const gameplay::EntityID& a_EntityID)
		{
			m_EntityID = a_EntityID;
		}

		/// <summary>
		/// Initializes the component after deserialization.
		/// </summary>
		virtual void Init();

		/// <summary>
		/// Initializes the component in runtime.
		/// </summary>
		virtual void InitRealtime()
		{}

		/// <summary>
		/// Creates an instance based on source data.
		/// </summary>
		/// <param name="a_SrcData">The source data.</param>
		virtual void Deserialize(const resources::SrcData& a_SrcData);

		/// <summary>
		/// Checks whether the component is destroyed.
		/// </summary>
		/// <returns>True if the component has been destroyed, false otherwise.</returns>
		bool IsDestroyed() const
		{
			return m_bIsDestroyed;
		}

		/// <summary>
		/// Marks the component as destroyed.
		/// </summary>
		void Destroy()
		{
			m_bIsDestroyed = true;
		}

		/// <summary>
		/// Updates the components.
		/// </summary>
		/// <param name="a_fDeltaTime">Delta time.</param>
		virtual void UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime);

		/// <summary>
		/// Retrieves the Entity ID.
		/// </summary>
		/// <returns>A reference to the Entity ID.</returns>
		const gameplay::EntityID& GetEntityID() const
		{
			return m_EntityID;
		}
	protected:
		/// <summary>
		/// Updates the components.
		/// </summary>
		/// <param name="a_fDeltaTime">Delta time.</param>
		virtual void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{}

		bool m_bInitialized = false;
		gameplay::EntityID m_EntityID;
		bool m_bIsDestroyed = false;

		BEGIN_SERIALIZE(Component)
		END_SERIALIZE(Component)

		// cache
		gameplay::EntityComponentSystem* m_pECS = nullptr;
	};
}