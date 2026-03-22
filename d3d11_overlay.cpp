#include "pch.h"

#include "d3d11_overlay.h"

#include <dxgi.h>
#include <dxgi1_2.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <wrl.h>

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "hook.h"

namespace D3D11_Overlay
{
	static D3D11_OVERLAY_ON_WNDPROC_CALLBACK CurrentOnWndProc = NULL;
	static D3D11_OVERLAY_BEFORE_FRAME_CALLBACK CurrentBeforeFrame = NULL;
	static D3D11_OVERLAY_ON_FRAME_CALLBACK CurrentOnFrame = NULL;

	static IDXGISwapChain* CurrentSwapChain = NULL;
	static HWND CurrentWindow = NULL;
	static ID3D11Device* CurrentDevice = NULL;
	static ID3D11RenderTargetView* CurrentRenderTargetView = NULL;
	static ID3D11DeviceContext* CurrentImmediateContext = NULL;
	static ImGuiContext* CurrentContext = NULL;
	static BOOL ImplWin32Initialized = FALSE;
	static BOOL ImplDX11Initialized = FALSE;
	static WNDPROC CurrentWndProc = NULL;

	static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (CurrentOnWndProc && !CurrentOnWndProc(hWnd, uMsg, wParam, lParam))
		{
			return 0;
		}

		return CurrentWndProc ? CallWindowProcW(CurrentWndProc, hWnd, uMsg, wParam, lParam) : DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	static BOOL SetupRenderer(IDXGISwapChain* SwapChain)
	{
		if (!SwapChain)
		{
			return FALSE;
		}

		CurrentSwapChain = SwapChain;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;

		if (FAILED(CurrentSwapChain->GetDesc(&SwapChainDesc)))
		{
			return FALSE;
		}

		CurrentWindow = SwapChainDesc.OutputWindow;

		if (FAILED(CurrentSwapChain->GetDevice(IID_PPV_ARGS(&CurrentDevice))))
		{
			return FALSE;
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> Buffer;

		if (FAILED(SwapChain->GetBuffer(0, IID_PPV_ARGS(&Buffer))))
		{
			return FALSE;
		}

		if (FAILED(CurrentDevice->CreateRenderTargetView(Buffer.Get(), NULL, &CurrentRenderTargetView)))
		{
			return FALSE;
		}

		CurrentContext = ImGui::CreateContext();

		ImGui::GetIO().IniFilename = NULL;

		ImGui::StyleColorsDark();

		if (!ImGui_ImplWin32_Init(CurrentWindow))
		{
			return FALSE;
		}

		ImplWin32Initialized = TRUE;

		CurrentDevice->GetImmediateContext(&CurrentImmediateContext);

		if (!ImGui_ImplDX11_Init(CurrentDevice, CurrentImmediateContext))
		{
			return FALSE;
		}

		ImplDX11Initialized = TRUE;

		CurrentWndProc = (WNDPROC)SetWindowLongPtrW(CurrentWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

		return TRUE;
	}

	static void CleanupRenderer()
	{
		if (CurrentWndProc)
		{
			SetWindowLongPtrW(CurrentWindow, GWLP_WNDPROC, (ULONG_PTR)CurrentWndProc);
			CurrentWndProc = NULL;
		}

		if (ImplDX11Initialized)
		{
			ImGui_ImplDX11_Shutdown();
			ImplDX11Initialized = FALSE;
		}

		if (ImplWin32Initialized)
		{
			ImGui_ImplWin32_Shutdown();
			ImplWin32Initialized = FALSE;
		}

		if (CurrentContext)
		{
			ImGui::DestroyContext(CurrentContext);
			CurrentContext = NULL;
		}

		if (CurrentImmediateContext)
		{
			CurrentImmediateContext->Release();
			CurrentImmediateContext = NULL;
		}

		if (CurrentRenderTargetView)
		{
			CurrentRenderTargetView->Release();
			CurrentRenderTargetView = NULL;
		}

		if (CurrentDevice)
		{
			CurrentDevice->Release();
			CurrentDevice = NULL;
		}

		CurrentWindow = NULL;
		CurrentSwapChain = NULL;
	}

	static HOOK DXGISwapChain_ReleaseHook = {};

	static ULONG DXGISwapChain_ReleaseDetour(IDXGISwapChain* _this)
	{
		ULONG RefCount = _this->AddRef() - 2;

		if (RefCount == 0)
		{
			if (CurrentSwapChain && CurrentSwapChain == _this)
			{
				CleanupRenderer();
			}
		}

		CALL_ORIGINAL(DXGISwapChain_ReleaseHook, DXGISwapChain_ReleaseDetour, _this);
		CALL_ORIGINAL(DXGISwapChain_ReleaseHook, DXGISwapChain_ReleaseDetour, _this);

		return RefCount;
	}

	static HOOK DXGISwapChain_PresentHook = {};

	static HRESULT DXGISwapChain_PresentDetour(IDXGISwapChain* _this, UINT SyncInterval, UINT Flags)
	{
		if (_this && !(Flags & DXGI_PRESENT_TEST))
		{
			if (!CurrentSwapChain)
			{
				if (!SetupRenderer(_this))
				{
					CleanupRenderer();
				}
			}

			if (CurrentSwapChain == _this)
			{
				if (CurrentBeforeFrame)
				{
					CurrentBeforeFrame();
				}

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				if (CurrentOnFrame)
				{
					CurrentOnFrame();
				}

				ImGui::Render();

				CurrentImmediateContext->OMSetRenderTargets(1, &CurrentRenderTargetView, NULL);

				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}
		}

		return CALL_ORIGINAL(DXGISwapChain_PresentHook, DXGISwapChain_PresentDetour, _this, SyncInterval, Flags);
	}

	static HOOK DXGISwapChain_ResizeBuffersHook = {};

	static HRESULT DXGISwapChain_ResizeBuffersDetour(IDXGISwapChain* _this, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
	{
		if (CurrentSwapChain && CurrentSwapChain == _this)
		{
			CleanupRenderer();
		}

		return CALL_ORIGINAL(DXGISwapChain_ResizeBuffersHook, DXGISwapChain_ResizeBuffersDetour, _this, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	}

	static void DXGISwapChain_Setup(IDXGISwapChain* SwapChain)
	{
		PVOID* SwapChainVTable = *(PVOID**)SwapChain;

		if (!IsHookActive(&DXGISwapChain_ReleaseHook) && !CreateHook(&DXGISwapChain_ReleaseHook, SwapChainVTable[2], DXGISwapChain_ReleaseDetour, TRUE))
		{
			return;
		}

		if (!IsHookActive(&DXGISwapChain_PresentHook) && !CreateHook(&DXGISwapChain_PresentHook, SwapChainVTable[8], DXGISwapChain_PresentDetour, TRUE))
		{
			return;
		}

		if (!IsHookActive(&DXGISwapChain_ResizeBuffersHook) && !CreateHook(&DXGISwapChain_ResizeBuffersHook, SwapChainVTable[13], DXGISwapChain_ResizeBuffersDetour, TRUE))
		{
			return;
		}
	}

	static HOOK DXGIFactory_CreateSwapChainHook = {};

	static HRESULT DXGIFactory_CreateSwapChainDetour(IDXGIFactory* _this, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
	{
		HRESULT hResult = CALL_ORIGINAL(DXGIFactory_CreateSwapChainHook, DXGIFactory_CreateSwapChainDetour, _this, pDevice, pDesc, ppSwapChain);

		if (SUCCEEDED(hResult) && ppSwapChain && *ppSwapChain)
		{
			DXGISwapChain_Setup(*ppSwapChain);
		}

		return hResult;
	}

	static HOOK DXGIFactory2_CreateSwapChainForHwndHook = {};

	static HRESULT DXGIFactory2_CreateSwapChainForHwndDetour(IDXGIFactory2* _this, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
	{
		HRESULT hResult = CALL_ORIGINAL(DXGIFactory2_CreateSwapChainForHwndHook, DXGIFactory2_CreateSwapChainForHwndDetour, _this, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);

		if (SUCCEEDED(hResult) && ppSwapChain && *ppSwapChain)
		{
			DXGISwapChain_Setup(*ppSwapChain);
		}

		return hResult;
	}

	static HOOK DXGIFactory2_CreateSwapChainForCoreWindowHook = {};

	static HRESULT DXGIFactory2_CreateSwapChainForCoreWindowDetour(IDXGIFactory2* _this, IUnknown* pDevice, IUnknown* pWindow, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
	{
		HRESULT hResult = CALL_ORIGINAL(DXGIFactory2_CreateSwapChainForCoreWindowHook, DXGIFactory2_CreateSwapChainForCoreWindowDetour, _this, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);

		if (SUCCEEDED(hResult) && ppSwapChain && *ppSwapChain)
		{
			DXGISwapChain_Setup(*ppSwapChain);
		}

		return hResult;
	}

	static HOOK DXGIFactory2_CreateSwapChainForCompositionHook = {};

	static HRESULT DXGIFactory2_CreateSwapChainForCompositionDetour(IDXGIFactory2* _this, IUnknown* pDevice, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
	{
		HRESULT hResult = CALL_ORIGINAL(DXGIFactory2_CreateSwapChainForCompositionHook, DXGIFactory2_CreateSwapChainForCompositionDetour, _this, pDevice, pDesc, pRestrictToOutput, ppSwapChain);

		if (SUCCEEDED(hResult) && ppSwapChain && *ppSwapChain)
		{
			DXGISwapChain_Setup(*ppSwapChain);
		}

		return hResult;
	}

	static void DXGIFactory_Setup(IDXGIFactory* Factory)
	{
		PVOID* FactoryVTable = *(PVOID**)Factory;

		if (!IsHookActive(&DXGIFactory_CreateSwapChainHook) && !CreateHook(&DXGIFactory_CreateSwapChainHook, FactoryVTable[10], DXGIFactory_CreateSwapChainDetour, TRUE))
		{
			return;
		}

		Microsoft::WRL::ComPtr<IDXGIFactory2> Factory2;

		if (SUCCEEDED(Factory->QueryInterface(IID_PPV_ARGS(&Factory2))))
		{
			PVOID* Factory2VTable = *(PVOID**)Factory2.Get();

			if (!IsHookActive(&DXGIFactory2_CreateSwapChainForHwndHook) && !CreateHook(&DXGIFactory2_CreateSwapChainForHwndHook, Factory2VTable[15], DXGIFactory2_CreateSwapChainForHwndDetour, TRUE))
			{
				return;
			}

			if (!IsHookActive(&DXGIFactory2_CreateSwapChainForCoreWindowHook) && !CreateHook(&DXGIFactory2_CreateSwapChainForCoreWindowHook, Factory2VTable[16], DXGIFactory2_CreateSwapChainForCoreWindowDetour, TRUE))
			{
				return;
			}

			if (!IsHookActive(&DXGIFactory2_CreateSwapChainForCompositionHook) && !CreateHook(&DXGIFactory2_CreateSwapChainForCompositionHook, Factory2VTable[24], DXGIFactory2_CreateSwapChainForCompositionDetour, TRUE))
			{
				return;
			}
		}
	}

	static HOOK CreateDXGIFactoryHook = {};

	static HRESULT CreateDXGIFactoryDetour(PVOID riid, IDXGIFactory** ppFactory)
	{
		HRESULT hResult = CALL_ORIGINAL(CreateDXGIFactoryHook, CreateDXGIFactoryDetour, riid, ppFactory);

		if (SUCCEEDED(hResult) && ppFactory && *ppFactory)
		{
			DXGIFactory_Setup(*ppFactory);
		}

		return hResult;
	}

	static HOOK CreateDXGIFactory1Hook = {};

	static HRESULT CreateDXGIFactory1Detour(PVOID riid, IDXGIFactory1** ppFactory)
	{
		HRESULT hResult = CALL_ORIGINAL(CreateDXGIFactory1Hook, CreateDXGIFactory1Detour, riid, ppFactory);

		if (SUCCEEDED(hResult) && ppFactory && *ppFactory)
		{
			DXGIFactory_Setup(*ppFactory);
		}

		return hResult;
	}

	static HOOK CreateDXGIFactory2Hook = {};

	static HRESULT CreateDXGIFactory2Detour(UINT Flags, PVOID riid, IDXGIFactory2** ppFactory)
	{
		HRESULT hResult = CALL_ORIGINAL(CreateDXGIFactory2Hook, CreateDXGIFactory2Detour, Flags, riid, ppFactory);

		if (SUCCEEDED(hResult) && ppFactory && *ppFactory)
		{
			DXGIFactory_Setup(*ppFactory);
		}

		return hResult;
	}

	static HOOK D3D11CreateDeviceAndSwapChainHook = {};

	static HRESULT D3D11CreateDeviceAndSwapChainDetour(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
	{
		HRESULT hResult = CALL_ORIGINAL(D3D11CreateDeviceAndSwapChainHook, D3D11CreateDeviceAndSwapChainDetour, pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);

		if (SUCCEEDED(hResult) && ppSwapChain && *ppSwapChain)
		{
			DXGISwapChain_Setup(*ppSwapChain);
		}

		return hResult;
	}

	static DWORD ThreadRoutine(LPVOID lpParameter)
	{
		WNDCLASSEXW WndClass = {};

		WndClass.cbSize = sizeof(WndClass);
		WndClass.style = CS_CLASSDC;
		WndClass.lpfnWndProc = WndProc;
		WndClass.hInstance = GetModuleHandleW(NULL);
		WndClass.lpszClassName = L"Temp Window Class";

		RegisterClassExW(&WndClass);

		HWND hWnd = CreateWindowExW(0, WndClass.lpszClassName, L"Temp Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, NULL, NULL, WndClass.hInstance, NULL);

		UINT CreateDeviceFlags = 0;
		D3D_FEATURE_LEVEL FeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

		DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};

		SwapChainDesc.BufferCount = 2;
		SwapChainDesc.BufferDesc.Width = 0;
		SwapChainDesc.BufferDesc.Height = 0;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.OutputWindow = hWnd;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.Windowed = TRUE;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		IDXGISwapChain* SwapChain = NULL;
		ID3D11Device* Device = NULL;
		D3D_FEATURE_LEVEL FeatureLevel;
		ID3D11DeviceContext* DeviceContext = NULL;

		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, CreateDeviceFlags, FeatureLevelArray, 2, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel, &DeviceContext) == DXGI_ERROR_UNSUPPORTED)
		{
			D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, CreateDeviceFlags, FeatureLevelArray, 2, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel, &DeviceContext);
		}

		if (SwapChain)
		{
			SwapChain->Release();
		}

		if (Device)
		{
			Device->Release();
		}

		if (DeviceContext)
		{
			DeviceContext->Release();
		}

		DestroyWindow(hWnd);
		UnregisterClassW(WndClass.lpszClassName, WndClass.hInstance);

		return 0;
	}

	bool Setup(D3D11_OVERLAY_ON_WNDPROC_CALLBACK OnWndProc, D3D11_OVERLAY_BEFORE_FRAME_CALLBACK BeforeFrame, D3D11_OVERLAY_ON_FRAME_CALLBACK OnFrame)
	{
		{
			CurrentOnWndProc = OnWndProc;
			CurrentBeforeFrame = BeforeFrame;
			CurrentOnFrame = OnFrame;
		}

		{
			HMODULE hDxgi = LoadLibraryW(L"dxgi.dll");

			if (!hDxgi)
			{
				return FALSE;
			}

			if (!IsHookActive(&CreateDXGIFactoryHook) && !CreateHook(&CreateDXGIFactoryHook, GetProcAddress(hDxgi, "CreateDXGIFactory"), CreateDXGIFactoryDetour, TRUE))
			{
				return FALSE;
			}

			if (!IsHookActive(&CreateDXGIFactory1Hook) && !CreateHook(&CreateDXGIFactory1Hook, GetProcAddress(hDxgi, "CreateDXGIFactory1"), CreateDXGIFactory1Detour, TRUE))
			{
				return FALSE;
			}

			if (!IsHookActive(&CreateDXGIFactory2Hook) && !CreateHook(&CreateDXGIFactory2Hook, GetProcAddress(hDxgi, "CreateDXGIFactory2"), CreateDXGIFactory2Detour, TRUE))
			{
				return FALSE;
			}
		}

		{
			HMODULE hD3d11 = LoadLibraryW(L"d3d11.dll");

			if (!hD3d11)
			{
				return FALSE;
			}

			if (!IsHookActive(&D3D11CreateDeviceAndSwapChainHook) && !CreateHook(&D3D11CreateDeviceAndSwapChainHook, GetProcAddress(hD3d11, "D3D11CreateDeviceAndSwapChain"), D3D11CreateDeviceAndSwapChainDetour, TRUE))
			{
				return FALSE;
			}
		}

		/* not needed if you inject before creating the device, but just in case */
		{
			HANDLE hThread = CreateThread(NULL, 0, ThreadRoutine, NULL, 0, NULL);

			if (hThread == INVALID_HANDLE_VALUE)
			{
				return FALSE;
			}

			CloseHandle(hThread);
		}

		return TRUE;
	}
}
