#include "DXHooks.h"


LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }

DXHooks::DXHooks(HINSTANCE hModule)
{
	DXHooks::utils = new Utils(hModule);
}

DXHooks::~DXHooks()
{
	DXHooks::pDevice->Release();
	DXHooks::pContext->Release();
	DXHooks::pSwapChain->Release();

	if (MH_Uninitialize() != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pSwapChainVtable[8]) != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pContextVTable[12]) != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pDeviceVTable[24]) != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pContextVTable[39]) != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pContextVTable[20]) != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pContextVTable[21]) != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pContextVTable[40]) != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pContextVTable[13]) != MH_OK) { return; }
	if (MH_DisableHook((DWORD_PTR*)pContextVTable[8]) != MH_OK) { return; }
}

void DXHooks::InitDX11() 
{
	HMODULE hDXGIDLL = 0;
	do
	{
		hDXGIDLL = GetModuleHandle("dxgi.dll");
		Sleep(8000);
	} while (!hDXGIDLL);

	Sleep(100);

	IDXGISwapChain* pSwapChain;

	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

	D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtainedLevel;
	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = DXHooks::MultisampleCount;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

	// LibOVR 0.4.3 requires that the width and height for the backbuffer is set even if
	// you use windowed mode, despite being optional according to the D3D11 documentation.
	scd.BufferDesc.Width = 1;
	scd.BufferDesc.Height = 1;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;

	UINT createFlags = 0;
#ifdef _DEBUG
	// This flag gives you some quite wonderful debug text. Not wonderful for performance, though!
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGISwapChain* d3dSwapChain = 0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		requestedLevels,
		sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		&obtainedLevel,
		&pContext)))
	{
		MessageBox(hWnd, "Failed to create directX device and swapchain!", "Error", MB_ICONERROR);
		return;
	}


	pSwapChainVtable = (DWORD_PTR*)pSwapChain;
	pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];

	pContextVTable = (DWORD_PTR*)pContext;
	pContextVTable = (DWORD_PTR*)pContextVTable[0];

	pDeviceVTable = (DWORD_PTR*)pDevice;
	pDeviceVTable = (DWORD_PTR*)pDeviceVTable[0];
}

HRESULT __stdcall DXHooks::hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{

	if (firstTime)
	{
		modelChecker = new ModelChecker();

		//get device and context
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&pDevice)))
		{
			pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
			pDevice->GetImmediateContext(&pContext);
		}

		//create depthstencilstate
		D3D11_DEPTH_STENCIL_DESC  stencilDesc;
		stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		stencilDesc.StencilEnable = true;
		stencilDesc.StencilReadMask = 0xFF;
		stencilDesc.StencilWriteMask = 0xFF;
		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		pDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::ENABLED)]);

		stencilDesc.DepthEnable = false;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		pDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::DISABLED)]);

		stencilDesc.DepthEnable = false;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		stencilDesc.StencilEnable = false;
		stencilDesc.StencilReadMask = UINT8(0xFF);
		stencilDesc.StencilWriteMask = 0x0;
		pDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::NO_READ_NO_WRITE)]);

		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; //
		stencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		stencilDesc.StencilEnable = false;
		stencilDesc.StencilReadMask = UINT8(0xFF);
		stencilDesc.StencilWriteMask = 0x0;

		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
		pDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::READ_NO_WRITE)]);

		//wireframe
		D3D11_RASTERIZER_DESC rwDesc;
		pContext->RSGetState(&rwState); // retrieve the current state

										// If rwState  has not be created yet, then RSGetState will fail
		if (rwState == NULL) {
			rwDesc.AntialiasedLineEnable = false;
			rwDesc.CullMode = D3D11_CULL_NONE;
			rwDesc.DepthBias = 0;
			rwDesc.DepthBiasClamp = 0.0f;
			rwDesc.DepthClipEnable = true;
			rwDesc.FillMode = D3D11_FILL_WIREFRAME;
			rwDesc.FrontCounterClockwise = false;
			rwDesc.MultisampleEnable = false;
			rwDesc.ScissorEnable = false;
			rwDesc.SlopeScaledDepthBias = 0.0f;
			pDevice->CreateRasterizerState(&rwDesc, &rwState);
			pContext->RSSetState(rwState);
		}

		pContext->RSGetState(&rwState); // retrieve the current state

		rwState->GetDesc(&rwDesc);    // get the desc of the state
		rwDesc.FillMode = D3D11_FILL_WIREFRAME;
		rwDesc.CullMode = D3D11_CULL_NONE;
		// create a whole new rasterizer state
		pDevice->CreateRasterizerState(&rwDesc, &rwState);

		//solid
		D3D11_RASTERIZER_DESC rsDesc;
		pContext->RSGetState(&rsState); // retrieve the current state
		rsState->GetDesc(&rsDesc);    // get the desc of the state
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_BACK;
		// create a whole new rasterizer state
		pDevice->CreateRasterizerState(&rsDesc, &rsState);

		//create font
		HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
		hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Tahoma", &pFontWrapper);
		pFW1Factory->Release();

		// use the back buffer address to create the render target
		if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RenderTargetTexture)))
		{
			pDevice->CreateRenderTargetView(RenderTargetTexture, NULL, &RenderTargetView);
			RenderTargetTexture->Release();
		}
		firstTime = false;
	}

	//shaders
	if (!psRed)
		DXHooks::GenerateShader(pDevice, &psRed, 1.0f, 0.0f, 0.0f);

	if (!psGreen)
		DXHooks::GenerateShader(pDevice, &psGreen, 0.0f, 1.0f, 0.0f);

	if (!psPink)
		DXHooks::GenerateShader(pDevice, &psPink, 255.0f, 20.0f, 147.0f);

	//bind the render target view to the output merger stage of the pipeline
	pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

	//logger
	if ((GetAsyncKeyState(VK_MENU)) && (GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x4C) & 1)) //ALT + CTRL + L toggles logger
		DXHooks::logger = !DXHooks::logger;

	return phookD3D11Present(pSwapChain, SyncInterval, Flags);
}

//==========================================================================================================================
HRESULT DXHooks::GenerateShader(ID3D11Device* pD3DDevice, ID3D11PixelShader** pShader, float r, float g, float b)
{
	char szCast[] = "struct VS_OUT"
		"{"
		" float4 Position : SV_Position;"
		" float4 Color : COLOR0;"
		"};"

		"float4 main( VS_OUT input ) : SV_Target"
		"{"
		" float4 fake;"
		" fake.a = 1.0f;"
		" fake.r = %f;"
		" fake.g = %f;"
		" fake.b = %f;"
		" return fake;"
		"}";
	ID3D10Blob* pBlob;
	char szPixelShader[1000];

	sprintf_s(szPixelShader, szCast, r, g, b);

	ID3DBlob* d3dErrorMsgBlob;

	HRESULT hr = D3DCompile(szPixelShader, sizeof(szPixelShader), "shader", NULL, NULL, "main", "ps_4_0", NULL, NULL, &pBlob, &d3dErrorMsgBlob);

	if (FAILED(hr))
		return hr;

	hr = pD3DDevice->CreatePixelShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pShader);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

//==========================================================================================================================

bool DXHooks::SetDepthEnable(bool depthenable, bool stencilenable)
{
	ID3D11DepthStencilState *depthstencilstatea;
	UINT ref;
	D3D11_DEPTH_STENCIL_DESC desc;

	pContext->OMGetDepthStencilState(&depthstencilstatea, &ref);
	depthstencilstatea->GetDesc(&desc);

	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthEnable = depthenable;

	desc.StencilEnable = stencilenable;

	HRESULT result = pDevice->CreateDepthStencilState(&desc, &depthstencilstatea);
	if (FAILED(result))
	{
		return false;
	}
	pContext->OMSetDepthStencilState(depthstencilstatea, 0);
	return true;
}

//SetDepthFunc wallhack
bool DXHooks::SetDepthFunc(D3D11_COMPARISON_FUNC depthfunc, bool stencilenable)
{
	ID3D11DepthStencilState *depthstencilstateb;
	UINT ref;
	D3D11_DEPTH_STENCIL_DESC desc;

	pContext->OMGetDepthStencilState(&depthstencilstateb, &ref);
	depthstencilstateb->GetDesc(&desc);

	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = depthfunc;

	desc.StencilEnable = stencilenable;

	HRESULT result = pDevice->CreateDepthStencilState(&desc, &depthstencilstateb);
	if (FAILED(result))
	{
		return false;
	}
	pContext->OMSetDepthStencilState(depthstencilstateb, 0);
	return true;
}

void __stdcall DXHooks::hookD3D11DrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	//get stride & vdesc.ByteWidth
	pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);

	if (veBuffer) {
		veBuffer->GetDesc(&vedesc);

	}

	//get indesc.ByteWidth
	pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);

	if (inBuffer) {
		inBuffer->GetDesc(&indesc);
	}

	if (pFontWrapper)
	{
		wchar_t reportValue[256];
		swprintf_s(reportValue, L"countnum = %d, matchLevel = %d depthNum = %d", countnum, matchLevel, depthNum);

		pFontWrapper->DrawString(pContext, reportValue, 20.0f, 220.0f, 100.0f, 0xffffffff, FW1_RESTORESTATE);
	}

	//wallhack example

	if (modelChecker->findModel(matchLevel, Stride, IndexCount, indesc.ByteWidth, vedesc.ByteWidth, Descr.Format))
	{
		ID3D11RasterizerState * _rwState;
		D3D11_RASTERIZER_DESC rwDesc;
		pContext->RSGetState(&rwState); // retrieve the current state

		rwDesc.AntialiasedLineEnable = true;
		rwDesc.CullMode = D3D11_CULL_NONE;
		rwDesc.DepthBias = depthNum;
		rwDesc.DepthBiasClamp = 0.0f;
		rwDesc.DepthClipEnable = false;
		rwDesc.FillMode = D3D11_FILL_SOLID;
		rwDesc.FrontCounterClockwise = false;
		rwDesc.MultisampleEnable = false;
		rwDesc.ScissorEnable = false;
		rwDesc.SlopeScaledDepthBias = 0.0f;
		pDevice->CreateRasterizerState(&rwDesc, &_rwState);
		pContext->RSSetState(_rwState);

		SetDepthEnable(false, false); //wallhack on behind walls
		pContext->PSSetShader(psPink, NULL, NULL); //behind walls
		phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
		SetDepthEnable(true, false); //wallhack off
		pContext->PSSetShader(psRed, NULL, NULL); //green

												  //SetDepthStencilState(DISABLED);
												  //pContext->PSSetShader(psPink, NULL, NULL);
												  //SetDepthStencilState(READ_NO_WRITE);
	}

	if (GetAsyncKeyState(VK_NUMPAD8) & 1)
		depthNum++;

	if (GetAsyncKeyState(VK_NUMPAD5) & 1)
		depthNum--;

	if (GetAsyncKeyState(VK_UP) & 1)
		strideNum++;

	if (GetAsyncKeyState(VK_DOWN) & 1)
		strideNum--;

	if (GetAsyncKeyState(VK_RIGHT) & 1)
		matchLevel++;

	if (GetAsyncKeyState(VK_LEFT) & 1) {
		strideNum = 0;
		countnum = 0;
		matchLevel = 0;
	}

	//hold down P key until a texture is wallhacked, press I to log values of those textures
	if (GetAsyncKeyState('O') & 1) //-
		countnum--;
	if (GetAsyncKeyState('P') & 1) //+
		countnum++;
	if ((GetAsyncKeyState(VK_MENU)) && (GetAsyncKeyState('9') & 1)) //reset, set to -1
		countnum = -1;
	if (countnum == indesc.ByteWidth / 1000)
		if (GetAsyncKeyState('I') & 1)
			utils->Log("Model(%d, %d, %d, %d, %d)", Stride, IndexCount, indesc.ByteWidth, vedesc.ByteWidth, Descr.Format);

	if (Stride == strideNum)
		if (GetAsyncKeyState('I') & 1)
			utils->Log("Stride == %d", Stride);

	//ALT + CTRL + L toggles logger
	if (logger)
	{

		if (strideNum == Stride) {
			pContext->PSSetShader(psRed, NULL, NULL);
		}

		if (countnum == indesc.ByteWidth / 1000)
		{
			pContext->PSSetShader(psGreen, NULL, NULL);
		}
	}

	return phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
}

//==========================================================================================================================

void __stdcall DXHooks::hookD3D11PSSetShaderResources(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews)
{

	pssrStartSlot = StartSlot;

	for (UINT j = 0; j < NumViews; j++)
	{
		//Resources loop
		ID3D11ShaderResourceView* pShaderResView = ppShaderResourceViews[j];
		if (pShaderResView)
		{
			pShaderResView->GetDesc(&Descr);

			if (
				(Descr.ViewDimension == D3D11_SRV_DIMENSION_BUFFER)
				||
				(Descr.ViewDimension == D3D11_SRV_DIMENSION_BUFFEREX)
				)
			{
				continue;//Skip buffer resources
			}
		}
	}

	return phookD3D11PSSetShaderResources(pContext, StartSlot, NumViews, ppShaderResourceViews);
}

//==========================================================================================================================

void __stdcall DXHooks::hookD3D11CreateQuery(ID3D11Device* pDevice, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery)
{
	//Disable Occlusion which prevents rendering player models through certain objects (used by wallhack to see models through walls at all distances, REDUCES FPS)
	if (pQueryDesc->Query == D3D11_QUERY_OCCLUSION)
	{
		D3D11_QUERY_DESC oqueryDesc = CD3D11_QUERY_DESC();
		(&oqueryDesc)->MiscFlags = pQueryDesc->MiscFlags;
		(&oqueryDesc)->Query = D3D11_QUERY_TIMESTAMP;

		return phookD3D11CreateQuery(pDevice, &oqueryDesc, ppQuery);
	}

	return phookD3D11CreateQuery(pDevice, pQueryDesc, ppQuery);
}

void __stdcall DXHooks::hookCreateVertexShader(ID3D10Device* pDevice, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10VertexShader **ppVertexShader)
{

	Beep(200, 200);

	ID3D10Blob *pIDisassembly;
	D3DDisassemble(pShaderBytecode, BytecodeLength, 0, 0, &pIDisassembly);

	if (pIDisassembly)
	{
		utils->Log("Vertex buffer hooked %s", pIDisassembly);
	}

	return phookCreateVertexShader(pDevice, pShaderBytecode, BytecodeLength, ppVertexShader);
}


//==========================================================================================================================

void __stdcall DXHooks::hookD3D11Draw(ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation)
{
	if (GetAsyncKeyState(VK_F9) & 1)
		utils->Log("Draw called");

	return phookD3D11Draw(pContext, VertexCount, StartVertexLocation);
}

//==========================================================================================================================

void __stdcall DXHooks::hookD3D11DrawInstanced(ID3D11DeviceContext* pContext, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	if (GetAsyncKeyState(VK_F9) & 1)
		utils->Log("DrawInstanced called");

	return phookD3D11DrawInstanced(pContext, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

//==========================================================================================================================

void __stdcall DXHooks::hookD3D11DrawIndexedInstanced(ID3D11DeviceContext* pContext, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	if (GetAsyncKeyState(VK_F9) & 1)
		utils->Log("DrawIndexedInstanced called");

	return phookD3D11DrawIndexedInstanced(pContext, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

//==========================================================================================================================

void __stdcall DXHooks::hookD3D11DrawInstancedIndirect(ID3D11DeviceContext* pContext, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs)
{
	if (GetAsyncKeyState(VK_F9) & 1)
		utils->Log("DrawInstancedIndirect called");

	return phookD3D11DrawInstancedIndirect(pContext, pBufferForArgs, AlignedByteOffsetForArgs);
}

//==========================================================================================================================

void __stdcall DXHooks::hookD3D11DrawIndexedInstancedIndirect(ID3D11DeviceContext* pContext, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs)
{
	if (GetAsyncKeyState(VK_F9) & 1)
		utils->Log("DrawIndexedInstancedIndirect called");

	return phookD3D11DrawIndexedInstancedIndirect(pContext, pBufferForArgs, AlignedByteOffsetForArgs);
}

//==========================================================================================================================

