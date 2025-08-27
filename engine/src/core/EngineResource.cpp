#include "EngineResource.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		std::string ResourceTypeToString(ResourceType a_ResourceType)
		{
			switch (a_ResourceType)
			{
				case ResourceType::ResourceType_Unknown:
				{
					return "Unknown";
				}
				case ResourceType::ResourceType_Texture:
				{
					return "Texture";
				}
				case ResourceType::ResourceType_Shader:
				{
					return "Shader";
				}
				case ResourceType::ResourceType_Mesh:
				{
					return "Mesh";
				}
				case ResourceType::ResourceType_Material:
				{
					return "Material";
				}
			}
			return "?";
		}

		//---------------------------------------------------------------------
		// EngineResource
		//---------------------------------------------------------------------
		EngineResource::EngineResource(const std::string& a_sName) : m_sName(a_sName)
		{}

		//---------------------------------------------------------------------
		void EngineResource::Destroy()
		{
			if (!m_bIsDestroyable)
			{
				return;
			}

			m_ResourceCategory = EngineResourceCategory::Unknown;
			m_ResourceType = ResourceType::ResourceType_Unknown;
			m_bIsDestroyable = true;
			m_sName = "";
			m_Path = "";
		}

		//---------------------------------------------------------------------
		bool EngineResource::IsDestroyable() const
		{
			return m_bIsDestroyable;
		}

		//---------------------------------------------------------------------
		void EngineResource::SetIsDestroyable(bool a_bIsDestroyable)
		{
			m_bIsDestroyable = a_bIsDestroyable;
		}

		//---------------------------------------------------------------------
		void EngineResource::SetResourceCategory(EngineResourceCategory a_ResourceCategory)
		{
			m_ResourceCategory = a_ResourceCategory;
		}

		//---------------------------------------------------------------------
		EngineResourceCategory EngineResource::GetResourceCategory() const
		{
			return m_ResourceCategory;
		}

		//---------------------------------------------------------------------
		ResourceType EngineResource::GetResourceType() const
		{
			return m_ResourceType;
		}

		//---------------------------------------------------------------------
		void EngineResource::SetResourceType(ResourceType a_ResourceType)
		{
			m_ResourceType = a_ResourceType;
		}

		//---------------------------------------------------------------------
		const std::string& EngineResource::GetName() const
		{
			return m_sName;
		}

		//---------------------------------------------------------------------
		const std::filesystem::path& EngineResource::GetPath() const
		{
			return m_Path;
		}
	}
}