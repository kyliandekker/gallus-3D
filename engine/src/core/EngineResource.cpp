#include "EngineResource.h"

namespace gallus
{
	namespace core
	{
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
		bool EngineResource::IsLocked() const
		{
			return m_bIsLocked;
		}

		//---------------------------------------------------------------------
		void EngineResource::SetIsLocked(bool a_bIsLocked)
		{
			m_bIsLocked = a_bIsLocked;
		}

		//---------------------------------------------------------------------
		bool EngineResource::IsUnique() const
		{
			return m_bIsUnique;
		}

		//---------------------------------------------------------------------
		void EngineResource::SetIsUnique(bool a_bIsUnique)
		{
			m_bIsUnique = a_bIsUnique;
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
		resources::AssetType EngineResource::GetResourceType() const
		{
			return m_AssetType;
		}

		//---------------------------------------------------------------------
		void EngineResource::SetResourceType(resources::AssetType a_ResourceType)
		{
			m_AssetType = a_ResourceType;
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

		//---------------------------------------------------------------------
		void EngineResource::SetPath(const std::filesystem::path& a_Path)
		{
			m_Path = a_Path;
		}
	}
}