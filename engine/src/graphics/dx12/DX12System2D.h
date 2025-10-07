#pragma once

#include "DX12PCH.h"
#include "core/System.h"

#include <glm/vec2.hpp>
#include <memory>
#include <mutex>
#include <chrono>

// core includes
#include "core/Event.h"

// graphics includes
#include "graphics/dx12/HeapAllocation.h"
#include "graphics/dx12/Camera.h"
#ifndef IMGUI_DISABLE
#include "graphics/imgui/ImGuiWindow.h"
#endif // IMGUI_DISABLE

// gameplay includes
#include "gameplay/systems/components/SpriteComponent.h"

#undef min
#undef max

namespace gallus
{
	namespace graphics
	{
		namespace win32
		{
			class Window;
		}
		namespace dx12
		{
/// Whether DX12 makes a full log of creating devices, resources, etc (spam).
#define LOG_DX12 1

			inline const uint8_t g_iBufferCount = 3; /// Number of swap chain buffers.
			inline bool g_bVSync = false; /// Whether V-Sync is enabled.

			class CommandQueue;
			class CommandList;
			class Texture;
			class DX12Resource;

			// Create the vertex input layout
			const D3D12_INPUT_ELEMENT_DESC g_aInputLayout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			// Root parameters for the game.
			enum RootParameters
			{
				CBV = 0,                // ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);
				SPRITE_UV = 1,    // b1: Sprite rect UVs (float2 uv0, float2 uv1)
				SPRITE_COLOR = 2,    // b1: Sprite rect UVs (float2 uv0, float2 uv1)
				TEX_SRV = 3,            // Texture2D texture0 : register(t0);
				NumRootParameters = 4
			};

			/// <summary>
			/// Tracks and calculates frames per second (FPS).
			/// </summary>
			class FPSCounter
			{
			public:

				/// <summary>
				/// Retrieves the current frames per second (FPS).
				/// </summary>
				/// <returns>The FPS as a double.</returns>
				double GetFPS() const;
				double GetDeltaTime() const;

				double GetTotalTime() const;
			private:
				friend class DX12System2D;

				/// <summary>
				/// Updates the FPS counter.
				/// </summary>
				void Update();

				/// <summary>
				/// Initializes the FPS counter.
				/// </summary>
				void Initialize();

				double m_FPS = 0.0; /// The FPS as a double.
				uint64_t m_FrameCounter = 0; /// The number of frames.
				double m_DeltaTime = 0.0; /// The number of seconds since the last frame.
				double m_ElapsedSeconds = 0.0; /// The number of seconds since the last frame.
				double m_TotalTime = 0.0; /// The number of seconds since the last frame.
				std::chrono::high_resolution_clock m_Clock; /// The clock.
				std::chrono::steady_clock::time_point m_T0 = m_Clock.now(); /// The clock from first frame.
			};

			//---------------------------------------------------------------------
			// DX12System2D
			//---------------------------------------------------------------------
			/// <summary>
			/// Represents a DirectX 12 rendering window, managing device resources, rendering, and synchronization. Only basic 2D rendering.
			/// </summary>
			class DX12System2D : public core::ThreadedSystem
			{
			public:
				/// <summary>
				/// Initializes the system, setting up necessary resources.
				/// </summary>
				/// <param name="a_bWait">Determines whether the application waits until the system has been fully initialized.</param>
				/// <param name="a_hWnd">Handle to the window.</param>
				/// <param name="a_vSize">Size of the window.</param>
				/// <param name="a_pWindow">The window.</param>
				/// <returns>True if the initialization was successful, otherwise false.</returns>
				bool Initialize(bool a_bWait, HWND a_hWnd, const glm::ivec2& a_vSize, win32::Window* a_pWindow);

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
				Camera& GetCamera()
				{
					return m_Camera;
				}
			protected:
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
				void Resize(const glm::ivec2& a_vPos, const glm::ivec2& a_vSize);

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

				/// <summary>
				/// Retrieves the current render target view.
				/// </summary>
				/// <param name="a_bUseRenderTexture">Whether to use the render texture instead of regular rtv.</param>
				/// <returns>Handle to the render target view.</returns>
				D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView(bool a_bUseRenderTexture);
			public:
				std::mutex m_RenderMutex;

				/// <summary>
				/// Loop method for the thread.
				/// </summary>
				void Loop() override;

				/// <summary>
				/// Processes win32 window events.
				/// </summary>
				void ProcessWindowEvents();

#ifndef IMGUI_DISABLE
				/// <summary>
				/// Renders the editor UI to the screen.
				/// </summary>
				void RenderUI(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE a_RTVHandle);
#endif // IMGUI_DISABLE

				/// <summary>
				/// Renders the DX12 stuff to the screen.
				/// </summary>
				void Render2D(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE a_RTVHandle);

				/// <summary>
				/// PResents everything to the DX12 render loop.
				/// </summary>
				void Present(std::shared_ptr<CommandQueue> a_pCommandQueue, std::shared_ptr<CommandList> a_pCommandList);

				/// <summary>
				/// Retrieves the RTV heap.
				/// </summary>
				/// <returns>Reference to the RTV heap allocation.</returns>
				HeapAllocation& GetRTV()
				{
					return m_RTV;
				};

				/// <summary>
				/// Retrieves the SRV heap.
				/// </summary>
				/// <returns>Reference to the SRV heap allocation.</returns>
				HeapAllocation& GetSRV()
				{
					return m_SRV;
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

#ifndef IMGUI_DISABLE
				/// <summary>
				/// Retrieves the imgui window.
				/// </summary>
				/// <returns>Reference to the imgui window.</returns>
				imgui::ImGuiWindow& GetImGuiWindow()
				{
					return m_ImGuiWindow;
				}
#endif // IMGUI_DISABLE

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

				const FPSCounter& GetFPS() const
				{
					return m_FpsCounter;
				}

				std::shared_ptr<Texture> GetRenderTexture();

				SimpleEvent<DX12System2D&> m_eOnInitialize;
				SimpleEvent<std::shared_ptr<dx12::CommandList>> m_eOnRender;
				SimpleEvent<const glm::ivec2&, const glm::ivec2&> m_eOnResize;
			protected:
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

				void CreateRenderTexture(const glm::ivec2& a_vSize);

				void AfterResize(const glm::ivec2& a_vSize);

				Microsoft::WRL::ComPtr<IDXGIAdapter4> m_pAdapter = nullptr;
				Microsoft::WRL::ComPtr<ID3D12Device2> m_pDevice = nullptr;

				std::shared_ptr<CommandQueue> m_pDirectCommandQueue = nullptr;
				std::shared_ptr<CommandQueue> m_pCopyCommandQueue = nullptr;

				HWND m_hWnd = nullptr;
				win32::Window* m_pWindow = nullptr;

				uint64_t m_aFenceValues[g_iBufferCount] = {};

				HeapAllocation
					m_SRV,
					m_RTV;

				Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr;

				Microsoft::WRL::ComPtr<IDXGISwapChain4> m_pSwapChain = nullptr;
				std::shared_ptr<DX12Resource> m_BackBuffers[g_iBufferCount];

				UINT m_iCurrentBackBufferIndex = 0;

				bool m_bIsTearingSupported = false;

				D3D12_VIEWPORT m_Viewport;
				D3D12_RECT m_ScissorRect;
				glm::ivec2 m_vSize = { 1920, 1080 };
#ifndef IMGUI_DISABLE
				imgui::ImGuiWindow m_ImGuiWindow;
#endif // IMGUI_DISABLE
				std::shared_ptr<Texture> m_pRenderTexture = nullptr;

				FPSCounter m_FpsCounter;

				Camera m_Camera;
			};
		}
	}
}