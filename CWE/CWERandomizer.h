#pragma once

#include <random>


namespace CWE
{


//乱数生成器
class Randomizer
{
private:
	friend class Uniform_int;
	friend class Uniform_double;
	friend class Bernoulli;

	std::random_device mSeed;
	std::mt19937 mMersenneTwister;
	std::uniform_int_distribution<> mUniDist;


	Randomizer();
	~Randomizer();

public:
	//インスタンスの取得
	static Randomizer& GetInstance();
	Randomizer(const Randomizer&) =delete;
	Randomizer& operator=(const Randomizer&) =delete;

	//乱数生成器に使う初期シード値のリセット(乱数の精度の向上のため)
	void ResetSeed();

	//乱数の取得
	//(1～100000)
	int GetRandom();

};



//一様分布乱数(int)
class Uniform_int
{
private:
	Randomizer& mRandomizer;
	std::uniform_int_distribution<> mUniDist;

public:
	//デフォルトは0～100
	Uniform_int();
	Uniform_int(const int& min, const int& max);
	~Uniform_int();

	//乱数の取得
	int GetRandom();

};


//一様分布乱数(double)
class Uniform_double
{
private:
	Randomizer& mRandomizer;
	std::uniform_real_distribution<> mUniDist;

public:
	//デフォルトは0.0～1.0
	Uniform_double();
	Uniform_double(const double& min, const double& max);
	~Uniform_double();

	//乱数の取得
	double GetRandom();

};


//ベルヌーイ分布乱数
class Bernoulli
{
private:
	Randomizer& mRandomizer;
	std::bernoulli_distribution mBernoDist;

public:
	//デフォルトは0.5(50%)
	Bernoulli();
	//probability=trueになる確率
	//(0.5=50%)
	Bernoulli(const double& probability);
	~Bernoulli();

	//乱数の取得
	bool GetRandom();

};



}//CWE