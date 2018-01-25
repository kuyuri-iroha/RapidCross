#pragma once

#include "CWEMath.h"
#include <fstream>
#include <vector>
#include <memory>



namespace CWE
{


struct PMXData
{
	struct Vertex
	{
		//ボーンウェイト情報
		struct Weight
		{
			enum Type
			{
				BDEF1,
				BDEF2,
				BDEF4,
				SDEF,
				Unknown,
			};

			Type mType;
			int mBorn1;
			int	mBorn2;
			int	mBorn3;
			int	mBorn4;
			float mWeight1;
			float mWeight2;
			float mWeight3;
			float mWeight4;
			math::Float3 mC;
			math::Float3 mR0;
			math::Float3 mR1;
		};//Weight

		//頂点座標
		math::Float3 mVertex;
		//法線
		math::Float3 mNormal;
		//UV座標
		math::Float2 mUV;
		//追加UV座標
		std::vector<math::Float4> mAdditionalUV;
		//ボーンウェイト
		Weight mWeight;
		//エッジ倍率
		float mEdgeMagnif;
	};//Vertex

	struct Surface
	{
		int mVertexIndex;
	};//Surface

	struct Material
	{
		math::Float4 mDiffuse;
		math::Float3 mSpecular;
		float mSpecularity;
		math::Float3 mAmbient;

		int mColorMapTextureIndex;
		int mToonTextureIndex;
		//スフィアテクスチャは非対応

		//材質ごとの頂点数
		int mVertexNum;
	};//Material

	struct Bone
	{
		//ボーン名
		std::wstring mName;
		//English version
		std::string mNameE;
		math::Float3 mPosition;
		int mFatherIndex;
		int mTransformationLevel;
		unsigned short mFlag;
		math::Float3 mCoordOffset;
		int mSonIndex;
		int mImpartParentIndex;
		float mImpartRate;
		//固定軸方向ベクトル
		math::Float3 mFixedAxis;
		//ローカルのX軸方向ベクトル
		math::Float3 mLocalAxisX;
		//ローカルのZ軸方向ベクトル
		math::Float3 mLocalAxisZ;
		int mExternalParentKey;
		int mIKTargetIndex;
		int mIKLoopCount;
		float m_IKUnitAngle;
		struct IKLink
		{
			int mIndex;
			bool mExistAngleLimited;
			math::Float3 mLimitAngleMin;
			math::Float3 mLimitAngleMax;
		};//IK
		std::vector<IKLink> mIKLinks;
	};//Bone


	std::vector<Vertex> mVertices;
	std::vector<Surface> mSurfaces;
	std::vector<std::wstring> mTexturePaths;
	std::vector<Material> mMaterials;
	std::vector<Bone> mBones;

};


struct VMDData
{
	struct FrameData
	{
		std::wstring mBoneName;
		unsigned mFrameNumber;
		math::Float3 mBonePos;
		math::Quaternion mBoneQuaternion;
		char mInterpolationParam[64];
	};

	std::vector<FrameData> mFrameData;
};


struct WAVData
{
	//Infomation
	struct WAVFormat
	{
		unsigned short mID;
		unsigned short mChannelsNum;
		unsigned long mSamplesPerSec;
		unsigned long mBytesPerSec;
		unsigned short mBlockSize;
		unsigned short mBitsPerSample;
	};

	WAVFormat mFormat;
	std::unique_ptr<unsigned char[]> mpData;
	unsigned mDataSize; 
};




class FileLoader
{
private:
	std::ifstream mFile;

	//UTF-16テキストファイルをワイド文字に変換して取得
	bool PMXGetString16(std::wstring &dest);

	//相対パスからディレクトリパスを抽出
	void DirectoryExtraction(wchar_t directoryPath[], const wchar_t relatePath[]);
	void DirectoryExtraction(wchar_t directoryPath[], const wchar_t relatePath[], unsigned relatePathLen);

public:
	FileLoader():mFile(){}
	~FileLoader(){}


	bool PMX(PMXData& data, const wchar_t file[]);
	bool VMD(VMDData& data, const wchar_t file[]);
	bool WAV(WAVData& data, const wchar_t file[]);
	
};



}// CWE
