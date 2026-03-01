#include "EngineResource.h"

// logger
#include "logger/Logger.h"

// resources
#include "resources/SrcData.h"

namespace gallus::resources
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

		m_ResourceCategory = EngineResourceCategory::Unknown;
		m_AssetType = resources::AssetType::None;
		m_bIsDestroyable = true;

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
	bool EngineResource::LoadMetaData(const core::Data& a_Data)
	{
		resources::SrcData srcData(a_Data);

		if (!srcData.IsValid())
		{
			LOGF(LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Failed loading data in meta file \"%s\".", m_sName.c_str());
			return false;
		}
		else
		{
			DeserializeFields(this, srcData, SerializationMethod::SerializationMethod_Metadata);
		}
		return true;
	}
}