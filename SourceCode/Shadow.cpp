#include "Shadow.h"
#include "Information.h"



// ===================================== ShadowMap =========================================

// コンストラクタ
ShadowMap::ShadowMap()
{

}


// デストラクタ
ShadowMap::~ShadowMap()
{
	SAFE_RELEASE(mpSRV);
	SAFE_RELEASE(mpDSV);
	SAFE_RELEASE(mpTexture);
}


// 生成
bool ShadowMap::Create()
{
	CWE::Graphic& manager =CWE::Graphic::GetInstance();
#define QUALITY 7000

	//テクスチャの作成
	D3D11_TEXTURE2D_DESC desc;
	desc.Width =QUALITY;
	desc.Height =QUALITY;
	desc.MipLevels =1;
	desc.ArraySize =1;
	desc.Format =DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count =1;
	desc.SampleDesc.Quality =0;
	desc.Usage =D3D11_USAGE_DEFAULT;
	desc.BindFlags =D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags =0;
	desc.MiscFlags =0;
	if(FAILED(manager.Device().CreateTexture2D(&desc, nullptr, &mpTexture)))
	{
		return false;
	}

	//デプスステンシルビューの作成
	CWE::DepthStencilViewDesc dsvDesc;
	dsvDesc.Format =DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension =D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags =0;
	dsvDesc.Texture2D.MipSlice =0;
	if(FAILED(manager.Device().CreateDepthStencilView(mpTexture, &dsvDesc, &mpDSV)))
	{
		return false;
	}

	//シェーダリソースビューの作成
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT, 0, 1, 0, 1);
	if(FAILED(manager.Device().CreateShaderResourceView(mpTexture, &srvDesc, &mpSRV)))
	{
		return false;
	}

	//ビューポートの設定
	mViewPort.Width =QUALITY;
	mViewPort.Height =QUALITY;
	mViewPort.TopLeftX =0.0f;
	mViewPort.TopLeftY =0.0f;
	mViewPort.MinDepth =0.0f;
	mViewPort.MaxDepth =1.0f;

	return true;
}


// パスの開始
void ShadowMap::Begin()
{
	using namespace CWE;
	Graphic& manager =Graphic::GetInstance();

	//デプスステンシルをクリア
	manager.DeviceContext().ClearDepthStencilView(mpDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//シャドウマップへの書き込みを指定
	manager.DeviceContext().OMSetRenderTargets(0, nullptr, mpDSV);
	//シャドウマップのビューポートをセット
	manager.DeviceContext().RSSetViewports(1, &mViewPort);
}


// パスの終了
void ShadowMap::End()
{
	CWE::Graphic::GetInstance().OMSetRenderTargetDefalut();
}



// ============================ ShadowMapDraw =====================================


// コンストラクタ
ShadowMapDraw::ShadowMapDraw()
{
	for(ID3D11Buffer*& p : mpVertexBuffers)
	{
		p =nullptr;
	}
}


// デストラクタ
ShadowMapDraw::~ShadowMapDraw()
{
	for(ID3D11Buffer*& p : mpVertexBuffers)
	{
		SAFE_RELEASE(p);
	}
}


// 板ポリを生成
bool ShadowMapDraw::CreatePolygon(float width, float height)
{
	using namespace CWE;
	Graphic& manager =Graphic::GetInstance();

	//頂点バッファの作成
	D3D11_BUFFER_DESC vertexDesc;
	vertexDesc.Usage =D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth =sizeof(math::Float3) *4;
	vertexDesc.BindFlags =D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags =0;
	vertexDesc.MiscFlags =0;
	vertexDesc.StructureByteStride =0;
	float magnifX =0.5f /manager.mWindows.GetWindowSizeX(),
		  magnifY =0.5f /manager.mWindows.GetWindowSizeY();
	math::Float3 vertices[4] ={
		{-float(width)*magnifX, float(height)*magnifY, 0.0f}, {float(width)*magnifX, float(height)*magnifY, 0.0f},
		{-float(width)*magnifX, -float(height)*magnifY, 0.0f}, {float(width)*magnifX, -float(height)*magnifY, 0.0f},
	};
	D3D11_SUBRESOURCE_DATA vertexBufferSub;
	vertexBufferSub.pSysMem =&vertices[0];
	vertexBufferSub.SysMemPitch =0;
	vertexBufferSub.SysMemSlicePitch =0;
	if(FAILED(manager.Device().CreateBuffer(&vertexDesc, &vertexBufferSub, &mpVertexBuffers[0])))
	{
		return false;
	}

	//UV座標の作成
	vertexDesc.ByteWidth =sizeof(math::Float2)*4;
	math::Float2 uv[4] ={
		{0.0f, 0.0f}, {1.0f, 0.0f},
		{0.0f, 1.0f}, {1.0f, 1.0f}
	};
	vertexBufferSub.pSysMem =&uv[0];
	if(FAILED(manager.Device().CreateBuffer(&vertexDesc, &vertexBufferSub, &mpVertexBuffers[1])))
	{
		return false;
	}

	//インデックスバッファの作成
	D3D11_BUFFER_DESC indexBuffer;
	indexBuffer.Usage =D3D11_USAGE_DEFAULT;
	indexBuffer.ByteWidth =sizeof(unsigned) *6;
	indexBuffer.BindFlags =D3D11_BIND_INDEX_BUFFER;
	indexBuffer.CPUAccessFlags =0;
	indexBuffer.MiscFlags =0;
	indexBuffer.StructureByteStride =0;
	unsigned indices[6] ={
		0, 1, 2,
		2, 1, 3,
	};
	D3D11_SUBRESOURCE_DATA indexBufferSub;
	indexBufferSub.pSysMem =&indices[0];
	indexBufferSub.SysMemPitch =0;
	indexBufferSub.SysMemSlicePitch =0;
	if(FAILED(manager.Device().CreateBuffer(&indexBuffer, &indexBufferSub, &mpVertexBuffers[2])))
	{
		return false;
	}

	return true;
}


void ShadowMapDraw::Draw(unsigned texSlot, ID3D11ShaderResourceView* pSRV) const
{
	using namespace CWE;
	Graphic& manager =Graphic::GetInstance();

	manager.RSResetUseState(RSState::Normal);
	const unsigned stride[] ={sizeof(math::Float3), sizeof(math::Float2)};
	const unsigned offset[] ={0, 0};
	manager.IASetVertexBuffers(0, 2, mpVertexBuffers, stride, offset);
	manager.IASetIndexBuffer(mpVertexBuffers[2], 0);
	manager.DeviceContext().PSSetShaderResources(texSlot, 1, &pSRV);

	// メッシュを描画
	manager.DrawIndexed(6, 0, 0);
}