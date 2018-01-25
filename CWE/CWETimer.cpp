#include "CWETimer.h"


namespace CWE
{


//コンストラクタ
Timer::Timer():
m_SetTime(-1), m_Started(false)
{

}

//デストラクタ
Timer::~Timer()
{

}


//経過時間を記録
//(CTimerを使用する場合この関数を1ループにつき1回呼び出す)
void Timer::Update()
{
	m_Now =std::chrono::system_clock::now();
	m_During =m_Now -m_Start;
}


//カウントスタート
void Timer::Start()
{
	m_Start =std::chrono::system_clock::now();
	m_Started =true;
}


//経過時間を取得 (マイクロ秒)
//1秒 = 1,000,000マイクロ秒
//(time<0) = エラー
void Timer::GetTime(long long& time)
{
	if(!m_Started) {time =-1; return;}
	time =std::chrono::duration_cast<std::chrono::microseconds>(m_During).count();
}


//経過時間を取得 (秒)
//(time<0) = エラー
void Timer::GetTime(double& time)
{
	if(!m_Started) {time =-1; return;}
	time =static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(m_During).count())/1000000;
}


//経過時間判定の設定
//true=成功 false=失敗
bool Timer::Set(const long long& setTime)
{
	if(setTime<0) {m_SetTime =-1; return false;}

	m_SetTime =setTime;
	return true;
}


//経過時間判定の設定(double)
//true=成功 false=失敗
bool Timer::Set(const double& setTime)
{
	if(setTime<0) {m_SetTime =-1; return false;}

	m_SetTime =static_cast<long long>(setTime*1000000);
	return true;
}


//セットした時間が経過したかどうか
//true=経過 false=未経過
bool Timer::TimeIsPassed()
{
	if((!m_Started) | (m_SetTime == -1)) {return false;}

	if(m_SetTime <= std::chrono::duration_cast<std::chrono::microseconds>(m_During).count())
	{
		return true;
	}
	return false;
}


}//CWE