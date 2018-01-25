#pragma once

#include "../CWE/CWELib.h"
#include <unordered_map>



// フォント作成クラス
class Font
{
public:
	// 文字セット
	enum class Charset : unsigned
	{
		ANSI =ANSI_CHARSET,
		ShiftJIS =SHIFTJIS_CHARSET
	};
	// アンチエイリアシングのクオリティ
	enum class Quality : unsigned
	{
		Low =GGO_GRAY2_BITMAP,
		Midium =GGO_GRAY4_BITMAP,
		High =GGO_GRAY8_BITMAP
	};

	// 文字テクスチャデータ
	struct TextureData
	{
		ID3D11ShaderResourceView* mpSRV;
		float mWidth,
			  mHeight;
		TextureData() : mpSRV(nullptr), mWidth(0.0f), mHeight(0.0f) {}
		TextureData(const TextureData& texData) : mpSRV(texData.mpSRV), mWidth(texData.mWidth), mHeight(texData.mHeight) {}
		~TextureData() {SAFE_RELEASE(mpSRV);}
		TextureData& operator=(const TextureData& texData) {mpSRV=texData.mpSRV; mWidth=texData.mWidth; mHeight=texData.mHeight;}
	};

private:
	std::unordered_map<wchar_t, TextureData> mTexture;
	bool mEnabled;
	CWE::Task mStatus;

	bool CreateTextureChar(wchar_t aChar, TextureData& data, HDC& hdc, Quality quality, CWE::Graphic& manager);
	bool CreateTextureANSI(HDC& hdc, Quality quality);
	bool CreateTextureShiftJIS();

public:
	Font();
	~Font();

	// フォントの作成
	bool Create(int size, int weight, Quality quality, bool italic, bool underline, bool strikeOut, Charset charset, const wchar_t* name);
	// フォントの作成(非同期処理用)
	static void Create(Font& thisData, int size, int weight, Quality quality, bool italic, bool underline, bool strikeOut, Charset charset, const wchar_t* name);
	// テクスチャデータの取得
	const TextureData* const Get(wchar_t wChar) const;
	// 使用可能かを取得
	inline bool Enabled() const {return mEnabled;}
	// 状態を取得
	inline const CWE::Task& Status() const {return mStatus;}
};