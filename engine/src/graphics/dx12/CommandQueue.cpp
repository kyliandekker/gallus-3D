// header
#include "CommandQueue.h"

// external
#include <d3d12.h>

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/DX12System2D.h"
#include "graphics/dx12/CommandList.h"

// logger
#include "logger/Logger.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// CommandQueue
			//---------------------------------------------------------------------
			CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE a_CommandListType) :
				m_CommandListType(a_CommandListType),
				m_iFenceValue(0)
			{
				D3D12_COMMAND_QUEUE_DESC desc = {};
				desc.Type = a_CommandListType;
				desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				desc.NodeMask = 0;

				Microsoft::WRL::ComPtr<ID3D12Device2>& device = core::ENGINE->GetDX12().GetDevice();
				if (FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pCommandQueue))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating command queue.");
					return;
				}
				if (FAILED(device->CreateFence(m_iFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating fence.");
					return;
				}

				m_FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
				if (!m_FenceEvent)
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating fence event.");
					return;
				}
			}

			//---------------------------------------------------------------------
			std::shared_ptr<CommandList> CommandQueue::GetCommandList()
			{
				Microsoft::WRL::ComPtr<ID3D12Device2>& device = core::ENGINE->GetDX12().GetDevice();
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
				std::shared_ptr<CommandList> commandList = std::make_shared<CommandList>();

				if (!m_CommandAllocatorQueue.empty() && IsFenceComplete(m_CommandAllocatorQueue.front().m_iFenceValue))
				{
					commandAllocator = m_CommandAllocatorQueue.front().m_pCommandAllocator;
					m_CommandAllocatorQueue.pop();

					if (FAILED(commandAllocator->Reset()))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resetting command allocator.");
						return commandList;
					}
				}
				else
				{
					commandAllocator = CreateCommandAllocator();
				}

				if (!m_CommandListQueue.empty())
				{
					commandList = m_CommandListQueue.front();
					m_CommandListQueue.pop();

					if (FAILED((commandList->GetCommandList()->Reset(commandAllocator.Get(), nullptr))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resetting command list.");
						return commandList;
					}
				}
				else
				{
					commandList->CreateCommandList(commandAllocator, m_CommandListType);
				}

				// Associate the command allocator with the command list so that it can be
				// retrieved when the command list is executed.
				if (FAILED(commandList->GetCommandList()->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get())))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed associating command allocator with command list.");
					return commandList;
				}

				return commandList;
			}

			//---------------------------------------------------------------------
			uint64_t CommandQueue::ExecuteCommandList(std::shared_ptr<CommandList> a_pCommandList)
			{
				a_pCommandList->GetCommandList()->Close();

				ID3D12CommandAllocator* commandAllocator;
				UINT dataSize = sizeof(commandAllocator);
				if (FAILED(a_pCommandList->GetCommandList()->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed setting private data for command list.");
					return -1;
				}

				ID3D12CommandList* const ppCommandLists[] = {
					a_pCommandList->GetCommandList().Get()
				};

				m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
				uint64_t fenceValue = Signal();

				m_CommandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
				m_CommandListQueue.push(a_pCommandList);

				// The ownership of the command allocator has been transferred to the ComPtr
				// in the command allocator queue. It is safe to release the reference 
				// in this temporary COM pointer here.
				commandAllocator->Release();

				return fenceValue;
			}

			//---------------------------------------------------------------------
			uint64_t CommandQueue::Signal()
			{
				uint64_t fenceValue = ++m_iFenceValue;
				m_pCommandQueue->Signal(m_pFence.Get(), fenceValue);
				return fenceValue;
			}

			//---------------------------------------------------------------------
			bool CommandQueue::IsFenceComplete(uint64_t a_iFenceValue)
			{
				return m_pFence->GetCompletedValue() >= a_iFenceValue;
			}

			//---------------------------------------------------------------------
			void CommandQueue::WaitForFenceValue(uint64_t a_iFenceValue)
			{
				if (!IsFenceComplete(a_iFenceValue))
				{
					m_pFence->SetEventOnCompletion(a_iFenceValue, m_FenceEvent);
					::WaitForSingleObject(m_FenceEvent, DWORD_MAX);
				}
			}

			//---------------------------------------------------------------------
			void CommandQueue::Flush()
			{
				WaitForFenceValue(Signal());
			}

			//---------------------------------------------------------------------
			const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& CommandQueue::GetCommandQueue() const
			{
				return m_pCommandQueue;
			}

			//---------------------------------------------------------------------
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
			{
				Microsoft::WRL::ComPtr<ID3D12Device2>& device = core::ENGINE->GetDX12().GetDevice();
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
				if (FAILED(device->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating command allocator.");
					return nullptr;
				}

				return commandAllocator;
			}
		}
	}
}