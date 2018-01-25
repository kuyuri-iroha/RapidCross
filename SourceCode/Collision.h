#pragma once

#include "../CWE/CWEMath.h"



// 点
typedef CWE::math::Float3 Point;
typedef CWE::math::Float3A PointA;

// 線分
struct Segment
{
	Point mP1,
		  mP2;
};

// カプセル
struct Capsule
{
	Segment mSegment;
	float mRadius;
	bool mAggression;
};



// 点と線の最短距離座標
CWE::math::Vector MinDistancePos(const CWE::math::Vector p, const CWE::math::Vector lineP1, const CWE::math::Vector lineP2);


// 点と線分の最短距離の2乗(最短距離座標)
CWE::math::Vector Segment_PointDistanceSqFromVector(const CWE::math::Vector segP1, const CWE::math::Vector segP2, const CWE::math::Vector p, CWE::math::Vector& minPos);
// 点と線分の最短距離の2乗
inline CWE::math::Vector Segment_PointDistanceSqFromVector(const CWE::math::Vector segP1, const CWE::math::Vector segP2, const CWE::math::Vector p)
{
	using namespace CWE::math;

	Vector minPos;
	return Segment_PointDistanceSqFromVector(segP1, segP2, p, minPos);
}


// 2線分間の最短距離の2乗(最短距離座標)
float DistanceSq(const Segment& seg1, const Segment& seg2, Point& seg1Min, Point& seg2Min);
// 2線分間の最短距離の2乗
inline float DistanceSq(const Segment& seg1, const Segment& seg2)
{
	using namespace CWE::math;

	PointA seg1Min{},
		   seg2Min{};
	return DistanceSq(seg1, seg2, seg1Min, seg2Min);
}


// カプセルとカプセルの衝突判定(最短距離座標)
bool Collide(const Capsule& cap1, const Capsule& cap2, Point& cap1Min, Point& cap2Min);
// カプセルとカプセルの衝突判定
inline bool Collide(const Capsule& cap1, const Capsule& cap2)
{
	PointA cap1Min{},
		   cap2Min{};
	return Collide(cap1, cap2, cap1Min, cap2Min);
}