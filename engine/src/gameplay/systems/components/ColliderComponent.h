#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <DirectXMath.h>
#include <set>

#include "utils/math.h"

namespace gallus::gameplay
{
	struct ColliderSettings : public ISerializableObject
	{
		Vector3 m_vOffset = { 0, 0, 0 };
		Vector3 m_vSize = { 1, 1, 1 };
		EntityID m_EntityID;

		BEGIN_SERIALIZE(ColliderSettings)
			SERIALIZE_FIELD(m_vOffset, "Offset", "The local offset of the collider relative to the object's pivot or position. Adjusts where the collider is positioned without moving the object itself.",
				.type = FieldSerializationType::FieldSerializationType_Vector3)
			SERIALIZE_FIELD(m_vSize, "Size", "The size of the collider in local space. Determines the width and height of the collision area.",
				.type = FieldSerializationType::FieldSerializationType_Vector3)
		END_SERIALIZE(ColliderSettings)
	};

	//---------------------------------------------------------------------
	// ColliderComponent
	//---------------------------------------------------------------------
	class ColliderComponent : public Component
	{
	public:
		void IgnoreEntity(const gameplay::EntityID& a_EntityID);
	protected:
		void Init() override;

		/// <summary>
		/// Updates the system's component.
		/// </summary>
		// <param name="a_fDeltaTime">The time it took since last frame.</param>
		void UpdateRealtime(float a_fDeltatime, UpdateTime a_UpdateTime) override;

		bool m_bConvex = false;
		ColliderSettings m_ColliderSettings;

		std::set<gameplay::EntityID> m_aEntitiesToIgnore;

		BEGIN_SERIALIZE_PARENT(ColliderComponent, Component)
			SERIALIZE_FIELD(m_ColliderSettings, "ColliderSettings", "The size and offset of the collider.",
				.type = FieldSerializationType::FieldSerializationType_Object)
			SERIALIZE_FIELD(m_bConvex, "Convex", "Uses the mesh's dimensions as collider.",
				.type = FieldSerializationType::FieldSerializationType_Bool)
			EXPOSE_GIZMO(m_ColliderSettings, EditorGizmoType::EditorGizmoType_Collider)
		END_SERIALIZE(ColliderComponent)
	};
}