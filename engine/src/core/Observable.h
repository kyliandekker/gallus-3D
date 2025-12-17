#pragma once

// core
#include "core/Event.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// Observable
		//---------------------------------------------------------------------
		/// <summary>
		/// A generic observable wrapper for a value of type <typeparamref name="T"/>.
		/// Provides change notification via an event whenever the value is updated.
		/// </summary>
		/// <typeparam name="T">The type of value being observed. Must be comparable with !=.</typeparam>
		template<typename T>
		class Observable
		{
		public:
			/// <summary>
			/// Constructs an observable with the default value of <typeparamref name="T"/>.
			/// </summary>
			Observable() = default;

			/// <summary>
			/// Constructs an observable with the specified initial value.
			/// </summary>
			/// <param name="value">The initial value to assign.</param>
			explicit Observable(const T& value)
				: m_Value(value)
			{}

			/// <summary>
			/// Gets the current value (const reference).
			/// </summary>
			/// <returns>The current value as a const reference.</returns>
			const T& get() const
			{
				return m_Value;
			}

			/// <summary>
			/// Gets the current value (mutable reference).
			/// </summary>
			/// <returns>The current value as a reference.</returns>
			T& get()
			{
				return m_Value;
			}

			/// <summary>
			/// Sets a new value. If the new value is different from the current one,
			/// triggers the <see cref="OnChanged"/> event with the old and new values.
			/// </summary>
			/// <param name="value">The new value to set.</param>
			void set(const T& value)
			{
				if (m_Value != value)
				{
					T oldVal = m_Value;
					m_Value = value;
					OnChange(oldVal, m_Value);
				}
			}

			/// <summary>
			/// Implicit conversion to a const reference of <typeparamref name="T"/>.
			/// </summary>
			operator const T& () const
			{
				return m_Value;
			}

			/// <summary>
			/// Implicit conversion to a mutable reference of <typeparamref name="T"/>.
			/// </summary>
			operator T& ()
			{
				return m_Value;
			}

			/// <summary>
			/// Assigns a new value to the observable. Equivalent to calling <see cref="set"/>.
			/// </summary>
			/// <param name="value">The new value to assign.</param>
			/// <returns>Reference to this <see cref="Observable"/>.</returns>
			Observable<T>& operator=(const T& value)
			{
				set(value);
				return *this;
			}

			/// <summary>
			/// Gets the event object that is triggered when the value changes.
			/// The event provides the old and new values as arguments.
			/// </summary>
			/// <returns>A reference to the <see cref="Event"/>.</returns>
			const Event<const T, const T>& OnChanged() const
			{
				return OnChange;
			}

			/// <summary>
			/// Gets the event object that is triggered when the value changes.
			/// The event provides the old and new values as arguments.
			/// </summary>
			/// <returns>A reference to the <see cref="Event"/>.</returns>
			Event<const T, const T>& OnChanged()
			{
				return OnChange;
			}

		private:
			T m_Value{};
			mutable Event<const T, const T> OnChange;
		};
	}
}