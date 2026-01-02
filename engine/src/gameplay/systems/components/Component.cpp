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
#ifdef _EDITOR
		void Component::Serialize(resources::SrcData& a_SrcData) const
		{
			SerializeFields(this, a_SrcData);
		}
#endif // _EDITOR

		void Component::Deserialize(const resources::SrcData& a_SrcData)
		{
			DeserializeFields(this, a_SrcData);
		}

		void Component::UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			auto ent = core::ENGINE->GetECS().GetEntity(m_EntityID).lock();
			if (!ent)
			{
				return;
			}

			if (!ent->IsActive())
			{
				return;
			}

			UpdateRealtime(a_fDeltaTime, a_UpdateTime);
		}
	}
}