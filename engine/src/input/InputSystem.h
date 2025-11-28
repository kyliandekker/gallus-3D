#pragma once

#include "core/System.h"

#include <chrono>
#include <map>

namespace gallus
{
	namespace input
	{
		class InputKey
		{
		public:
			InputKey() = default;
			InputKey(char a_cKey) : m_cKey(a_cKey)
			{}

			bool IsKeyDown() const;
			bool IsDoubleDown() const;
			bool IsKey() const;
		protected:
			char m_cKey = 'W';
			mutable bool m_bPressed = false;
			mutable std::chrono::steady_clock::time_point m_LastClickTime =
				std::chrono::steady_clock::now();
			static constexpr double DOUBLE_CLICK_THRESHOLD = 0.25; // 250ms
		};

		//---------------------------------------------------------------------
		// InputSystem
		//---------------------------------------------------------------------
		/// <summary>
		/// Handles user input in realtime in-game.
		/// </summary>
		class InputSystem : public core::ThreadedSystem
		{
		public:
			void RegisterKey(char a_cKey);

			bool IsKeyDown(char a_cKey);
			bool IsDoubleDown(char a_cKey);
			bool IsKey(char a_cKey);
		protected:
			mutable std::map<char, InputKey> m_mKeys;

			void Loop() override;
			bool Sleep() const override;
			bool InitThreadWorker() override;
			void Finalize() override;
		};
	}
}