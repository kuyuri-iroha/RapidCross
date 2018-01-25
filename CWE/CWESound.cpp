#include "CWESound.h"
#include "CWEUsefullyMacro.h"


namespace CWE
{

MasterSound::MasterSound()
{
}

MasterSound::~MasterSound()
{
	SafeDestroy(mpMasteringVoice);
	SAFE_RELEASE(mpXAudio2);
}


MasterSound& MasterSound::GetInstance()
{
	static MasterSound instance;
	return instance;
}


bool MasterSound::Init()
{
	UINT32 flags =NULL;
#ifdef _DEBUG
	flags |=XAUDIO2_DEBUG_ENGINE;
#endif // _DEBUG


	// コア
	if(FAILED(XAudio2Create(&mpXAudio2, flags)))
	{
		return false;
	}

	// マスターボイス
	if(FAILED(mpXAudio2->CreateMasteringVoice(&mpMasteringVoice)))
	{
		return false;
	}

	return true;
}


void MasterSound::End()
{
	SafeDestroy(mpMasteringVoice);
	SAFE_RELEASE(mpXAudio2);
}



Sound::Sound():
	mResource(Resource::GetInstance()), mHandle()
{
}

Sound::Sound(const Sound &original):
	mResource(Resource::GetInstance())
{
	Copy(original);
}

Sound& Sound::operator=(const Sound& original)
{
	Copy(original);
	return *this;
}

Sound::~Sound()
{
	Release();
}


// Load file.
void Sound::Load(const wchar_t *filePath, const FileFormat &format)
{
	mHandle =mResource.Create(filePath, format);
}

// Get the Sound is live.
bool Sound::Is() const
{
	return mResource.IsSound(mHandle);
}


// Get load result.
const Task& Sound::LoadResult()const
{
	const SoundData& data =mResource.GetSoundData(mHandle);
	return data.mEnabled;
}

// Copy data.
void Sound::Copy(const Sound& original)
{
	mHandle =mResource.CopySoundHandle(original.mHandle);
}


// Release data.
void Sound::Release()
{
	mResource.SoundRelease(mHandle);
}


// Get now volume
float Sound::GetVolume() const
{
	SoundData& data =mResource.GetSoundData(mHandle);
	if(data.mEnabled != Task::Success) {return 0.0f;}

	float result{};
	data.mpSourceVoice->GetVolume(&result);
	return result;
}


// Set the volume
bool Sound::SetVolume(float volume)
{
	SoundData& data =mResource.GetSoundData(mHandle);
	if(data.mEnabled != Task::Success) {return false;}

	return SUCCEEDED(data.mpSourceVoice->SetVolume(volume));
}


bool Sound::Start(bool loop, unsigned loopBegin, unsigned loopLength)
{
	SoundData& data =mResource.GetSoundData(mHandle);
	if(data.mEnabled != Task::Success) {return false;} //正規データでなければ却下

	Stop();
	data.mpSourceVoice->FlushSourceBuffers();

	data.mBuffer.LoopCount =loop ? XAUDIO2_LOOP_INFINITE : 0;
	data.mBuffer.LoopBegin =loopBegin;
	data.mBuffer.LoopLength =loopLength;

	if(FAILED(data.mpSourceVoice->SubmitSourceBuffer(&(data.mBuffer))))
	{
		return false;
	}

	return SUCCEEDED(data.mpSourceVoice->Start());
}


bool Sound::Stop()
{
	SoundData& data =mResource.GetSoundData(mHandle);
	if(data.mEnabled != Task::Success) {return false;} //正規データでなければ却下
	return SUCCEEDED(data.mpSourceVoice->Stop());
}


bool Sound::Ended()
{
	SoundData& data =mResource.GetSoundData(mHandle);
	if(data.mEnabled != Task::Success) {return true;} //正規データでなければ却下

	XAUDIO2_VOICE_STATE state;
	data.mpSourceVoice->GetState(&state);
	return !(state.BuffersQueued > 0);
}


}