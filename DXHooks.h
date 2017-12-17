#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")
#include <d3d11.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>//generateshader
#include <thread>
#include "MinHook/include/MinHook.h" //detour x86&x64
#include "FW1FontWrapper/FW1FontWrapper.h" //font
#include "ModelChecker.h"
#include "Utils.h"

typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef void(__stdcall *D3D11DrawHook) (ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation);
typedef void(__stdcall *D3D11DrawIndexedHook) (ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
typedef void(__stdcall *D3D11DrawInstancedHook) (ID3D11DeviceContext* pContext, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
typedef void(__stdcall *D3D11DrawIndexedInstancedHook) (ID3D11DeviceContext* pContext, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
typedef void(__stdcall *D3D11DrawInstancedIndirectHook) (ID3D11DeviceContext* pContext, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);
typedef void(__stdcall *D3D11DrawIndexedInstancedIndirectHook) (ID3D11DeviceContext* pContext, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);
typedef void(__stdcall *D3D11PSSetShaderResourcesHook) (ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
typedef void(__stdcall *D3D11CreateQueryHook) (ID3D11Device* pDevice, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery);
typedef void(__stdcall *D3D11CreateVertexShaderHook) (ID3D10Device* pDevice, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10VertexShader **ppVertexShader);

class DXHooks
{
public:
	DXHooks(HINSTANCE hModule);
	~DXHooks();
	void DXHooks::InitDX11();

	//TODO: Refactor Helper methods to separate object
	bool DXHooks::SetDepthEnable(bool depthenable, bool stencilenable);
	bool DXHooks::SetDepthFunc(D3D11_COMPARISON_FUNC depthfunc, bool stencilenable);
	HRESULT DXHooks::GenerateShader(ID3D11Device* pD3DDevice, ID3D11PixelShader** pShader, float r, float g, float b);
	
	//Hooked DX11 Methods
	HRESULT __stdcall DXHooks::hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	void __stdcall DXHooks::hookD3D11DrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	void __stdcall DXHooks::hookD3D11PSSetShaderResources(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
	void __stdcall DXHooks::hookD3D11CreateQuery(ID3D11Device* pDevice, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery);
	void __stdcall DXHooks::hookCreateVertexShader(ID3D10Device* pDevice, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10VertexShader **ppVertexShader);
	void __stdcall DXHooks::hookD3D11Draw(ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation);
	void __stdcall DXHooks::hookD3D11DrawInstanced(ID3D11DeviceContext* pContext, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
	void __stdcall DXHooks::hookD3D11DrawIndexedInstanced(ID3D11DeviceContext* pContext, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
	void __stdcall DXHooks::hookD3D11DrawInstancedIndirect(ID3D11DeviceContext* pContext, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);
	void __stdcall DXHooks::hookD3D11DrawIndexedInstancedIndirect(ID3D11DeviceContext* pContext, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);

	// Hooking
	D3D11PresentHook phookD3D11Present = NULL;
	D3D11DrawHook phookD3D11Draw = NULL;
	D3D11DrawIndexedHook phookD3D11DrawIndexed = NULL;
	D3D11DrawInstancedHook phookD3D11DrawInstanced = NULL;
	D3D11DrawIndexedInstancedHook phookD3D11DrawIndexedInstanced = NULL;
	D3D11DrawInstancedIndirectHook phookD3D11DrawInstancedIndirect = NULL;
	D3D11DrawIndexedInstancedIndirectHook phookD3D11DrawIndexedInstancedIndirect = NULL;
	D3D11PSSetShaderResourcesHook phookD3D11PSSetShaderResources = NULL;
	D3D11CreateQueryHook phookD3D11CreateQuery = NULL;
	D3D11CreateVertexShaderHook phookCreateVertexShader = NULL;

	DWORD_PTR* pSwapChainVtable = NULL;
	DWORD_PTR* pContextVTable = NULL;
	DWORD_PTR* pDeviceVTable = NULL;
private:
	// Constructor
	int MultisampleCount = 1;
	IDXGISwapChain* pSwapChain;



	ID3D11Device *pDevice = NULL;
	ID3D11DeviceContext *pContext = NULL;



	IFW1Factory *pFW1Factory = NULL;
	IFW1FontWrapper *pFontWrapper = NULL;

	bool firstTime = true;
	ModelChecker *modelChecker;

	enum eDepthState
	{
		ENABLED,
		DISABLED,
		READ_NO_WRITE,
		NO_READ_NO_WRITE,
		_DEPTH_COUNT
	};
	ID3D11DepthStencilState* myDepthStencilStates[static_cast<int>(eDepthState::_DEPTH_COUNT)];



	//rendertarget
	ID3D11Texture2D* RenderTargetTexture;
	ID3D11RenderTargetView* RenderTargetView = NULL;

	//shader
	ID3D11PixelShader* psRed = NULL;
	ID3D11PixelShader* psPink = NULL;
	ID3D11PixelShader* psGreen = NULL;

	//vertex
	ID3D11Buffer *veBuffer;
	UINT Stride = 0;
	UINT veBufferOffset = 0;
	D3D11_BUFFER_DESC vedesc;

	//index
	ID3D11Buffer *inBuffer;
	DXGI_FORMAT inFormat;
	UINT        inOffset;
	D3D11_BUFFER_DESC indesc;

	//wh
	char *state;
	ID3D11RasterizerState * rwState;
	ID3D11RasterizerState * rsState;

	//pssetshaderresources
	UINT pssrStartSlot;
	D3D11_SHADER_RESOURCE_VIEW_DESC  Descr;
	ID3D11ShaderResourceView* ShaderResourceView;

	//used for logging/cycling through values
	Utils *utils;
	bool logger = false;
	int countnum = 0;
	int strideNum = 0;
	int matchLevel = 0;
	int depthNum = 0;
	char szString[64];
};

