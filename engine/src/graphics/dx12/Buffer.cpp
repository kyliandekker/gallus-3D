#include "graphics/dx12/Buffer.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Buffer
			//---------------------------------------------------------------------
			Buffer::Buffer(const std::string& a_sName) : DX12Resource(a_sName)
			{}

			//---------------------------------------------------------------------
			Buffer::Buffer(const D3D12_RESOURCE_DESC& a_ResDesc,
				size_t a_iNumElements, size_t a_iElementSize,
				const std::string& a_sName) : DX12Resource(a_sName)
			{
				CreateViews(a_iNumElements, a_iElementSize);
			}

			//---------------------------------------------------------------------
			void Buffer::CreateViews(size_t a_iNumElements, size_t a_iElementSize)
			{
				throw std::exception("Unimplemented function.");
			}
		}
	}
}