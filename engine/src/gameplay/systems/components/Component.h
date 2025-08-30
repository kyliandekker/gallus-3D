#pragma once

#include <rapidjson/document.h>
#include <string>

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Component
		//---------------------------------------------------------------------
		class Component
		{
		public:
			virtual ~Component() = default;

			virtual void Init()
			{}

			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			virtual void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const = 0;

			/// <summary>
			/// Deserializes data from a json document and loads it into the component.
			/// </summary>
			/// <param name="a_Document">The json document that contains the data.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			virtual void Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) = 0;

			bool IsDestroyed() const
			{
				return m_bIsDestroyed;
			}

			void Destroy()
			{
				m_bIsDestroyed = true;
			}
		private:
			bool m_bIsDestroyed = false;
		};
	}
}