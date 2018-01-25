#include "CWEStatic.h"
#include "CWEGraphicManager.h"


namespace CWE
{


//マウスの表示をセット
bool ResetMouseVisible(const bool& visible)
{
	if(visible == false)
	{
		while(ShowCursor(FALSE) > -1 ){};
	}
	else
	{
		while(ShowCursor(TRUE) < 0 ){};
	}

	return visible;
}


// グラフィックデバイスが初期化済みかを確認
bool GraphicInitialized()
{
	return Graphic::GetInstance().mpDevice != nullptr;
}


}//CWE