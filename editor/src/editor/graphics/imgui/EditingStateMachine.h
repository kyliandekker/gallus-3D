#pragma once

#include <cstdint>

namespace gallus::graphics::imgui
{
    enum class EditingInteractionState : uint8_t
    {
        EditingInteractionState_None,
        EditingInteractionState_StartInteraction,
        EditingInteractionState_InInteraction,
        EditingInteractionState_EndInteraction
    };

    class EditingStateMachine
    {
    public:
        EditingStateMachine()
            : m_bEditing(false)
        {}

        EditingInteractionState Update(bool a_bActive)
        {
            EditingInteractionState result = EditingInteractionState::EditingInteractionState_None;

            if (!m_bEditing && a_bActive)
            {
                // just started
                result = EditingInteractionState::EditingInteractionState_StartInteraction;
                m_bEditing = true;
            }
            else if (m_bEditing && a_bActive)
            {
                // ongoing interaction
                result = EditingInteractionState::EditingInteractionState_InInteraction;
            }
            else if (m_bEditing && !a_bActive)
            {
                // finished interaction
                result = EditingInteractionState::EditingInteractionState_EndInteraction;
                m_bEditing = false;
            }

            return result;
        }

        void Reset()
        {
            m_bEditing = false;
        }

        bool IsEditing() const
        {
            return m_bEditing;
        }
    private:
        bool m_bEditing;
    };
}