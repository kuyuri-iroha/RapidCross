#include "CWEPolygon.h"
#include "CWEMotion.h"
#include "CWEMacroConstants.h"
#include <algorithm>




namespace CWE
{


// 2D表示用の射影変換行列
math::Float4x4 Polygon::m2DProj ={
	2.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 2.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};


// デフォルトコンストラクタ
Polygon::Polygon():
	mResource(Resource::GetInstance()), mTexSlot(0)
{
}

// コピー用コンストラクタ
Polygon::Polygon(const Polygon &original, bool copyAll):
	mResource(Resource::GetInstance()), mHandle(), mTexSlot(original.mTexSlot)
{
	mHandle =mResource.CopyPolygonHandle(original.mHandle);
	if(copyAll) {mPosition =original.mPosition;}
}

Polygon::Polygon(const Polygon &original):
	mResource(Resource::GetInstance()), mHandle(), mTexSlot(original.mTexSlot), mPosition(original.mPosition)
{
	mHandle =mResource.CopyPolygonHandle(original.mHandle);
}

Polygon& Polygon::operator=(const Polygon& original)
{
	mHandle =mResource.CopyPolygonHandle(original.mHandle);
	mTexSlot =original.mTexSlot;
	mPosition =original.mPosition;
	return *this;
}

// デストラクタ
Polygon::~Polygon()
{
	Release();
}


// ファイルのロード
void Polygon::Load(const wchar_t *filePath, const FileFormat &format)
{
	mHandle =mResource.Create(filePath, format);
}


// 現在のハンドルの参照先が使用可能かどうか
const Task& Polygon::LoadResult()const
{
	const PolygonData& data =mResource.GetPolygonData(mHandle);
	return data.mEnabled;
}


// データのコピー
void Polygon::Copy(const Polygon &original, bool copyAll)
{
	mHandle =mResource.CopyPolygonHandle(original.mHandle);
	mTexSlot =original.mTexSlot;
	if(copyAll) {mPosition =original.mPosition;}
}


// データの開放
void Polygon::Release()
{
	mResource.PolygonRelease(mHandle);
}


// 描画
void Polygon::Draw(bool cull)const
{
	const PolygonData& data =mResource.GetPolygonData(mHandle);
	if(data.mEnabled != Task::Success) {return;} //正規データでなければ描画しない

	switch(data.mFileFormat)
	{
	case FileFormat::PMX:
		Draw3D(data, cull, false);
		break;

	case FileFormat::Picture:
		Draw2D(data);
		break;

	case FileFormat::Unknown:
		return;
		break;
	}
}


// メッシュごとに描画
void Polygon::DrawMesh(unsigned meshNumber, bool cull)const
{
	const PolygonData& data =mResource.GetPolygonData(mHandle);
	if(data.mEnabled != Task::Success) {return;} //正規データでなければ描画しない

	switch(data.mFileFormat)
	{
	case FileFormat::PMX:
		Draw3D(data, cull, true, meshNumber);
		break;

	case FileFormat::Picture:
		Draw2D(data);
		break;

	case FileFormat::Unknown:
		return;
		break;
	}
}


// メッシュデータの取得
const std::vector<Mesh>& Polygon::GetMeshData(bool* result) const
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(result)
	{
		*result =(data.mEnabled == Task::Success);
	}
	return data.mMesh;
}


// メッシュ数の取得
unsigned Polygon::ReadMeshNum()const
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(data.mEnabled != Task::Success) {return -1;}
	return data.mMesh.size();
}


// メッシュ情報の取得
const Mesh& Polygon::ReadMesh(unsigned meshNumber)const
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	CWE_ASSERT(meshNumber<data.mMesh.size());
	CWE_ASSERT(data.mEnabled == Task::Success);
	return data.mMesh[meshNumber];
}


// ボーンデータの取得
const std::vector<Bone>& Polygon::GetBoneData(bool* result) const
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(result)
	{
		*result =(data.mEnabled == Task::Success);
	}
	return data.mBones;
}

std::vector<Bone>& Polygon::GetBoneData(bool* result)
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(result)
	{
		*result =(data.mEnabled == Task::Success);
	}
	return data.mBones;
}


// ボーン総数の取得
unsigned Polygon::GetBoneNum()const
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(data.mEnabled != Task::Success) {return -1;}
	return data.mBones.size();
}


// ボーン情報の取得
const Bone& Polygon::GetBone(unsigned boneNumber)const
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	CWE_ASSERT(boneNumber<mResource.GetPolygonData(mHandle).mBones.size());
	CWE_ASSERT(data.mEnabled == Task::Success);
	return mResource.GetPolygonData(mHandle).mBones[boneNumber];
}


// モーションを適用して再生
bool Polygon::MotionUpdate(Motion& motion, float frame)
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(data.mEnabled != Task::Success) {return false;}
	const unsigned boneNum =data.mBones.size();
	unsigned notLastFrame =0;
	unsigned i =0;
	ResetPose(data);

	// モーションを再生
	for(i=0; i<boneNum; ++i)
	{
		if(motion.Update(data.mBones[i], frame))
		{
			++notLastFrame; //終わっていなかったら加算
		}
	}

	// モーション後のFK更新
	FKUpdate(data);

	// IKの適用
	IKApply(data);

	//頂点変換行列作成
	for(i=0; i<boneNum; ++i)
	{
		math::MatrixMultiply(&data.mpHierarchy[i]->mMatrix, data.mpHierarchy[i]->mInvOffsetPose, data.mpHierarchy[i]->mPose);
	}

	return notLastFrame != 0;
}


// モーションを補間して適用
void Polygon::MotionLerp(Motion& from, Motion& to, float t)
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(data.mEnabled != Task::Success) {return;}
	const unsigned boneNum =data.mBones.size();
	math::Float3A pos1, pos2;
	math::QuaternionA rotation1, rotation2;
	ResetPose(data);

	//モーションを適用
	for(auto& bone : data.mBones)
	{
		//ポーズの取得
		from.GetNowPose(bone, pos1, rotation1);
		to.GetNowPose(bone, pos2, rotation2);

		//補間
		math::VectorLerp(&bone.mPosition, pos1, pos2, t, t, t);
		
		math::QuaternionSlerp(&bone.mRotation, rotation1, rotation2, t);

		//ボーンに適用
		math::MatrixTranslationR(&bone.mPose, bone.mPosition, bone.mRotation); //mPoseを一時的に流用
		math::MatrixMultiply(&bone.mRelatPose, bone.mPose, bone.mRelatPose);
	}

	//モーション後のFK更新
	FKUpdate(data);

	//IKの適用
	IKApply(data);

	//頂点変換行列作成
	for(auto& hierarchy : data.mpHierarchy)
	{
		math::MatrixMultiply(&hierarchy->mMatrix, hierarchy->mInvOffsetPose, hierarchy->mPose);
	}
}


// FKをボーンに適用
bool Polygon::FKApply()
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(data.mEnabled != Task::Success) {return false;}

	FKUpdate(data);
	return true;
}


// 頂点変換行列の更新
bool Polygon::UpdateSkinningMatrix()
{
	PolygonData& data =mResource.GetPolygonData(mHandle);
	if(data.mEnabled != Task::Success) {return false;}

	for(unsigned i=0; i<data.mpHierarchy.size(); ++i)
	{
		math::MatrixMultiply(&data.mpHierarchy[i]->mMatrix, data.mpHierarchy[i]->mInvOffsetPose, data.mpHierarchy[i]->mPose);
	}

	return true;
}


// =============== プライベート =================================


// 相対姿勢リセット
void Polygon::ResetPose(PolygonData& data)
{
	for(auto& bone:data.mBones)
	{
		bone.mRelatPose =bone.mOffsetRelatPose;
	}
}


// FKでのボーンの更新
void Polygon::FKUpdate(PolygonData& data)
{
	const unsigned boneNum =data.mpHierarchy.size();
	
	//階層計算
	for(unsigned i=0; i<boneNum; ++i)
	{
		//FK適用
		if(data.mpHierarchy[i]->mFather != -1) //ルートボーンじゃなければ
		{
			math::MatrixMultiply(&data.mpHierarchy[i]->mPose, data.mpHierarchy[i]->mRelatPose, data.mBones[data.mpHierarchy[i]->mFather].mPose);
		}
		else //ルートボーンなら
		{
			data.mpHierarchy[i]->mPose =data.mpHierarchy[i]->mRelatPose;
		}
	}
}
void Polygon::FKUpdate(PolygonData& data, unsigned boneIndex)
{
	const unsigned boneNum =data.mpHierarchy.size();
	CWE_ASSERT(boneIndex<boneNum);
	bool start =false;
	
	//階層計算
	for(unsigned i=0; i<boneNum; ++i)
	{
		if(data.mpHierarchy[i]->mSelf == boneIndex)
		{
			start =true;
		}

		if(start)
		{
			//FK適用
			if(data.mpHierarchy[i]->mFather != -1) //ルートボーンじゃなければ
			{
				math::MatrixMultiply(&data.mpHierarchy[i]->mPose, data.mpHierarchy[i]->mRelatPose, data.mBones[data.mpHierarchy[i]->mFather].mPose);
			}
			else //ルートボーンなら
			{
				data.mpHierarchy[i]->mPose =data.mpHierarchy[i]->mRelatPose;
			}
		}
	}//for()
}


// IKの適用
void Polygon::IKApply(PolygonData& data)
{
	using namespace math;
	const unsigned ikBoneNum =data.mIKBone.size();
	unsigned j =0,
			 k =0;
	Vector epsil =VectorSet(0.0000001f, 0.0000001f, 0.0000001f, 0.0000001f); //許容誤差値
	Matrix mInvLink_RelatRota;
	Vector vTarget,
		   vIK;
	float rotaAngle;
	Vector rotaAxis;
	Vector qRotation;
	float pitch, yaw, roll,
		  prevPitch, prevYaw, prevRoll;
	Matrix mRotation,
		   mInvRotation;

	for(unsigned i=0; i<ikBoneNum; ++i)
	{
		//近似算出ループ
		for(j=0; j<data.mIKBone[i].mLoop+30; ++j) //精度調整のためループ回数を調整
		{
			//IKリンクボーンループ
			for(k=0; k<data.mIKBone[i].mLinks.size(); ++k)
			{
				IKBone::Link& ikLink =data.mIKBone[i].mLinks[k];

				//リンクボーンのローカル空間に変換
				mInvLink_RelatRota =MatrixInverse(LoadMatrix(ikLink.mpBone->mPose));
				vTarget =VectorSet(data.mIKBone[i].mpTarget->mPose(3).x(), data.mIKBone[i].mpTarget->mPose(3).y(), data.mIKBone[i].mpTarget->mPose(3).z(), 0.0f);
				vIK =VectorSet(data.mIKBone[i].mpSelf->mPose(3).x(), data.mIKBone[i].mpSelf->mPose(3).y(), data.mIKBone[i].mpSelf->mPose(3).z(), 0.0f);

				vTarget =Vector3Transform(vTarget, mInvLink_RelatRota);
				vTarget =Vector3Normalize(vTarget);
				vIK =Vector3Transform(vIK, mInvLink_RelatRota);
				vIK =Vector3Normalize(vIK);

				if(Vector3NearlyEqual(vTarget, vIK, epsil)) //近似方向であれば
				{
					continue;
				}

				//回転角(成す角)
				rotaAngle =VectorGetX(
					Vector3AngleBetweenNormal(vTarget, vIK)
					);

				//回転角の単位角制限
				rotaAngle =std::min<float>(rotaAngle, data.mIKBone[i].mUnitAngle);
				rotaAngle =std::max<float>(rotaAngle, -data.mIKBone[i].mUnitAngle);

				//回転軸
				rotaAxis =Vector3Cross(vTarget, vIK);
				rotaAxis =Vector3Normalize(rotaAxis);

				if(Vector3NearlyEqual(rotaAxis, VectorSet(0.0f, 0.0f, 0.0f, 0.0f), epsil)) //2ベクトルが並行であれば
				{
					continue;
				}

				//回転クォータニオン
				qRotation =QuaternionRotationAxis(rotaAxis, rotaAngle);
				if(ikLink.mExistAngleLimited) //角度制限あり
				{
					//オイラー角に変換
					RollPitchYawFromQuaternion(pitch, yaw, roll, qRotation);

					//既に算出されている回転からオイラー角を算出
					mInvLink_RelatRota =MatrixIdentity();
					mInvLink_RelatRota.r[0] =LoadVector(ikLink.mpBone->mRelatPose(0));
					mInvLink_RelatRota.r[1] =LoadVector(ikLink.mpBone->mRelatPose(1));
					mInvLink_RelatRota.r[2] =LoadVector(ikLink.mpBone->mRelatPose(2));
					RollPitchYawFromMatrix(prevPitch, prevYaw, prevRoll, mInvLink_RelatRota);

					//角度制限
					if(!NearlyEqual(prevPitch, 0.0f))
					{
						if(NearlyEqual(ikLink.mLimitAngleMin.x()-ikLink.mLimitAngleMax.x(), 0.0f)) {prevPitch =0.0f;}
						pitch =std::max<float>(pitch, ikLink.mLimitAngleMin.x()-prevPitch);
						pitch =std::min<float>(pitch, ikLink.mLimitAngleMax.x()-prevPitch);
					}
					else
					{
						pitch =std::max<float>(pitch, ikLink.mLimitAngleMin.x());
						pitch =std::min<float>(pitch, ikLink.mLimitAngleMax.x());
					}
					if(!NearlyEqual(prevYaw, 0.0f))
					{
						if(NearlyEqual(ikLink.mLimitAngleMin.y()-ikLink.mLimitAngleMax.y(), 0.0f)) {prevYaw =0.0f;}
						yaw =std::max<float>(yaw, ikLink.mLimitAngleMin.y()-prevYaw);
						yaw =std::min<float>(yaw, ikLink.mLimitAngleMax.y()-prevYaw);
					}
					else
					{
						yaw =std::max<float>(yaw, ikLink.mLimitAngleMin.y());
						yaw =std::min<float>(yaw, ikLink.mLimitAngleMax.y());
					}
					if(!NearlyEqual(prevRoll, 0.0f))
					{
						if(NearlyEqual(ikLink.mLimitAngleMin.z()-ikLink.mLimitAngleMax.z(), 0.0f)) {prevRoll =0.0f;}
						roll =std::max<float>(roll, ikLink.mLimitAngleMin.z()-prevRoll);
						roll =std::min<float>(roll, ikLink.mLimitAngleMax.z()-prevRoll);
					}
					else
					{
						roll =std::max<float>(roll, ikLink.mLimitAngleMin.z());
						roll =std::min<float>(roll, ikLink.mLimitAngleMax.z());
					}
					
					//回転行列
					mRotation =MatrixRotationRollPitchYaw(pitch, yaw, roll);
				}
				else //角度制限なし
				{
					mRotation =MatrixRotationQuaternion(qRotation);
				}

				//相対行列に適用
				StoreMatrix(&ikLink.mpBone->mRelatPose,
					MatrixMultiply(mRotation, LoadMatrix(ikLink.mpBone->mRelatPose)));

				//限定的なFKの更新
				FKUpdate(data, ikLink.mpBone->mSelf);
			}
		}
	}
}


// 3D描画
void Polygon::Draw3D(const PolygonData& data, bool cull, bool eachMesh, unsigned meshNumber)const
{
	Graphic& manager =Graphic::GetInstance();

	// カリングを無効化
	manager.RSResetUseState(cull ? RSState::Normal : RSState::NonCull);
	// IAに頂点バッファを設定
	const unsigned stride[] ={sizeof(math::Float3), sizeof(math::Float2), sizeof(math::Float3), sizeof(math::Int4), sizeof(math::Float4), sizeof(math::Float3), sizeof(math::Float3), sizeof(math::Float3)};
	const unsigned offset[] ={0, 0, 0, 0, 0, 0, 0, 0};
	manager.IASetVertexBuffers(0, data.VERTEX_BUFFER_NUM, data.mpVertexBuffers, stride, offset);
	// IAにインデックス・バッファを設定
	manager.IASetIndexBuffer(data.mpIndexBuffer, 0);

	// これまでに描画した頂点数
	unsigned drewVertexNum =0;
	unsigned i =0;

	// メッシュ毎の描画かどうか
	if(eachMesh)
	{
		CWE_ASSERT(meshNumber<data.mMesh.size());
		i =meshNumber;
		++meshNumber;
		//描画するメッシュの最初の頂点番号を算出
		for(unsigned j=0; j<i; ++j)
		{
			drewVertexNum +=data.mMesh[j].mVertexNum;
		}
	}
	else
	{
		meshNumber =data.mMesh.size();
	}

	// メッシュを1つ1つ描画
	for(; i<meshNumber; ++i)
	{
		// シェーダリソースビューをピクセルシェーダにセット
		manager.PSSetShaderResources(mTexSlot, 1, &data.mMesh[i].mpTexture);

		// メッシュを描画
		manager.DrawIndexed(data.mMesh[i].mVertexNum, drewVertexNum, 0);

		// これまでに描画した頂点数を記録
		drewVertexNum +=data.mMesh[i].mVertexNum;
	}
}


// 2D描画
void Polygon::Draw2D(const PolygonData& data)const
{
	Graphic& manager =Graphic::GetInstance();

	// カリングを有効化
	manager.RSResetUseState(RSState::Normal);
	// IAに頂点バッファを設定
	const unsigned stride[] ={sizeof(math::Float3), sizeof(math::Float2)};
	const unsigned offset[] ={0, 0};
	manager.IASetVertexBuffers(0, 2, data.mpVertexBuffers, stride, offset);
	// IAにインデックス・バッファを設定
	manager.IASetIndexBuffer(data.mpIndexBuffer, 0);

	// シェーダリソースビューをピクセルシェーダにセット
	manager.PSSetShaderResources(mTexSlot, 1, &data.mMesh[0].mpTexture);		

	// メッシュを描画
	manager.DrawIndexed(data.mMesh[0].mVertexNum, 0, 0);
}




}// CWE