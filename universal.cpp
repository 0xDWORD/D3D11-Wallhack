#include <thread>
#include "DXHooks.h"

HINSTANCE _hModule = NULL;

DWORD __stdcall initDX(LPVOID) {
	DXHooks *dxHook = new DXHooks(_hModule);
	dxHook->InitDX11();
	if (MH_Initialize() != MH_OK) { return 1; }

	if (MH_CreateHook((DWORD_PTR*)dxHook->pSwapChainVtable[8], (LPVOID) dxHook->hookD3D11Present, reinterpret_cast<void**>(&dxHook->phookD3D11Present)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pSwapChainVtable[8]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pContextVTable[12], dxHook->hookD3D11DrawIndexed, reinterpret_cast<void**>(&dxHook->phookD3D11DrawIndexed)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pContextVTable[12]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pDeviceVTable[24], dxHook->hookD3D11CreateQuery, reinterpret_cast<void**>(&dxHook->phookD3D11CreateQuery)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pDeviceVTable[24]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pDeviceVTable[12], dxHook->hookCreateVertexShader, reinterpret_cast<void**>(&dxHook->phookCreateVertexShader)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pDeviceVTable[12]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pContextVTable[39], dxHook->hookD3D11DrawIndexedInstancedIndirect, reinterpret_cast<void**>(&dxHook->phookD3D11DrawIndexedInstancedIndirect)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pContextVTable[39]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pContextVTable[20], dxHook->hookD3D11DrawIndexedInstanced, reinterpret_cast<void**>(&dxHook->phookD3D11DrawIndexedInstanced)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pContextVTable[20]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pContextVTable[21], dxHook->hookD3D11DrawInstanced, reinterpret_cast<void**>(&dxHook->phookD3D11DrawInstanced)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pContextVTable[21]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pContextVTable[40], dxHook->hookD3D11DrawInstancedIndirect, reinterpret_cast<void**>(&dxHook->phookD3D11DrawInstancedIndirect)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pContextVTable[40]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pContextVTable[13], dxHook->hookD3D11Draw, reinterpret_cast<void**>(&dxHook->phookD3D11Draw)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pContextVTable[13]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dxHook->pContextVTable[8], dxHook->hookD3D11PSSetShaderResources, reinterpret_cast<void**>(&dxHook->phookD3D11PSSetShaderResources)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)dxHook->pContextVTable[8]) != MH_OK) { return 1; }

	DWORD dwOld;
	VirtualProtect(dxHook->phookD3D11Present, 2, PAGE_EXECUTE_READWRITE, &dwOld);
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{ 
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH: // A process is loading the DLL.
		_hModule = hModule;
		Beep(100, 100);
		DisableThreadLibraryCalls(hModule);
		//std::thread(&InitializeHook, hModule);

		CreateThread(NULL, 0, initDX, NULL, 0, NULL);
		break;

	case DLL_PROCESS_DETACH: // A process unloads the DLL.
		break;
	}
	return TRUE;
}

/*
dx11 vtable CONTEXT index
Index: 0 | QueryInterface
Index: 1 | Addref
Index: 2 | Release
Index: 3 | GetDevice
Index: 4 | GetPrivateData
Index: 5 | SetPrivateData
Index: 6 | SetPrivateDataInterface
Index: 7 | VSSetConstantBuffers
Index: 8 | PSSetShaderResources
Index: 9 | PSSetShader
Index: 10 | SetSamplers
Index: 11 | SetShader
Index: 12 | DrawIndexed
Index: 13 | Draw
Index: 14 | Map
Index: 15 | Unmap
Index: 16 | PSSetConstantBuffer
Index: 17 | IASetInputLayout
Index: 18 | IASetVertexBuffers
Index: 19 | IASetIndexBuffer
Index: 20 | DrawIndexedInstanced
Index: 21 | DrawInstanced
Index: 22 | GSSetConstantBuffers
Index: 23 | GSSetShader
Index: 24 | IASetPrimitiveTopology
Index: 25 | VSSetShaderResources
Index: 26 | VSSetSamplers
Index: 27 | Begin
Index: 28 | End
Index: 29 | GetData
Index: 30 | GSSetPredication
Index: 31 | GSSetShaderResources
Index: 32 | GSSetSamplers
Index: 33 | OMSetRenderTargets
Index: 34 | OMSetRenderTargetsAndUnorderedAccessViews
Index: 35 | OMSetBlendState
Index: 36 | OMSetDepthStencilState
Index: 37 | SOSetTargets
Index: 38 | DrawAuto
Index: 39 | DrawIndexedInstancedIndirect
Index: 40 | DrawInstancedIndirect
Index: 41 | Dispatch
Index: 42 | DispatchIndirect
Index: 43 | RSSetState
Index: 44 | RSSetViewports
Index: 45 | RSSetScissorRects
Index: 46 | CopySubresourceRegion
Index: 47 | CopyResource
Index: 48 | UpdateSubresource
Index: 49 | CopyStructureCount
Index: 50 | ClearRenderTargetView
Index: 51 | ClearUnorderedAccessViewUint
Index: 52 | ClearUnorderedAccessViewFloat
Index: 53 | ClearDepthStencilView
Index: 54 | GenerateMips
Index: 55 | SetResourceMinLOD
Index: 56 | GetResourceMinLOD
Index: 57 | ResolveSubresource
Index: 58 | ExecuteCommandList
Index: 59 | HSSetShaderResources
Index: 60 | HSSetShader
Index: 61 | HSSetSamplers
Index: 62 | HSSetConstantBuffers
Index: 63 | DSSetShaderResources
Index: 64 | DSSetShader
Index: 65 | DSSetSamplers
Index: 66 | DSSetConstantBuffers
Index: 67 | DSSetShaderResources
Index: 68 | CSSetUnorderedAccessViews
Index: 69 | CSSetShader
Index: 70 | CSSetSamplers
Index: 71 | CSSetConstantBuffers
Index: 72 | VSGetConstantBuffers
Index: 73 | PSGetShaderResources
Index: 74 | PSGetShader
Index: 75 | PSGetSamplers
Index: 76 | VSGetShader
Index: 77 | PSGetConstantBuffers
Index: 78 | IAGetInputLayout
Index: 79 | IAGetVertexBuffers
Index: 80 | IAGetIndexBuffer
Index: 81 | GSGetConstantBuffers
Index: 82 | GSGetShader
Index: 83 | IAGetPrimitiveTopology
Index: 84 | VSGetShaderResources
Index: 85 | VSGetSamplers
Index: 86 | GetPredication
Index: 87 | GSGetShaderResources
Index: 88 | GSGetSamplers
Index: 89 | OMGetRenderTargets
Index: 90 | OMGetRenderTargetsAndUnorderedAccessViews
Index: 91 | OMGetBlendState
Index: 92 | OMGetDepthStencilState
Index: 93 | SOGetTargets
Index: 94 | RSGetState
Index: 95 | RSGetViewports
Index: 96 | RSGetScissorRects
Index: 97 | HSGetShaderResources
Index: 98 | HSGetShader
Index: 99 | HSGetSamplers
Index: 100 | HSGetConstantBuffers
Index: 101 | DSGetShaderResources
Index: 102 | DSGetShader
Index: 103 | DSGetSamplers
Index: 104 | DSGetConstantBuffers
Index: 105 | CSGetShaderResources
Index: 106 | CSGetUnorderedAccessViews
Index: 107 | CSGetShader
Index: 108 | CSGetSamplers
Index: 109 | CSGetConstantBuffers
Index: 110 | ClearState
Index: 111 | Flush
Index: 112 | GetType
Index: 113 | GetContextFlags
Index: 114 | FinishCommandList


// ID3D11 DEVICE virtuals
*QueryInterface 						0
*AddRef 								1
*Release 								2
*CreateBuffer 							3
*CreateTexture1D 						4
*CreateTexture2D 						5
*CreateTexture3D 						6
*CreateShaderResourceView				7
*CreateUnorderedAccessView 				8
*CreateRenderTargetView 				9
*CreateDepthStencilView 				10
*CreateInputLayout 						11
*CreateVertexShader 					12
*CreateGeometryShader 					13
*CreateGeometryShaderWithStreamOutput	14
*CreatePixelShader 						15
*CreateHullShader 						16
*CreateDomainShader						17
*CreateComputeShader					18
*CreateClassLinkage 					19
*CreateBlendState 						20
*CreateDepthStencilState 				21
*CreateRasterizerState 					22
*CreateSamplerState						23
*CreateQuery 							24
*CreatePredicate 						25
*CreateCounter 							26
*CreateDeferredContext 					27
*OpenSharedResource 					28
*CheckFormatSupport 					29
*CheckMultisampleQualityLevels 			30
*CheckCounterInfo 						31
*CheckCounter 							32
*CheckFeatureSupport 					33
*GetPrivateData 						34
*SetPrivateData 						35
*SetPrivateDataInterface 				36
*GetFeatureLevel 						37
*GetCreationFlags 						38
*GetDeviceRemovedReason 				39
*GetImmediateContext 					40
*SetExceptionMode 						41
*GetExceptionMode 						42


// IDXGI SWAPCHAIN virtuals
[0]    7405CADA    (CMTUseCountedObject<CDXGISwapChain>::QueryInterface)
[1]    7405C9A7    (CMTUseCountedObject<CDXGISwapChain>::AddRef)
[2]    7405C9D8    (CMTUseCountedObject<CDXGISwapChain>::Release)
[3]    7405D6BF    (CDXGISwapChain::SetPrivateData)
[4]    7405F6FC    (CDXGISwapChain::SetPrivateDataInterface)
[5]    7405D6AF    (CDXGISwapChain::GetPrivateData)
[6]    7406106A    (CDXGISwapChain::GetParent)
[7]    7405EFDE    (CDXGISwapChain::GetDevice)
[8]    74061BD1    (CDXGISwapChain::Present)
[9]    740617A7    (CDXGISwapChain::GetBuffer)
[10]    74065CD6    (CDXGISwapChain::SetFullscreenState)
[11]    740662DC    (CDXGISwapChain::GetFullscreenState)
[12]    74061146    (CDXGISwapChain::GetDesc)
[13]    740655ED    (CDXGISwapChain::ResizeBuffers)
[14]    74065B8D    (CDXGISwapChain::ResizeTarget)
[15]    7406197B    (CDXGISwapChain::GetContainingOutput)
[16]    74066524    (CDXGISwapChain::GetFrameStatistics)
[17]    74066A58    (CDXGISwapChain::GetLastPresentCount)
[18]    740612C6    (CDXGISwapChain::GetDesc1)
[19]    740613E0    (CDXGISwapChain::GetFullscreenDesc)
[20]    740614F9    (CDXGISwapChain::GetHwnd)
[21]    7406156D    (CDXGISwapChain::GetCoreWindow)
[22]    74061D0D    (CDXGISwapChain[::IDXGISwapChain1]::Present1)
[23]    74062069    (CDXGISwapChain::IsTemporaryMonoSupported)
[24]    740615BB    (CDXGISwapChain::GetRestrictToOutput)
[25]    740615FB    (CDXGISwapChain::SetBackgroundColor)
[26]    740616F1    (CDXGISwapChain::GetBackgroundColor)
[27]    7406173F    (CDXGISwapChain::SetRotation)
[28]    74061770    (CDXGISwapChain::GetRotation)
[29]    7405CC1A    (CMTUseCountedObject<CDXGISwapChain>::`vector deleting destructor')
[30]    7405181E    (CMTUseCountedObject<CDXGISwapChain>::LUCCompleteLayerConstruction)
[31]    7405CBA5    (DXGID3D10ETWRundown)


// DXGI VTable:
[0]	6ED3F979	(CMTUseCountedObject<CDXGISwapChain>::QueryInterface)
[1]	6ED3F84D	(CMTUseCountedObject<CDXGISwapChain>::AddRef)
[2]	6ED3F77D	(CMTUseCountedObject<CDXGISwapChain>::Release)
[3]	6ED6A6D7	(CDXGISwapChain::SetPrivateData)
[4]	6ED6A904	(CDXGISwapChain::SetPrivateDataInterface)
[5]	6ED72BC9	(CDXGISwapChain::GetPrivateData)
[6]	6ED6DCDD	(CDXGISwapChain::GetParent)
[7]	6ED69BF4	(CDXGISwapChain::GetDevice)
[8]	6ED3FAAD	(CDXGISwapChain::Present)
[9]	6ED40209	(CDXGISwapChain::GetBuffer)
[10]	6ED47C1C	(CDXGISwapChain::SetFullscreenState)
[11]	6ED48CD9	(CDXGISwapChain::GetFullscreenState)
[12]	6ED40CB1	(CDXGISwapChain::GetDesc)
[13]	6ED48A3B	(CDXGISwapChain::ResizeBuffers)
[14]	6ED6F153	(CDXGISwapChain::ResizeTarget)
[15]	6ED47BA5	(CDXGISwapChain::GetContainingOutput)
[16]	6ED6D9B5	(CDXGISwapChain::GetFrameStatistics)
[17]	6ED327B5	(CDXGISwapChain::GetLastPresentCount)
[18]	6ED43400	(CDXGISwapChain::GetDesc1)
[19]	6ED6D9D0	(CDXGISwapChain::GetFullscreenDesc)
[20]	6ED6DA90	(CDXGISwapChain::GetHwnd)
[21]	6ED6D79F	(CDXGISwapChain::GetCoreWindow)
[22]	6ED6E352	(?Present1@?QIDXGISwapChain2@@CDXGISwapChain@@UAGJIIPBUDXGI_PRESENT_PARAMETERS@@@Z)
[23]	6ED6E240	(CDXGISwapChain::IsTemporaryMonoSupported)
[24]	6ED44146	(CDXGISwapChain::GetRestrictToOutput)
[25]	6ED6F766	(CDXGISwapChain::SetBackgroundColor)
[26]	6ED6D6B9	(CDXGISwapChain::GetBackgroundColor)
[27]	6ED4417B	(CDXGISwapChain::SetRotation)
[28]	6ED6DDE3	(CDXGISwapChain::GetRotation)
[29]	6ED6FF85	(CDXGISwapChain::SetSourceSize)
[30]	6ED6DF4F	(CDXGISwapChain::GetSourceSize)
[31]	6ED6FCBD	(CDXGISwapChain::SetMaximumFrameLatency)
[32]	6ED6DBE5	(CDXGISwapChain::GetMaximumFrameLatency)
[33]	6ED6D8CD	(CDXGISwapChain::GetFrameLatencyWaitableObject)
[34]	6ED6FB45	(CDXGISwapChain::SetMatrixTransform)
[35]	6ED6DAD0	(CDXGISwapChain::GetMatrixTransform)
[36]	6ED6C155	(CDXGISwapChain::CheckMultiplaneOverlaySupportInternal)
[37]	6ED6E82D	(CDXGISwapChain::PresentMultiplaneOverlayInternal)
[38]	6ED4397A	(CMTUseCountedObject<CDXGISwapChain>::`vector deleting destructor')
[39]	6ED4EAE0	(CSwapBuffer::AddRef)
[40]	6ED46C81	(CMTUseCountedObject<CDXGISwapChain>::LUCBeginLayerDestruction)
*/
