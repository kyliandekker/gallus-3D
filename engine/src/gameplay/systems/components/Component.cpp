#include "Component.h"

// core
#include "core/Engine.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/Entity.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	void Component::Deserialize(const resources::SrcData& a_SrcData)
	{
		DeserializeFields(this, a_SrcData);
		Init();
	}

	//---------------------------------------------------------------------
	void Component::Init()
	{
		m_pECS = GetEngine().GetECS();
		m_bInitialized = true;
	}

	//---------------------------------------------------------------------
	void Component::UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime)
	{
		if (!m_pECS)
		{
			return;
		}

		std::shared_ptr<gameplay::Entity> ent = m_pECS->GetEntity(m_EntityID).lock();
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