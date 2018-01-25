#include "CWERandomizer.h"


namespace CWE
{



//======================= 乱数生成器 =================================

//コンストラクタ
Randomizer::Randomizer():
mMersenneTwister(mSeed()), mUniDist(1, 100000)
{
}

//デストラクタ
Randomizer::~Randomizer()
{
}

//インスタンスの取得
Randomizer& Randomizer::GetInstance()
{
	static Randomizer instance;
	return instance;
}


//乱数生成器に使う初期シード値のリセット(乱数の精度の向上のため)
void Randomizer::ResetSeed()
{
	mMersenneTwister =std::mt19937(mSeed());
}


//乱数の取得
//(1～100000)
int Randomizer::GetRandom()
{
	return mUniDist(mMersenneTwister);
}



//====================== 一様分布(int) =================================

//コンストラクタ
Uniform_int::Uniform_int():
mRandomizer(Randomizer::GetInstance()), mUniDist(0, 100)
{
}

//引数付きコンストラクタ
Uniform_int::Uniform_int(const int& min, const int& max):
mRandomizer(Randomizer::GetInstance()), mUniDist(min, max)
{
}

//デストラクタ
Uniform_int::~Uniform_int()
{
}


//乱数の取得
int Uniform_int::GetRandom()
{
	return mUniDist(mRandomizer.mMersenneTwister);
}



//====================== 一様分布(double) =================================

//コンストラクタ
Uniform_double::Uniform_double():
mRandomizer(Randomizer::GetInstance()), mUniDist(0.0, 1.0)
{

}

//引数付きコンストラクタ
Uniform_double::Uniform_double(const double& min, const double& max):
mRandomizer(Randomizer::GetInstance()), mUniDist(min, max)
{
}

//デストラクタ
Uniform_double::~Uniform_double()
{
}


//乱数の取得
double Uniform_double::GetRandom()
{
	return mUniDist(mRandomizer.mMersenneTwister);
}



//====================== ベルヌーイ分布 =================================

//コンストラクタ
Bernoulli::Bernoulli():
mRandomizer(Randomizer::GetInstance()), mBernoDist(0.5)
{

}

//引数付きコンストラクタ
Bernoulli::Bernoulli(const double& probability):
mRandomizer(Randomizer::GetInstance()), mBernoDist(probability)
{
}

//デストラクタ
Bernoulli::~Bernoulli()
{
}


//乱数の取得
bool Bernoulli::GetRandom()
{
	return mBernoDist(mRandomizer.mMersenneTwister);
}



}//CWE