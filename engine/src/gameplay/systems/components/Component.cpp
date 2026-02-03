#include "Component.h"

// core
#include "core/Engine.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/Entity.h"

// utils
#include "utils/string_extensions.h"

namespace gallus
{
	namespace gameplay
	{
#ifdef _EDITOR
		//---------------------------------------------------------------------
		// Component
		//---------------------------------------------------------------------
		void Component::Serialize(resources::SrcData& a_SrcData) const
		{
			SerializeFields(this, a_SrcData);
		}
#endif // _EDITOR

		//---------------------------------------------------------------------
		void Component::Deserialize(const resources::SrcData& a_SrcData)
		{
			DeserializeFields(this, a_SrcData);
			Init();
		}

		//---------------------------------------------------------------------
		void Component::Init()
		{
			m_pECS = core::ENGINE->GetECS();
			m_bInitialized = true;
		}

		//---------------------------------------------------------------------
		void Component::UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			if (!m_pECS)
			{
				return;
			}

			// TODO: Further optimization would probably be to actually cache the entity instead of ecs.
			auto ent = m_pECS->GetEntity(m_EntityID).lock();
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