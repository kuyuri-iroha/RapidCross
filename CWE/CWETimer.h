#pragma once

#include <chrono>

namespace CWE
{


//実時間計測機能をマイクロ秒単位で提供するクラス
class Timer
{
private:
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>
		m_Start,
		m_Now;
	std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period>
		m_During;

	long long m_SetTime; //0未満であった場合エラー

	bool m_Started;

public:
	Timer();
	~Timer();

	//カウントがスタートしているか
	inline const bool& GetStarted() {return m_Started;}

	//経過時間を記録
	//(Timerを使用する場合この関数を1ループにつき1回呼び出す)
	//(なお、相当な精度が必要な場合は経過時間取得系関数の呼び出し前に呼び出す)
	void Update();

	//カウントスタート(リスタートも可能)
	void Start();

	//経過時間を取得 (マイクロ秒)
	//1秒 = 1,000,000マイクロ秒
	//(time<0) = エラー
	void GetTime(long long& time);

	//経過時間を取得 (秒)
	//(time<0) = エラー
	void GetTime(double& time);

	//タイマーのセット(long long)
	//true=成功 false=失敗
	bool Set(const long long& setTime);

	//タイマーのセット(double)
	//true=成功 false=失敗
	bool Set(const double& setTime);

	//セットした時間が経過したかどうか
	//true=経過 false=未経過
	bool TimeIsPassed();

};


}//CWE


