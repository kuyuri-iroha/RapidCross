#pragma once

#include "../CWE/CWELib.h"


// シャドウマップの実装
class ShadowMap
{
private:
	ID3D11Texture2D* mpTexture;
	ID3D11DepthStencilView* mpDSV;
	ID3D11ShaderResourceView* mpSRV;
	CD3D11_VIEWPORT mViewPort;
	
public:
	ShadowMap();
	~ShadowMap();

	inline static void GetProjection(CWE::math::Float4x4& projection) {CWE::math::OrthographicLHTp(&projection, 950.0f, 950.0f, 100.0f, 1000.0f);}

	bool Create();
	void Begin();
	void End();

	ID3D11ShaderResourceView* GetSRV() {return mpSRV;}

};


// シャドウマップ描画用クラス
class ShadowMapDraw
{
private:
	ID3D11Buffer* mpVertexBuffers[3];

public:
	ShadowMapDraw();
	~ShadowMapDraw();

	bool CreatePolygon(float width, float height);
	void Draw(unsigned texSlot, ID3D11ShaderResourceView* pSRV) const;
};