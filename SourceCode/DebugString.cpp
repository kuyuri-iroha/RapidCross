#include "DebugString.h"
#include "Information.h"
#include "../CWE/CWEMacroConstants.h"
#include <vector>
#include <string>
#include <cstdarg>


// =============================== DebugOutputCache =====================================

// デバッグ出力機能を有効にするかのプリコンパイル
#define DEBUG_STRING_ENABLED

struct DebugOutputCache
{
	float m_PosX,
		  m_PosY;
	std::string m_String;
};

std::vector<DebugOutputCache> gCache;



// =============================== DebugString ===========================================

// キャッシュのカウンター
unsigned DebugString::smCacheCount =0;


// コンストラクタ
DebugString::DebugString():
	TaskBase(INT_MIN+1, TaskName::DebugString),
	mMagnifX(1.0f /float(CWE_DEFAULT_WINDOW_SIZE_X)), mMagnifY(1.0f /float(CWE_DEFAULT_WINDOW_SIZE_Y))
{
	mUpdate =false;
}


// デストラクタ
DebugString::~DebugString()
{
	End();
}


// 文字に対応したインデックスを取得
int DebugString::GetIndex(char aChar)
{
	switch(aChar)
	{
	case ' ':
		return -1;
		break;

	case '0':
		return 0;
		break;

	case '1':
		return 1;
		break;

	case '2':
		return 2;
		break;

	case '3':
		return 3;
		break;

	case '4':
		return 4;
		break;

	case '5':
		return 5;
		break;

	case '6':
		return 6;
		break;

	case '7':
		return 7;
		break;

	case '8':
		return 8;
		break;

	case '9':
		return 9;
		break;

	case 'a':
		return 10;
		break;

	case 'b':
		return 11;
		break;

	case 'c':
		return 12;
		break;

	case 'd':
		return 13;
		break;

	case 'e':
		return 14;
		break;

	case 'f':
		return 15;
		break;

	case 'g':
		return 16;
		break;

	case 'h':
		return 17;
		break;

	case 'i':
		return 18;
		break;

	case 'j':
		return 19;
		break;

	case 'k':
		return 20;
		break;

	case 'l':
		return 21;
		break;

	case 'm':
		return 22;
		break;

	case 'n':
		return 23;
		break;

	case 'o':
		return 24;
		break;

	case 'p':
		return 25;
		break;

	case 'q':
		return 26;
		break;

	case 'r':
		return 27;
		break;

	case 's':
		return 28;
		break;

	case 't':
		return 29;
		break;

	case 'u':
		return 30;
		break;

	case 'v':
		return 31;
		break;

	case 'w':
		return 32;
		break;

	case 'x':
		return 33;
		break;

	case 'y':
		return 34;
		break;

	case 'z':
		return 35;
		break;

	case '.':
		return 36;
		break;

	default:
		return -1;
		break;
	}
}


#if defined(DEBUG_STRING_ENABLED)

// 出力文字列をキャッシュ
void DebugString::Output(float x, float y, const char* string, ...)
{
	++smCacheCount;

	//キャッシュサイズがあふれたらキャッシュサイズを調整
	if(gCache.size() < smCacheCount) {gCache.resize(smCacheCount);}

	//フォーマット文字列の組み立て
	va_list argl;
	char buf[256] ={'\0'};
	va_start(argl, string);
	vsprintf_s(buf, 256U, string, argl);
	va_end(argl);

	// キャッシュ
	gCache[smCacheCount-1].m_String =buf;
	gCache[smCacheCount-1].m_PosX =x;
	gCache[smCacheCount-1].m_PosY =y;
}

#else

void DebugString::Output(float x, float y, const char* string, ...) {}

#endif


// 更新
void DebugString::Update(TaskExec::List& taskList)
{
}


// 描画
void DebugString::Draw()
{
	using namespace CWE;
	Information& info =Information::GetInfo();
	Graphic& manager =Graphic::GetInstance();

	//シェーダのセット
	info.mUIShader.Begin();

	//定数バッファの有効化
	info.mUIShader.EnableConstantBuffer();

	info.mUIShader.UpdateProjection();

	//キャッシュされている全ての文字列を描画
	int index;
	manager.OMSetDepthBuffer(false);
	for(unsigned i=0; i<smCacheCount; ++i)
	{
		const DebugOutputCache& cache =gCache[i];

		//文字列の位置のセット
		mPos.x() =cache.m_PosX*mMagnifX;
		mPos.y() =cache.m_PosY*mMagnifY;

		//文字列描画
		for(unsigned j=0; j<cache.m_String.size(); ++j)
		{
			//インデックスの取得
			index =GetIndex(cache.m_String[j]);

			if(index != -1) //空白でなければ
			{
				//定数バッファの更新
				math::World2DTp(&info.mUIShader.GetCBData()->mWorld, mPos);
				info.mUIShader.mCB.Map();
				CopyMemory(info.mUIShader.mCB.Access(), info.mUIShader.GetCBData(), sizeof(*info.mUIShader.GetCBData()));
				info.mUIShader.mCB.Unmap();

				//描画
				if(Information::GetInfo().mRuningScene != TaskName::Title) {info.DrawDebugChar(index);}
			}

			//次の文字の位置を算出
			mPos.x() +=0.015f;
		}
	}
	manager.OMSetDepthBuffer(true);

	//キャッシュカウントをリセット
	smCacheCount =0;
}


void DebugString::End()
{
	Information::GetInfo().mUIShader.DisableConstantBuffer();
}