#pragma once

#include "DX12PCH.h"
#include "core/System.h"

// external
#include <memory>
#include <mutex>

// core
#include "engineEntryPoints.h"
#include "core/Event.h"
#include "utils/math.h"

// graphics
#include "graphics/dx12/ShaderDefs.h"
#include "graphics/dx12/FPSCounter.h"

#undef min
#undef max

namespace gallus
{
	class IVector2;
}
namespace gallus::gameplay
{
	class SpriteComponent;
}
namespace gallus::graphics::win32
{
	class Window;
}
namespace gallus::graphics::dx12
{
	/// Whether DX12 makes a full log of creating devices, resources, etc (spam).
#define LOG_DX12 1

	inline const uint8_t g_iBufferCount = 3; /// Number of swap chain buffers.
	inline bool g_bVSync = false; /// Whether V-Sync is enabled.

	class CommandQueue;
	class CommandList;
	class DX12Resource;
	class HeapAllocation;
	class DX12UploadBufferAllocator;
	class Camera;
	class Texture;
	class Material;
	class DirectionalLight;
	class RenderView;

	enum DimensionDrawMode
	{
		DimensionDrawMode_2D3D,
		DimensionDrawMode_3D,
		DimensionDrawMode_2D,
	};

	//---------------------------------------------------------------------
	// DX12System
	//---------------------------------------------------------------------
	/// <summary>
	/// Represents a DirectX 12 rendering window, managing device resources, rendering, and synchronization. Only basic 2D rendering.
	/// </summary>
	class DX12System : public core::ThreadedSystem
	{
	public:
		/// <summary>
		/// Constructs the DX12 System.
		/// </summary>
		DX12System();

		/// <summary>
		/// Destroys the DX12 System.
		/// </summary>
		~DX12System();

		/// <summary>
		/// Initializes the system, setting up necessary resources.
		/// </summary>
		/// <param name="a_bWait">Determines whether the application waits until the system has been fully initialized.</param>
		/// <param name="a_hWnd">Handle to the window.</param>
		/// <param name="a_vSize">Size of the window.</param>
		/// <param name="a_pWindow">The window.</param>
		/// <param name="a_iNumViews">Number of views that should be allocated.</param>
		/// <returns>True if the initialization was successful, otherwise false.</returns>
		bool Initialize(bool a_bWait, HWND a_hWnd, const IVector2& a_vSize, win32::Window* a_pWindow, size_t a_iNumViews);

		/// <summary>
		/// Cleans up resources and destroys the dx12 window.
		/// </summary>
		/// <returns>True if destruction succeeds, otherwise false.</returns>
		bool Destroy() override;

		/// <summary>
		/// Retrieves the current back buffer fence value.
		/// </summary>
		/// <returns>Integer representing the back buffer fence value.</returns>
		uint64_t GetCurrentFenceValue() const
		{
			return m_aFenceValues[GetCurrentBackBufferIndex()];
		}

		/// <summary>
		/// Retrieves the camera.
		/// </summary>
		/// <returns>Reference to the camera.</returns>
		Camera& GetCamera();
		
		/// <summary>
		/// Retrieves the directional light.
		/// </summary>
		/// <returns>Pointer to the directional light.</returns>
		std::weak_ptr<DirectionalLight> GetDirectionalLight()
		{
			return m_pDirectionalLight;
		}
	protected:
		/// <summary>
		/// Indicates whether the thread should sleep or wake to run work.
		/// Must be implemented by subclasses.
		/// </summary>
		/// <returns>True if thread should sleep (wait), false to proceed immediately.</returns>
		bool Sleep() const override
		{
			return true;
		}

		/// <summary>
		/// Called once on the thread to perform initialization steps.
		/// </summary>
		/// <returns>True if the initialization was successful, otherwise false.</returns>
		bool InitThreadWorker() override;

		/// <summary>
		/// Retrieves the DXGI adapter.
		/// </summary>
		/// <param name="a_bUseWarp">Whether to use the WARP adapter.</param>
		/// <returns>True if the adapter was successfully retrieved, otherwise false.</returns>
		bool GetAdapter(bool a_bUseWarp);

		/// <summary>
		/// Creates the DirectX 12 device.
		/// </summary>
		/// <param name="a_Adapter">The DXGI adapter to use for the device.</param>
		/// <returns>True if the device was successfully created, otherwise false.</returns>
		bool CreateDevice();

		/// <summary>
		/// Checks if tearing is supported for V-Sync-less rendering.
		/// </summary>
		/// <returns>True if tearing is supported, otherwise false.</returns>
		bool CheckTearingSupport();

		/// <summary>
		/// Creates the swap chain.
		/// </summary>
		/// <returns>True if the swap chain was successfully created, otherwise false.</returns>
		bool CreateSwapChain();

		/// <summary>
		/// Creates the RTV.
		/// </summary>
		void CreateRTV();

		/// <summary>
		/// Creates the SRV.
		/// </summary>
		void CreateSRV();

		/// <summary>
		/// Creates the DSV.
		/// </summary>
		void CreateDSV();

		/// <summary>
		/// Destroys the system, releasing resources and performing necessary cleanup.
		/// </summary>
		/// <returns>True if the destruction was successful, otherwise false.</returns>
		void Finalize() override;

		/// <summary>
		/// Ensures all previously submitted GPU commands are completed before continuing execution.
		/// </summary>
		/// <remarks>
		/// This method blocks the CPU until the GPU has finished processing all commands up to this point. 
		/// It is useful for synchronization, resource cleanup, or preparing for device destruction.
		/// </remarks>
		void Flush();

		/// <summary>
		/// Resizes the window.
		/// </summary>
		/// <param name="a_vPos">The position of the window.</param>
		/// <param name="a_vSize">The size of the window.</param>
		void Resize(const IVector2& a_vPos, const IVector2& a_vSize);

		/// <summary>
		/// Retrieves the current back buffer.
		/// </summary>
		/// <returns>ComPtr to the current back buffer.</returns>
		const std::shared_ptr<DX12Resource>& GetCurrentBackBuffer() const;

		/// <summary>
		/// Retrieves the current back buffer index.
		/// </summary>
		/// <returns>Integer representing the back buffer index.</returns>
		UINT GetCurrentBackBufferIndex() const;
	public:
		std::mutex m_RenderMutex;

		/// <summary>
		/// Resizes the depth buffer.
		/// </summary>
		void SetDepthBuffer(std::unique_ptr<DX12Resource>& a_pDepthBuffer);

		/// <summary>
		/// Loop method for the thread.
		/// </summary>
		void Loop() override;

		/// <summary>
		/// Processes win32 window events.
		/// </summary>
		void ProcessWindowEvents();

		/// <summary>
		/// Renders the DX12 stuff to the screen.
		/// </summary>
		void RenderObjects(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE a_RTVHandle);

		/// <summary>
		/// Presents everything to the DX12 render loop.
		/// </summary>
		void Present(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList);

		/// <summary>
		/// Retrieves the RTV heap.
		/// </summary>
		/// <returns>Reference to the RTV heap allocation.</returns>
		HeapAllocation& GetRTV()
		{
			return *m_RTV.get();
		};

		/// <summary>
		/// Retrieves the SRV heap.
		/// </summary>
		/// <returns>Reference to the SRV heap allocation.</returns>
		HeapAllocation& GetSRV()
		{
			return *m_SRV.get();
		};

		/// <summary>
		/// Retrieves the DSV heap.
		/// </summary>
		/// <returns>Reference to the DSV heap allocation.</returns>
		HeapAllocation& GetDSV()
		{
			return *m_DSV.get();
		};

		/// <summary>
		/// Retrieves the DirectX 12 device.
		/// </summary>
		/// <returns>ComPtr to the ID3D12Device2.</returns>
		Microsoft::WRL::ComPtr<ID3D12Device2>& GetDevice()
		{
			return m_pDevice;
		};

		/// <summary>
		/// Retrieves the DirectX 12 root signature.
		/// </summary>
		/// <returns>ComPtr to the ID3D12RootSignature.</returns>
		Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature()
		{
			return m_pRootSignature;
		};

		/// <summary>
		/// Retrieves a command queue of a certain type.
		/// </summary>
		/// <param name="a_Type">The type of command queue such as direct, copy, etc.</param>
		/// <returns>Reference to the CommandQueue.</returns>
		std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE a_Type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		/// <summary>
		/// Updates the render target views (RTVs) for the back buffers.
		/// </summary>
		void UpdateRenderTargetViews();

		/// <summary>
		/// Retrieves the fps info.
		/// </summary>
		/// <returns>Reference to the fps class containing all fps info.</returns>
		const FPSCounter& GetFPS() const
		{
			return m_FpsCounter;
		}

		/// <summary>
		/// Retrieves the camera.
		/// </summary>
		/// <returns>Reference to the camera.</returns>
		Camera& GetActiveCamera()
		{
			return *m_pActiveCamera;
		}

		/// <summary>
		/// Sets the active camera.
		/// </summary>
		/// <param name="a_Camera">The new active camera.</param>
		void SetActiveCamera(Camera& a_Camera)
		{
			m_pActiveCamera = &a_Camera;
		}
		
		/// <summary>
		/// Sets the dimension draw mode, rendering only 2D, 3D or both.
		/// </summary>
		/// <param name="a_DimensionDrawMode">The dimension draw mode.</param>
		void SetDimensionDrawMode(DimensionDrawMode a_DimensionDrawMode)
		{
			m_DimensionDrawMode = a_DimensionDrawMode;
		}
		
		/// <summary>
		/// Retrieves the dimension draw mode, rendering only 2D, 3D or both.
		/// </summary>
		/// <returns>The dimension draw mode.</returns>
		DimensionDrawMode GetDimensionDrawMode() const
		{
			return m_DimensionDrawMode;
		}
		
		/// <summary>
		/// Reorders all sprite components based on layer index.
		/// </summary>
		void ReorderSpriteComponents();

		SimpleEvent<DX12System&, std::shared_ptr<CommandList>> m_eOnRenderTargetCreate;
		SimpleEvent<> m_eOnRenderTargetCleaned;
		SimpleEvent<const IVector2&, const IVector2&> m_eOnResize;
		SimpleEvent<HWND, UINT, WPARAM, LPARAM> m_eOnEventsProcess;
		SimpleEvent<std::shared_ptr<dx12::CommandList>> m_eOnRenderTexReady;
		Event<float> m_eOnNewFrame;

		const Event<float>& OnNewFrame() const
		{
			return m_eOnNewFrame;
		}

		std::shared_ptr<DX12UploadBufferAllocator> GetTransformAllocator()
		{
			return m_pTransformAllocation;
		}

		std::shared_ptr<DX12UploadBufferAllocator> GetSkinningDataAllocator()
		{
			return m_pSkinningDataAllocation;
		}

		std::shared_ptr<RenderView> RegisterView(size_t a_iIndex);
	protected:
		/// <summary>
		/// Called in render when a new frame is presented.
		/// </summary>
		/// <param name="a_fDeltaTime">Time it took since last frame.</param>
		void NewFrame(float a_fDeltaTime);

		/// <summary>
		/// Creates the command queues.
		/// </summary>
		/// <returns>True if creation was successful, otherwise false.</returns>
		bool CreateCommandQueues();

		/// <summary>
		/// Creates the views (rtv, srv, etc).
		/// </summary>
		/// <returns>True if creation was successful, otherwise false.</returns>
		bool CreateViews();

		/// <summary>
		/// Creates the root signature.
		/// </summary>
		/// <returns>True if creation was successful, otherwise false.</returns>
		bool CreateRootSignature();

		/// <summary>
		/// Method called after successful resize.
		/// </summary>
		/// <param name="a_vSize">The size of the window.</param>
		void AfterResize(const IVector2& a_vSize);

		std::vector<std::shared_ptr<RenderView>> m_aRenderViews;

		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_pAdapter = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_pDevice = nullptr;

		std::shared_ptr<CommandQueue> m_pDirectCommandQueue = nullptr;
		std::shared_ptr<CommandQueue> m_pCopyCommandQueue = nullptr;

		HWND m_hWnd = nullptr;
		win32::Window* m_pWindow = nullptr;

		uint64_t m_aFenceValues[g_iBufferCount] = {};
		std::unique_ptr<HeapAllocation>
			m_SRV,
			m_RTV,
			m_DSV;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_pSwapChain = nullptr;
		std::shared_ptr<DX12Resource> m_BackBuffers[g_iBufferCount];

		UINT m_iCurrentBackBufferIndex = 0;

		bool m_bIsTearingSupported = false;

		D3D12_VIEWPORT m_RenderTexViewport = {};
		D3D12_RECT m_RenderTexScissorRect = {};

		D3D12_VIEWPORT m_WindowViewport = {};
		D3D12_RECT m_WindowScissorRect = {};

		IVector2 m_vSize = { 1920, 1080 };

		ID3D12PipelineState* m_pRenderTextureShaderBind = {};
		std::shared_ptr<DirectionalLight> m_pDirectionalLight = {};

		std::shared_ptr<DX12UploadBufferAllocator> m_pTransformAllocation = {};
		std::shared_ptr<DX12UploadBufferAllocator> m_pSkinningDataAllocation = {};

		FPSCounter m_FpsCounter = {};

		std::unique_ptr<Camera> m_pCamera;
		Camera* m_pActiveCamera = nullptr;
		DimensionDrawMode m_DimensionDrawMode = DimensionDrawMode::DimensionDrawMode_2D3D;

		std::vector<std::weak_ptr<gameplay::SpriteComponent>> m_aOrderedTextures;
	};
}