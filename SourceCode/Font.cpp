#include "Font.h"




// コンストラクタ
Font::Font():
	mEnabled(false), mStatus(CWE::Task::Working)
{

}

// デストラクタ
Font::~Font()
{

}


// 一文字単位のテクスチャ作成
bool Font::CreateTextureChar(wchar_t aChar, TextureData& data, HDC& hdc, Quality quality, CWE::Graphic& manager)
{
	unsigned charCode =0;
	TEXTMETRIC tm;
	GLYPHMETRICS gm;
	const MAT2 m ={
		{0, 1}, {0, 0}, {0, 0}, {0, 1}
	};
	ID3D11Texture2D* pTexture =nullptr;

	//フォントビットマップ作成
	charCode =static_cast<unsigned>(aChar);
	GetTextMetricsW(hdc, &tm);
	unsigned memSize =GetGlyphOutlineW(hdc, charCode, static_cast<unsigned>(quality), &gm, NULL, NULL, &m);
	unsigned char* pBitmap =new unsigned char[memSize];
	GetGlyphOutlineW(hdc, charCode, static_cast<unsigned>(quality), &gm, memSize, pBitmap, &m);

	//テクスチャの作成
	CD3D11_TEXTURE2D_DESC desc(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 
		gm.gmCellIncX, tm.tmHeight, 1, 1, 
		D3D11_BIND_SHADER_RESOURCE, 
		D3D11_USAGE_DYNAMIC, 
		D3D11_CPU_ACCESS_WRITE
		);
	if(FAILED(manager.Device().CreateTexture2D(&desc, 0, &pTexture)))
	{
		return false;
	}

	data.mWidth =float(desc.Width);
	data.mHeight =float(desc.Height);

	//テクスチャをマップ
	D3D11_MAPPED_SUBRESOURCE mapped;
	if(FAILED(manager.DeviceContext().Map(pTexture, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	unsigned char* pBits =(unsigned char*)mapped.pData;

	//ビットマップをテクスチャに書き込み
	int offsetX =gm.gmptGlyphOrigin.x,
		offsetY =tm.tmAscent -gm.gmptGlyphOrigin.y,
		width =gm.gmBlackBoxX +(4 -(gm.gmBlackBoxX %4)) %4,
		height =gm.gmBlackBoxY;
	int	level =0;
	int x=0, y=0;
	unsigned alpha=0, color=0;

	switch(quality)
	{
	case Quality::Low:
		level =5;
		break;

	case Quality::Midium:
		level =17;
		break;

	case Quality::High:
		level =65;
		break;

	default:
		CWE_ASSERT(false);
		break;
	}
	memset(pBits, 0, mapped.RowPitch*tm.tmHeight);

	for(y=offsetY; y<offsetY+height; ++y)
	{
		for(x=offsetX; x<offsetX+width; ++x)
		{
			alpha =(255 *pBitmap[x -offsetX +width *(y -offsetY)]) /(level -1);
			color =0x00ffffff | (alpha <<24);
			CopyMemory((unsigned char*)pBits +mapped.RowPitch *y +4 *x, &color, sizeof(unsigned));
		}
	}

	// テクスチャをアンマップ
	manager.DeviceContext().Unmap(pTexture, D3D11CalcSubresource(0, 0, 1));

	//テクスチャへの書き込み終了
	SAFE_DELETE_ARRAY(pBitmap);

	//レンダーターゲットビューを作成
	if(FAILED(manager.Device().CreateShaderResourceView(pTexture, NULL, &data.mpSRV)))
	{
		SAFE_RELEASE(pTexture);
		return false;
	}

	//テクスチャを手放す
	SAFE_RELEASE(pTexture);

	return true;
}


// ANSI(英語)のフォントテクスチャを作成
bool Font::CreateTextureANSI(HDC& hdc, Quality quality)
{
	CWE::Graphic& manager =CWE::Graphic::GetInstance();
	TextureData* data;
	wchar_t aChar =L'!';
	for(unsigned i=unsigned(aChar); i<=126; ++i)
	{
		data =&mTexture[aChar];
		if(!CreateTextureChar(aChar, *data, hdc, quality, manager))
		{
			return false;
		}
		++aChar;
	}

	return true;
}


// ShiftJIS(日本語)のフォントテクスチャを作成
bool Font::CreateTextureShiftJIS()
{
	CWE_ASSERT(false); //非対応
	return false;
}


// フォントの作成
bool Font::Create(int size, int weight, Quality quality, bool italic, bool underline, bool strikeOut, Charset charset, const wchar_t* name)
{
	if(mEnabled) {return false;} //作成済みであれば失敗

	bool result =true;

	HFONT font =CreateFontW(
		size, 0, 0, 0, weight,
		italic ? TRUE : FALSE,
		underline ? TRUE : FALSE,
		strikeOut ? TRUE : FALSE,
		static_cast<unsigned>(charset),
		OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, (FIXED_PITCH|FF_MODERN),
		name
		);

	HDC context =GetDC(NULL);
	HFONT oldFont =HFONT(SelectObject(context, font));

	//指定された文字セットによるフォントテクスチャの作成
	switch(charset)
	{
	case Charset::ANSI:
		result =CreateTextureANSI(context, quality);
		break;

	case Charset::ShiftJIS:
		result =CreateTextureShiftJIS();
		break;

	default:
		CWE_ASSERT(false);
		break;
	}

	//デバイスコンテキストとフォントハンドルの開放
	SelectObject(context, oldFont);
	DeleteObject(font );
	ReleaseDC(NULL, context);

	mEnabled =result;
	mStatus =result ? CWE::Task::Success : CWE::Task::Failed;

	return result;
}

// フォントの作成(非同期処理用)
void Font::Create(Font& thisData, int size, int weight, Quality quality, bool italic, bool underline, bool strikeOut, Charset charset, const wchar_t* name)
{
	thisData.mStatus =CWE::Task::Working;

	if(thisData.Create(size, weight, quality, italic, underline, strikeOut, charset, name))
	{
		thisData.mStatus =CWE::Task::Success;
	}
	else
	{
		thisData.mStatus =CWE::Task::Failed;
	}
}


// テクスチャデータの取得
const Font::TextureData* const Font::Get(wchar_t aChar) const
{
	if(mTexture.count(aChar) == 0 || !mEnabled)
	{
		return nullptr;
	}

	return &mTexture.at(aChar);
}