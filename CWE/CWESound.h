#pragma once

#include "CWEFileLoader.h"
#include <list>
#include "CWEGraphicResource.h"
#include "CWETypeDefinition.h"

namespace CWE
{

class Graphic;

// User sound class
class Sound
{
	friend class MasterSound;

private:
	Resource& mResource;
	ResourceHandle mHandle;

public:
	Sound();
	Sound(const Sound &original);
	Sound& operator=(const Sound& original);
	~Sound();
	
	// ファイルのロード(新規リソースの生成)
	void Load(const wchar_t *filePath, const FileFormat &format);
	// ハンドルの参照先が存在するかどうか
	bool Is() const;
	// 現在のハンドルの参照先が使用可能かどうか
	const Task& LoadResult()const;
	// データのコピー
	void Copy(const Sound &original);
	// データの開放
	void Release();

	float GetVolume() const;
	bool SetVolume(float volume);
	bool Start(bool loop, unsigned loopBegin, unsigned loopLength);
	bool Stop();
	bool Ended();

};


// Sounds operator
class MasterSound
{
public:
	static MasterSound& GetInstance();
	bool Init();
	void End();

private:
	IXAudio2* mpXAudio2;
	IXAudio2MasteringVoice* mpMasteringVoice;

	MasterSound();
	~MasterSound();

	friend class CWE::Resource;

};


}