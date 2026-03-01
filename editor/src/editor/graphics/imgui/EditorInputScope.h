#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// Editor input scopes
	//---------------------------------------------------------------------
	enum class EditorInputScope : uint32_t
	{
		EditorInputScope_Shift = 1 << 6,
		EditorInputScope_Ctrl = 1 << 5,
		EditorInputScope_SceneHoverRightClick = 1 << 4,
		EditorInputScope_SceneHover = 1 << 3,
		EditorInputScope_Viewport = 1 << 2,
		EditorInputScope_Operations = 1 << 1,
	};

	inline std::string EditorInputScopeToString(EditorInputScope a_EditorInputScope)
	{
		switch (a_EditorInputScope)
		{
			case EditorInputScope::EditorInputScope_Shift:
			{
				return "Shift";
			}
			case EditorInputScope::EditorInputScope_Ctrl:
			{
				return "Ctrl";
			}
			case EditorInputScope::EditorInputScope_Viewport:
			{
				return "Viewport";
			}
			case EditorInputScope::EditorInputScope_SceneHover:
			{
				return "SceneHover";
			}
			case EditorInputScope::EditorInputScope_SceneHoverRightClick:
			{
				return "SceneHoverRightClick";
			}
			case EditorInputScope::EditorInputScope_Operations:
			{
				return "Operations";
			}
		}
		return "";
	}

	static uint32_t s_EditorInputScopeFlags = 0;

	inline void ActivateInputScope(EditorInputScope a_InputScope)
	{
		s_EditorInputScopeFlags |= static_cast<uint32_t>(a_InputScope);
	}

	inline void DeactivateInputScope(EditorInputScope a_InputScope)
	{
		s_EditorInputScopeFlags &= ~static_cast<uint32_t>(a_InputScope);
	}

	inline bool IsInputScopeActive(EditorInputScope a_InputScope)
	{
		if ((s_EditorInputScopeFlags & static_cast<uint32_t>(a_InputScope)) != 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//---------------------------------------------------------------------
	// Keybinds
	//---------------------------------------------------------------------
	enum class Keybind
	{
		Keybind_Zoom,

		Keybind_W,
		Keybind_A,
		Keybind_S,
		Keybind_D,
		Keybind_Q,
		Keybind_E,

		Keybind_T,
		Keybind_P,
		Keybind_R,
		Keybind_Ctrl,
		Keybind_Shift,
		Keybind_Right_Mouse
	};

	struct KeybindInputScope
	{
		Keybind m_eKeybind;
		uint32_t m_uInputScopes = 0; // bitmask of allowed input scopes
	};

	static std::vector<KeybindInputScope> s_aKeybinds;

	//---------------------------------------------------------------------
	// Set or overwrite the input scope mask for a keybind
	//---------------------------------------------------------------------
	inline void SetKeybindInputScope(Keybind a_Keybind, uint32_t a_uInputScopes)
	{
		for (KeybindInputScope& kbCtx : s_aKeybinds)
		{
			if (kbCtx.m_eKeybind == a_Keybind)
			{
				kbCtx.m_uInputScopes |= a_uInputScopes;
				return;
			}
		}

		KeybindInputScope newKbCtx;
		newKbCtx.m_eKeybind = a_Keybind;
		newKbCtx.m_uInputScopes |= a_uInputScopes;
		s_aKeybinds.push_back(newKbCtx);
	}

	inline bool AreOnlyKeysDown(const std::vector<ImGuiKey>& keys)
	{
		// Check that all keys in the vector are down
		for (ImGuiKey key : keys)
		{
			if (!ImGui::IsKeyDown(key))
			{
				return false;
			}
		}

		// Check that no other key is down
		for (size_t k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_COUNT; k++)
		{
			ImGuiKey currentKey = static_cast<ImGuiKey>(k);

			// If this key is down but not in our list, fail
			if (ImGui::IsKeyDown(currentKey) &&
				std::find(keys.begin(), keys.end(), currentKey) == keys.end())
			{
				return false;
			}
		}

		return true;
	}

	inline bool IsKeyOnlyDown(ImGuiKey key)
	{
		return AreOnlyKeysDown({ key });
	}

	inline bool IsAnyKeyDown()
	{
		for (size_t k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_COUNT; k++)
		{
			ImGuiKey currentKey = static_cast<ImGuiKey>(k);

			if (ImGui::IsKeyDown(currentKey))
			{
				return true;
			}
		}
		return false;
	}

	inline std::vector<EditorInputScope> GetAllActiveInputScopes()
	{
		std::vector<EditorInputScope> activeInputScopes;

		uint32_t flags = s_EditorInputScopeFlags;
		while (flags != 0)
		{
			uint32_t lowestBit = flags & (~flags + 1);
			activeInputScopes.push_back(static_cast<EditorInputScope>(lowestBit));
			flags &= ~lowestBit;
		}

		return activeInputScopes;
	}

	inline bool HasHigherPriorityKeybindInputScopeActive(const KeybindInputScope& a_KeybindInputScope, EditorInputScope a_InputScope)
	{
		uint32_t keybindInputScopes = a_KeybindInputScope.m_uInputScopes;
		uint32_t inputscopeValue = static_cast<uint32_t>(a_InputScope);

		uint32_t bit = inputscopeValue << 1;
		while (bit != 0)
		{
			if ((keybindInputScopes & bit) != 0)
			{
				if ((s_EditorInputScopeFlags & bit) != 0)
				{
					return true;
				}
			}

			bit <<= 1;
		}

		return false;
	}

	//---------------------------------------------------------------------
	// Check if a keybind can activate based on currently active input scopes
	//---------------------------------------------------------------------
	inline bool CanActivate(Keybind a_Keybind, EditorInputScope a_InputScope, bool a_bOnlyKeybind = true)
	{
		ImGuiIO& io = ImGui::GetIO();

		// Find the keybind input scope
		KeybindInputScope* kbCtx = nullptr;
		for (KeybindInputScope& ctx : s_aKeybinds)
		{
			if (ctx.m_eKeybind == a_Keybind)
			{
				kbCtx = &ctx;
				break;
			}
		}

		if (!kbCtx)
		{
			return false;
		}

		bool input = false;
		switch (a_Keybind)
		{
			case Keybind::Keybind_Zoom:
			{
				input = io.MouseWheel != 0.0f && (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_MouseWheelY) : ImGui::IsKeyDown(ImGuiKey_MouseWheelY));
				break;
			}
			case Keybind::Keybind_W:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_W) : ImGui::IsKeyDown(ImGuiKey_W));
				break;
			}
			case Keybind::Keybind_A:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_A) : ImGui::IsKeyDown(ImGuiKey_A));
				break;
			}
			case Keybind::Keybind_S:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_S) : ImGui::IsKeyDown(ImGuiKey_S));
				break;
			}
			case Keybind::Keybind_D:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_D) : ImGui::IsKeyDown(ImGuiKey_D));
				break;
			}
			case Keybind::Keybind_Q:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_Q) : ImGui::IsKeyDown(ImGuiKey_Q));
				break;
			}
			case Keybind::Keybind_E:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_E) : ImGui::IsKeyDown(ImGuiKey_E));
				break;
			}
			case Keybind::Keybind_T:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_T) : ImGui::IsKeyDown(ImGuiKey_T));
				break;
			}
			case Keybind::Keybind_P:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_P) : ImGui::IsKeyDown(ImGuiKey_P));
				break;
			}
			case Keybind::Keybind_R:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_R) : ImGui::IsKeyDown(ImGuiKey_R));
				break;
			}
			case Keybind::Keybind_Ctrl:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_LeftCtrl) : ImGui::IsKeyDown(ImGuiKey_LeftCtrl));
				break;
			}
			case Keybind::Keybind_Shift:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_LeftShift) : ImGui::IsKeyDown(ImGuiKey_LeftShift));
				break;
			}
			case Keybind::Keybind_Right_Mouse:
			{
				input = (a_bOnlyKeybind ? IsKeyOnlyDown(ImGuiKey_MouseRight) : ImGui::IsKeyDown(ImGuiKey_MouseRight));
				break;
			}
		}

		if (!input)
		{
			return false;
		}

		// Check if there are even any input scopes active currently.
		uint32_t activeFlags = s_EditorInputScopeFlags;
		if (activeFlags == 0)
		{
			return false;
		}
		
		// Check if the provided input scope is active.
		if (!IsInputScopeActive(a_InputScope))
		{
			return false;
		}

		if (HasHigherPriorityKeybindInputScopeActive(*kbCtx, a_InputScope))
		{
			return false;
		}

		//uint32_t highestBit = 1u << 31;
		//while ((highestBit & activeFlags) == 0)
		//{
		//	highestBit >>= 1;
		//}

		//EditorInputScope input scope = (EditorInputScope) highestBit;

		//if (highestBitInKeybind >= highestBit && static_cast<uint32_t>(a_InputScope) < highestBitInKeybind)
		//{
		//	return false;
		//}

		return input;
	}
}