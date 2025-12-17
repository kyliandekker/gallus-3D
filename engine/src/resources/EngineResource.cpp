#include "EngineResource.h"

namespace gallus
{
	namespace resources
	{
		//---------------------------------------------------------------------
		// EngineResource
		//---------------------------------------------------------------------
		bool EngineResource::Destroy()
		{
			if (!m_bIsDestroyable)
			{
				return false;
			}

#ifdef _LOAD_BY_PATH
			m_Path = "";
#endif // _LOAD_BY_PATH
			m_sName = "";
			m_ResourceCategory = EngineResourceCategory::Unknown;
			m_AssetType = resources::AssetType::None;
			m_bIsDestroyable = true;
			m_bIsLocked = false;
			m_bIsUnique = false;
#ifdef _EDITOR
			m_bIsDirty = false;
			m_bIsDirty.OnChanged().invoke(false, false);
#endif // _EDITOR

			return true;
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
		resources::AssetType EngineResource::GetAssetType() const
		{
			return m_AssetType;
		}

		//---------------------------------------------------------------------
		void EngineResource::SetAssetType(resources::AssetType a_AssetType)
		{
			m_AssetType = a_AssetType;
		}

		//---------------------------------------------------------------------
		const std::string EngineResource::GetName() const
		{
			return m_sName;
		}

#ifdef _LOAD_BY_PATH
		//---------------------------------------------------------------------
		const std::filesystem::path& EngineResource::GetPath() const
		{
			return m_Path;
		}
#endif // _LOAD_BY_PATH

		//---------------------------------------------------------------------
		bool EngineResource::LoadByName(const std::string& a_sName)
		{
			if (!Destroy())
			{
				return false;
			}

#ifdef _LOAD_BY_PATH
			m_Path = "";
#endif // _LOAD_BY_PATH
			m_sName = a_sName;
#ifdef _EDITOR
			m_bIsDirty = false;
			m_bIsDirty.OnChanged().invoke(false, false);
#endif // _EDITOR

			return true;
		}

#ifdef _LOAD_BY_PATH
		//---------------------------------------------------------------------
		bool EngineResource::LoadByPath(const fs::path & a_Path)
		{
			if (!Destroy())
			{
				return false;
			}

			m_Path = a_Path;
			m_sName = a_Path.filename().generic_string();
#ifdef _EDITOR
			m_bIsDirty = false;
			m_bIsDirty.OnChanged().invoke(false, false);
#endif // _EDITOR

			return true;
		}
#endif // _LOAD_BY_PATH
	}
}