#pragma once

#include "Event.h" // your gallus::Event definition

namespace gallus
{
	namespace core
	{
		template<typename T>
		class Observable
		{
		public:
			Observable() = default;

			explicit Observable(const T& value)
				: m_Value(value)
			{}

			// Getters
			const T& get() const
			{
				return m_Value;
			}
			T& get()
			{
				return m_Value;
			}

	  // Setter
			void set(const T& value)
			{
				if (m_Value != value)
				{
					T oldVal = m_Value;
					m_Value = value;
					OnChange(oldVal, m_Value);
				}
			}

			// Implicit conversion
			operator const T& () const
			{
				return m_Value;
			}
			operator T& ()
			{
				return m_Value;
			}

	  // Assignment
			Observable<T>& operator=(const T& value)
			{
				set(value);
				return *this;
			}

			// Event access
			Event<const T, const T&>& OnChanged()
			{
				return OnChange;
			}

		private:
			T m_Value{};
			Event<const T, const T&> OnChange;
		};
	}
}