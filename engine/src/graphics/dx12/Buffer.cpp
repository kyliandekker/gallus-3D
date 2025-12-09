#include "Buffer.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			bool Buffer::LoadByName(const std::string& a_sName)
			{
				if (!DX12Resource::LoadByName(a_sName))
				{
					return false;
				}
				return true;
			}
			
			//---------------------------------------------------------------------
			bool Buffer::LoadByName(const std::string& a_sName, const D3D12_RESOURCE_DESC& a_ResDesc, size_t a_iNumElements, size_t a_iElementSize)
			{
				if (!Buffer::LoadByName(a_sName))
				{
					return false;
				}

				CreateViews(a_iNumElements, a_iElementSize);

				return true;
			}

			//---------------------------------------------------------------------
			void Buffer::CreateViews(size_t a_iNumElements, size_t a_iElementSize)
			{
				throw std::exception("Unimplemented function.");
			}
		}
	}
}