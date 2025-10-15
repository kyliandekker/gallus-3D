#pragma once

#include <rapidjson/document.h>
#include <string>

#include "gameplay/EntityID.h"
#include "gameplay/systems/UpdateTime.h"

namespace gallus
{
	namespace resources
	{
		class SrcData;
	}
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Component
		//---------------------------------------------------------------------
		class Component
		{
		public:
			/// <summary>
			/// Deconstructs the component.
			/// </summary>
			virtual ~Component() = default;

			/// <summary>
			/// Initializes the component.
			/// </summary>
			virtual void Init(const gameplay::EntityID& a_EntityID)
			{
				m_EntityID = a_EntityID;
			}

			/// <summary>
			/// Initializes the component in runtime.
			/// </summary>
			virtual void InitRealtime()
			{
			}

			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			virtual void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const = 0;
			
			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			virtual void Deserialize(const resources::SrcData& a_SrcData) = 0;

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
			void UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime);

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

			gameplay::EntityID m_EntityID;
			bool m_bIsDestroyed = false;
		};
	}
}