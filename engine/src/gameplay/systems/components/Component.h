#pragma once

// external
#include <string>

// gameplay
#include "gameplay/EntityID.h"
#include "gameplay/systems/UpdateTime.h"

// editor
#include "editor/EditorExpose.h"

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
		class Component : public IExposableToEditor
		{
		public:
			/// <summary>
			/// Deconstructs the component.
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
			/// Initializes the component in runtime.
			/// </summary>
			virtual void InitRealtime()
			{
				m_bInitialized = true;
			}

#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_SrcData">The json document that the data will be put into.</param>
			virtual void Serialize(resources::SrcData& a_SrcData) const;
#endif // _EDITOR

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

			BEGIN_EXPOSE_FIELDS(Component)
			END_EXPOSE_FIELDS(Component)
			BEGIN_EXPOSE_GIZMOS(Component)
			END_EXPOSE_GIZMOS(Component)
			END_EXPOSE_TO_EDITOR(Component)
		};
	}
}