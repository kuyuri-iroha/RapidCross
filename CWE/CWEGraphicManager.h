#pragma once

#include "CWEIncludedAPI.h"
#include "CWEMath.h"
#include "CWEMacroConstants.h"
#include "CWEUsefullyMacro.h"
#include "CWEWindows.h"
#include "CWEError.h"
#include <string>


namespace CWE
{
// 前方宣言
class Graphic;

namespace callbacks//使用禁止
{
LRESULT CALLBACK CWE_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}


//前方宣言
class Viewport;
class Resource;



//グラフィックの品質
enum class Quality
{
	Low =1,
	Middle =0,
	High =2,
	Maximum =3
};


// 入力エレメント用のデータ分類
enum class InputClassification
{
	VertexData =0,
	InstanceData =1
};

// 使用するラスタライザステート選択用
enum class RSState
{
	Normal =0,
	NonCull =1,
	WireFrame =2
};


// 使用するサンプラステートの選択用
enum class SamplerStatus
{
	Liner =0, //線形補間
	Point =1, //補間なし
};





// フォーマット
typedef DXGI_FORMAT Format;

// 前方宣言
struct IAAndVS;
struct GS;
struct PS;

// シェーダマクロ定義
struct ShaderMacro
{
private:
	friend struct IAAndVS;
	friend struct GS;
	friend struct PS;
	D3D_SHADER_MACRO mShaderMacro;

public:
	ShaderMacro(): mShaderMacro() {}
	~ShaderMacro() {}
	ShaderMacro(const char* name, const char* definition);

	void Set(const char* name, const char* definition);
};


// 入力エレメント
struct IAInputElement
{
private:
	friend struct IAAndVS;
	D3D11_INPUT_ELEMENT_DESC mIE;

public:
	IAInputElement():mIE() {}
	~IAInputElement() {}
	IAInputElement(const char* semanticName, unsigned semanticIndex, const Format& format, unsigned inputSlot, unsigned alignedByteOffset, InputClassification inputSlotClass, unsigned instanceDataStepRate);

	void Set(const char* semanticName, unsigned semanticIndex, const Format& format, unsigned inputSlot, unsigned alignedByteOffset, InputClassification inputSlotClass, unsigned instanceDataStepRate);
};


// 入力レイアウト&頂点シェーダオブジェクト
struct IAAndVS
{
private:
	friend class CWE::Graphic;
	ID3D11InputLayout* mpInputLayout;
	ID3D11VertexShader* mpVertexShader;

public:
	IAAndVS(): mpInputLayout(nullptr), mpVertexShader(nullptr) {}
	~IAAndVS() {SAFE_RELEASE(mpInputLayout); SAFE_RELEASE(mpVertexShader);}

	// 入力レイアウト&頂点シェーダオブジェクトの作成
	// (shaderMacroNumの指定をミスすると不正アクセス例外が発生するので注意)
	bool Create(const wchar_t* shaderFile, const ShaderMacro* pShaderMacros, const char* functionName, const IAInputElement* pInputElements, unsigned numElements);

};


// ジオメトリシェーダオブジェクト
struct GS
{
private:
	friend class CWE::Graphic;
	ID3D11GeometryShader* mpGeometryShader;

public:
	GS(): mpGeometryShader(nullptr) {}
	~GS() {SAFE_RELEASE(mpGeometryShader);}

	// ジオメトリシェーダオブジェクトの作成
	bool Create(const wchar_t* shaderFile, const ShaderMacro* pShaderMacros, const char* functionName);

};


// ピクセルシェーダオブジェクト
struct PS
{
private:
	friend class CWE::Graphic;
	ID3D11PixelShader* mpPixelShader;

public:
	PS(): mpPixelShader(nullptr) {}
	~PS() {SAFE_RELEASE(mpPixelShader);}

	// ピクセルシェーダオブジェクトの作成
	bool Create(const wchar_t* shaderFile, const ShaderMacro* pShaderMacros, const char* functionName);

};



// サンプラーステートオブジェクト
struct SamplerState
{
private:
	friend class CWE::Graphic;
	ID3D11SamplerState* mpSamplerState;

public:
	SamplerState(): mpSamplerState(nullptr) {}
	~SamplerState() {SAFE_RELEASE(mpSamplerState);}

	// プリセットから作成
	bool Create(const SamplerStatus& preset);
	// シャドウマップ用の作成
	bool CreateForShadow();
};


// コンスタントバッファオブジェクト
struct ConstantBuffer
{
private:
	friend class CWE::Graphic;
	ID3D11Buffer* mpConstantBuffer;
	D3D11_MAPPED_SUBRESOURCE mResource;
	bool mMapped;

public:
	ConstantBuffer(): mpConstantBuffer(nullptr) {}
	~ConstantBuffer() {SAFE_RELEASE(mpConstantBuffer);}

	// コンスタントバッファの作成
	bool Create(size_t bufferSize);

	// 書き込み開始
	bool Map();
	//書き込み終了
	void Unmap();
	//サブリソースへの書き込み
	void* const Access();

};


// レンダーターゲットビューオブジェクト
struct RenderTargetView
{
private:
	friend class CWE::Graphic;
	ID3D11RenderTargetView* mpRenderTargetView;

public:
	RenderTargetView(): mpRenderTargetView(nullptr) {}
	~RenderTargetView() {SAFE_RELEASE(mpRenderTargetView);}

	// 作成
	template<typename T_Resource>
	bool Create(T_Resource& resource)
	{
		if(FAILED( Graphic::GetInstance().Device().CreateRenderTargetView(resource.GetResource(), nullptr, &mpRenderTargetView) ))
		{
			return false;
		}

		return true;
	}

	// クリア
	void Clear();
};



// 深度/ステンシルビュー記述オブジェクト
typedef D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;


// 深度/ステンシルビューオブジェクト
struct DepthStencilView
{
private:
	friend class CWE::Graphic;
	ID3D11DepthStencilView* mpDepthStencilView;

public:
	DepthStencilView(): mpDepthStencilView(nullptr) {}
	~DepthStencilView() {SAFE_RELEASE(mpDepthStencilView);}

	// 作成
	template<typename T_Resource>
	bool Create(T_Resource& resource, const DepthStencilViewDesc& desc)
	{
		if(FAILED( Graphic::GetInstance().Device().CreateDepthStencilView(resource.GetResource(), &desc, &mpDepthStencilView) ))
		{
			return false;
		}

		return true;
	}
	// クリア
	void Clear();

};


// シェーダリソースビュー
struct ShaderResourceView
{
private:
	ID3D11ShaderResourceView* mpShaderResourceView;

public:
	ShaderResourceView(): mpShaderResourceView(nullptr) {}
	~ShaderResourceView() {}

	//作成
	template<typename T_Resource>
	bool Create(T_Resource& resource)
	{
		if(FAILED( Graphic::GetInstance().Device().CreateShaderResourceView(resource.GetResource(), nullptr, &mpShaderResourceView) ))
		{
			return false;
		}
		return true;
	}
};


// 2Dテクスチャ記述オブジェクト
typedef D3D11_TEXTURE2D_DESC Texture2DDesc;


// 2Dテクスチャオブジェクト
struct Texture2D
{
private:
	friend struct RenderTargetView;
	friend struct DepthStencilView;
	friend struct ShaderResourceView;
	ID3D11Texture2D* mpTexture;
	inline ID3D11Texture2D* GetResource() {return mpTexture;}

public:
	Texture2D(): mpTexture(nullptr) {}
	~Texture2D() {SAFE_RELEASE(mpTexture);}

	// テクスチャの作成
	bool Create(const Texture2DDesc& desc);
	inline const ID3D11Texture2D* const GetResource() const {return mpTexture;}
};





//描画系管理クラス
class Graphic
{
private:
	//フレンド
	friend bool Init(unsigned width, unsigned height, bool windowMode, const wchar_t windowTitle[128]);
	friend bool Update();
	friend void End();
	friend LRESULT CALLBACK CWE::callbacks::CWE_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	friend bool GraphicInitialized();
	friend class CWE::Viewport;
	friend class CWE::Resource;
	friend struct CWE::IAAndVS;
	friend struct CWE::GS;
	friend struct CWE::PS;
	friend struct CWE::SamplerState;
	friend struct CWE::ConstantBuffer;



	//ビューポート初期化用デフォルト値
	const int NearZ =1,
			  FarZ  =1000;



	//インターフェイス
	IDXGIFactory* mpFactory; //DXGI
	ID3D11Device* mpDevice; //デバイス
	ID3D11DeviceContext* mpImmediateContext; //デバイス・コンテキスト
	IDXGISwapChain* mpSwapChain; //スワップ・チェイン

	//======= 描画パイプライン ==========

	// ラスタライザ
	struct Rasterizer
	{
		//ラスタライザステート
		ID3D11RasterizerState* mpRasterizerState[3];
	} mRS;

	// 出力マージャー
	struct OutputMerger
	{
		//ブレンドステート
		ID3D11BlendState* mpBlendState;
		//深度/ステンシルステート
		ID3D11DepthStencilState* mpDepthStencilState;
		//深度/ステンシル ビュー
		ID3D11DepthStencilView* mpDepthStencilView;
		//通常スクリーン用描画ターゲットビュー
		ID3D11RenderTargetView* mpRenderTargetView;
	}mOM;
	//描画時のフォーマット
	DXGI_FORMAT mFormat[2];
	DXGI_SAMPLE_DESC mMSAAQuality;
	unsigned mSyncInterval;


	//初期化
	bool Init();
	//終了処理
	void End();
	//更新処理
	void Update();

	//デバイスとスワップチェインの作成
	bool CreateDeviceAndSwapChain();
	//ブレンドステートの作成
	bool CreateBlendState();
	//ラスタライザステートの作成
	bool CreateRasterizerState();
	//深度/ステンシルステートの作成
	bool CreateDepthStencilState();
	//描画パイプラインの初期化
	void InitRenderingPipeline();

	//デバイスの消失処理
	void ChackDeviceLost();

	//バックバッファーのリセット
	//true=成功 false=失敗
	bool ResetBackBuffer();

	//画面サイズの変更に伴う描画パイプラインの復元
	void RenderingPipelineRestoration();

	//Windowsコールバック関数用の描画領域変更関数
	void InnerResetWindowSize();
	//フルスクリーンモードを正しい状態に戻す
	void InnerWindowModeStateRestoration();


	Graphic();
	~Graphic();
	Graphic(const Graphic&) =delete;
	Graphic& operator=(const Graphic&) =delete;

public:
	//シングルトンの実装
	static Graphic& GetInstance();

	// Windows情報
	Windows mWindows;

	// デバイスが有効かどうか
	inline bool CheckDeviceIs() {return mpDevice?true:false;}
	// 外部からのデバイスの使用
	inline ID3D11Device& Device() {CWE_ASSERT(CheckDeviceIs()); return *mpDevice;}
	// 外部からのデバイスコンテキストの使用
	inline ID3D11DeviceContext& DeviceContext() {CWE_ASSERT(CheckDeviceIs()); return *mpImmediateContext;}

	//各シェーダのファイルパスのセット
	void VSSetShader(const IAAndVS& shader);
	void GSSetShader(const GS& shader);
	void PSSetShader(const PS& shader);

	// 各シェーダのリムーブ
	void VSRemoveShader();
	void GSRemoveShader();
	void PSRemoveShader();

	//ウィンドウモードのリセット
	//true=ウィンドウモード false=フルスクリーンモード
	void ResetWindowMode(bool windowMode);

	//ディスプレイモードのリセット
	//返り値: true=成功 false=失敗
	bool ResetDisplayMode(unsigned width, unsigned height/*, const unsigned& refreshRate*/);

	//Set the sync interval.
	//inter : FPS = Refresh rate / inter
	void SetSyncInterval(unsigned inter);

	// 使用するラスタライザステートのリセット
	void RSResetUseState(const RSState& useState);

	// サンプラーステートのセット
	void VSSetSampler(unsigned slot, const SamplerState& samplerState);
	void GSSetSampler(unsigned slot, const SamplerState& samplerState);
	void PSSetSampler(unsigned slot, const SamplerState& samplerState);

	// 定数バッファのセット
	void VSSetConstantBuffer(unsigned slot, const ConstantBuffer& constantBuffer);
	void GSSetConstantBuffer(unsigned slot, const ConstantBuffer& constantBuffer);
	void PSSetConstantBuffer(unsigned slot, const ConstantBuffer& constantBuffer);

	// 深度バッファの有効化・無効化
	void OMSetDepthBuffer(bool enable);
	// 描画ターゲットをセット
	void OMSetRenderTarget(const RenderTargetView& renderTargetView);
	void OMSetRenderTarget(const DepthStencilView& depthStencilView);
	void OMSetRenderTarget(const RenderTargetView& renderTargetView, const DepthStencilView& depthStencilView);
	// 描画ターゲットをデフォルトのものにセット
	void OMSetRenderTargetDefalut();

	// IAに頂点バッファを設定
	void IASetVertexBuffers(unsigned startSlot, unsigned numBuffers, ID3D11Buffer *const *ppVertexBuffers, const unsigned* pStrides, const unsigned *pOffsets);
	// IAにインデックスバッファを設定
	void IASetIndexBuffer(ID3D11Buffer* pIndexBuffer, unsigned offset);

	// シェーダリソースビューをピクセルシェーダにセット
	void PSSetShaderResources(unsigned startSlot, unsigned numViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);

	// インデックス付き描画
	void DrawIndexed(unsigned indexCount, unsigned startIndexLocation, int baseVertexLocation);
};



}//CWE

