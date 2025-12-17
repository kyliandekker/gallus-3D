#include "Component.h"

// core
#include "core/Engine.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/Entity.h"

// utils
#include "utils/string_extensions.h"

namespace gallus
{
	namespace gameplay
	{
		void Component::Serialize(resources::SrcData& a_SrcData) const
		{
			SerializeEditorExposable(this, a_SrcData);
		}

		void Component::Deserialize(const resources::SrcData& a_SrcData)
		{
			DeserializeEditorExposable(this, a_SrcData);
		}

		void Component::UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			const Entity* entity = core::ENGINE->GetECS().GetEntity(m_EntityID);
			if (entity == nullptr)
			{
				return;
			}

			if (!entity->IsActive())
			{
				return;
			}

			UpdateRealtime(a_fDeltaTime, a_UpdateTime);
		}
	}
}