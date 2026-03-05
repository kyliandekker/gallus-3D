#include "RigidbodyComponent.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// RigidbodyComponent
	//---------------------------------------------------------------------
	constexpr float GRAVITY = 1000.0f;
	
	//---------------------------------------------------------------------
	void RigidbodyComponent::AddForce(const DirectX::XMFLOAT2& a_vForce)
	{
		m_vForce.x += a_vForce.x;
		m_vForce.y += a_vForce.y;
	}
	
	//---------------------------------------------------------------------
	void RigidbodyComponent::SetMass(float a_fMass)
	{
		if (a_fMass <= 0.0f)
		{
			a_fMass = 0.0001f; // Prevent division by zero
		}
		m_fMass = a_fMass;
	}

	//---------------------------------------------------------------------
	void RigidbodyComponent::SetLinearDamping(float a_fLinearDamping)
	{
		if (a_fLinearDamping < 0.0f)
		{
			a_fLinearDamping = 0.0f;
		}
		else if (a_fLinearDamping > 1.0f)
		{
			a_fLinearDamping = 1.0f;
		}
		m_fLinearDamping = a_fLinearDamping;
	}

	//---------------------------------------------------------------------
	void RigidbodyComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
	{
	}
}