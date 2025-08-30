#include "DX12System2D.h"

#include "logger/Logger.h"
#include "graphics/win32/Window.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "core/Tool.h"

#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "gameplay/systems/SpriteSystem.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			constexpr glm::ivec2 RENDER_TEX_SIZE = glm::ivec2(1920, 1080);

			//---------------------------------------------------------------------
			// DX12System2D
			//---------------------------------------------------------------------
			bool DX12System2D::Initialize(bool a_bWait, HWND a_hWnd, const glm::ivec2& a_vSize, win32::Window* a_pWindow)
			{
				m_vSize = a_vSize;
				m_hWnd = a_hWnd;
				m_pWindow = a_pWindow;

				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Initializing dx12 system.");
				return ThreadedSystem::Initialize(a_bWait);
			}

			//---------------------------------------------------------------------
			bool DX12System2D::Destroy()
			{
				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Destroying dx12 system.");
				return ThreadedSystem::Destroy();
			}

			//---------------------------------------------------------------------
			bool DX12System2D::InitThreadWorker()
			{
#if _DEBUG
				// Always enable the debug layer before doing anything DX12 related
				// so all possible errors generated while creating DX12 objects
				// are caught by the debug layer.
				Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
				if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving debug interface.");
					return false;
				}
				debugInterface->EnableDebugLayer();

				Microsoft::WRL::ComPtr<ID3D12Debug1> debugController1 = nullptr;
				if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController1))))
				{
					debugController1->SetEnableGPUBasedValidation(TRUE);
				}
#endif // _DEBUG
				// Get the adapter.
				if (!GetAdapter(false))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving adapter.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Retrieved adapter.");
#endif // LOG_DX!2

				// Create the device.
				if (!CreateDevice())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating device.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Created device.");
#endif // LOG_DX!2

				ID3D12InfoQueue* infoQueue = nullptr;
				if (SUCCEEDED(m_pDevice->QueryInterface(IID_PPV_ARGS(&infoQueue))))
				{
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);

					// Optionally filter out less important messages
					D3D12_MESSAGE_ID denyIds[] = {
						D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
						D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
					};

					D3D12_INFO_QUEUE_FILTER filter = {};
					filter.DenyList.NumIDs = _countof(denyIds);
					filter.DenyList.pIDList = denyIds;
					infoQueue->PushStorageFilter(&filter);

					infoQueue->Release();
				}

				// Create the command queues.
				if (!CreateCommandQueues())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating command queues.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Created command queues.");
#endif // LOG_DX!2

				m_bIsTearingSupported = CheckTearingSupport();

#ifdef _EDITOR
				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);
				m_ScissorRect = CD3DX12_RECT(0, 0, RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);

				m_Camera.Init(RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);
#else
				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_vSize.x), static_cast<float>(m_vSize.y));
				m_ScissorRect = CD3DX12_RECT(0, 0, static_cast<float>(m_vSize.x), static_cast<float>(m_vSize.y));

				m_Camera.Init(m_vSize.x, m_vSize.y);
#endif // _EDITOR
				m_Camera.GetTransform().SetPosition({ 0.0f, 0.0f });

				if (!DirectX::XMVerifyCPUSupport())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed verifying DirectX Math support.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Verified DirectX Math support.");
#endif // LOG_DX!2

				// Create the swap chain.
				if (!CreateSwapChain())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating swap chain.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Created swap chain.");
#endif // LOG_DX!2

				// Create views.
				if (!CreateViews())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating views.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Created views.");
#endif // LOG_DX!2

				if (!CreateRootSignature())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating root signature.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Created root signature.");
#endif // LOG_DX!2

				// Get the direct command queue.
				std::shared_ptr<CommandQueue> dCommandQueue = GetCommandQueue();
				std::shared_ptr<CommandList> dCommandList = dCommandQueue->GetCommandList();

				// Get the copy command queue.
				std::shared_ptr<CommandQueue> cCommandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				std::shared_ptr<CommandList> cCommandList = cCommandQueue->GetCommandList();

				// Used for creating stuff like root signature, etc.
				if (!BeforeInitialize(dCommandQueue, dCommandList))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed before initialization could succeed.");
					return false;
				}

#ifdef _EDITOR
				CreateRenderTexture({ RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y });
#endif // _EDITOR

				m_eOnInitialize(*this);
#ifndef IMGUI_DISABLE
				m_ImGuiWindow.Initialize();
#endif // IMGUI_DISABLE
				if (!AfterInitialize(dCommandQueue, dCommandList))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed after initialization.");
					return false;
				}

				std::shared_ptr<Texture> texture = core::TOOL->GetResourceAtlas().LoadTexture("tex_missing.png", cCommandList); // Default texture.
				texture->SetResourceCategory(core::EngineResourceCategory::Missing);
				texture->SetIsDestroyable(false);

				std::shared_ptr<Shader> shader = core::TOOL->GetResourceAtlas().LoadShader("vertexShader.hlsl", "pixelShader.hlsl"); // Default shader.
				shader->SetResourceCategory(core::EngineResourceCategory::Missing);
				shader->SetIsDestroyable(false);

				std::shared_ptr<Mesh> mesh = core::TOOL->GetResourceAtlas().LoadMesh("generic_mesh", cCommandList); // Default mesh.
				mesh->SetResourceCategory(core::EngineResourceCategory::Missing);
				mesh->SetIsDestroyable(false);

				uint64_t fenceValue = cCommandQueue->ExecuteCommandList(cCommandList);
				cCommandQueue->WaitForFenceValue(fenceValue);

				m_MeshComponent.Init();
				m_MeshComponent.SetTexture(texture);
				m_MeshComponent.SetShader(shader);
				m_MeshComponent.SetMesh(mesh);

				UpdateRenderTargetViews();
#ifndef IMGUI_DISABLE
				m_ImGuiWindow.OnRenderTargetCreated(dCommandList);
#endif // IMGUI_DISABLE

				core::TOOL->GetResourceAtlas().TransitionResources(dCommandList);

				fenceValue = dCommandQueue->ExecuteCommandList(dCommandList);
				dCommandQueue->WaitForFenceValue(fenceValue);

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_DX12, "Initialized dx12 system.");

				return true;
			}

			//---------------------------------------------------------------------
			bool DX12System2D::GetAdapter(bool a_bUseWarp)
			{
				Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
				UINT createFactoryFlags = 0;
#if defined(_DEBUG)
				createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

				if (FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating DXGI Factory.");
					return false;
				}

				Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;

				if (a_bUseWarp)
				{
					if (FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating warp adapter.");
						return false;
					}
					if (FAILED(dxgiAdapter1.As(&m_pAdapter)))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting adapter.");
						return false;
					}
				}
				else
				{
					SIZE_T maxDedicatedVideoMemory = 0;
					for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
					{
						DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
						dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

						// Check to see if the adapter can create a D3D12 m_pDevice without actually 
						// creating it. The adapter with the largest dedicated video memory
						// is favored.
						if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
							SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
							D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
							dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
						{
							maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
							if (FAILED(dxgiAdapter1.As(&m_pAdapter)))
							{
								LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting adapter.");
								return false;
							}
						}
					}
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Created adapter.");
#endif // LOG_DX!2

				return true;
			}

			//---------------------------------------------------------------------
			bool DX12System2D::CreateDevice()
			{
				if (FAILED(D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating m_pDevice.");
					return false;
				}
				//    NAME_D3D12_OBJECT(d3d12Device2);

				// Enable debug messages in debug mode.
#ifdef _DEBUG
				Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
				if (SUCCEEDED(m_pDevice.As(&pInfoQueue)))
				{
					pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
					pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
					pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

					// Suppress whole categories of messages
					//D3D12_MESSAGE_CATEGORY Categories[] = {};

					// Suppress messages based on their severity level
					D3D12_MESSAGE_SEVERITY Severities[] =
					{
						D3D12_MESSAGE_SEVERITY_INFO
					};

					// Suppress individual messages by their ID
					D3D12_MESSAGE_ID DenyIds[] = {
						D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
						D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
						D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
					};

					D3D12_INFO_QUEUE_FILTER NewFilter = {};
					//NewFilter.DenyList.NumCategories = _countof(Categories);
					//NewFilter.DenyList.pCategoryList = Categories;
					NewFilter.DenyList.NumSeverities = _countof(Severities);
					NewFilter.DenyList.pSeverityList = Severities;
					NewFilter.DenyList.NumIDs = _countof(DenyIds);
					NewFilter.DenyList.pIDList = DenyIds;

					if (FAILED(pInfoQueue->PushStorageFilter(&NewFilter)))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed pushing storage filter.");
						return false;
					}
				}
#endif // _DEBUG

				return true;
			}

			//---------------------------------------------------------------------
			bool DX12System2D::CheckTearingSupport()
			{
				BOOL allowTearing = FALSE;

				// Rather than create the DXGI 1.5 factory interface directly, we create the
				// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
				// graphics debugging tools which will not support the 1.5 factory interface 
				// until a future update.
				Microsoft::WRL::ComPtr<IDXGIFactory4> factory4;
				if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
				{
					Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
					if (SUCCEEDED(factory4.As(&factory5)))
					{
						factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
							&allowTearing, sizeof(allowTearing));
					}
				}

				return allowTearing == TRUE;
			}

			//---------------------------------------------------------------------
			bool DX12System2D::CreateSwapChain()
			{
				Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory4;
				UINT createFactoryFlags = 0;
#ifdef _DEBUG
				createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

				if (FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating DXGI factory.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Created DXGI factory.");
#endif // LOG_DX!2

				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.Width = m_vSize.x;
				swapChainDesc.Height = m_vSize.y;
				swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.Stereo = FALSE;
				swapChainDesc.SampleDesc = { 1, 0 };
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = g_iBufferCount;
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
				// It is recommended to always allow tearing if tearing support is available.
				swapChainDesc.Flags = m_bIsTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
				ID3D12CommandQueue* pCommandQueue = GetCommandQueue()->GetCommandQueue().Get();

				Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
				if (FAILED(dxgiFactory4->CreateSwapChainForHwnd(
					pCommandQueue,
					m_hWnd,
					&swapChainDesc,
					nullptr,
					nullptr,
					&swapChain1)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating swap chain.");
					return false;
				}

				// Disable the Alt+Enter full screen toggle feature. Switching to full screen
				// will be handled manually.
				if (FAILED(dxgiFactory4->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed associating window.");
					return false;
				}

				if (FAILED(swapChain1.As(&m_pSwapChain)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed casting swap chain.");
					return false;
				}

				m_iCurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

				return m_pSwapChain != nullptr;
			}

			//---------------------------------------------------------------------
			void DX12System2D::CreateRTV()
			{
				size_t numBuffers = g_iBufferCount;

#ifdef _EDITOR
				numBuffers++;
#endif // _EDITOR

				D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
				rtvHeapDesc.NumDescriptors = static_cast<UINT>(numBuffers);
				rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				m_RTV = HeapAllocation(rtvHeapDesc);
			}

			//---------------------------------------------------------------------
			void DX12System2D::CreateSRV()
			{
				D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
				srvHeapDesc.NumDescriptors = 100;  // Adjust based on how many textures you need
				srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // Important for binding!
				m_SRV = HeapAllocation(srvHeapDesc);
			}

			//---------------------------------------------------------------------
			void DX12System2D::Finalize()
			{
				std::lock_guard<std::mutex> lock(m_RenderMutex);
#ifndef IMGUI_DISABLE
				m_ImGuiWindow.Destroy();
#endif // IMGUI_DISABLE

				Flush();

				LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_DX12, "Destroyed dx12 system.");
			}

			//---------------------------------------------------------------------
			void DX12System2D::Flush()
			{
				m_pDirectCommandQueue->Flush();
				m_pCopyCommandQueue->Flush();
			}

			//---------------------------------------------------------------------
			void DX12System2D::UpdateRenderTargetViews()
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTV.GetCPUDescriptorHandleForHeapStart());

#ifdef _EDITOR
				// Create RTV for custom render target texture
				if (m_pRenderTexture->GetResource())
				{
					m_pDevice->CreateRenderTargetView(m_pRenderTexture->GetResource().Get(), nullptr, rtvHandle);
				}
				rtvHandle.Offset(m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
#endif // _EDITOR

				for (int i = 0; i < g_iBufferCount; ++i)
				{
					Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
					if (FAILED(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving buffer.");
						return;
					}

					m_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

					m_BackBuffers[i] = backBuffer;

					rtvHandle.Offset(m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
				}
			}

			//---------------------------------------------------------------------
#ifdef _EDITOR
			std::shared_ptr<Texture> DX12System2D::GetRenderTexture()
			{
				return m_pRenderTexture;
			}
#endif // _EDITOR

			//---------------------------------------------------------------------
			bool DX12System2D::CreateCommandQueues()
			{
				m_pDirectCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
				m_pCopyCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);

				return true;
			}

			//---------------------------------------------------------------------
			bool DX12System2D::CreateViews()
			{
				CreateRTV();
				CreateSRV();

				return true;
			}

			//---------------------------------------------------------------------
			bool DX12System2D::CreateRootSignature()
			{
				// Define descriptor ranges
				CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[1]{};

				// SRV for the texture at register t0
				descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

				// Define root parameters
				CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters]{};

				// CBV at register b0 (Model-View-Projection Matrix)
				rootParameters[RootParameters::CBV].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

				// Texture SRV at register t0 (binds a texture)
				rootParameters[RootParameters::TEX_SRV].InitAsDescriptorTable(1, &descriptorRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);

				// Define static sampler at register s0 (replaces the removed descriptor table sampler)
				const CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] = {
					CD3DX12_STATIC_SAMPLER_DESC(
						0,  // Register s0
						D3D12_FILTER_MIN_MAG_MIP_POINT,  // Bilinear filtering
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP
					)
				};

				// Define root signature flags
				const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

				// Create root signature descriptor
				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
				rootSignatureDescription.Init_1_1(
					_countof(rootParameters), rootParameters,
					_countof(staticSamplers), staticSamplers, // Use static samplers
					rootSignatureFlags
				);

				// Serialize the root signature
				Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
				Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
				const HRESULT hr = D3DX12SerializeVersionedRootSignature(
					&rootSignatureDescription,
					D3D_ROOT_SIGNATURE_VERSION_1_1,
					&rootSignatureBlob,
					&errorBlob
				);

				if (FAILED(hr))
				{
					std::string errorMessage(static_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed serializing root signature: \"%s\".", errorMessage.c_str());
					return false;
				}

				// Create the root signature
				if (FAILED(m_pDevice->CreateRootSignature(0,
					rootSignatureBlob->GetBufferPointer(),
					rootSignatureBlob->GetBufferSize(),
					IID_PPV_ARGS(&m_pRootSignature))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating root signature.");
					return false;
				}

				return true;
			}

			//---------------------------------------------------------------------
			bool DX12System2D::BeforeInitialize(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList)
			{
				return true;
			}

			//---------------------------------------------------------------------
			bool DX12System2D::AfterInitialize(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList)
			{
				return true;
			}

			//---------------------------------------------------------------------
			void DX12System2D::AfterResize(const glm::ivec2& a_vSize)
			{}

			//---------------------------------------------------------------------
			void DX12System2D::Resize(const glm::ivec2& a_vPos, const glm::ivec2& a_vSize)
			{
				if (a_vSize.x == 0 || a_vSize.y == 0)
				{
					return;
				}

				Flush();
				for (int i = 0; i < g_iBufferCount; ++i)
				{
					m_BackBuffers[i].Reset();  // This will release the old resource properly
				}

#ifndef IMGUI_DISABLE
				m_ImGuiWindow.OnRenderTargetCleaned();
#endif // IMGUI_DISABLE

				DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
				if (FAILED(m_pSwapChain->GetDesc(&swapChainDesc)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed getting desc from swap chain.");
					return;
				}
				if (FAILED(m_pSwapChain->ResizeBuffers(g_iBufferCount, a_vSize.x,
					a_vSize.y, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags)))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resizing buffers.");
					return;
				}

				AfterResize(a_vSize);

				m_iCurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

				UpdateRenderTargetViews();

				m_eOnResize(a_vPos, a_vSize);
#ifndef IMGUI_DISABLE
				m_ImGuiWindow.Resize(a_vPos, a_vSize);
#endif

#ifdef _EDITOR
				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);
				m_ScissorRect = CD3DX12_RECT(0, 0, RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);
				m_Camera.Init(RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);
#else
				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(a_vSize.x), static_cast<float>(a_vSize.y));
				m_ScissorRect = CD3DX12_RECT(0, 0, static_cast<float>(a_vSize.x), static_cast<float>(a_vSize.y));
				m_Camera.Init(a_vSize.x, a_vSize.y);
#endif // _EDITOR

				m_vSize = glm::vec2(a_vSize.x, a_vSize.y);
			}

			//---------------------------------------------------------------------
			const Microsoft::WRL::ComPtr<ID3D12Resource>& DX12System2D::GetCurrentBackBuffer() const
			{
				return m_BackBuffers[m_iCurrentBackBufferIndex];
			}

			//---------------------------------------------------------------------
			UINT DX12System2D::GetCurrentBackBufferIndex() const
			{
				return m_iCurrentBackBufferIndex;
			}

			//---------------------------------------------------------------------
			D3D12_CPU_DESCRIPTOR_HANDLE DX12System2D::GetCurrentRenderTargetView(bool a_bUseRenderTexture)
			{
				size_t backBufferStart = 0;

#ifdef _EDITOR
				if (a_bUseRenderTexture && m_pRenderTexture->IsValid())
				{
					return m_RTV.GetCPUHandle(0);
				}
				backBufferStart++;
#endif // _EDITOR

				return m_RTV.GetCPUHandle(backBufferStart + m_iCurrentBackBufferIndex);
			}

			//---------------------------------------------------------------------
			void DX12System2D::Loop()
			{
				if (!m_bInitialized.load())
				{
					return;
				}

				ProcessWindowEvents();

				std::shared_ptr<CommandQueue> commandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
				std::shared_ptr<CommandList> commandList = commandQueue->GetCommandList();

				UINT currentBackBufferIndex = GetCurrentBackBufferIndex();
				const Microsoft::WRL::ComPtr<ID3D12Resource>& backBuffer = GetCurrentBackBuffer();

				commandList->TransitionResource(backBuffer,
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

				const FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

				D3D12_CPU_DESCRIPTOR_HANDLE currentRtv;

				D3D12_CPU_DESCRIPTOR_HANDLE rtv = GetCurrentRenderTargetView(false);
				commandList->GetCommandList()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

				currentRtv = rtv;
#ifdef _EDITOR
				commandList->TransitionResource(m_pRenderTexture->GetResource(),
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					D3D12_RESOURCE_STATE_RENDER_TARGET);

				D3D12_CPU_DESCRIPTOR_HANDLE editorRtv = GetCurrentRenderTargetView(true);
				commandList->GetCommandList()->ClearRenderTargetView(editorRtv, clearColor, 0, nullptr);

				currentRtv = editorRtv;
#endif // _EDITOR

				//------------------------------------------
				// RENDER GAME
				//------------------------------------------
				Render2D(commandQueue, commandList, currentRtv);

				//------------------------------------------
				// EDITOR ONLY
				//------------------------------------------
#ifdef _EDITOR
				currentRtv = rtv;

				// Transition back to SRV for ImGui usage
				commandList->TransitionResource(m_pRenderTexture->GetResource(),
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

				RenderUI(commandQueue, commandList, currentRtv);
#endif // _EDITOR

				Present(commandQueue, commandList);
			}

			//---------------------------------------------------------------------
			void DX12System2D::ProcessWindowEvents()
			{
				std::lock_guard<std::mutex> lock(m_RenderMutex);

				{
					std::lock_guard<std::mutex> lock(m_pWindow->g_EventMutex);

					while (!m_pWindow->GetEventQueue().empty())
					{
						const win32::WindowsMsg& event = m_pWindow->GetEventQueue().front();
						m_pWindow->GetEventQueue().pop();

						if (event.msg == WM_EXITSIZEMOVE || event.msg == WM_SIZE)
						{
							Resize(m_pWindow->GetPosition(), m_pWindow->GetRealSize());
						}
#ifndef IMGUI_DISABLE
						m_ImGuiWindow.WndProcHandler(event.hwnd, event.msg, event.wParam, event.lParam);
#endif // IMGUI_DISABLE
					}
				}
			}

			//---------------------------------------------------------------------
#ifndef IMGUI_DISABLE
			void DX12System2D::RenderUI(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE a_RTVHandle)
			{
				a_pCommandList->GetCommandList()->OMSetRenderTargets(1, &a_RTVHandle, FALSE, nullptr);

				m_ImGuiWindow.Render(a_pCommandList);
			}
#endif // IMGUI_DISABLE

			//---------------------------------------------------------------------
			void DX12System2D::Render2D(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE a_RTVHandle)
			{
				core::TOOL->GetResourceAtlas().TransitionResources(a_pCommandList);

				a_pCommandList->GetCommandList()->OMSetRenderTargets(1, &a_RTVHandle, FALSE, nullptr);

				a_pCommandList->GetCommandList()->SetGraphicsRootSignature(m_pRootSignature.Get());

				a_pCommandList->GetCommandList()->RSSetViewports(1, &m_Viewport);
				a_pCommandList->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);

				ID3D12DescriptorHeap* descriptorHeaps[] = { m_SRV.GetHeap().Get() };
				a_pCommandList->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

				// TODO: RENDER LOOP.
				for (auto& pair : core::TOOL->GetECS().GetSystem<gameplay::SpriteSystem>().GetComponents())
				{
					pair.second.Render(a_pCommandList, pair.first, m_Camera);
				}

				m_eOnRender(a_pCommandList);
			}

			//---------------------------------------------------------------------
			void DX12System2D::Present(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList)
			{
				const UINT currentBackBufferIndex = GetCurrentBackBufferIndex();
				const Microsoft::WRL::ComPtr<ID3D12Resource>& backBuffer = GetCurrentBackBuffer();

				// Present
				{
					a_pCommandList->TransitionResource(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

					m_aFenceValues[currentBackBufferIndex] = a_pCommandQueue->ExecuteCommandList(a_pCommandList);

					const UINT syncInterval = g_bVSync ? 1 : 0;
					const UINT presentFlags = m_bIsTearingSupported && !g_bVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
					if (FAILED(m_pSwapChain->Present(syncInterval, presentFlags)))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed presenting.");
						return;
					}
					m_iCurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

					a_pCommandQueue->WaitForFenceValue(m_aFenceValues[m_iCurrentBackBufferIndex]);
				}
			}

			//---------------------------------------------------------------------
			std::shared_ptr<CommandQueue> DX12System2D::GetCommandQueue(D3D12_COMMAND_LIST_TYPE a_Type)
			{
				std::shared_ptr<CommandQueue> commandQueue = nullptr;
				switch (a_Type)
				{
					case D3D12_COMMAND_LIST_TYPE_DIRECT:
					{
						commandQueue = m_pDirectCommandQueue;
						break;
					}
					case D3D12_COMMAND_LIST_TYPE_COPY:
					{
						commandQueue = m_pCopyCommandQueue;
						break;
					}
					default:
					{
						break;
					}
				}

				return commandQueue;
			}

			//---------------------------------------------------------------------
#ifdef _EDITOR
			void DX12System2D::CreateRenderTexture(const glm::ivec2& a_vSize)
			{
				if (m_pRenderTexture && m_pRenderTexture->GetResource())
				{
					m_pRenderTexture->Destroy();
				}

				D3D12_RESOURCE_DESC texDesc = {};
				texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				texDesc.Width = a_vSize.x;
				texDesc.Height = a_vSize.y;
				texDesc.DepthOrArraySize = 1;
				texDesc.MipLevels = 1;
				texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				texDesc.SampleDesc.Count = 1;
				texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				m_pRenderTexture = core::TOOL->GetResourceAtlas().LoadTextureByDescription("RenderTexture", texDesc);
			}
#endif // _EDITOR
		}
	}
}