#include "CWEGraphicManager.h"
#include "CWEError.h"
#include "CWEPolygon.h"
#include <vector>


#define RAW_STRING_W(var) L#var


namespace CWE
{

//==================================================================================================================================================================================================
//
// レンダリングパイプライン
//
//==================================================================================================================================================================================================


static float CLEAR_COLOR[4] ={0.0f, 0.0f, 0.0f, 1.0f};


// シェーダーマクロ定義
ShaderMacro::ShaderMacro(const char* name, const char* definition)
{
	Set(name, definition);
}

void ShaderMacro::Set(const char* name, const char* definition)
{
	mShaderMacro.Name =name;
	mShaderMacro.Definition =definition;
}


// ========== レンダリングパイプライン ==================

// 入力エレメント
IAInputElement::IAInputElement(const char* semanticName, unsigned semanticIndex, const Format& format, unsigned inputSlot, unsigned alignedByteOffset, InputClassification inputSlotClass, unsigned instanceDataStepRate)
{
	Set(semanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate);
}

void IAInputElement::Set(const char* semanticName, unsigned semanticIndex, const Format& format, unsigned inputSlot, unsigned alignedByteOffset, InputClassification inputSlotClass, unsigned instanceDataStepRate)
{
	mIE.SemanticName =semanticName;
	mIE.SemanticIndex =semanticIndex;
	mIE.Format =format;
	mIE.InputSlot =inputSlot;
	mIE.AlignedByteOffset =alignedByteOffset;
	mIE.InputSlotClass =inputSlotClass==InputClassification::VertexData ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
	mIE.InstanceDataStepRate =instanceDataStepRate;
}


// 入力レイアウト&頂点シェーダオブジェクト
bool IAAndVS::Create(const wchar_t* shaderFile, const ShaderMacro* pShaderMacros, const char* functionName, const IAInputElement* pInputElements, unsigned numElements)
{
	Graphic& master =Graphic::GetInstance();

	// デバイスが生成済みであることを保証する
	CWE_ASSERT(master.mpDevice);

	//シェーダコードのコンパイルオプション
#if defined(DEBUG) || defined(_DEBUG)
	UINT compileOption =D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
	//コンパイル済みシェーダの使用が前提
	UINT compileOption =D3D10_SHADER_OPTIMIZATION_LEVEL3 | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#endif

	//シェーダのバイトコードを格納するブロブ
	ID3DBlob* pBlob =nullptr;

	//頂点シェーダコードのコンパイル
	HRESULT hr;
	if(pShaderMacros) {D3DX11CompileFromFile(shaderFile, &pShaderMacros[0].mShaderMacro, NULL, functionName, "vs_5_0", compileOption, NULL, NULL, &pBlob, NULL, &hr);}
	else {D3DX11CompileFromFile(shaderFile, NULL, NULL, functionName, "vs_5_0", compileOption, NULL, NULL, &pBlob, NULL, &hr);}
	if(FAILED(hr))
	{
		FatalError::GetInstance().Outbreak(L"頂点シェーダの作成に失敗しました\nファイルが破損している可能性があります");
		return false;
	}
	//頂点シェーダの作成
	if(FAILED(master.mpDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &mpVertexShader)))
	{
		//開放してから終了
		SAFE_RELEASE(pBlob);
		FatalError::GetInstance().Outbreak(L"頂点シェーダの作成に失敗しました\nファイルが破損している可能性があります");
		return false;
	}

	//入力レイアウトオブジェクトの作成
	if(!pInputElements) {return false;}
	if(FAILED(master.mpDevice->CreateInputLayout(&pInputElements[0].mIE, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &mpInputLayout)))
	{
		//開放してから終了
		SAFE_RELEASE(pBlob);
		FatalError::GetInstance().Outbreak(L"入力レイアウトオブジェクトの作成に失敗しました\n処理を継続できません");
		return false;
	}
	//頂点シェーダオブジェクトの作成終了
	SAFE_RELEASE(pBlob);

	return true;
}


// ジオメトリシェーダオブジェクトの作成
bool GS::Create(const wchar_t* shaderFile, const ShaderMacro* pShaderMacros, const char* functionName)
{
	Graphic& master =Graphic::GetInstance();

	// デバイスが生成済みであることを保証する
	CWE_ASSERT(master.mpDevice);

	//シェーダコードのコンパイルオプション
#if defined(DEBUG) || defined(_DEBUG)
	UINT compileOption =D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
	UINT compileOption =D3D10_SHADER_OPTIMIZATION_LEVEL3 | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#endif

	//シェーダのバイトコードを格納するブロブ
	ID3DBlob* pBlob =nullptr;

	//ジオメトリシェーダコードのコンパイル
	HRESULT hr;
	if(pShaderMacros) {D3DX11CompileFromFile(shaderFile, &pShaderMacros[0].mShaderMacro, NULL, functionName, "gs_5_0", compileOption, NULL, NULL, &pBlob, NULL, &hr);}
	else {D3DX11CompileFromFile(shaderFile, NULL, NULL, functionName, "gs_5_0", compileOption, NULL, NULL, &pBlob, NULL, &hr);}
	if(FAILED(hr))
	{
		FatalError::GetInstance().Outbreak(L"ジオメトリシェーダの作成に失敗しました\nファイルが破損している可能性があります");
		return false;
	}

	//ジオメトリシェーダの作成
	if(FAILED(master.mpDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &mpGeometryShader)))
	{
		//開放してから終了
		SAFE_RELEASE(pBlob);
		FatalError::GetInstance().Outbreak(L"ジオメトリシェーダの作成に失敗しました\nファイルが破損している可能性があります");
		return false;
	}
	//ジオメトリシェーダオブジェクトの作成終了
	SAFE_RELEASE(pBlob);

	return true;
}


// ピクセルシェーダオブジェクトの作成
bool PS::Create(const wchar_t* shaderFile, const ShaderMacro* pShaderMacros, const char* functionName)
{
	Graphic& master =Graphic::GetInstance();

	// デバイスが生成済みであることを保証する
	CWE_ASSERT(master.mpDevice);

	//シェーダコードのコンパイルオプション
#if defined(DEBUG) || defined(_DEBUG)
	UINT compileOption =D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#else
	//コンパイル済みシェーダの使用が前提
	UINT compileOption =D3D10_SHADER_OPTIMIZATION_LEVEL3 | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;
#endif

	//シェーダのバイトコードを格納するブロブ
	ID3DBlob* pBlob =nullptr;

	//ピクセルシェーダコードのコンパイル
	HRESULT hr;
	if(pShaderMacros) {D3DX11CompileFromFile(shaderFile, &pShaderMacros[0].mShaderMacro, NULL, functionName, "ps_5_0", compileOption, NULL, NULL, &pBlob, NULL, &hr);}
	else {D3DX11CompileFromFile(shaderFile, NULL, NULL, functionName, "ps_5_0", compileOption, NULL, NULL, &pBlob, NULL, &hr);}
	if(FAILED(hr))
	{
		FatalError::GetInstance().Outbreak(L"ピクセルシェーダの作成に失敗しました\nファイルが破損している可能性があります");
		return false;
	}

	//ピクセルシェーダの作成
	if(FAILED(master.mpDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &mpPixelShader)))
	{
		//開放してから終了
		SAFE_RELEASE(pBlob);
		FatalError::GetInstance().Outbreak(L"ピクセルシェーダの作成に失敗しました\nファイルが破損している可能性があります");
		return false;
	}
	//ピクセルシェーダオブジェクトの作成終了
	SAFE_RELEASE(pBlob);

	return true;
}


// プリセットから作成
bool SamplerState::Create(const SamplerStatus& preset)
{
	Graphic& master =Graphic::GetInstance();
	// デバイスが生成済みであることを保証する
	CWE_ASSERT(master.mpDevice);

	//サンプラーステートの設定
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.AddressU =D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV =D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW =D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias =0.0f;
	samplerDesc.MaxAnisotropy =0;
	samplerDesc.ComparisonFunc =D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] =0.0f;
	samplerDesc.BorderColor[1] =0.0f;
	samplerDesc.BorderColor[2] =0.0f;
	samplerDesc.BorderColor[3] =0.0f;
	samplerDesc.MaxLOD =FLT_MAX;
	samplerDesc.MinLOD =-FLT_MAX;

	switch(preset)
	{
	case SamplerStatus::Liner:
		samplerDesc.Filter =D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	case SamplerStatus::Point:
		samplerDesc.Filter =D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	}


	//サンプラーステートの作成
	if(FAILED(master.mpDevice->CreateSamplerState(&samplerDesc, &mpSamplerState)))
	{
		FatalError::GetInstance().Outbreak(L"サンプラーステートの作成に失敗しました\nプログラムを終了します");
		return false;
	}

	return true;
}

// シャドウマップ用の作成
bool SamplerState::CreateForShadow()
{
	Graphic& master =Graphic::GetInstance();
	// デバイスが生成済みであることを保証する
	CWE_ASSERT(master.mpDevice);

	//サンプラーステートの設定
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.AddressU =D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV =D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW =D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias =0.0f;
	samplerDesc.MaxAnisotropy =1;
	samplerDesc.ComparisonFunc =D3D11_COMPARISON_LESS_EQUAL;
	samplerDesc.Filter =D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.BorderColor[0] =1.0f;
	samplerDesc.BorderColor[1] =1.0f;
	samplerDesc.BorderColor[2] =1.0f;
	samplerDesc.BorderColor[3] =1.0f;
	samplerDesc.MaxLOD =FLT_MAX;
	samplerDesc.MinLOD =-FLT_MAX;

	//サンプラーステートの作成
	if(FAILED(master.mpDevice->CreateSamplerState(&samplerDesc, &mpSamplerState)))
	{
		FatalError::GetInstance().Outbreak(L"サンプラーステートの作成に失敗しました\nプログラムを終了します");
		return false;
	}

	return true;
}


// コンスタントバッファの作成
bool ConstantBuffer::Create(size_t bufferSize)
{
	Graphic& master =Graphic::GetInstance();
	//デバイスが生成済みであることを保証する
	CWE_ASSERT(master.mpDevice);

	//設定
	D3D11_BUFFER_DESC constBafferDesc;
	constBafferDesc.Usage =D3D11_USAGE_DYNAMIC;
	constBafferDesc.BindFlags =D3D11_BIND_CONSTANT_BUFFER;
	constBafferDesc.CPUAccessFlags =D3D11_CPU_ACCESS_WRITE;
	constBafferDesc.MiscFlags =0;
	constBafferDesc.StructureByteStride =0;
	constBafferDesc.ByteWidth =bufferSize;

	//定数バッファの作成
	if(FAILED(master.mpDevice->CreateBuffer(&constBafferDesc, NULL, &mpConstantBuffer)))
	{
		FatalError::GetInstance().Outbreak(L"コンスタントバッファの作成に失敗しました\nプログラムを終了します");
		return false;
	}

	return true;
}

// コンスタントバッファの書き込み開始
bool ConstantBuffer::Map()
{
	Graphic& master =Graphic::GetInstance();
	//デバイスが生成済みであることを保証する
	CWE_ASSERT(master.mpImmediateContext);

	if(FAILED(master.mpImmediateContext->Map(mpConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource)))
	{
		return false;
	}

	mMapped =true;
	return true;
}

// コンスタントバッファの書き込み終了
void ConstantBuffer::Unmap()
{
	Graphic& master =Graphic::GetInstance();
	//デバイスが生成済みであることを保証する
	CWE_ASSERT(master.mpImmediateContext);

	master.mpImmediateContext->Unmap(mpConstantBuffer, 0);

	mMapped =false;
}

// サブリソースへの書き込み
void* const ConstantBuffer::Access()
{
	if(!mMapped)
	{
		return nullptr;
	}
	return mResource.pData;
}


// ============ レンダーターゲットビューオブジェクト ==========

// クリア
void RenderTargetView::Clear()
{
	Graphic::GetInstance().DeviceContext().ClearRenderTargetView(mpRenderTargetView, CLEAR_COLOR);
}


// ============ 深度/ステンシルビューオブジェクト =============

// クリア
void DepthStencilView::Clear()
{
	//今は固定値
	Graphic::GetInstance().DeviceContext().ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}



//==================================================================================================================================================================================================
//
// テクスチャ
//
//==================================================================================================================================================================================================


// ======== 2Dテクスチャ =================

// テクスチャの作成
bool Texture2D::Create(const Texture2DDesc& desc)
{
	if(FAILED( Graphic::GetInstance().Device().CreateTexture2D(&desc, nullptr, &mpTexture) )) //サブリソースは未実装
	{
		return false;
	}
	
	return true;
}


//==================================================================================================================================================================================================
//
// 描画系管理クラス
//
//==================================================================================================================================================================================================

//コンストラクタ
Graphic::Graphic():
mWindows(),
mpFactory(nullptr), mpDevice(nullptr), mpImmediateContext(nullptr), mpSwapChain(nullptr),
mRS(), mOM(), mSyncInterval(1)
{
	mFormat[0] =DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //バックバッファ
	mFormat[1] =DXGI_FORMAT_D24_UNORM_S8_UINT; //深度/ステンシルバッファ

	mMSAAQuality.Count =1;
	mMSAAQuality.Quality =0;
}

//デストラクタ
Graphic::~Graphic()
{
}

//インスタンスの取得
Graphic& Graphic::GetInstance()
{
	static Graphic instance;
	return instance;
}


//初期化
bool Graphic::Init()
{
	//IDXGIFactoryインターフェイスの取得
	if(FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&mpFactory)))
	{
		FatalError::GetInstance().Outbreak(FatalError::smDeviceCreationErrorMS);
		return false;
	}
	//デバイスとスワップチェインの作成
	if(!CreateDeviceAndSwapChain()) {return false;}
	//描画パイプライン系オブジェクトの作成
	if(!CreateBlendState()) {return false;}
	if(!CreateRasterizerState()) {return false;}
	if(!CreateDepthStencilState()) {return false;}
	
	//バックバッファーのリセット
	if(!ResetBackBuffer())
	{
		FatalError::GetInstance().Outbreak(FatalError::smDeviceCreationErrorMS);
		return false;
	}

	//[Alt]+[Enter]キーによる画面モード切り替え機能を無効にする(効果なし)
	mpFactory->MakeWindowAssociation(mWindows.mMainWindow, DXGI_MWA_NO_ALT_ENTER);

	//描画パイプラインの初期化
	InitRenderingPipeline();
	RenderingPipelineRestoration();

	return true;
}


//終了処理
void Graphic::End()
{
	//デバイスステートのクリア
	if(mpImmediateContext) {mpImmediateContext->ClearState();}

	//スワップチェインをウインドウモードにする
	if(mpSwapChain) {mpSwapChain->SetFullscreenState(FALSE, NULL);}

	//取得したインターフェイスの開放
	SAFE_RELEASE(mOM.mpDepthStencilState);
	for(int i=0; i<_countof(mRS.mpRasterizerState); ++i) {SAFE_RELEASE(mRS.mpRasterizerState[i]);}
	SAFE_RELEASE(mOM.mpBlendState);
	SAFE_RELEASE(mOM.mpBlendState);
	SAFE_RELEASE(mOM.mpRenderTargetView);
	SAFE_RELEASE(mpSwapChain);
	SAFE_RELEASE(mpImmediateContext);
	SAFE_RELEASE(mpDevice);
	SAFE_RELEASE(mpFactory);
}


//更新処理
void Graphic::Update()
{
	//バックバッファの表示
	mpSwapChain->Present(mSyncInterval, 0);

	//描画ターゲットのクリア
	mpImmediateContext->ClearRenderTargetView(mOM.mpRenderTargetView, CLEAR_COLOR);
	//深度/ステンシルのクリア
	mpImmediateContext->ClearDepthStencilView(mOM.mpDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


//Device and swap chain creation.
bool Graphic::CreateDeviceAndSwapChain()
{
	D3D_FEATURE_LEVEL featureLevels =D3D_FEATURE_LEVEL_11_0,
					  featureLevelsSupported;

	//Create device.
	if(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevels, 1, D3D11_SDK_VERSION, &mpDevice, &featureLevelsSupported, &mpImmediateContext))
	{
		FatalError::GetInstance().Outbreak(FatalError::smDeviceCreationErrorMS);
		return false;
	}

	//Description of swap chain
	DXGI_SWAP_CHAIN_DESC swapStatus;
	ZeroMemory(&swapStatus, sizeof(swapStatus));
	swapStatus.BufferCount =1;
	swapStatus.BufferDesc.Width =mWindows.mInitialWindowSizeX;
	swapStatus.BufferDesc.Height =mWindows.mInitialWindowSizeY;
	swapStatus.BufferDesc.Format =mFormat[0];
	swapStatus.BufferDesc.RefreshRate.Numerator =60;
	swapStatus.BufferDesc.RefreshRate.Denominator =1;
//	swapStatus.BufferDesc.Scaling =DXGI_MODE_SCALING_CENTERED;
//	swapStatus.BufferDesc.ScanlineOrdering =DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	swapStatus.BufferUsage =DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapStatus.OutputWindow =mWindows.mMainWindow;
	swapStatus.SampleDesc =mMSAAQuality;
	swapStatus.Windowed =mWindows.mIsWindowMode;
	swapStatus.Flags =DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//Create swap chain.
	if(FAILED(mpFactory->CreateSwapChain(mpDevice, &swapStatus, &mpSwapChain)))
	{
		FatalError::GetInstance().Outbreak(FatalError::smDeviceCreationErrorMS);
		return false;
	}

	return true;
}


//ブレンドステートの作成
bool Graphic::CreateBlendState()
{
	//ブレンドステートの設定
	D3D11_BLEND_DESC blendState;
	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));
	blendState.AlphaToCoverageEnable =FALSE;
	blendState.IndependentBlendEnable =FALSE;
	blendState.RenderTarget[0].BlendEnable =TRUE;
	blendState.RenderTarget[0].SrcBlend =D3D11_BLEND_SRC_ALPHA;;
	blendState.RenderTarget[0].DestBlend =D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOp =D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha =D3D11_BLEND_ONE;
	blendState.RenderTarget[0].DestBlendAlpha =D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].BlendOpAlpha =D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].RenderTargetWriteMask =D3D11_COLOR_WRITE_ENABLE_ALL;

	//ブレンドステートオブジェクトの作成
	if(FAILED(mpDevice->CreateBlendState(&blendState, &mOM.mpBlendState)))
	{
		FatalError::GetInstance().Outbreak(L"ブレンドステートの作成に失敗しました\nプログラムを終了します");
		return false;
	}

	return true;
}


//ラスタライザステートの作成
bool Graphic::CreateRasterizerState()
{
	//ラスタライザステートの設定
	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode =D3D11_FILL_SOLID;
	rasterizerState.CullMode =D3D11_CULL_BACK;
	rasterizerState.FrontCounterClockwise =FALSE;
	rasterizerState.DepthBias =0;
	rasterizerState.DepthBiasClamp =0;
	rasterizerState.SlopeScaledDepthBias =0;
	rasterizerState.DepthClipEnable =TRUE; //深度値クリッピング
	rasterizerState.ScissorEnable =FALSE; //シザー矩形クリッピング
	rasterizerState.MultisampleEnable =FALSE; //マルチサンプリング
	rasterizerState.AntialiasedLineEnable =FALSE; //線のマルチサンプリング

	//標準ラスタライザステートオブジェクトの作成
	if(FAILED(mpDevice->CreateRasterizerState(&rasterizerState, &mRS.mpRasterizerState[0])))
	{
		FatalError::GetInstance().Outbreak(L"ラスタライザステートの作成に失敗しました\nプログラムを終了します");
		return false;
	}
	// カリングなしラスタライザステートオブジェクト
	rasterizerState.CullMode =D3D11_CULL_NONE;
	if(FAILED(mpDevice->CreateRasterizerState(&rasterizerState, &mRS.mpRasterizerState[1])))
	{
		FatalError::GetInstance().Outbreak(L"ラスタライザステートの作成に失敗しました\nプログラムを終了します");
		return false;
	}
	// ワイヤーフレームラスタライザステートオブジェクト
	rasterizerState.FillMode =D3D11_FILL_WIREFRAME;
	rasterizerState.CullMode =D3D11_CULL_NONE;
	if(FAILED(mpDevice->CreateRasterizerState(&rasterizerState, &mRS.mpRasterizerState[2])))
	{
		FatalError::GetInstance().Outbreak(L"ラスタライザステートの作成に失敗しました\nプログラムを終了します");
		return false;
	}

	return true;
}


//深度/ステンシルステートの作成
bool Graphic::CreateDepthStencilState()
{
	//深度/ステンシルステートの設定
	D3D11_DEPTH_STENCIL_DESC depthStencilState;
	depthStencilState.DepthEnable =TRUE;
	depthStencilState.DepthWriteMask =D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilState.DepthFunc =D3D11_COMPARISON_LESS;
	depthStencilState.StencilEnable =FALSE;
	depthStencilState.StencilReadMask =0;
	depthStencilState.StencilWriteMask =0;
		//面が表を向いているときのステンシルテストの設定
	depthStencilState.FrontFace.StencilFailOp =D3D11_STENCIL_OP_KEEP;
	depthStencilState.FrontFace.StencilDepthFailOp =D3D11_STENCIL_OP_KEEP;
	depthStencilState.FrontFace.StencilPassOp =D3D11_STENCIL_OP_KEEP;
	depthStencilState.FrontFace.StencilFunc =D3D11_COMPARISON_NEVER;
		//面が裏を向いているときのステンシルテストの設定
	depthStencilState.BackFace.StencilFailOp =D3D11_STENCIL_OP_KEEP;
	depthStencilState.BackFace.StencilDepthFailOp =D3D11_STENCIL_OP_KEEP;
	depthStencilState.BackFace.StencilPassOp =D3D11_STENCIL_OP_KEEP;
	depthStencilState.BackFace.StencilFunc =D3D11_COMPARISON_ALWAYS;

	//深度/ステンシルステートオブジェクトの作成
	if(FAILED(mpDevice->CreateDepthStencilState(&depthStencilState, &mOM.mpDepthStencilState)))
	{
		FatalError::GetInstance().Outbreak(L"深度/ステンシルステートの作成に失敗しました\nプログラムを終了します");
		return false;
	}

	return true;
}


//描画パイプラインの初期化
void Graphic::InitRenderingPipeline()
{
	// IAにプリミティブを三角形リストでセット
	mpImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// RSにラスタライザステートオブジェクトを設定
	mpImmediateContext->RSSetState(mRS.mpRasterizerState[0]);
}


//デバイスの消失処理
void Graphic::ChackDeviceLost()
{
	if(mpDevice->GetDeviceRemovedReason() != S_OK)
	{
		FatalError::GetInstance().Outbreak(L"DirectXデバイスがロストしました\n処理を継続できません");
	}
}


//バックバッファの初期化
bool Graphic::ResetBackBuffer()
{
	//スワップチェインからバックバッファを取得する
	ID3D11Texture2D* pBackBuffer =0;
	if(FAILED(mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer)))
	{
		return false;
	}

	//取得したバックバッファの情報を保存
	D3D11_TEXTURE2D_DESC backBufferStatus;
	pBackBuffer->GetDesc(&backBufferStatus);

	//スワップチェインから取得したバックバッファを元に描画ターゲットビューを作る
	if(FAILED(mpDevice->CreateRenderTargetView(pBackBuffer, NULL, &mOM.mpRenderTargetView)))
	{
		SAFE_RELEASE(pBackBuffer);
		return false;
	}
	//以降バックバッファは直接使わないので解放
	SAFE_RELEASE(pBackBuffer);


	//深度/ステンシルテクスチャの設定
	D3D11_TEXTURE2D_DESC depthStencilStatus =backBufferStatus;
//	depthStencilStatus.Width =backBufferStatus.Width;
//	depthStencilStatus.Height =backBufferStatus.Height;
	depthStencilStatus.MipLevels =1;
	depthStencilStatus.ArraySize =1;
	depthStencilStatus.Format =mFormat[1];
	depthStencilStatus.SampleDesc =backBufferStatus.SampleDesc;
	depthStencilStatus.Usage =D3D11_USAGE_DEFAULT;
	depthStencilStatus.BindFlags =D3D11_BIND_DEPTH_STENCIL;
	depthStencilStatus.CPUAccessFlags =NULL;
	depthStencilStatus.MiscFlags =NULL;
	//深度/ステンシルテクスチャの作成
	ID3D11Texture2D* pDepthStencil =0;
	if(FAILED(mpDevice->CreateTexture2D(&depthStencilStatus, NULL, &pDepthStencil)))
	{
		SAFE_RELEASE(pDepthStencil);
		return false;
	}

	//深度/ステンシルビューの設定
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewStatus;
	depthStencilViewStatus.Format =depthStencilStatus.Format;
	depthStencilViewStatus.ViewDimension =D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewStatus.Flags =0;
	depthStencilViewStatus.Texture2D.MipSlice =0;
	HRESULT hr;
	//深度/ステンシルビューの作成
	if(FAILED(hr =mpDevice->CreateDepthStencilView(pDepthStencil, NULL, &mOM.mpDepthStencilView)))
	{
		SAFE_RELEASE(pDepthStencil);
		return false;
	}
	SAFE_RELEASE(pDepthStencil);

	//描画パイプラインの復元
	RenderingPipelineRestoration();

	return true;
}


//画面サイズの変更に伴う描画パイプラインの復元
void Graphic::RenderingPipelineRestoration()
{
	//OMに描画ターゲットビューと深度/ステンシルビューを設定
	mpImmediateContext->OMSetRenderTargets(1, &mOM.mpRenderTargetView, mOM.mpDepthStencilView);
	//OMにブレンドステートオブジェクトを設定
	const FLOAT blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	mpImmediateContext->OMSetBlendState(mOM.mpBlendState, blendFactor, 0xffffffff);
	//OMに深度/ステンシルステートオブジェクトを設定
	mpImmediateContext->OMSetDepthStencilState(mOM.mpDepthStencilState, 0);
}




//Windowsコールバック関数用の描画領域サイズ変更関数
void Graphic::InnerResetWindowSize()
{
	//描画ターゲットのリリース
	mpImmediateContext->OMSetRenderTargets(0, NULL, NULL);
	SAFE_RELEASE(mOM.mpRenderTargetView);

	//スワップチェインを設定済みのバッファに変更する
	mpSwapChain->ResizeBuffers(1, 0, 0, mFormat[0], DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
}


//フルスクリーンモードを正しい状態に戻す
void Graphic::InnerWindowModeStateRestoration()
{
	mpSwapChain->SetFullscreenState(mWindows.mIsWindowMode ? FALSE:TRUE, NULL);
	MessageBox(mWindows.mMainWindow, L"Alt+Enterによる画面モード切替には対応していません。", L"非対応の動作", MB_OK);
}


// 頂点シェーダの切り替え
void Graphic::VSSetShader(const IAAndVS& shader)
{
	CWE_ASSERT(shader.mpInputLayout&&shader.mpVertexShader);
	mpImmediateContext->IASetInputLayout(shader.mpInputLayout);
	mpImmediateContext->VSSetShader(shader.mpVertexShader, nullptr, 0);
}


// ジオメトリシェーダの切り替え
void Graphic::GSSetShader(const GS& shader)
{
	CWE_ASSERT(shader.mpGeometryShader);
	mpImmediateContext->GSSetShader(shader.mpGeometryShader, nullptr, 0);
}


// ピクセルシェーダの切り替え
void Graphic::PSSetShader(const PS& shader)
{
	CWE_ASSERT(shader.mpPixelShader);
	mpImmediateContext->PSSetShader(shader.mpPixelShader, nullptr, 0);
}


// 各シェーダのリムーブ
void Graphic::VSRemoveShader()
{
	mpImmediateContext->IASetInputLayout(nullptr);
	mpImmediateContext->VSSetShader(nullptr, nullptr, 0);
}

void Graphic::GSRemoveShader()
{
	mpImmediateContext->GSSetShader(nullptr, nullptr, 0);
}

void Graphic::PSRemoveShader()
{
	mpImmediateContext->PSSetShader(nullptr, nullptr, 0);
}


//ウィンドウモードのリセット
void Graphic::ResetWindowMode(bool windowMode)
{
	BOOL currentWindowMode;

	mpSwapChain->GetFullscreenState(&currentWindowMode, NULL);
	if(windowMode == (currentWindowMode?false:true)) {return;}

	mpSwapChain->SetFullscreenState(!currentWindowMode, NULL);

	//コアに変更を保存
	mWindows.mIsWindowMode =(currentWindowMode?true:false);
}


//ディスプレイモードのリセット
bool Graphic::ResetDisplayMode(unsigned width, unsigned height/*, const unsigned& refreshRate*/)
{
	//フルスクリーンモード時は変更を受け付けない
	if(!mWindows.mIsWindowMode)
	{
		return false;
	}

	//変更内容をセット
	DXGI_MODE_DESC modeState;

	modeState.Width =width;
	modeState.Height =height;
	modeState.RefreshRate.Numerator =60;
	modeState.RefreshRate.Denominator =1;
	modeState.Format =mFormat[0];
	modeState.ScanlineOrdering =DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	modeState.Scaling =DXGI_MODE_SCALING_CENTERED;

	//コアの前状態を保存
	int beforeWidth =mWindows.mWindowSizeX,
		beforeHeight =mWindows.mWindowSizeY;
	double beforeExRateX =mWindows.mWindowSizeExRateX,
		   beforeExRateY =mWindows.mWindowSizeExRateY;
	//コアに変更を保存
	mWindows.mWindowSizeX =width;
	mWindows.mWindowSizeY =height;
	mWindows.mWindowSizeExRateX =mWindows.mWindowSizeX/mWindows.mInitialWindowSizeX;
	mWindows.mWindowSizeExRateY =mWindows.mWindowSizeY/mWindows.mInitialWindowSizeY;

	//スワップチェインに変更内容を保存
	if(FAILED(mpSwapChain->ResizeTarget(&modeState)))
	{
		//失敗時にコアの変更を前状態に戻す
		mWindows.mWindowSizeX =beforeHeight;
		mWindows.mWindowSizeY =beforeWidth;
		mWindows.mWindowSizeExRateX =beforeExRateX;
		mWindows.mWindowSizeExRateY =beforeExRateY;
		return false;
	}
	
	return true;
}


//Set the sync interval.
void Graphic::SetSyncInterval(unsigned inter)
{
	mSyncInterval =inter;
}


// 使用するラスタライザステートのリセット
void Graphic::RSResetUseState(const RSState& useState)
{
	switch(useState)
	{
	case RSState::Normal:
		mpImmediateContext->RSSetState(mRS.mpRasterizerState[0]);
		break;

	case RSState::NonCull:
		mpImmediateContext->RSSetState(mRS.mpRasterizerState[1]);
		break;

	case RSState::WireFrame:
		mpImmediateContext->RSSetState(mRS.mpRasterizerState[2]);
		break;
	}
}


// サンプラーステートのセット
void Graphic::VSSetSampler(unsigned slot, const SamplerState& samplerState)
{
	CWE_ASSERT(slot<D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	mpImmediateContext->VSSetSamplers(slot, 1, &samplerState.mpSamplerState);
}
void Graphic::GSSetSampler(unsigned slot, const SamplerState& samplerState)
{
	CWE_ASSERT(slot<D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	mpImmediateContext->GSSetSamplers(slot, 1, &samplerState.mpSamplerState);
}
void Graphic::PSSetSampler(unsigned slot, const SamplerState& samplerState)
{
	CWE_ASSERT(slot<D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	mpImmediateContext->PSSetSamplers(slot, 1, &samplerState.mpSamplerState);
}


// コンスタントバッファのセット
void Graphic::VSSetConstantBuffer(unsigned slot, const ConstantBuffer& constantBuffer)
{
	CWE_ASSERT(slot<D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	mpImmediateContext->VSSetConstantBuffers(slot, 1, &constantBuffer.mpConstantBuffer);
}
void Graphic::GSSetConstantBuffer(unsigned slot, const ConstantBuffer& constantBuffer)
{
	CWE_ASSERT(slot<D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	mpImmediateContext->GSSetConstantBuffers(slot, 1, &constantBuffer.mpConstantBuffer);
}
void Graphic::PSSetConstantBuffer(unsigned slot, const ConstantBuffer& constantBuffer)
{
	CWE_ASSERT(slot<D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	mpImmediateContext->PSSetConstantBuffers(slot, 1, &constantBuffer.mpConstantBuffer);
}


// 深度バッファの有効化・無効化
void Graphic::OMSetDepthBuffer(bool enable)
{
	mpImmediateContext->OMSetRenderTargets(1, &mOM.mpRenderTargetView, enable?mOM.mpDepthStencilView:nullptr);
}


// 描画ターゲットをセット
void Graphic::OMSetRenderTarget(const RenderTargetView& renderTargetView)
{
	mpImmediateContext->OMSetRenderTargets(1, &renderTargetView.mpRenderTargetView, mOM.mpDepthStencilView);
}
void Graphic::OMSetRenderTarget(const DepthStencilView& depthStencilView)
{
	mpImmediateContext->OMSetRenderTargets(1, nullptr, depthStencilView.mpDepthStencilView);
}
void Graphic::OMSetRenderTarget(const RenderTargetView& renderTargetView, const DepthStencilView& depthStencilView)
{
	mpImmediateContext->OMSetRenderTargets(1, &renderTargetView.mpRenderTargetView, depthStencilView.mpDepthStencilView);
}

// 描画ターゲットをデフォルトのものにセット
void Graphic::OMSetRenderTargetDefalut()
{
	mpImmediateContext->OMSetRenderTargets(1, &mOM.mpRenderTargetView, mOM.mpDepthStencilView);
}


// IAに頂点バッファを設定
void Graphic::IASetVertexBuffers(unsigned startSlot, unsigned numBuffers, ID3D11Buffer *const *ppVertexBuffers, const unsigned* pStrides, const unsigned *pOffsets)
{
	mpImmediateContext->IASetVertexBuffers(startSlot, numBuffers, ppVertexBuffers, pStrides, pOffsets);
}


// IAにインデックスバッファを設定
void Graphic::IASetIndexBuffer(ID3D11Buffer* pIndexBuffer, unsigned offset)
{
	mpImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
}


// シェーダリソースビューをピクセルシェーダにセット
void Graphic::PSSetShaderResources(unsigned startSlot, unsigned numViews, ID3D11ShaderResourceView *const *ppShaderResourceViews)
{
	mpImmediateContext->PSSetShaderResources(startSlot, numViews, ppShaderResourceViews);
}


// インデックス付き描画
void Graphic::DrawIndexed(unsigned indexCount, unsigned startIndexLocation, int baseVertexLocation)
{
	mpImmediateContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}



}//CWE