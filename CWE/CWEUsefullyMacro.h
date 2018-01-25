#pragma once




//安全なデバイス管理権限の開放
#define SAFE_UNACQUIRE(device) {if((device)){(device)->Unacquire();}}
//安全なリリース
#define SAFE_RELEASE(ptr) {if((ptr)){(ptr)->Release(); (ptr)=nullptr;}}
//安全なデリート
#define SAFE_DELETE(ptr) {if((ptr)){delete(ptr); (ptr)=nullptr;}}
//安全な配列デリート
#define SAFE_DELETE_ARRAY(ptr) {if((ptr)){delete[](ptr); (ptr)=nullptr;}}
//Safe the Destory function.
template<class pT>
void SafeDestroy(pT& p)
{
	if(p){
		p->DestroyVoice();
		p =nullptr;
	}
}

