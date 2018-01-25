#include "CWEFileLoader.h"
#include <algorithm>
#include <array>



namespace CWE
{


bool FileLoader::PMXGetString16(std::wstring &dest)
{
	wchar_t wBuffer[512] ={};
	int textSize;

	mFile.read(reinterpret_cast<char*>(&textSize), 4);

	mFile.read(reinterpret_cast<char*>(&wBuffer), textSize);
	dest =std::wstring(wBuffer, wBuffer+textSize/2);

	return true;
}


void FileLoader::DirectoryExtraction(wchar_t directoryPath[], const wchar_t fullPath[])
{
	const int length =wcslen(fullPath);

	DirectoryExtraction(directoryPath, fullPath, length);
}

void FileLoader::DirectoryExtraction(wchar_t directoryPath[], const wchar_t fullPath[], unsigned fullPathLen)
{
	if(!directoryPath) {return;}

	for(unsigned i=1; i<=fullPathLen; ++i)
	{
		if(fullPath[fullPathLen-i] == L'\\')
		{
			for(unsigned j=0; j<fullPathLen-i+1; ++j)
			{
				directoryPath[j] =fullPath[j];
			}
		}
	}
}


bool FileLoader::PMX(PMXData &data, const wchar_t file[])
{
	if(!file)
	{
		return false;
	}
	if(512 < wcslen(file))
	{
		return false;
	}

	wchar_t directoryPath[512]{};
	DirectoryExtraction(directoryPath, file);

	data.mVertices.clear();
	data.mSurfaces.clear();
	data.mTexturePaths.clear();
	data.mMaterials.clear();

	mFile.open(file, (std::ios::binary|std::ios::in));
	if(mFile.fail())
	{
		mFile.close();
		return false;
	}

	std::array<unsigned char, 4> pmxHeder{};
	const std::array<unsigned char, 4> magicNumberPMX{0x50, 0x4d, 0x58, 0x20};
	float version{};
	enum HeaderData
	{
		EncodnigFormat =0,
		NumberOfAddUV,
		VertexIndexSize,
		TextureIndexSize,
		MaterialIndexSize,
		BoneIndexSize,
		RigidBodyIndexSize,
		NumberOfHeaderData
	};
	unsigned char hederDataLength{};
	std::array<unsigned char, 8> hederData{};
	unsigned arrayLength{};
	int vertexNum{};
	unsigned char weightCMethod{};
	int surfaceNum{};
	int textureNum{};
	std::wstring path;
	int materialNum{};
	unsigned char shareToonFlag{};
	int boneNum{};


	for(int i=0; i<4; ++i)
	{
		pmxHeder[i] =mFile.get();
	}
	if(pmxHeder != magicNumberPMX)
	{
		mFile.close(); return false;
	}
	//ver2.0à»äOÇÕîÒëŒâû
	mFile.read(reinterpret_cast<char*>(&version), 4);
	if(!math::NearlyEqual(version, 2.0f))
	{
		mFile.close();
		return false;
	}

	hederDataLength =mFile.get();
	if(hederDataLength != 8)
	{
		mFile.close();
		return false;
	}
	for(int i=0; i<8; ++i)
	{
		hederData[i] =mFile.get();
	}
	//UTF-8ÇÕîÒëŒâû
	if(hederData[0] != 0)
	{
		mFile.close();
		return false;
	}

	for(int j=0; j<4; ++j)
	{
		mFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		for(unsigned i=0; i<arrayLength; ++i)
		{
			mFile.get();
		}
	}

	//Vertex
	using Vertex =PMXData::Vertex;
	mFile.read(reinterpret_cast<char*>(&vertexNum), 4);

	data.mVertices.resize(vertexNum);
	if(hederData[NumberOfAddUV] != 0)
	{
		for(int i=0; i<vertexNum; ++i)
		{
			data.mVertices[i].mAdditionalUV.resize(hederData[NumberOfAddUV]);
		}
	}

	for(int i=0; i<vertexNum; ++i)
	{
		mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mVertex), 12);
		mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mNormal), 12);
		mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mUV), 8);
		if(hederData[NumberOfAddUV] != 0)
		{
			for(int j=0; j<hederData[NumberOfAddUV]; ++j)
			{
				mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mAdditionalUV[j]), 16);
			}
		}
		weightCMethod =mFile.get();

		switch(weightCMethod)
		{
		case Vertex::Weight::BDEF1:
			data.mVertices[i].mWeight.mType =Vertex::Weight::BDEF1;
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn1), hederData[BoneIndexSize]);
			data.mVertices[i].mWeight.mBorn2 =-1;
			data.mVertices[i].mWeight.mBorn3 =-1;
			data.mVertices[i].mWeight.mBorn4 =-1;
			data.mVertices[i].mWeight.mWeight1 =1.0f;
			break;

		case Vertex::Weight::BDEF2:
			data.mVertices[i].mWeight.mType =Vertex::Weight::BDEF2;
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn1), hederData[BoneIndexSize]);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn2), hederData[BoneIndexSize]);
			data.mVertices[i].mWeight.mBorn3 =-1;
			data.mVertices[i].mWeight.mBorn4 =-1;
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mWeight1), 4);
			data.mVertices[i].mWeight.mWeight2 =1.0f -data.mVertices[i].mWeight.mWeight1;
			break;

		case Vertex::Weight::BDEF4:
			data.mVertices[i].mWeight.mType =Vertex::Weight::BDEF4;
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn1), hederData[BoneIndexSize]);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn2), hederData[BoneIndexSize]);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn3), hederData[BoneIndexSize]);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn4), hederData[BoneIndexSize]);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mWeight1), 4);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mWeight2), 4);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mWeight3), 4);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mWeight4), 4);
			break;

		case Vertex::Weight::SDEF:
			data.mVertices[i].mWeight.mType =Vertex::Weight::SDEF;
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn1), hederData[BoneIndexSize]);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mBorn2), hederData[BoneIndexSize]);
			data.mVertices[i].mWeight.mBorn3 =-1;
			data.mVertices[i].mWeight.mBorn4 =-1;
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mWeight1), 4);
			data.mVertices[i].mWeight.mWeight2 =1.0f -data.mVertices[i].mWeight.mWeight1;
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mC), 12);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mR0), 12);
			mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mWeight.mR1), 12);
			break;

		default:
			mFile.close();
			return false;
		}

		mFile.read(reinterpret_cast<char*>(&data.mVertices[i].mEdgeMagnif), 4);

		if(data.mVertices[i].mWeight.mBorn1==-1)
		{
			mFile.close();
			return false;
		}
	}

	//Surface
	mFile.read(reinterpret_cast<char*>(&surfaceNum), 4);

	data.mSurfaces.resize(surfaceNum);

	for(int i=0; i<surfaceNum; ++i)
	{
		mFile.read(reinterpret_cast<char*>(&data.mSurfaces[i].mVertexIndex), hederData[VertexIndexSize]);

		if(data.mSurfaces[i].mVertexIndex==-1 || data.mSurfaces[i].mVertexIndex==-1 || data.mSurfaces[i].mVertexIndex==-1)
		{
			mFile.close();
			return false;
		}
	}

	//Texture
	mFile.read(reinterpret_cast<char*>(&textureNum), 4);

	data.mTexturePaths.resize(textureNum);
	for(int i=0; i<textureNum; ++i)
	{
		PMXGetString16(path);
		data.mTexturePaths[i] =directoryPath;
		data.mTexturePaths[i] +=path;
	}

	//Material
	mFile.read(reinterpret_cast<char*>(&materialNum), 4);

	data.mMaterials.resize(materialNum);
	for(int i=0; i<materialNum; ++i)
	{
		for(int j=0; j<2; ++j)
		{
			mFile.read(reinterpret_cast<char*>(&arrayLength), 4);
			for(unsigned i=0; i<arrayLength; ++i)
			{
				mFile.get();
			}
		}

		mFile.read(reinterpret_cast<char*>(&data.mMaterials[i].mDiffuse), 16);
		mFile.read(reinterpret_cast<char*>(&data.mMaterials[i].mSpecular), 12);
		mFile.read(reinterpret_cast<char*>(&data.mMaterials[i].mSpecularity), 4);
		mFile.read(reinterpret_cast<char*>(&data.mMaterials[i].mAmbient), 12);

		mFile.get();
		for(int i=0; i<16; ++i)
		{
			mFile.get();
		}
		for(int i=0; i<4; ++i)
		{
			mFile.get();
		}

		mFile.read(reinterpret_cast<char*>(&data.mMaterials[i].mColorMapTextureIndex), hederData[TextureIndexSize]);
		for(unsigned char i=0; i<hederData[TextureIndexSize]; ++i)
		{
			mFile.get();
		}
		mFile.get();

		shareToonFlag =mFile.get();
		if(shareToonFlag)
		{
			mFile.get();
		}
		else
		{
			mFile.read(reinterpret_cast<char*>(&data.mMaterials[i].mToonTextureIndex), hederData[TextureIndexSize]);
		}

		mFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		for(unsigned i=0; i<arrayLength; ++i)
		{
			mFile.get();
		}

		mFile.read(reinterpret_cast<char*>(&data.mMaterials[i].mVertexNum), 4);
	}

	//Bone
	mFile.read(reinterpret_cast<char*>(&boneNum), 4);

	data.mBones.resize(boneNum);
	int ikLinkSize =0;
	unsigned char angleLim =0;
	for(int i=0; i<boneNum; ++i)
	{
		PMXGetString16(data.mBones[i].mName);
		mFile.read(reinterpret_cast<char*>(&arrayLength), 4);
		data.mBones[i].mNameE.resize(arrayLength);
		for(unsigned j=0; j<arrayLength; ++j)
		{
			data.mBones[i].mNameE[j] =mFile.get();
		}
		
		mFile.read(reinterpret_cast<char*>(&data.mBones[i].mPosition), 12);

		mFile.read(reinterpret_cast<char*>(&data.mBones[i].mFatherIndex), hederData[BoneIndexSize]);
		if(boneNum <= data.mBones[i].mFatherIndex)
		{
			data.mBones[i].mFatherIndex =-1;
		}
		
		mFile.read(reinterpret_cast<char*>(&data.mBones[i].mTransformationLevel), 4);
		
		mFile.read(reinterpret_cast<char*>(&data.mBones[i].mFlag), 2);

		enum BoneFlagMask
		{
			AccessPoint =0x0001,
			IK =0x0020,
			ImpartTranslation =0x0100,
			ImpartRotation =0x0200,
			AxisFixing =0x0400,
			LocalAxis =0x0800,
			ExternalParentTrans =0x2000,
		};
		if(data.mBones[i].mFlag & AccessPoint)
		{
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mSonIndex), hederData[BoneIndexSize]);
			if(boneNum <= data.mBones[i].mSonIndex)
			{
				data.mBones[i].mSonIndex =-1;
			}
		}
		else
		{
			data.mBones[i].mSonIndex =-1;
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mCoordOffset), 12);
		}
		if((data.mBones[i].mFlag & ImpartTranslation) || (data.mBones[i].mFlag & ImpartRotation))
		{
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mImpartParentIndex), hederData[BoneIndexSize]);
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mImpartRate), 4);
		}
		if(data.mBones[i].mFlag & AxisFixing)
		{
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mFixedAxis), 12);
		}
		if(data.mBones[i].mFlag & LocalAxis)
		{
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mLocalAxisX), 12);
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mLocalAxisZ), 12);
		}
		if(data.mBones[i].mFlag & ExternalParentTrans)
		{
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mExternalParentKey), 4);
		}
		if(data.mBones[i].mFlag & IK)
		{
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mIKTargetIndex), hederData[5]);
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].mIKLoopCount), 4);
			mFile.read(reinterpret_cast<char*>(&data.mBones[i].m_IKUnitAngle), 4);
			mFile.read(reinterpret_cast<char*>(&ikLinkSize), 4);
			data.mBones[i].mIKLinks.resize(ikLinkSize);
			for(int j=0; j<ikLinkSize; ++j)
			{
				mFile.read(reinterpret_cast<char*>(&data.mBones[i].mIKLinks[j].mIndex), hederData[5]);
				angleLim =mFile.get();
				data.mBones[i].mIKLinks[j].mExistAngleLimited =false;
				if(angleLim == 1)
				{
					mFile.read(reinterpret_cast<char*>(&data.mBones[i].mIKLinks[j].mLimitAngleMin), 12);
					mFile.read(reinterpret_cast<char*>(&data.mBones[i].mIKLinks[j].mLimitAngleMax), 12);
					data.mBones[i].mIKLinks[j].mExistAngleLimited =true;
				}
			}
		}
		else
		{
			data.mBones[i].mIKTargetIndex =-1;
		}
	}

	mFile.close();

	return true;
}


bool FileLoader::VMD(VMDData& data, const wchar_t file[])
{
	if(!file)
	{
		return false;
	}

	data.mFrameData.clear();

	mFile.open(file, (std::ios::binary|std::ios::in));
	if(mFile.fail())
	{
		mFile.close();
		return false;
	}

	//Header
	std::array<char, 30> magicNumber{};
	std::array<char, 30> magicNumberSample{"Vocaloid Motion Data 0002"};
	for(unsigned i=0; i<30; ++i)
	{
		magicNumber[i] =mFile.get();
	}
	if(magicNumber != magicNumberSample)
	{
		mFile.close();
		return false;
	}
	for(unsigned i=0; i<20; ++i)
	{
		mFile.get();
	}
	unsigned frameDataNum{};
	mFile.read(reinterpret_cast<char*>(&frameDataNum), 4);

	//Frame data
	data.mFrameData.resize(frameDataNum);
	char boneNameMulti[15]{};
	wchar_t boneNameWide[15]{};
	size_t convertedChars{};
	setlocale(LC_ALL, "jpn");
	for(unsigned i=0; i<frameDataNum; ++i)
	{
		for(unsigned j=0; j<15; ++j)
		{
			boneNameMulti[j] =mFile.get();
		}
		mbstowcs_s(&convertedChars, boneNameWide, 15, boneNameMulti, _TRUNCATE);
		data.mFrameData[i].mBoneName =std::wstring(boneNameWide, boneNameWide+wcslen(boneNameWide));
		convertedChars =0;
		mFile.read(reinterpret_cast<char*>(&data.mFrameData[i].mFrameNumber), 4);
		mFile.read(reinterpret_cast<char*>(&data.mFrameData[i].mBonePos), 12);
		mFile.read(reinterpret_cast<char*>(&data.mFrameData[i].mBoneQuaternion), 16);
		for(unsigned j=0; j<64; ++j)
		{
			data.mFrameData[i].mInterpolationParam[j] =mFile.get();
		}
	}

	setlocale(LC_ALL, "C");

	mFile.close();

	return true;
}


bool FileLoader::WAV(WAVData& data, const wchar_t file[])
{
	// Chunk data
	struct WAVChunk
	{
		char mID[4];
		int m_Size;
	};

	WAVChunk chunk;
	char type[4] ={};
	WAVData result;

	if(wcsnlen_s(file, 1) == 0)
	{
		return false;
	}

	mFile.open(file, (std::ios::binary|std::ios::in));
	if(mFile.fail())
	{
		mFile.close();
		return false;
	}

	// Chunk info
	mFile.read(reinterpret_cast<char*>(&chunk), 8);
	if(strncmp(chunk.mID, "RIFF", 4) != 0)
	{
		return false;
	}

	// File type
	mFile.read(reinterpret_cast<char*>(type), 4);
	if(strncmp(type, "WAVE", 4) != 0)
	{
		return false;
	}

	// Format
	mFile.read(reinterpret_cast<char*>(&chunk), 8);
	if(strncmp(chunk.mID, "fmt ", 4) != 0)
	{
		return false;
	}
	mFile.read(reinterpret_cast<char*>(&result.mFormat), 16);
	if(result.mFormat.mID != 1)
	{
		return false;
	}
	if(chunk.m_Size != 16)
	{
		mFile.seekg(chunk.m_Size -16, std::ios::cur);
	}

	// Data
	mFile.read(reinterpret_cast<char*>(&chunk), 8);
	if(strncmp(chunk.mID, "fact", 4) == 0)
	{
		mFile.seekg(chunk.m_Size, std::ios::cur);
		mFile.read(reinterpret_cast<char*>(&chunk), 8);
	}
	if(strncmp(chunk.mID, "data", 4) == 0)
	{
		result.mDataSize =chunk.m_Size;
		result.mpData.reset(new unsigned char [result.mDataSize]);
		mFile.read(reinterpret_cast<char*>(&result.mpData[0]), result.mDataSize);
	}
	else
	{
		return false;
	}

	//ê¨å˜
	data =std::move(result);
	return true;
}



}// CWE