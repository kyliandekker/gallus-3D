#include "DX12System2D.h"

// core includes
#include "core/Engine.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/win32/Window.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12ShaderBind.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"

// gameplay includes
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/MeshSystem.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			constexpr glm::ivec2 RENDER_TEX_SIZE = glm::ivec2(1920, 1080);

			double FPSCounter::GetFPS() const
			{
				return m_FPS;
			}

			double FPSCounter::GetDeltaTime() const
			{
				return m_DeltaTime;
			}

			double FPSCounter::GetTotalTime() const
			{
				return m_TotalTime;
			}

			void FPSCounter::Update()
			{
				m_FrameCounter++;

				auto t1 = m_Clock.now();
				std::chrono::duration<double> deltaTime = t1 - m_T0;
				m_T0 = t1;

				m_DeltaTime = deltaTime.count(); // Convert duration to seconds

				m_ElapsedSeconds += m_DeltaTime;
				m_TotalTime += m_DeltaTime;

				if (m_ElapsedSeconds > 1.0)
				{
					m_FPS = m_FrameCounter / m_ElapsedSeconds;
					m_FrameCounter = 0;
					m_ElapsedSeconds = 0.0;
				}
			}

			void FPSCounter::Initialize()
			{
				m_FPS = 0.0;
				m_FrameCounter = 0;
				m_ElapsedSeconds = 0.0;
				m_TotalTime = 0.0;
				m_T0 = m_Clock.now();
			}

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

				if (!DirectX::XMVerifyCPUSupport())
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed verifying DirectX Math support.");
					return false;
				}
#if LOG_DX12 == 1
				LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Verified DirectX Math support.");
#endif // LOG_DX!2
				for (size_t i = 0; i < g_iBufferCount; i++)
				{
					m_BackBuffers[i] = std::make_shared<DX12Resource>("BackBuffer_" + std::to_string(i));
				}

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

				CreateRenderTexture({ RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y });

				m_eOnInitialize(*this);
#ifndef IMGUI_DISABLE
				m_ImGuiWindow.Initialize();
#endif // IMGUI_DISABLE

				std::shared_ptr<Texture> texture = core::ENGINE->GetResourceAtlas().LoadTexture("tex_missing.png", cCommandList); // Default texture.
				texture->SetResourceCategory(core::EngineResourceCategory::Missing);
				texture->SetIsDestroyable(false);

				std::shared_ptr<Texture> logo = core::ENGINE->GetResourceAtlas().LoadTexture("icon.png", cCommandList); // Logo.
				logo->SetResourceCategory(core::EngineResourceCategory::System);
				logo->SetIsDestroyable(false);

				std::shared_ptr<PixelShader> pixelShader = core::ENGINE->GetResourceAtlas().LoadPixelShader("pixelShader.hlsl"); // Default shader.
				std::shared_ptr<VertexShader> vertexShader = core::ENGINE->GetResourceAtlas().LoadVertexShader("vertexShader.hlsl"); // Default shader.
				pixelShader->SetResourceCategory(core::EngineResourceCategory::Missing);
				pixelShader->SetIsDestroyable(false);
				vertexShader->SetResourceCategory(core::EngineResourceCategory::Missing);
				vertexShader->SetIsDestroyable(false);

				std::shared_ptr<DX12ShaderBind> shaderBind = core::ENGINE->GetResourceAtlas().LoadShaderBind(pixelShader.get(), vertexShader.get()); // Default shader.

				std::shared_ptr<PixelShader> renderTexPixelShader = core::ENGINE->GetResourceAtlas().LoadPixelShader("renderTexPixelShader.hlsl"); // Default shader.
				std::shared_ptr<VertexShader> renderTexVertexShader = core::ENGINE->GetResourceAtlas().LoadVertexShader("renderTexVertexShader.hlsl"); // Default shader.
				renderTexPixelShader->SetResourceCategory(core::EngineResourceCategory::Missing);
				renderTexPixelShader->SetIsDestroyable(false);
				renderTexVertexShader->SetResourceCategory(core::EngineResourceCategory::Missing);
				renderTexVertexShader->SetIsDestroyable(false);

				std::shared_ptr<DX12ShaderBind> renderTexShaderBind = core::ENGINE->GetResourceAtlas().LoadShaderBind(renderTexPixelShader.get(), renderTexVertexShader.get()); // Render Tex shader.

				std::shared_ptr<Mesh> mesh = core::ENGINE->GetResourceAtlas().LoadMesh("square"); // Default mesh.
				MeshPartData& squarePrimitive = s_PRIMITIVES[(int)PRIMITIVES::SQUARE];
				mesh->SetMeshData(squarePrimitive);
				mesh->SetResourceCategory(core::EngineResourceCategory::Missing);
				mesh->SetIsDestroyable(false);
				mesh->UploadMesh(cCommandList);

				uint64_t fenceValue = cCommandQueue->ExecuteCommandList(cCommandList);
				cCommandQueue->WaitForFenceValue(fenceValue);

				UpdateRenderTargetViews();

				// Create RTV for custom render target texture
				if (m_pRenderTexture->GetResource())
				{
					m_pDevice->CreateRenderTargetView(m_pRenderTexture->GetResource().Get(), nullptr, m_RTV.GetCPUHandle(g_iBufferCount));
				}

				Resize({}, m_vSize);

				m_Camera.Init(RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);
				m_Camera.Transform().SetPosition({ 0.0f, 0.0f });
				
#ifndef IMGUI_DISABLE
				m_ImGuiWindow.OnRenderTargetCreated(dCommandList);
#endif // IMGUI_DISABLE
				core::ENGINE->GetResourceAtlas().TransitionResources(dCommandList);

				fenceValue = dCommandQueue->ExecuteCommandList(dCommandList);
				dCommandQueue->WaitForFenceValue(fenceValue);

				m_FpsCounter.Initialize();

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
				swapChainDesc.Scaling = DXGI_SCALING_NONE;
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
				size_t numBuffers = g_iBufferCount + 1;

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
				for (int i = 0; i < g_iBufferCount; ++i)
				{
					Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
					if (FAILED(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
					{
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving buffer.");
						return;
					}

					m_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RTV.GetCPUHandle(i));

					m_BackBuffers[i]->SetResource(backBuffer);
				}

				//if (m_pRenderTexture->GetResource())
				//{
				//	m_pDevice->CreateRenderTargetView(m_pRenderTexture->GetResource().Get(), nullptr, m_RTV.GetCPUHandle(g_iBufferCount));
				//}
			}

			//---------------------------------------------------------------------
			std::shared_ptr<Texture> DX12System2D::GetRenderTexture()
			{
				return m_pRenderTexture;
			}

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

				// b1: Sprite UV rect
				rootParameters[SPRITE_UV].InitAsConstants(4, 1); 

				// Texture SRV at register t0 (binds a texture)
				rootParameters[RootParameters::TEX_SRV].InitAsDescriptorTable(1, &descriptorRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);

				// Define static sampler at register s0 (replaces the removed descriptor table sampler)
				const CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] =
				{
					CD3DX12_STATIC_SAMPLER_DESC(
						0,                              // Register s0
						D3D12_FILTER_MIN_MAG_MIP_POINT, // nearest-neighbor
						D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
						D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
						D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
						0.0f,                           // Mip LOD bias
						0,                              // max anisotropy (ignored for point)
						D3D12_COMPARISON_FUNC_ALWAYS,
						D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
						0.0f,                           // MinLOD
						0.0f,                           // MaxLOD -> force top mip only
						D3D12_SHADER_VISIBILITY_PIXEL
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
					m_BackBuffers[i]->GetResource().Reset();  // This will release the old resource properly
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
				m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, m_vSize.x, m_vSize.y);
				m_ScissorRect = CD3DX12_RECT(0, 0, m_vSize.x, m_vSize.y);

				m_vSize = glm::vec2(a_vSize.x, a_vSize.y);
			}

			//---------------------------------------------------------------------
			const std::shared_ptr<DX12Resource>& DX12System2D::GetCurrentBackBuffer() const
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
				if (a_bUseRenderTexture && m_pRenderTexture->IsValid())
				{
					size_t backBufferStart = g_iBufferCount;

					return m_RTV.GetCPUHandle(backBufferStart);
				}

				return m_RTV.GetCPUHandle(m_iCurrentBackBufferIndex);
			}

			//---------------------------------------------------------------------
			void DX12System2D::Loop()
			{
				if (!m_bInitialized.load())
				{
					return;
				}

				m_FpsCounter.Update();

				std::shared_ptr<CommandQueue> cCommandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				std::shared_ptr<CommandList> cCommandList = cCommandQueue->GetCommandList();

				for (auto& pair : core::ENGINE->GetECS().GetSystem<gameplay::MeshSystem>().GetComponents())
				{
					if (pair.second.GetMesh())
					{
						pair.second.GetMesh()->UploadMesh(cCommandList);
					}
				}

				uint64_t fenceValue = cCommandQueue->ExecuteCommandList(cCommandList);
				cCommandQueue->WaitForFenceValue(fenceValue);

				ProcessWindowEvents();

				auto commandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
				auto commandList = commandQueue->GetCommandList();

				auto backBuffer = GetCurrentBackBuffer();
				UINT backIndex = GetCurrentBackBufferIndex();
				const FLOAT clearColor[] = { 0, 0, 0, 1 };

				// Keep RTV handles outside of scopes
				D3D12_CPU_DESCRIPTOR_HANDLE backRtv;  // For back buffer

				// 1. Render 2D scene into RenderTexture
				if (m_pRenderTexture->CanBeDrawn())
				{
					// Transition RenderTexture -> RTV
					m_pRenderTexture->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

					// Render onto the render tex rtv (true arg does this).
					D3D12_CPU_DESCRIPTOR_HANDLE rtRtv = GetCurrentRenderTargetView(true);
					commandList->GetCommandList()->ClearRenderTargetView(rtRtv, clearColor, 0, nullptr);
					commandList->GetCommandList()->OMSetRenderTargets(1, &rtRtv, FALSE, nullptr);

					Render2D(commandQueue, commandList, rtRtv);

					m_pRenderTexture->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				}

				backBuffer->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

				// back buffer rtv.
				backRtv = GetCurrentRenderTargetView(false);
				commandList->GetCommandList()->ClearRenderTargetView(backRtv, clearColor, 0, nullptr);
				commandList->GetCommandList()->OMSetRenderTargets(1, &backRtv, FALSE, nullptr);
#ifndef _EDITOR
				// 1. Render RenderTexture onto quad.
				if (m_pRenderTexture->CanBeDrawn())
				{
					// Bind pipeline + root signature
					commandList->GetCommandList()->SetPipelineState(
						core::ENGINE->GetResourceAtlas().GetRenderTexShaderBind()->GetPipelineState());
					commandList->GetCommandList()->SetGraphicsRootSignature(
						core::ENGINE->GetDX12().GetRootSignature().Get());

					m_pRenderTexture->Bind(commandList, 2);

					commandList->GetCommandList()->RSSetViewports(1, &m_Viewport);
					commandList->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);

					// Draw fullscreen quad (shader-generated)
					commandList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
					commandList->GetCommandList()->IASetVertexBuffers(0, 0, nullptr);
					commandList->GetCommandList()->DrawInstanced(3, 1, 0, 0);
				}
#endif // _EDITOR

#ifdef _EDITOR
				RenderUI(commandQueue, commandList, backRtv);
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
				core::ENGINE->GetResourceAtlas().TransitionResources(a_pCommandList);
				a_pCommandList->GetCommandList()->OMSetRenderTargets(1, &a_RTVHandle, FALSE, nullptr);
				a_pCommandList->GetCommandList()->SetGraphicsRootSignature(m_pRootSignature.Get());

				D3D12_VIEWPORT rtViewport{};
				rtViewport.TopLeftX = 0.0f;
				rtViewport.TopLeftY = 0.0f;
				rtViewport.Width = static_cast<float>(RENDER_TEX_SIZE.x);
				rtViewport.Height = static_cast<float>(RENDER_TEX_SIZE.y);
				rtViewport.MinDepth = 0.0f;
				rtViewport.MaxDepth = 1.0f;

				CD3DX12_RECT rtScissor(0, 0, RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);

				a_pCommandList->GetCommandList()->RSSetViewports(1, &rtViewport);
				a_pCommandList->GetCommandList()->RSSetScissorRects(1, &rtScissor);

				ID3D12DescriptorHeap* descriptorHeaps[] = { m_SRV.GetHeap().Get() };
				a_pCommandList->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

				std::lock_guard<std::recursive_mutex> lock(core::ENGINE->GetECS().m_EntityMutex);
				for (auto& pair : core::ENGINE->GetECS().GetSystem<gameplay::SpriteSystem>().GetComponents())
				{
					pair.second.Render(a_pCommandList, pair.first, m_Camera);
				}
				for (auto& pair : core::ENGINE->GetECS().GetSystem<gameplay::MeshSystem>().GetComponents())
				{
					pair.second.Render(a_pCommandList, pair.first, m_Camera);
				}

				m_eOnRender(a_pCommandList);
			}

			//---------------------------------------------------------------------
			void DX12System2D::Present(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList)
			{
				const UINT currentBackBufferIndex = GetCurrentBackBufferIndex();
				const std::shared_ptr<DX12Resource>& backBuffer = GetCurrentBackBuffer();

				// Present
				{
					backBuffer->Transition(a_pCommandList, D3D12_RESOURCE_STATE_PRESENT);

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

				m_pRenderTexture = core::ENGINE->GetResourceAtlas().LoadTextureByDescription("RenderTexture", texDesc);
			}
		}
	}
}