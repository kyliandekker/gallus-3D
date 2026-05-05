#include "DX12System.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/DX12Resource.h"
#include "graphics/dx12/HeapAllocation.h"
#include "graphics/dx12/Camera.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/DirectionalLight.h"
#include "graphics/dx12/DX12UploadBufferAllocator.h"
#include "graphics/dx12/RenderView.h"

#include "graphics/dx12/ShaderFactory.h"

#include "graphics/win32/Window.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/MeshSystem.h"

// resources
#include "resources/ResourceAtlas.h"

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	// DX12System
	//---------------------------------------------------------------------
	DX12System::DX12System() = default;		
	DX12System::~DX12System() = default;		

	//---------------------------------------------------------------------
	bool DX12System::Initialize(bool a_bWait, HWND a_hWnd, const IVector2& a_vSize, win32::Window* a_pWindow, size_t a_iNumViews)
	{
		m_vSize = a_vSize;
		m_hWnd = a_hWnd;
		m_pWindow = a_pWindow;

		for (size_t i = 0; i < a_iNumViews; i++)
		{
			m_aRenderViews.push_back(std::make_shared<RenderView>());
		}

		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Initializing dx12 System.");
		return ThreadedSystem::Initialize(a_bWait);
	}

	//---------------------------------------------------------------------
	bool DX12System::Destroy()
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Destroying dx12 System.");
		return ThreadedSystem::Destroy();
	}

	//---------------------------------------------------------------------
	bool DX12System::InitThreadWorker()
	{
#if _DEBUG
		// Always enable the debug layer before doing anything DX12 related
		// so all possible errors generated while creating DX12 objects
		// are caught by the debug layer.
		Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
		if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed initializing DX12 System: Failed to get debug interface.");
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
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed initializing DX12 System: Failed to retrieve adapter.");
			return false;
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully retrieved adapter.");
#endif // LOG_DX!2

		// Create the device.
		if (!CreateDevice())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed initializing DX12 System: Failed creating device.");
			return false;
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully created device.");
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
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed initializing DX12 System: Failed creating command queues.");
			return false;
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully created command queues.");
#endif // LOG_DX!2

		m_bIsTearingSupported = CheckTearingSupport();

		if (!DirectX::XMVerifyCPUSupport())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed initializing DX12 System: Failed verifying DirectX Math support.");
			return false;
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully verified DirectX Math support.");
#endif // LOG_DX!2
		for (size_t i = 0; i < g_iBufferCount; i++)
		{
			m_BackBuffers[i] = std::make_shared<DX12Resource>();
			m_BackBuffers[i]->SetName("BackBuffer_" + std::to_string(i));
		}

		// Create the swap chain.
		if (!CreateSwapChain())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed initializing DX12 System: Failed creating swap chain.");
			return false;
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully created swap chain.");
#endif // LOG_DX!2

		// Create views.
		if (!CreateViews())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed initializing DX12 System: Failed creating views.");
			return false;
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully created views.");
#endif // LOG_DX!2

		if (!CreateRootSignature())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed initializing DX12 System: Failed creating root signature.");
			return false;
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully created root signature.");
#endif // LOG_DX!2

		m_pSkinningDataAllocation = std::make_shared<DX12UploadBufferAllocator>();
		m_pSkinningDataAllocation->Initialize(
			m_pDevice.Get(),
			sizeof(SkinningData),
			1000,
			256
		);
		m_pSkinningDataAllocation->Allocate();

		// Get the direct command queue.
		std::shared_ptr<CommandQueue> dCommandQueue = GetCommandQueue();
		std::shared_ptr<CommandList> dCommandList = dCommandQueue->GetCommandList();

		// Get the copy command queue.
		std::shared_ptr<CommandQueue> cCommandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

		resources::ResourceAtlas* resourceAtlas = GetEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return false;
		}

		std::weak_ptr<Texture> texturePtr = resourceAtlas->LoadTexture("tex_missing.png", cCommandQueue); // Default texture.
		if (std::shared_ptr<Texture> tex = texturePtr.lock())
		{
			tex->SetResourceCategory(resources::EngineResourceCategory::Missing);
			tex->SetIsDestroyable(false);
		}

		std::shared_ptr<PixelShader> pixelShaderPtr = resourceAtlas->LoadPixelShader("pixelShader.cso"); // Default shader.
		std::shared_ptr<VertexShader> vertexShaderPtr = resourceAtlas->LoadVertexShader("vertexShader.cso"); // Default shader.

		pixelShaderPtr->SetResourceCategory(resources::EngineResourceCategory::Missing);
		pixelShaderPtr->SetIsDestroyable(false);

		vertexShaderPtr->SetResourceCategory(resources::EngineResourceCategory::Missing);
		vertexShaderPtr->SetIsDestroyable(false);

		PipelineStateCache::GetOrCreate(pixelShaderPtr, vertexShaderPtr, DXGI_FORMAT_D32_FLOAT);

		std::shared_ptr<PixelShader> renderTexPixelShaderPtr = resourceAtlas->LoadPixelShader("renderTexPixelShader.cso"); // Default render tex shader.
		std::shared_ptr<VertexShader> renderTexVertexShaderPtr = resourceAtlas->LoadVertexShader("renderTexVertexShader.cso"); // Default render tex shader.
		renderTexPixelShaderPtr->SetResourceCategory(resources::EngineResourceCategory::Missing);
		renderTexPixelShaderPtr->SetIsDestroyable(false);
		renderTexVertexShaderPtr->SetResourceCategory(resources::EngineResourceCategory::Missing);
		renderTexVertexShaderPtr->SetIsDestroyable(false);

		m_pRenderTextureShaderBind = PipelineStateCache::GetOrCreate(renderTexPixelShaderPtr, renderTexVertexShaderPtr, DXGI_FORMAT_UNKNOWN);

		std::weak_ptr<Mesh> meshPtr = resourceAtlas->LoadMesh("mod_missing.glb", cCommandQueue); // Default mesh.
		if (std::shared_ptr<Mesh> mesh = meshPtr.lock())
		{
			mesh->SetResourceCategory(resources::EngineResourceCategory::Missing);
			mesh->SetIsDestroyable(false);
		}

		std::weak_ptr<Mesh> squareMeshPtr = resourceAtlas->LoadMeshEmpty("square"); // Square mesh.
		if (std::shared_ptr<Mesh> mesh = squareMeshPtr.lock())
		{
			MeshPartData& squarePrimitive = s_PRIMITIVES[(int) PRIMITIVES::SQUARE];
			mesh->LoadData(squarePrimitive, cCommandQueue);
			mesh->SetResourceCategory(resources::EngineResourceCategory::Engine);
			mesh->SetAssetType(resources::AssetType::Mesh);
			mesh->SetIsDestroyable(false);
		}

		std::weak_ptr<Mesh> cubeMeshPtr = resourceAtlas->LoadMeshEmpty("cube"); // Cube mesh.
		if (std::shared_ptr<Mesh> mesh = cubeMeshPtr.lock())
		{
			MeshPartData& squarePrimitive = s_PRIMITIVES[(int) PRIMITIVES::CUBE];
			mesh->LoadData(squarePrimitive, cCommandQueue);
			mesh->SetResourceCategory(resources::EngineResourceCategory::Engine);
			mesh->SetAssetType(resources::AssetType::Mesh);
			mesh->SetIsDestroyable(false);
		}

		std::weak_ptr<Mesh> cylinderMeshPtr = resourceAtlas->LoadMeshEmpty("cylinder"); // Cylinder mesh.
		if (std::shared_ptr<Mesh> mesh = cylinderMeshPtr.lock())
		{
			MeshPartData& squarePrimitive = s_PRIMITIVES[(int) PRIMITIVES::CYLINDER];
			mesh->LoadData(squarePrimitive, cCommandQueue);
			mesh->SetResourceCategory(resources::EngineResourceCategory::Engine);
			mesh->SetAssetType(resources::AssetType::Mesh);
			mesh->SetIsDestroyable(false);
		}

		std::weak_ptr<Mesh> sphereMeshPtr = resourceAtlas->LoadMeshEmpty("sphere"); // Sphere mesh.
		if (std::shared_ptr<Mesh> mesh = sphereMeshPtr.lock())
		{
			MeshPartData& squarePrimitive = s_PRIMITIVES[(int) PRIMITIVES::SPHERE];
			mesh->LoadData(squarePrimitive, cCommandQueue);
			mesh->SetResourceCategory(resources::EngineResourceCategory::Engine);
			mesh->SetAssetType(resources::AssetType::Mesh);
			mesh->SetIsDestroyable(false);
		}

		std::weak_ptr<Material> material = resourceAtlas->LoadMaterialEmpty("default"); // Material.
		if (std::shared_ptr<Material> mat = material.lock())
		{
			mat->SetColor({1, 1, 1, 1});
			mat->SetEnableLighting(false);
			mat->SetResourceCategory(resources::EngineResourceCategory::Engine);
			mat->SetAssetType(resources::AssetType::Material);
			mat->SetIsDestroyable(false);
		}

		cCommandQueue->Flush();

		UpdateRenderTargetViews();

		Resize({0, 0}, m_vSize);

		m_pCamera = std::make_unique<Camera>();

		m_pCamera->Init(RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);
		m_pCamera->GetTransform().SetPosition({ 0.0f, 1.0f, -2.0f });
		
		m_pActiveCamera = m_pCamera.get();

		m_RenderTexViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);
		m_RenderTexScissorRect = CD3DX12_RECT(0, 0, RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y);

		m_pTransformAllocation = std::make_shared<DX12UploadBufferAllocator>();
		m_pTransformAllocation->Initialize(
			m_pDevice.Get(),
			sizeof(ShaderTransform),
			1000,
			256
		);
		m_pTransformAllocation->Allocate(); // default 0, 0, 0 allocation.
		
		m_eOnRenderTargetCreate(*this, dCommandList);

		int fenceValue = dCommandQueue->ExecuteCommandList(dCommandList);
		dCommandQueue->WaitForFenceValue(fenceValue);
		
		m_pDirectionalLight = std::make_shared<DirectionalLight>();
		
		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return false;
		}

		ecs->OnEntitiesUpdated() += std::bind(&DX12System::ReorderSpriteComponents, this);
		ecs->OnEntityComponentsUpdated() += std::bind(&DX12System::ReorderSpriteComponents, this);
		ReorderSpriteComponents();

		m_FpsCounter.Initialize();
		m_FpsCounter.m_eOnNewFrame += std::bind(&DX12System::NewFrame, this, std::placeholders::_1);
		m_FpsCounter.DisableFPSLimit();

		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_DX12, "Successfully initialized DX12 System.");

		return true;
	}

	//---------------------------------------------------------------------
	bool DX12System::GetAdapter(bool a_bUseWarp)
	{
		Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
		UINT createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

		if (FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory))))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving adapter: Failed creating DXGI Factory.");
			return false;
		}

		Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;

		if (a_bUseWarp)
		{
			if (FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1))))
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving adapter: Failed creating warp adapter.");
				return false;
			}
			if (FAILED(dxgiAdapter1.As(&m_pAdapter)))
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving adapter: Failed casting adapter.");
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
						LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving adapter: Failed casting adapter.");
						return false;
					}
				}
			}
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully created adapter.");
#endif // LOG_DX!2

		return true;
	}

	//---------------------------------------------------------------------
	bool DX12System::CreateDevice()
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
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating device: Failed pushing storage filter.");
				return false;
			}
		}
#endif // _DEBUG

		return true;
	}

	//---------------------------------------------------------------------
	bool DX12System::CheckTearingSupport()
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
	bool DX12System::CreateSwapChain()
	{
		Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory4;
		UINT createFactoryFlags = 0;
#ifdef _DEBUG
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

		if (FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4))))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating swapchain: Failed creating DXGI factory.");
			return false;
		}
#if LOG_DX12 == 1
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully created DXGI factory.");
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
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating swapchain: Failed associating window.");
			return false;
		}

		if (FAILED(swapChain1.As(&m_pSwapChain)))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating swapchain: Failed casting swap chain.");
			return false;
		}

		m_iCurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		return m_pSwapChain != nullptr;
	}

	//---------------------------------------------------------------------
	void DX12System::CreateRTV()
	{
		size_t numBuffers = g_iBufferCount + m_aRenderViews.size();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = static_cast<UINT>(numBuffers);
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_RTV = std::make_unique<HeapAllocation>(rtvHeapDesc);

		for (size_t i = 0; i < g_iBufferCount; i++)
		{
			m_RTV->Allocate();
		}
	}

	//---------------------------------------------------------------------
	void DX12System::CreateSRV()
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 100;  // Adjust based on how many textures you need
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // Important for binding!
		m_SRV = std::make_unique<HeapAllocation>(srvHeapDesc);
	}

	//---------------------------------------------------------------------
	void DX12System::CreateDSV()
	{
		// Create the descriptor heap for the depth-stencil view.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = m_aRenderViews.size();
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_DSV = std::make_unique<HeapAllocation>(dsvHeapDesc);
	}

	//---------------------------------------------------------------------
	void DX12System::Finalize()
	{
		std::lock_guard<std::mutex> lock(m_RenderMutex);

		Flush();

		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_DX12, "Successfully destroyed dx12 System.");
	}

	//---------------------------------------------------------------------
	void DX12System::Flush()
	{
		m_pDirectCommandQueue->Flush();
		m_pCopyCommandQueue->Flush();
	}

	//---------------------------------------------------------------------
	void DX12System::UpdateRenderTargetViews()
	{
		for (int i = 0; i < g_iBufferCount; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			if (FAILED(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed retrieving buffer.");
				return;
			}

			m_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RTV->GetCPUHandle(i));

			m_BackBuffers[i]->SetResource(backBuffer);
		}
	}

	//---------------------------------------------------------------------
	bool DX12System::CreateCommandQueues()
	{
		m_pDirectCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_pCopyCommandQueue = std::make_shared<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);

		return true;
	}

	//---------------------------------------------------------------------
	bool DX12System::CreateViews()
	{
		CreateRTV();
		CreateSRV();
		CreateDSV();

		return true;
	}

	//---------------------------------------------------------------------
	bool DX12System::CreateRootSignature()
	{
		// Define descriptor ranges
		CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[1]{};

		// SRV for the texture at register t0
		descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		// Define root parameters
		CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParameters]{};

		// b0: Transform
		rootParameters[RootParameters::TRANSFORM].InitAsConstantBufferView(RootParameters::TRANSFORM);

		// b1: Texture UV rect
		rootParameters[RootParameters::SPRITE_UV].InitAsConstants(4, RootParameters::SPRITE_UV);

		// b2: Material
		rootParameters[RootParameters::MATERIAL].InitAsConstantBufferView(RootParameters::MATERIAL);

		// b3: Directional Light
		rootParameters[RootParameters::DIRECTIONAL_LIGHT].InitAsConstants(
			sizeof(DirectionalLightData) / 4,
			RootParameters::DIRECTIONAL_LIGHT
		);

		// b4: Skinning Data
		rootParameters[RootParameters::SKINNING_DATA].InitAsConstantBufferView(RootParameters::SKINNING_DATA);

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
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating root signature: Failed serializing root signature: \"%s\".", errorMessage.c_str());
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
	void DX12System::AfterResize(const IVector2& a_vSize)
	{}

	//---------------------------------------------------------------------
	void DX12System::Resize(const IVector2& a_vPos, const IVector2& a_vSize)
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

		m_eOnRenderTargetCleaned();

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		if (FAILED(m_pSwapChain->GetDesc(&swapChainDesc)))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resizing: Failed getting desc from swap chain.");
			return;
		}
		if (FAILED(m_pSwapChain->ResizeBuffers(g_iBufferCount, a_vSize.x,
			a_vSize.y, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags)))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resizing: Failed resizing buffers.");
			return;
		}

		AfterResize(a_vSize);

		m_iCurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		UpdateRenderTargetViews();

		m_eOnResize(a_vPos, a_vSize);
		m_WindowViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, m_vSize.x, m_vSize.y);
		m_WindowScissorRect = CD3DX12_RECT(0, 0, m_vSize.x, m_vSize.y);

		m_vSize = IVector2(a_vSize.x, a_vSize.y);
	}

	//---------------------------------------------------------------------
	void DX12System::SetDepthBuffer(std::unique_ptr<DX12Resource>& a_pDepthBuffer, size_t a_iIndex)
	{
		a_pDepthBuffer = std::make_unique<DX12Resource>();
		m_DSV->Deallocate(a_iIndex);

		// Resize screen dependent resources.
		// Create a depth buffer.
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		CD3DX12_HEAP_PROPERTIES heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC tex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, RENDER_TEX_SIZE.x, RENDER_TEX_SIZE.y,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		if (!a_pDepthBuffer->SetResourceData("Depth Buffer", tex, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimizedClearValue))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed resizing depth buffer: Failed creating committed resource.");
			return;
		}

		// Update the depth-stencil view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		m_pDevice->CreateDepthStencilView(a_pDepthBuffer->GetResource().Get(), &dsv,
			m_DSV->GetCPUHandle(a_iIndex));
	}

	//---------------------------------------------------------------------
	const std::shared_ptr<DX12Resource>& DX12System::GetCurrentBackBuffer() const
	{
		return m_BackBuffers[m_iCurrentBackBufferIndex];
	}

	//---------------------------------------------------------------------
	UINT DX12System::GetCurrentBackBufferIndex() const
	{
		return m_iCurrentBackBufferIndex;
	}

	//---------------------------------------------------------------------
	void DX12System::Loop()
	{
		if (!m_bInitialized.load())
		{
			return;
		}

		m_FpsCounter.Update();
	}

	//---------------------------------------------------------------------
	std::shared_ptr<RenderView> DX12System::RegisterView(size_t a_iIndex)
	{
		return m_aRenderViews[a_iIndex];
	}

	//---------------------------------------------------------------------
	void DX12System::NewFrame(float a_fDeltaTime)
	{
		m_eOnNewFrame(m_FpsCounter.GetFPS());

		ProcessWindowEvents();

		std::shared_ptr<CommandQueue> commandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		std::shared_ptr<CommandList> commandList = commandQueue->GetCommandList();

		const std::shared_ptr<DX12Resource>& backBuffer = GetCurrentBackBuffer();
		UINT backIndex = GetCurrentBackBufferIndex();
		const FLOAT clearColor[] = { 0, 0, 0, 0 };

		commandList->GetCommandList()->SetGraphicsRootSignature(m_pRootSignature.Get());

		for (std::shared_ptr<RenderView>& renderView : m_aRenderViews)
		{
			std::shared_ptr<Texture> renderTex = renderView->m_pRenderTarget.lock();
			if (renderTex)
			{
				// Transition RenderTexture -> RTV
				renderTex->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

				D3D12_CPU_DESCRIPTOR_HANDLE rtRtv = m_RTV->GetCPUHandle(renderView->m_iRTVIndex);
				D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_DSV->GetCPUHandle(renderView->m_iDSVIndex);

				commandList->GetCommandList()->ClearRenderTargetView(rtRtv, clearColor, 0, nullptr);
				commandList->GetCommandList()->ClearDepthStencilView(
					dsv,
					D3D12_CLEAR_FLAG_DEPTH,
					1.0f,
					0,
					0,
					nullptr
				);

				commandList->GetCommandList()->OMSetRenderTargets(1, &rtRtv, FALSE, &dsv);

				commandList->GetCommandList()->RSSetViewports(1, &renderView->m_Viewport);
				commandList->GetCommandList()->RSSetScissorRects(1, &renderView->m_ScissorRect);

				renderView->m_RenderFunc(commandQueue, commandList);

				renderTex->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			}
		}

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_SRV->GetHeap().Get() };
		commandList->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		backBuffer->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// back buffer rtv.
		D3D12_CPU_DESCRIPTOR_HANDLE backRtv = m_RTV->GetCPUHandle(m_iCurrentBackBufferIndex);
		commandList->GetCommandList()->ClearRenderTargetView(backRtv, clearColor, 0, nullptr);

		commandList->GetCommandList()->RSSetViewports(1, &m_WindowViewport);
		commandList->GetCommandList()->RSSetScissorRects(1, &m_WindowScissorRect);

		commandList->GetCommandList()->OMSetRenderTargets(1, &backRtv, FALSE, nullptr);

		m_eOnRenderTexReady(commandList);

		Present(commandQueue, commandList);
	}

	//---------------------------------------------------------------------
	void DX12System::ProcessWindowEvents()
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
				m_eOnEventsProcess(event.hwnd, event.msg, event.wParam, event.lParam);
			}
		}
	}

	//---------------------------------------------------------------------
	void DX12System::RenderObjects(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE a_RTVHandle)
	{
		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		if (!ecs->Running())
		{
			return;
		}

		if (!m_pActiveCamera)
		{
			return;
		}

		a_pCommandList->GetCommandList()->SetGraphicsRootSignature(m_pRootSignature.Get());
		
		a_pCommandList->GetCommandList()->RSSetViewports(1, &m_RenderTexViewport);
		a_pCommandList->GetCommandList()->RSSetScissorRects(1, &m_RenderTexScissorRect);

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_SRV->GetHeap().Get() };
		a_pCommandList->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		std::lock_guard<std::recursive_mutex> lock(ecs->m_EntityMutex);
		gameplay::MeshSystem* meshSys = ecs->GetSystem<gameplay::MeshSystem>();
		if (!meshSys)
		{
			return;
		}
		for (auto& pair : meshSys->GetComponents())
		{
			pair.second->Render(a_pCommandList, pair.first, *m_pActiveCamera);
		}
		for (std::weak_ptr<gameplay::SpriteComponent> sprite : m_aOrderedTextures)
		{
			if (std::shared_ptr<gameplay::SpriteComponent> spritePtr = sprite.lock())
			{
				spritePtr->Render(a_pCommandList, spritePtr->GetEntityID(), *m_pActiveCamera);
			}
		}
	}

	//---------------------------------------------------------------------
	void DX12System::Present(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList)
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
	std::shared_ptr<CommandQueue> DX12System::GetCommandQueue(D3D12_COMMAND_LIST_TYPE a_Type)
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
	Camera& DX12System::GetCamera()
	{
		return *m_pCamera.get();
	}
	
	//---------------------------------------------------------------------
	void DX12System::ReorderSpriteComponents()
	{
		m_aOrderedTextures.clear();

		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		gameplay::SpriteSystem* spriteSys = ecs->GetSystem<gameplay::SpriteSystem>();
		if (!spriteSys)
		{
			return;
		}

		// Collect weak_ptr references to components
		for (auto& pair : spriteSys->GetComponents())
		{
			m_aOrderedTextures.push_back(pair.second);
		}

		// Sort back-to-front by order
		std::sort(
			m_aOrderedTextures.begin(),
			m_aOrderedTextures.end(),
			[](const std::weak_ptr<gameplay::SpriteComponent>& a, const std::weak_ptr<gameplay::SpriteComponent>& b)
			{
				std::shared_ptr<gameplay::SpriteComponent> aLocked = a.lock();
				std::shared_ptr<gameplay::SpriteComponent> bLocked = b.lock();

				if (!aLocked)
				{
					return false;
				}

				if (!bLocked)
				{
					return true;
				}

				return aLocked->GetOrder() < bLocked->GetOrder();
			}
		);
	}
}