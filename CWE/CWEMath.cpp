#include "CWEMath.h"
#include "CWEViewPort.h"



namespace CWE
{
namespace math
{

// ================= 定数定義 ============================

// ジンバルロック回避用X軸制限
//(要デバッグ)
static float AvoidGimbalLockByPalamX =ConvertToRadians(85.0f);



// ================= 算術型関数 ==========================

// Float4x4
bool Float4x4::operator== (const Float4x4& float4x4)
{
	return (mRow[0]==float4x4.mRow[0] &&
		mRow[1]==float4x4.mRow[1] &&
		mRow[2]==float4x4.mRow[2] &&
		mRow[3]==float4x4.mRow[3]);
}
bool Float4x4::operator!= (const Float4x4& float4x4)
{
	return !(*this == float4x4);
}




//**********************************************************************************
// ================= Vector & Matrix 版算術関数 ====================================
//**********************************************************************************

// ================= ベクトル系関数 =====================

// スカラー倍
Vector VectorScale(const Vector v, float scale)
{
	Vector result =VectorSet(VectorGetX(v) *scale, VectorGetY(v) *scale, VectorGetZ(v) *scale, VectorGetW(v) *scale);
	return result;
}


// ================= その他の算術関数 ===================

// 回転クォータニオンからオイラー角を算出
void RollPitchYawFromQuaternion(float& pitch, float& yaw, float& roll, const Vector qRotation)
{
	Matrix mRotation =MatrixRotationQuaternion(qRotation);

	//ピッチ(X軸回転)
	pitch =ASin(-mRotation(2, 1));
	//ジンバルロック回避
	if(AvoidGimbalLockByPalamX < fabsf(pitch))
	{
		pitch =(pitch<0)?-AvoidGimbalLockByPalamX : AvoidGimbalLockByPalamX;
	}
	float cosineX =Cos(pitch);
	//ヨー(Y軸回転)
	yaw =ATan((mRotation(2, 0)/cosineX), (mRotation(2, 2)/cosineX));
	//ロール(Z軸回転)
	roll =ATan((mRotation(0, 1)/cosineX), (mRotation(1, 1)/cosineX));
	//使える角度に丸める
	pitch =ModAngle(pitch);
	yaw =ModAngle(yaw);
	roll =ModAngle(roll);
}


// 回転行列からオイラー角を算出
void RollPitchYawFromMatrix(float& pitch, float& yaw, float& roll, const Matrix& mRotation)
{
	//ピッチ(X軸回転)
	pitch =ASin(-mRotation(2, 1));
	//ジンバルロック回避
	if(AvoidGimbalLockByPalamX < fabsf(pitch))
	{
		pitch =(pitch<0)?-AvoidGimbalLockByPalamX : AvoidGimbalLockByPalamX;
	}
	float cosineX =Cos(pitch);
	//ヨー(Y軸回転)
	yaw =ATan((mRotation(2, 0)/cosineX), (mRotation(2, 2)/cosineX));
	//ロール(Z軸回転)
	roll =ATan((mRotation(0, 1)/cosineX), (mRotation(1, 1)/cosineX));
	//使える角度に丸める
	pitch =ModAngle(pitch);
	yaw =ModAngle(yaw);
	roll =ModAngle(roll);
}



//**********************************************************************************
// ================= 算術関数 ======================================================
//**********************************************************************************

// ============== ベクトル系関数 ========================

// スカラー倍
void VectorScale(Float3* result, const Float3& v, float scale)
{
	CWE_ASSERT(result);
	result->x() =v.x() *scale;
	result->y() =v.y() *scale;
	result->z() =v.z() *scale;
}


// 2点間の距離
float VectorDistance(const Float3& v1, const Float3& v2)
{
	return XMVectorGetX(
				XMVector3Length(
					LoadVector(v2)-LoadVector(v1)
					)
			);
}


// 2点間の距離の2乗
float VectorDistanceSq(const Float3& v1, const Float3& v2)
{
	return XMVectorGetX(
				XMVector3LengthSq(
					LoadVector(v2)-LoadVector(v1)
					)
			);
}


// 2つの正規化済みベクトル間のラジアン角(なす角)
float VectorAngleBetweenNormals(const Float3& v1, const Float3& v2)
{
	return XMVectorGetX(
				XMVector3AngleBetweenNormals(
					LoadVector(v1), LoadVector(v2)
					)
			);
}


// 外積
void VectorCross(Float3* result, const Float3& v1, const Float3& v2)
{
	StoreVector(result, XMVector3Cross(LoadVector(v1), LoadVector(v2)));
}


// 正規化
void VectorNormalize(Float3* result, const Float3& source)
{
	StoreVector(result, XMVector3Normalize(LoadVector(source)));
}


// 正規化されているかを判定
//bool IsNormalized(const Float3& v);


// ベクトル線形補間
void VectorLerp(Float3* result, const Float3& v1, const Float3& v2, float tx, float ty, float tz)
{
	StoreVector(result, 
		XMVectorLerpV(LoadVector(v1), LoadVector(v2), XMVectorSet(tx, ty, tz, 0.0f))
		);
}



// ============== クォータニオン系関数 ==================

// 正規化されたベクトルをY軸回転(クォータニオン)
void QuaternionRotationY(Float3* pNormalizedPos, float angle)
{
	StoreVector(
		pNormalizedPos,
		XMVector3Rotate(LoadVector(*pNormalizedPos),
			XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), angle)
			)
		);
}

// 正規化されたベクトルをY軸回転(クォータニオン)
void QuaternionRotationX(Float3* pNormalizedPos, float angle)
{
	StoreVector(
		pNormalizedPos,
		XMVector3Rotate(LoadVector(*pNormalizedPos),
			XMQuaternionRotationAxis(XMVectorSet(-pNormalizedPos->z(), 0.0f, pNormalizedPos->x(), 0.0f), angle)
			)
		);
}


// 単位クォータニオンを作成
//void SetIdentity(Float4& quaternion);


// 単位クォータニオンかを判定
//bool IsIdentity(const Float4& quaternion)



// ============== 行列系関数 ============================

// 正射影変換行列作成
void OrthographicLH(Float4x4* projection, float viewWidth, float viewHeight, float nearZ, float farZ)
{
	StoreMatrix(projection, XMMatrixOrthographicLH(viewHeight, viewWidth, nearZ, farZ));
}

// 正射影変換行列の転置行列の作成
void OrthographicLHTp(Float4x4* projection, float viewWidth, float viewHeight, float nearZ, float farZ)
{
	StoreMatrix(projection, XMMatrixTranspose(XMMatrixOrthographicLH(viewHeight, viewWidth, nearZ, farZ)));
}

// パースペクティブ射影変換行列作成
void PerspectiveFovLHTp(Float4x4* projection, float fovAngleY, float aspectRatio, float nearZ, float farZ)
{
	StoreMatrix(projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ)));
}

// ビュー変換行列の作成
void LookAtLH(Float4x4* view, const Float3& eyePosition, const Float3& focusPosition, const Float3& upDirection)
{
	StoreMatrix(view, XMMatrixLookAtLH(LoadVector(eyePosition), LoadVector(focusPosition), LoadVector(upDirection)));
}

// ビュー変換行列の転置行列の作成
void LookAtLHTp(Float4x4* view, const Float3& eyePosition, const Float3& focusPosition, const Float3& upDirection)
{
	StoreMatrix(view, XMMatrixTranspose(XMMatrixLookAtLH(LoadVector(eyePosition), LoadVector(focusPosition), LoadVector(upDirection))));
}


// ワールド変換行列作成
void WorldTp(Float4x4* world, const Float3& offset, const Float3& scale)
{
	StoreMatrix(world, XMMatrixTranspose(XMMatrixScaling(scale.x(), scale.y(), scale.z()) *XMMatrixTranslation(offset.x(), offset.y(), offset.z())));
}

void WorldTp(Float4x4* world, const Float3& offset, const Quaternion& rotation)
{
	StoreMatrix(world, XMMatrixTranspose(XMMatrixRotationQuaternion(LoadQuaternion(rotation)) *XMMatrixTranslation(offset.x(), offset.y(), offset.z())));
}

void WorldTp(Float4x4* world, const Float3& offset, const Quaternion& rotation, float scale)
{
	StoreMatrix(world, XMMatrixTranspose(XMMatrixScaling(scale, scale, scale) *XMMatrixRotationQuaternion(LoadQuaternion(rotation)) *XMMatrixTranslation(offset.x(), offset.y(), offset.z())));
}

void WorldTp(Float4x4* world, const Float3& offset, float scale)
{
	StoreMatrix(world, XMMatrixTranspose(XMMatrixScaling(scale, scale, scale) *XMMatrixTranslation(offset.x(), offset.y(), offset.z())));
}

void WorldTp(Float4x4* world, const Float3& offset)
{
	StoreMatrix(world, XMMatrixTranspose(XMMatrixTranslation(offset.x(), offset.y(), offset.z())));
}


//2Dワールド変換行列作成
void World2DTp(Float4x4* world, const Float3& offset, const Float2& scale)
{
	StoreMatrix(world, XMMatrixTranspose(XMMatrixScaling(scale.x(), scale.y(), 1.0f) *XMMatrixTranslation(offset.x(), offset.y(), 0.0f)));
}

void World2DTp(Float4x4* world, const Float3& offset, float scale)
{
	StoreMatrix(world, XMMatrixTranspose(XMMatrixScaling(scale, scale, 1.0f) *XMMatrixTranslation(offset.x(), offset.y(), 0.0f)));
}

void World2DTp(Float4x4* world, const Float3& offset)
{
	StoreMatrix(world, XMMatrixTranspose(XMMatrixTranslation(offset.x(), offset.y(), 0.0f)));
}


// パーティクルビルボード用のワールド変換行列作成
void WorldBillboardTp(Float4x4* world, const Float3& offset, const Float2& scale, const Eye& eye)
{
	// 逆行列で回転行列を作成
	Matrix rotation =XMMatrixIdentity();
	rotation =XMMatrixLookAtLH(XMVECTOR(), XMVectorSet(eye.GetFocusPos().x()-eye.GetPos().x(), eye.GetFocusPos().y()-eye.GetPos().y(), eye.GetFocusPos().z()-eye.GetPos().z(), 0.0f), LoadVector(eye.GetUpDir()));
	Vector determ;
	rotation =XMMatrixInverse(&determ, rotation);

	StoreMatrix(world, XMMatrixTranspose(XMMatrixScaling(scale.x(), scale.y(), 1.0f) *rotation *XMMatrixTranslation(offset.x(), offset.y(), offset.z())));
}

void WorldBillboardTp(Float4x4* world, const Float3& offset, float scale, const Eye& eye)
{
	// 逆行列で回転行列を作成
	Matrix rotation =XMMatrixIdentity();
	rotation =XMMatrixLookAtLH(XMVECTOR(), XMVectorSet(eye.GetFocusPos().x()-eye.GetPos().x(), eye.GetFocusPos().y()-eye.GetPos().y(), eye.GetFocusPos().z()-eye.GetPos().z(), 0.0f), LoadVector(eye.GetUpDir()));
	Vector determ;
	rotation =XMMatrixInverse(&determ, rotation);

	StoreMatrix(world, XMMatrixTranspose(XMMatrixScaling(scale, scale, 1.0f) *rotation *XMMatrixTranslation(offset.x(), offset.y(), offset.z())));
}

void WorldBillboardTp(Float4x4* world, const Float3& offset, const Eye& eye)
{
	// 逆行列で回転行列を作成
	Matrix rotation =XMMatrixIdentity();
	rotation =XMMatrixLookAtLH(XMVECTOR(), XMVectorSet(eye.GetFocusPos().x()-eye.GetPos().x(), eye.GetFocusPos().y()-eye.GetPos().y(), eye.GetFocusPos().z()-eye.GetPos().z(), 0.0f), LoadVector(eye.GetUpDir()));
	Vector determ;
	rotation =XMMatrixInverse(&determ, rotation);

	StoreMatrix(world, XMMatrixTranspose(rotation *XMMatrixTranslation(offset.x(), offset.y(), offset.z())));
}


// 転置行列作成
void MatrixTranspose(Float4x4* result, const Float4x4& source)
{
	(*result)(0, 0) =source(0, 0);
	(*result)(1, 0) =source(0, 1);
	(*result)(2, 0) =source(0, 2);
	(*result)(3, 0) =source(0, 3);
	(*result)(0, 1) =source(1, 0);
	(*result)(1, 1) =source(1, 1);
	(*result)(2, 1) =source(1, 2);
	(*result)(3, 1) =source(1, 3);
	(*result)(0, 2) =source(2, 0);
	(*result)(1, 2) =source(2, 1);
	(*result)(2, 2) =source(2, 2);
	(*result)(3, 2) =source(2, 3);
	(*result)(0, 3) =source(3, 0);
	(*result)(1, 3) =source(3, 1);
	(*result)(2, 3) =source(3, 2);
	(*result)(3, 3) =source(3, 3);
}


// 単位行列かどうか
//bool IsIdentity(const Float3x3& matrix);

//bool IsIdentity(const Float4x4& matrix);


// 異種行列変換
void MatrixConvert(Float4x4* result, const Float4x3& source)
{
	CWE_ASSERT(result);

	VectorConvert(&(*result)(0), source(0));
	VectorConvert(&(*result)(1), source(1));
	VectorConvert(&(*result)(2), source(2));
	VectorConvert(&(*result)(3), source(3));
	(*result)(3).w() =1.0f;
}




// ================= その他の算術関数 =============================

// ベジェ曲線関数
void Bezier(Float2* bezier, const Float2& p0, const Float2& p1, const Float2& p2, const Float2& p3, float t)
{
	float t01 =1-t;
	bezier->x() =t01*t01*t01*p0.x() + 3*t01*t01*t*p1.x() + 3*t01*t*t*p2.x() + t*t*t*p3.x();
	bezier->y() =t01*t01*t01*p0.y() + 3*t01*t01*t*p1.y() + 3*t01*t*t*p2.y() + t*t*t*p3.y();
}

float CubicBezier(float x1, float y1, float x2, float y2, float t)
{
	float param = 0.5f;
	float approx =0.0f;

	//X function of the bezier curve.
	auto fx =[x1, x2](float tx){
		float s =1.0f -tx;
		return (3 *s*s *tx *x1) + (3 *s *tx*tx *x2) + (tx*tx*tx);
	};
	//Y function of the bezier curve.
	auto fy =[y1, y2](float ty){
		float s =1.0f -ty;
		return (3 *s*s *ty *y1) + (3 *s *ty*ty *y2) + (ty*ty*ty);
	};

	//Calculate the parameter from direction points using Newton's method.
	for (int i = 0; i < 15; i++)
	{
		approx =fx(param);

		if(math::NearlyEqual(approx, t)) {break;}

		if(approx > t)
		{
			param -= 1.0f / float(4 << i);
		}
		else //approx < t
		{
			param += 1.0f / float(4 << i);
		}
	}

	return fy(param);
}


// ベクトルからベクトルへ回転するクォータニオンを算出
void VectorToVectorRotaion(Quaternion* result, const Float3& fromV, const Float3& toV)
{
	Vector aFromV =LoadVector(fromV),
		   aToV =LoadVector(toV);
	Vector angle =XMVector3AngleBetweenVectors(aFromV, aToV),
		   axis =XMVector3Cross(aFromV, aToV);

	StoreQuaternion(result, XMQuaternionRotationAxis(axis, XMVectorGetX(angle)));
}


// 回転クォータニオンからオイラー角を算出
void RollPitchYawFromQuaternion(float& pitch, float& yaw, float& roll, const Quaternion& rotation)
{
	Matrix mRotation =XMMatrixRotationQuaternion(LoadQuaternion(rotation));

	//ピッチ(X軸回転)
	pitch =ASin(-mRotation(2, 1));
	//ジンバルロック回避
	if(AvoidGimbalLockByPalamX < fabsf(pitch))
	{
		pitch =(pitch<0)?-AvoidGimbalLockByPalamX : AvoidGimbalLockByPalamX;
	}
	float cosineX =Cos(pitch);
	//ヨー(Y軸回転)
	yaw =ATan((mRotation(2, 0)/cosineX), (mRotation(2, 2)/cosineX));
	//ロール(Z軸回転)
	roll =ATan((mRotation(0, 1)/cosineX), (mRotation(1, 1)/cosineX));
}



}// math
}// CWE