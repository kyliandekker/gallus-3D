#pragma once

#include <rapidjson/document.h>
#include <string>

#include "gameplay/EntityID.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Component
		//---------------------------------------------------------------------
		class Component
		{
		public:
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
			/// Deserializes data from a json document and loads it into the component.
			/// </summary>
			/// <param name="a_Document">The json document that contains the data.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			virtual void Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) = 0;

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
			virtual void UpdateRealtime(float a_fDeltaTime)
			{}

			const gameplay::EntityID& GetEntityID() const
			{
				return m_EntityID;
			}
		protected:
			gameplay::EntityID m_EntityID;
			bool m_bIsDestroyed = false;
		};
	}
}