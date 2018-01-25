#pragma once

#include "CWEIncludedAPI.h"
#include "CWEGraphicManager.h"
#include "CWESound.h"
#include "CWETypeDefinition.h"
#include "CWEUsefullyMacro.h"
#include "CWERandomizer.h"
#include "CWEMath.h"
#include "CWEFileLoader.h"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <deque>
#include <array>
#include <functional>


namespace CWE
{
class Graphic;
class MasterSound;
bool Update();
void End();

class Polygon;
struct Eye;


// 3Dポリゴンのメッシュ
struct Mesh
{
	ID3D11ShaderResourceView* mpTexture;
	unsigned mVertexNum;
	float mSpecular;
};


// ボーンデータ
struct Bone
{
	int mSelf; //自己のインデックス
	int mFather; //親ボーンインデックス

	std::wstring mName;
	math::Float3 mPosition; //位置変形
	math::Quaternion mRotation; //回転変形

	math::Float4x4 mInvOffsetPose; //絶対初期姿勢逆行列
	math::Float4x4 mOffsetRelatPose; //初期相対姿勢行列
	math::Float4x4 mRelatPose; //相対姿勢行列
	math::Float4x4 mPose; //絶対姿勢行列
	math::Float4x4 mMatrix; //頂点変換行列

	Bone(): mSelf(-1), mFather(-1) {}
	~Bone() {}
};

// IKボーンデータ
struct IKBone
{
	Bone* mpSelf;
	float mUnitAngle;
	Bone* mpTarget;
	unsigned mLoop;
	struct Link
	{
		Bone* mpBone;
		bool mExistAngleLimited;
		math::Float3 mLimitAngleMin;
		math::Float3 mLimitAngleMax;
	};
	std::vector<Link> mLinks;

	IKBone(): mpSelf(nullptr), mpTarget(nullptr) {};
	IKBone(Bone& self): mpSelf(&self), mpTarget(nullptr) {}
	~IKBone() {}
};


// 2D,3Dを問わないポリゴンの情報
struct PolygonData
{
	// 頂点バッファの数
	static const unsigned VERTEX_BUFFER_NUM =8;

	const wchar_t *mFilePath;
	unsigned mReferenceCount; //使用しているモジュールの参照カウント
	Task mEnabled; //ロードに成功したか
	FileFormat mFileFormat;

	// リソース
	ID3D11Buffer *mpVertexBuffers[VERTEX_BUFFER_NUM];
	ID3D11Buffer *mpIndexBuffer;
	unsigned mIndexCount;
	std::vector<Mesh> mMesh;
	std::vector<Bone> mBones;
	std::vector<Bone*> mpHierarchy; //ボーン情報の階層優先ソート版
	std::vector<IKBone> mIKBone;

	PolygonData():mFilePath(0), mReferenceCount(0), mEnabled(Task::Working), mFileFormat(FileFormat::Unknown), mpIndexBuffer(0), mIndexCount(0)
	{
		for(int i=0; i<VERTEX_BUFFER_NUM; ++i) 
		{
			mpVertexBuffers[i] =nullptr;
		}
	}
	~PolygonData()
	{
		for(unsigned i=0; i<VERTEX_BUFFER_NUM; ++i)
		{
			SAFE_RELEASE(mpVertexBuffers[i]);
		}
		SAFE_RELEASE(mpIndexBuffer);
		for(unsigned j=0; j<mMesh.size(); ++j) {SAFE_RELEASE(mMesh[j].mpTexture);}
	}
};


// 1フレーム単位のモーション情報
struct FrameData
{
	unsigned mFrameNumber; //フレーム番号(何フレーム目の処理か)
	math::Float3 mBonePos; //ボーンの位置
	math::Quaternion mBoneQuaternion; //ボーンのクォータニオン
	std::array<float, 4> mXInterpolation, //X補間パラメーター
						  mYInterpolation, //Y補間パラメーター
						  mZInterpolation, //Z補間パラメーター
						  mRInterpolation; //回転補間パラメーター

	bool operator<(const FrameData& frameData)const {return mFrameNumber < frameData.mFrameNumber;}
};


// モーション情報
struct MotionData
{
	const wchar_t *mFilePath;
	unsigned mReferenceCount; //使用しているモジュールの参照カウント
	Task mEnabled; //ロードに成功したか
	FileFormat mFileFormat;

	// フレームデータ(<ボーン名, <順列したフレームデータ, 再生中のキーフレームインデックス>>)
	std::unordered_map<std::wstring, std::pair<std::vector<FrameData>, unsigned>> mFrameData;
	//最後のキーフレーム
	unsigned mLastKeyFrame;


	MotionData():mReferenceCount(0), mEnabled(Task::Working), mFileFormat(FileFormat::Unknown), mLastKeyFrame(0)
	{
	}
	~MotionData()
	{
	}
};


// Sound Data
struct SoundData
{
	const wchar_t *mFilePath;
	unsigned mReferenceCount; //使用しているモジュールの参照カウント
	Task mEnabled; //ロードに成功したか
	FileFormat mFileFormat;

	IXAudio2SourceVoice* mpSourceVoice;
	CWE::WAVData mSoundData;
	XAUDIO2_BUFFER mBuffer;

	SoundData(): mReferenceCount(), mEnabled(Task::Working), mFileFormat(FileFormat::Unknown), mpSourceVoice(), mBuffer()
	{}
	~SoundData()
	{
		SafeDestroy(mpSourceVoice);
	}
};


// ロードタスクの格納型
struct LoadTask
{
	std::function<void(Resource&, int)> mTask;
	ResourceHandle mHandle;

	LoadTask(std::function<void(Resource&, int)> task, const ResourceHandle &handle): mTask(task), mHandle(handle) {}
	~LoadTask() {}
};


// リソース管理クラス
class Resource
{
private:
	friend bool CWE::Update();
	friend void CWE::End();

	Graphic& mGraphicM;
	MasterSound& mSoundM;

	Resource();
	~Resource();
	Resource(const Resource&) =delete;
	Resource& operator=(const Resource&) =delete;


	// リソース
	std::unordered_map<int, PolygonData> mPolygons;
	std::unordered_map<int, MotionData> mMotions;
	std::unordered_map<int, SoundData> mSounds;
	// スレッド
	std::thread* mpWorker;
	std::mutex mMutex;

	// ハンドル生成用番号
	int mHandleNumber;

	// ロードタスクバッファ
	std::deque<LoadTask> mTaskBuffer;


	// 終了
	void End();

	// リソースの生成(スレッド関数)
	static void CreateAll(Resource &resource);
	// リソースのロードのリクエスト
	void LoadRequest(const ResourceHandle &handle, const FileFormat& format);
	// 各種ファイルからリソースの生成(本体)
	static void CreateFromPMX(Resource& resource, int handle);
	static void CreateFromPicture(Resource& resource, int handle);
	static void CreateFromVMD(Resource& resource, int handle);
	static void CreateFromWAVE(Resource& resource, int handle);
	// リソースのロード(本体)
	void LoadExecution(bool ending);
	// ヒエラルキーソート用equal_range
	void EqualRange(unsigned* lower, unsigned* upper, int value, const std::vector<Bone*>& tmpB);

public:
	static Resource& GetInstance();

	// リソースの生成
	ResourceHandle Create(const wchar_t *filePath, const FileFormat& format);
	// リソースハンドルのコピー
	const ResourceHandle& CopyPolygonHandle(const ResourceHandle &handle);
	const ResourceHandle& CopyMotionHandle(const ResourceHandle& handle);
	const ResourceHandle& CopySoundHandle(const ResourceHandle& handle);
	// リソースの開放
	void PolygonRelease(const ResourceHandle &handle);
	void MotionRelease(const ResourceHandle& handle);
	void SoundRelease(const ResourceHandle& handle);
	// リソースが存在するか(ポリゴンデータ)
	inline bool IsPolygon(const ResourceHandle& handle) const {return mPolygons.find(handle.mHandle) != mPolygons.end();}
	// リソースが存在するか(モーションデータ)
	inline bool IsMotion(const ResourceHandle& handle) const {return mMotions.find(handle.mHandle) != mMotions.end();}
	// リソースが存在するか(サウンドデータ)
	inline bool IsSound(const ResourceHandle& handle) const {return mSounds.find(handle.mHandle) != mSounds.end();}
	// リソースデータの受け渡し(ポリゴンデータ)
	PolygonData& GetPolygonData(const ResourceHandle& handle);
	const PolygonData& GetPolygonData(const ResourceHandle& handle)const;
	// リソースデータの受け渡し(モーションデータ)
	MotionData& GetMotionData(const ResourceHandle& handle);
	const MotionData& GetMotionData(const ResourceHandle& handle)const;
	// リソースデータの受け渡し(サウンドデータ)
	SoundData& GetSoundData(const ResourceHandle& handle);
	const SoundData& GetSoundData(const ResourceHandle& handle)const;

};



}// CWE


