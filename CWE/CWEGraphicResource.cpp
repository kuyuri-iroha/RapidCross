#include "CWEGraphicResource.h"
#include "CWEGraphicManager.h"
#include "CWEPolygon.h"
#include <algorithm>


namespace CWE
{


// コンストラクタ
Resource::Resource():
	mGraphicM(Graphic::GetInstance()), mSoundM(MasterSound::GetInstance()),
	mpWorker(0), mHandleNumber(INT_MIN)
{
}

// デストラクタ
Resource::~Resource()
{
	End();
}

// インスタンスの取得
Resource& Resource::GetInstance()
{
	static Resource instance;
	return instance;
}


// 終了
void Resource::End()
{
	mSounds.clear();
	mMotions.clear();
	mPolygons.clear();

	// スレッドの後処理
	if(mpWorker)
	{
		mpWorker->join();
		SAFE_DELETE(mpWorker);
	}
}


// リソースのロード
void Resource::LoadExecution(bool ending)
{
	if(!mTaskBuffer.empty())
	{
		// 全体が終了するとき
		if(ending)
		{
			mTaskBuffer.clear();
			return;
		}

		if(!mpWorker)
		{
			// ワーカースレッドを立ち上げる
			mpWorker =new std::thread(CreateAll, std::ref(*this));
		}
	}
	else
	{
		if(mpWorker)
		{
			mpWorker->detach();
			SAFE_DELETE(mpWorker);
		}
	}
}


// リソースの生成
ResourceHandle Resource::Create(const wchar_t *filePath, const FileFormat& format)
{
	ResourceHandle handle;
	//ハンドルの生成
	handle.mHandle =mHandleNumber;
	++mHandleNumber;
	//4,294,967,295個以上のリソースを生成すると安全性が保障されなくなる

	switch(format)
	{
	// ポリゴンファイル
	case FileFormat::PMX:
	case FileFormat::Picture:
		mMutex.lock();
		mPolygons[handle.mHandle].mFilePath =filePath;
		mMutex.unlock();
		mPolygons.at(handle.mHandle).mFileFormat =format;

		LoadRequest(handle, format);

		++mPolygons.at(handle.mHandle).mReferenceCount;
		break;

	// アニメーションファイル
	case FileFormat::VMD:
		mMutex.lock();
		mMotions[handle.mHandle].mFilePath =filePath;
		mMutex.unlock();
		mMotions.at(handle.mHandle).mFileFormat =format;

		LoadRequest(handle, format);

		++mMotions.at(handle.mHandle).mReferenceCount;
		break;

	// 音声ファイル
	case FileFormat::WAVE:
		mMutex.lock();
		mSounds[handle.mHandle].mFilePath =filePath;
		mMutex.unlock();
		mSounds.at(handle.mHandle).mFileFormat =format;

		LoadRequest(handle, format);

		++mSounds.at(handle.mHandle).mReferenceCount;
		break;

	// 不明ファイル
	case FileFormat::Unknown:
		CWE_ASSERT(false);
		break;

	// 非対応列挙子
	default:
		CWE_ASSERT(false);
		break;
	}
	

	return handle;
}


// リソースハンドルのコピー
const ResourceHandle& Resource::CopyPolygonHandle(const ResourceHandle& handle)
{
	CWE_ASSERT(mPolygons.find(handle.mHandle) != mPolygons.end()); //不正なハンドルの禁止
	++mPolygons.at(handle.mHandle).mReferenceCount;
	return handle;
}
const ResourceHandle& Resource::CopyMotionHandle(const ResourceHandle& handle)
{
	CWE_ASSERT(mMotions.find(handle.mHandle) != mMotions.end()); //不正なハンドルの禁止
	++mMotions.at(handle.mHandle).mReferenceCount;
	return handle;
}
const ResourceHandle& Resource::CopySoundHandle(const ResourceHandle& handle)
{
	CWE_ASSERT(mSounds.find(handle.mHandle) != mSounds.end()); //不正なハンドルの禁止
	++mSounds.at(handle.mHandle).mReferenceCount;
	return handle;
}


// リソースの開放
void Resource::PolygonRelease(const ResourceHandle& handle)
{
	if(mPolygons.find(handle.mHandle) == mPolygons.end()) {return;} //不正なハンドルの無視
	PolygonData& refP =mPolygons.at(handle.mHandle);
	--refP.mReferenceCount;
	// 参照カウントが0になったら削除
	if(refP.mReferenceCount <=0)
	{
		mMutex.lock();
		mPolygons.erase(handle.mHandle);
		mMutex.unlock();
	}
}
void Resource::MotionRelease(const ResourceHandle& handle)
{
	if(mMotions.find(handle.mHandle) == mMotions.end()) {return;} //不正なハンドルの無視
	MotionData& refA =mMotions.at(handle.mHandle);
	--refA.mReferenceCount;
	// 参照カウントが0になったら削除
	if(refA.mReferenceCount <=0)
	{
		mMutex.lock();
		mMotions.erase(handle.mHandle);
		mMutex.unlock();
	}
}
void Resource::SoundRelease(const ResourceHandle& handle)
{
	if(mSounds.find(handle.mHandle) == mSounds.end()) {return;} //不正なハンドルの無視
	SoundData& refA =mSounds.at(handle.mHandle);
	--refA.mReferenceCount;
	// 参照カウントが0になったら削除
	if(refA.mReferenceCount <=0)
	{
		mMutex.lock();
		mSounds.erase(handle.mHandle);
		mMutex.unlock();
	}
}


// リソースデータの受け渡し(ポリゴンデータ)
PolygonData& Resource::GetPolygonData(const ResourceHandle& handle)
{
	CWE_ASSERT(mPolygons.find(handle.mHandle) != mPolygons.end()); //不正なハンドルの禁止
	return mPolygons.at(handle.mHandle);
}
const PolygonData& Resource::GetPolygonData(const ResourceHandle& handle)const
{
	CWE_ASSERT(mPolygons.find(handle.mHandle) != mPolygons.end()); //不正なハンドルの禁止
	return mPolygons.at(handle.mHandle);
}


// リソースデータの受け渡し(モーションデータ)
MotionData& Resource::GetMotionData(const ResourceHandle& handle)
{
	CWE_ASSERT(mMotions.find(handle.mHandle) != mMotions.end()); //不正なハンドルの禁止
	return mMotions.at(handle.mHandle);
}
const MotionData& Resource::GetMotionData(const ResourceHandle& handle)const
{
	CWE_ASSERT(mMotions.find(handle.mHandle) != mMotions.end()); //不正なハンドルの禁止
	return mMotions.at(handle.mHandle);
}


// リソースデータの受け渡し(サウンドデータ)
SoundData& Resource::GetSoundData(const ResourceHandle& handle)
{
	CWE_ASSERT(mSounds.find(handle.mHandle) != mSounds.end()); //不正なハンドルの禁止
	return mSounds.at(handle.mHandle);
}
const SoundData& Resource::GetSoundData(const ResourceHandle& handle)const
{
	CWE_ASSERT(mSounds.find(handle.mHandle) != mSounds.end()); //不正なハンドルの禁止
	return mSounds.at(handle.mHandle);
}



// ====================== 内部使用関数 ==========================


// リソースの生成(スレッド関数)
void Resource::CreateAll(Resource &resource)
{
	std::function<void(Resource&, int)> task;

	while(!resource.mTaskBuffer.empty())
	{
		Sleep(10);

		task =resource.mTaskBuffer.front().mTask;
		task(resource, resource.mTaskBuffer.front().mHandle.mHandle);
		if(!resource.mTaskBuffer.empty())
		{
			resource.mMutex.lock();
			resource.mTaskBuffer.pop_front();
			resource.mMutex.unlock();
		}
	}
}


// リソースのロードのリクエスト
void Resource::LoadRequest(const ResourceHandle &handle, const FileFormat& format)
{
	mMutex.lock();
	switch(format)
	{
	case FileFormat::PMX:
		mTaskBuffer.emplace_back(Resource::CreateFromPMX, handle);
		break;

	case FileFormat::Picture:
		mTaskBuffer.emplace_back(Resource::CreateFromPicture, handle);
		break;

	case FileFormat::VMD:
		mTaskBuffer.emplace_back(Resource::CreateFromVMD, handle);
		break;

	case FileFormat::WAVE:
		mTaskBuffer.emplace_back(Resource::CreateFromWAVE, handle);
		break;

	case FileFormat::Unknown:
		CWE_ASSERT(false); //フォーマット不明はありえない
		break;

	default:
		CWE_ASSERT(false); //非対応列挙子
		break;
	}
	mMutex.unlock();
}


// 各種ファイルからリソースの生成(本体)
// PMXファイル
void Resource::CreateFromPMX(Resource &resource, int handle)
{
	FileLoader loader;
	PMXData data;
	unsigned i=0;

	PolygonData& refP =resource.mPolygons.at(handle);

	if(!loader.PMX(data, refP.mFilePath))
	{
		// ロードが失敗した場合
		refP.mEnabled =Task::Failed;
		return;
	}

	// 頂点バッファの生成
	D3D11_BUFFER_DESC vertexBuffer;
	vertexBuffer.Usage =D3D11_USAGE_DEFAULT;
	vertexBuffer.ByteWidth =sizeof(math::Float3) *data.mVertices.size();
	vertexBuffer.BindFlags =D3D11_BIND_VERTEX_BUFFER;
	vertexBuffer.CPUAccessFlags =0;
	vertexBuffer.MiscFlags =0;
	vertexBuffer.StructureByteStride =0;
		//頂点配列の抽出
	std::vector<math::Float3> vertices;
	vertices.resize(data.mVertices.size());
	for(i=0; i<vertices.size(); ++i) {vertices[i] =data.mVertices[i].mVertex;}
	D3D11_SUBRESOURCE_DATA vertexBufferSub;
	vertexBufferSub.pSysMem =&vertices[0];
	vertexBufferSub.SysMemPitch =0;
	vertexBufferSub.SysMemSlicePitch =0;
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexBuffer, &vertexBufferSub, &refP.mpVertexBuffers[0])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}

	// UV座標の作成
	vertexBuffer.ByteWidth =sizeof(math::Float2) *data.mVertices.size();
	std::vector<math::Float2> uv;
	uv.resize(data.mVertices.size());
	for(i=0; i<uv.size(); ++i) {uv[i] =data.mVertices[i].mUV;}
	vertexBufferSub.pSysMem =&uv[0];
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexBuffer, &vertexBufferSub, &refP.mpVertexBuffers[1])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}

	// 法線バッファの作成
	vertexBuffer.ByteWidth =sizeof(math::Float3) *data.mVertices.size();
	for(i=0; i<vertices.size(); ++i) {vertices[i] =data.mVertices[i].mNormal;}
	vertexBufferSub.pSysMem =&vertices[0];
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexBuffer, &vertexBufferSub, &refP.mpVertexBuffers[2])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}

	// ボーンインデックスバッファの作成
	vertexBuffer.ByteWidth =sizeof(math::Int4) *data.mVertices.size();
	std::vector<math::Int4> bIndx;
	bIndx.resize(data.mVertices.size());
	for(i=0; i<bIndx.size(); ++i)
	{
		bIndx[i].x() =data.mVertices[i].mWeight.mBorn1;
		bIndx[i].y() =data.mVertices[i].mWeight.mBorn2;
		bIndx[i].z() =data.mVertices[i].mWeight.mBorn3;
		bIndx[i].w() =data.mVertices[i].mWeight.mBorn4;
	}
	vertexBufferSub.pSysMem =&bIndx[0];
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexBuffer, &vertexBufferSub, &refP.mpVertexBuffers[3])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}

	// ボーンウェイトバッファの作成
	vertexBuffer.ByteWidth =sizeof(math::Float4) *data.mVertices.size();
	std::vector<math::Float4> bWeight;
	bWeight.resize(data.mVertices.size());
	for(i=0; i<bWeight.size(); ++i)
	{
		bWeight[i].x() =data.mVertices[i].mWeight.mWeight1;
		bWeight[i].y() =data.mVertices[i].mWeight.mWeight2;
		bWeight[i].z() =data.mVertices[i].mWeight.mWeight3;
		bWeight[i].w() =data.mVertices[i].mWeight.mWeight4;
	}
	vertexBufferSub.pSysMem =&bWeight[0];
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexBuffer, &vertexBufferSub, &refP.mpVertexBuffers[4])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}

	// SDEF用Cパラメータバッファの作成
	vertexBuffer.ByteWidth =sizeof(math::Float3) *data.mVertices.size();
	for(i=0; i<vertices.size(); ++i) {vertices[i] =data.mVertices[i].mWeight.mC;}
	vertexBufferSub.pSysMem =&vertices[0];
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexBuffer, &vertexBufferSub, &refP.mpVertexBuffers[5])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}
	// SDEF用R0パラメータバッファの作成
	for(i=0; i<vertices.size(); ++i) {vertices[i] =data.mVertices[i].mWeight.mR0;}
	vertexBufferSub.pSysMem =&vertices[0];
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexBuffer, &vertexBufferSub, &refP.mpVertexBuffers[6])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}
	// SDEF用R1パラメータバッファの作成
	for(i=0; i<vertices.size(); ++i) {vertices[i] =data.mVertices[i].mWeight.mR1;}
	vertexBufferSub.pSysMem =&vertices[0];
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexBuffer, &vertexBufferSub, &refP.mpVertexBuffers[7])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}

	// インデックスバッファの生成
	D3D11_BUFFER_DESC indexBuffer;
	indexBuffer.Usage =D3D11_USAGE_DEFAULT;
	indexBuffer.ByteWidth =sizeof(PMXData::Surface) *data.mSurfaces.size();
	indexBuffer.BindFlags =D3D11_BIND_INDEX_BUFFER;
	indexBuffer.CPUAccessFlags =0;
	indexBuffer.MiscFlags =0;
	indexBuffer.StructureByteStride =0;
	D3D11_SUBRESOURCE_DATA indexBufferSub;
	indexBufferSub.pSysMem =&data.mSurfaces[0];
	indexBufferSub.SysMemPitch =0;
	indexBufferSub.SysMemSlicePitch =0;
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&indexBuffer, &indexBufferSub, &refP.mpIndexBuffer)))
	{
		refP.mEnabled =Task::Failed;
		return;
	}
	// インデックスバッファの総数を格納
	refP.mIndexCount =data.mSurfaces.size();

	// メッシュの作成
	refP.mMesh.resize(data.mMaterials.size());
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	D3DX11_IMAGE_INFO imageInfo;
	ZeroMemory(&imageInfo, sizeof(imageInfo));
	for(i=0; i<refP.mMesh.size(); ++i)
	{
		// テクスチャなしのメッシュは非対応
		CWE_ASSERT(data.mMaterials[i].mColorMapTextureIndex != 255);

		// シェーダリソースビューの設定
		if(FAILED(D3DX11GetImageInfoFromFileW(data.mTexturePaths[data.mMaterials[i].mColorMapTextureIndex].c_str(), NULL, &imageInfo, NULL)))
		{
			refP.mEnabled =Task::Failed;
			return;
		}
		loadInfo.Width =imageInfo.Width;
		loadInfo.Height =imageInfo.Height;
		loadInfo.Depth =imageInfo.Depth;
		loadInfo.FirstMipLevel =0;
		loadInfo.MipLevels =imageInfo.MipLevels;
		loadInfo.Usage =D3D11_USAGE_DEFAULT;
		loadInfo.BindFlags =D3D11_BIND_SHADER_RESOURCE;
		loadInfo.CpuAccessFlags =NULL;
		loadInfo.MiscFlags =NULL;
		loadInfo.Format =DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		loadInfo.Filter =D3DX11_FILTER_LINEAR|D3DX11_FILTER_SRGB_IN;
		loadInfo.MipFilter =D3DX11_FILTER_LINEAR;
		loadInfo.pSrcInfo =&imageInfo;

		// シェーダリソースビューの作成
		if(FAILED(D3DX11CreateShaderResourceViewFromFileW(resource.mGraphicM.mpDevice, data.mTexturePaths[data.mMaterials[i].mColorMapTextureIndex].c_str(), &loadInfo, NULL, &refP.mMesh[i].mpTexture, nullptr)))
		{
			refP.mEnabled =Task::Failed;
			return;
		}
		
		// メッシュの頂点数
		refP.mMesh[i].mVertexNum =data.mMaterials[i].mVertexNum;

		// メッシュのスペキュラー係数
		refP.mMesh[i].mSpecular =data.mMaterials[i].mSpecularity;
	}
	refP.mMesh.shrink_to_fit(); //無駄なメモリをなくす
	

	// ボーンの作成
	unsigned boneNum =data.mBones.size();
	refP.mBones.resize(boneNum);
	for(i=0; i<boneNum; ++i)
	{
		// 自己のインデックス
		refP.mBones[i].mSelf =i;
		// 親ボーン
		refP.mBones[i].mFather =data.mBones[i].mFatherIndex;
		//ボーン初期位置
		refP.mBones[i].mPosition =data.mBones[i].mPosition;
		// ボーン名
		refP.mBones[i].mName =data.mBones[i].mName;
	}
	refP.mBones.shrink_to_fit();


	// 階層優先ソート版ボーンポインタ配列の作成
	unsigned filledCnt=0, prevFilledCnt=0;
	refP.mpHierarchy.resize(boneNum);
		//ボーンのポインタを受け渡し
	for(i=0; i<boneNum; ++i)
	{
		refP.mpHierarchy[i] =&refP.mBones[i];
	}
	refP.mpHierarchy.shrink_to_fit();
	std::vector<Bone*> tmp(refP.mpHierarchy);
		//親ボーン番号が若い順にソート
	std::sort(tmp.begin(), tmp.end(), 
		[](const Bone* b1, const Bone* b2)->bool{return b1->mFather < b2->mFather;});
		//ルートボーンを格納
	for(i=0; i<boneNum; ++i)
	{
		if(tmp[i]->mFather != -1) {break;}
		refP.mpHierarchy[i] =tmp[i];
		++prevFilledCnt;
	}
	filledCnt +=prevFilledCnt;
	unsigned lower=0, upper=0;
	unsigned indx=0;
	unsigned k=0;
		//ソートループ
	while(true)
	{
		i =filledCnt -prevFilledCnt;
		prevFilledCnt =0;
			//前回のループで格納したボーンを走査
		for(; i<filledCnt; ++i)
		{
				//指定ボーンを親ボーンに持つボーンの最初と最後の添え字を算出
			resource.EqualRange(&lower, &upper, refP.mpHierarchy[i]->mSelf, tmp);

				//算出したボーンを格納
			for(indx=lower, k=filledCnt+prevFilledCnt; indx<upper; ++indx, ++k)
			{
				refP.mpHierarchy[k] =tmp[indx];
				++prevFilledCnt;
			}//for
		}//for
		filledCnt +=prevFilledCnt;
			//最下層到達
		if(prevFilledCnt == 0) {break;}
	}//while

	// 諸初期行列作成
	math::Float3A tmpPos;
	for(i=0; i<boneNum; ++i)
	{
		//逆絶対初期行列作成
		math::MatrixTranslation(&refP.mpHierarchy[i]->mInvOffsetPose, refP.mpHierarchy[i]->mPosition);
		refP.mpHierarchy[i]->mPose =refP.mpHierarchy[i]->mInvOffsetPose;
		math::MatrixInverse(&refP.mpHierarchy[i]->mInvOffsetPose, refP.mpHierarchy[i]->mInvOffsetPose);

		if(refP.mpHierarchy[i]->mFather != -1) //親がいれば
		{
			//相対位置を算出
			math::VectorSubtract(&tmpPos, refP.mpHierarchy[i]->mPosition, refP.mBones[refP.mpHierarchy[i]->mFather].mPosition);
			//初期相対姿勢行列作成
			math::MatrixTranslation(&refP.mpHierarchy[i]->mOffsetRelatPose, tmpPos);
		}
		else
		{
			//初期相対姿勢行列作成
			math::MatrixTranslation(&refP.mpHierarchy[i]->mOffsetRelatPose, refP.mpHierarchy[i]->mPosition);
		}
		refP.mpHierarchy[i]->mRelatPose =refP.mpHierarchy[i]->mOffsetRelatPose;
	}


	// IKボーン作成
	refP.mIKBone.reserve(10);
	for(i=0; i<boneNum; ++i)
	{
		if(data.mBones[i].mIKTargetIndex != -1) // IKボーンならば
		{
			refP.mIKBone.emplace_back(refP.mBones[i]);
		}
	}
	refP.mIKBone.shrink_to_fit();
	const unsigned ikBoneNum =refP.mIKBone.size();
		//IKパラメータの設定
	for(unsigned h=0; h<ikBoneNum; ++h)
	{
		PMXData::Bone& refBone =data.mBones[refP.mIKBone[h].mpSelf->mSelf];
			//ターゲットボーン
		refP.mIKBone[h].mpTarget =&refP.mBones[refBone.mIKTargetIndex];
		refP.mIKBone[h].mLoop =(refBone.mIKLoopCount<0)?0:refBone.mIKLoopCount;
			//単位角
		refP.mIKBone[h].mUnitAngle =refBone.m_IKUnitAngle;
			//IKリンク
		refP.mIKBone[h].mLinks.resize(refBone.mIKLinks.size());
		for(i=0; i<refP.mIKBone[h].mLinks.size(); ++i)
		{
			refP.mIKBone[h].mLinks[i].mpBone =&refP.mBones[refBone.mIKLinks[i].mIndex];
			refP.mIKBone[h].mLinks[i].mExistAngleLimited =refBone.mIKLinks[i].mExistAngleLimited;
			refP.mIKBone[h].mLinks[i].mLimitAngleMin =refBone.mIKLinks[i].mLimitAngleMin;
			refP.mIKBone[h].mLinks[i].mLimitAngleMax =refBone.mIKLinks[i].mLimitAngleMax;
		}
	}


	// ロード成功
	refP.mEnabled =Task::Success;
}

// 画像ファイル
void Resource::CreateFromPicture(Resource &resource, int handle)
{
	PolygonData& refP =resource.mPolygons.at(handle);

	// メッシュの作成
	refP.mMesh.resize(1);
	refP.mMesh.shrink_to_fit();
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	D3DX11_IMAGE_INFO imageInfo;
	ZeroMemory(&imageInfo, sizeof(imageInfo));

	// シェーダリソースビューの設定
	if(FAILED(D3DX11GetImageInfoFromFileW(refP.mFilePath, NULL, &imageInfo, NULL)))
	{
		refP.mEnabled =Task::Failed;
		return;
	}
	loadInfo.Width =imageInfo.Width;
	loadInfo.Height =imageInfo.Height;
	loadInfo.Depth =imageInfo.Depth;
	loadInfo.FirstMipLevel =0;
	loadInfo.MipLevels =imageInfo.MipLevels;
	loadInfo.Usage =D3D11_USAGE_DEFAULT;
	loadInfo.BindFlags =D3D11_BIND_SHADER_RESOURCE;
	loadInfo.CpuAccessFlags =NULL;
	loadInfo.MiscFlags =NULL;
	loadInfo.Format =DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	loadInfo.Filter =D3DX11_FILTER_LINEAR|D3DX11_FILTER_SRGB_IN;
	loadInfo.MipFilter =D3DX11_FILTER_LINEAR;
	loadInfo.pSrcInfo =&imageInfo;

	// シェーダリソースビューの作成
	if(FAILED(D3DX11CreateShaderResourceViewFromFileW(resource.mGraphicM.mpDevice, refP.mFilePath, &loadInfo, NULL, &refP.mMesh[0].mpTexture, nullptr)))
	{
		refP.mEnabled =Task::Failed;
		return;
	}
		
	// メッシュの頂点数
	refP.mMesh[0].mVertexNum =6;

	// メッシュのスペキュラー係数
	refP.mMesh[0].mSpecular =0.0f;

	// 頂点バッファの作成
	D3D11_BUFFER_DESC vertexDesc;
	vertexDesc.Usage =D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth =sizeof(math::Float3) *4;
	vertexDesc.BindFlags =D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags =0;
	vertexDesc.MiscFlags =0;
	vertexDesc.StructureByteStride =0;
	float magnifX =0.5f /resource.mGraphicM.mWindows.GetWindowSizeX(),
		  magnifY =0.5f /resource.mGraphicM.mWindows.GetWindowSizeY();
	math::Float3 vertices[4] ={
		{-float(imageInfo.Width)*magnifX, float(imageInfo.Height)*magnifY, 0.0f}, {float(imageInfo.Width)*magnifX, float(imageInfo.Height)*magnifY, 0.0f},
		{-float(imageInfo.Width)*magnifX, -float(imageInfo.Height)*magnifY, 0.0f}, {float(imageInfo.Width)*magnifX, -float(imageInfo.Height)*magnifY, 0.0f},
	};
	D3D11_SUBRESOURCE_DATA vertexBufferSub;
	vertexBufferSub.pSysMem =&vertices[0];
	vertexBufferSub.SysMemPitch =0;
	vertexBufferSub.SysMemSlicePitch =0;
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexDesc, &vertexBufferSub, &refP.mpVertexBuffers[0])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}

	// UV座標の作成
	vertexDesc.ByteWidth =sizeof(math::Float2) *4;
	math::Float2 uv[4] ={
		{0.0f, 0.0f}, {1.0f, 0.0f},
		{0.0f, 1.0f}, {1.0f, 1.0f}
	};
	vertexBufferSub.pSysMem =&uv[0];
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&vertexDesc, &vertexBufferSub, &refP.mpVertexBuffers[1])))
	{
		refP.mEnabled =Task::Failed;
		return;
	}

	// 法線バッファは無し

	// インデックスバッファの作成
	D3D11_BUFFER_DESC indexBuffer;
	indexBuffer.Usage =D3D11_USAGE_DEFAULT;
	indexBuffer.ByteWidth =sizeof(unsigned) *6;
	indexBuffer.BindFlags =D3D11_BIND_INDEX_BUFFER;
	indexBuffer.CPUAccessFlags =0;
	indexBuffer.MiscFlags =0;
	indexBuffer.StructureByteStride =0;
	unsigned indices[6] ={
		0, 1, 2,
		2, 1, 3,
	};
	D3D11_SUBRESOURCE_DATA indexBufferSub;
	indexBufferSub.pSysMem =&indices[0];
	indexBufferSub.SysMemPitch =0;
	indexBufferSub.SysMemSlicePitch =0;
	if(FAILED(resource.mGraphicM.mpDevice->CreateBuffer(&indexBuffer, &indexBufferSub, &refP.mpIndexBuffer)))
	{
		refP.mEnabled =Task::Failed;
		return;
	}
	// インデックスバッファの数を格納
	refP.mIndexCount =6;


	// ロード成功
	refP.mEnabled =Task::Success;
}


// VMDファイル
void Resource::CreateFromVMD(Resource& resource, int handle)
{
	FileLoader loader;
	VMDData data;
	FrameData frameData;
	unsigned j=0, paramNum =0;

	MotionData& refA =resource.mMotions.at(handle);

	if(!loader.VMD(data, refA.mFilePath))
	{
		// ロードが失敗した場合
		refA.mEnabled =Task::Failed;
		return;
	}

	// 読み込み
	for(unsigned i=0; i<data.mFrameData.size(); ++i)
	{
		// フレーム番号
		frameData.mFrameNumber =data.mFrameData[i].mFrameNumber;
		// ボーン位置
		frameData.mBonePos =data.mFrameData[i].mBonePos;
		// ボーン回転クォータニオン
		frameData.mBoneQuaternion =data.mFrameData[i].mBoneQuaternion;
		// ベジェ曲線制御点パラメーター
		paramNum =0;
		for(j=0; j<4; ++j)
		{
			frameData.mXInterpolation[j] =static_cast<float>(data.mFrameData[i].mInterpolationParam[paramNum]);
			frameData.mYInterpolation[j] =static_cast<float>(data.mFrameData[i].mInterpolationParam[++paramNum]);
			frameData.mZInterpolation[j] =static_cast<float>(data.mFrameData[i].mInterpolationParam[++paramNum]);
			frameData.mRInterpolation[j] =static_cast<float>(data.mFrameData[i].mInterpolationParam[++paramNum]);
			++paramNum;
		}

		// ベクターに追加
		refA.mFrameData[data.mFrameData[i].mBoneName].first.emplace_back(frameData);
	}

	// モーションデータをキーフレーム基準で整列
	for(auto& data : refA.mFrameData)
	{
		std::sort(data.second.first.begin(), data.second.first.end());

		//モーションデータの最終キーフレームを取得
		if(refA.mLastKeyFrame < data.second.first[data.second.first.size()-1].mFrameNumber)
		{
			refA.mLastKeyFrame =data.second.first[data.second.first.size()-1].mFrameNumber;
		}
	}


	// ロード成功
	refA.mEnabled =Task::Success;
}


// WAVEファイル
void Resource::CreateFromWAVE(Resource& resource, int handle)
{
	FileLoader loader;

	SoundData& refS =resource.mSounds.at(handle);

	if(!loader.WAV(refS.mSoundData, refS.mFilePath))
	{
		// ロードが失敗した場合
		refS.mEnabled =Task::Failed;
		return;
	}

	// Get format of wave file
	CWE::WAVData::WAVFormat &fmt =refS.mSoundData.mFormat;
	WAVEFORMATEX wfx ={};
	wfx.wFormatTag =WAVE_FORMAT_PCM;
	wfx.nChannels =fmt.mChannelsNum;
	wfx.nSamplesPerSec =fmt.mSamplesPerSec;
	wfx.wBitsPerSample =fmt.mBitsPerSample;
	wfx.nBlockAlign =wfx.nChannels * wfx.wBitsPerSample / 8;
	wfx.nAvgBytesPerSec =wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize =0;

	// Create the source voice
	if(FAILED(resource.mSoundM.mpXAudio2->CreateSourceVoice(&refS.mpSourceVoice, &wfx)))
	{
		refS.mEnabled =Task::Failed;
		return;
	}

	// Submit the wave sample data
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData =reinterpret_cast<const BYTE*>(refS.mSoundData.mpData.get());
	buffer.AudioBytes =refS.mSoundData.mDataSize;
	buffer.Flags =XAUDIO2_END_OF_STREAM;
	if(FAILED(refS.mpSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		refS.mEnabled =Task::Failed;
		return;
	}

	refS.mBuffer =buffer;

	// ロード成功
	refS.mEnabled =Task::Success;
}


// ヒエラルキーソート用equal_range
void Resource::EqualRange(unsigned* lower, unsigned* upper, int value, const std::vector<Bone*>& tmpB)
{
	unsigned last=0, len=0, half=0, mid=0;

	//lower_range
	*lower =0;
	last =tmpB.size();
	for(len =last-*lower; len != 0; )
	{
		half =len /2;
		mid =*lower;
		mid +=half;
		if(tmpB[mid]->mFather < value)
		{
			len -= half + 1;
			*lower =++mid;
		}
		else
		{
			len =half;
		}
	}

	//upper_range
	*upper =0;
	for (unsigned len =last-*upper; len != 0; )
	{
		half =len /2;
		mid =*upper;
		mid +=half;
		if(!(value < tmpB[mid]->mFather))
		{
			len -=half+1;
			*upper =++mid;
		}
		else
		{
			len =half;
		}
	}
}




}// CWE