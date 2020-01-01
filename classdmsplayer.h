#pragma once
#include <windows.h>
#include <Dsound.h>
#include <atomic>
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")
#pragma comment  (lib,"Dsound.lib")
#pragma comment  (lib,"Dxguid.lib")

struct DmspSound
{
	LPBYTE base;
	INT32 offset;
	INT32 size;	
	INT32 loop; //Enable only DSBGMPlayer,2nd offset of loop,adress:base + offset + loop ;-1 is not loop
};
class DSCreated
{
private:
	WAVEFORMATEX Format;
	DWORD EnableLevel;
	LPDIRECTSOUND pDSound;
	volatile UINT32 Reference;
	~DSCreated();
public:
	void AddRef();
	void Release();
	DSCreated(HWND hwnd);
	WAVEFORMATEX* GetFormatPtr();
	LPDIRECTSOUND GetDSPtr();
	DWORD GetEnableLevel();
	UINT32 GetReferenceNum();
};
class DMSPlayer
{
protected:
	DSBUFFERDESC DmspDesc;
	LPDIRECTSOUNDBUFFER DSBuffer;
	LPVOID pDestBuffer;
	DmspSound Sound;
	DSCreated* dsUsed;
	volatile std::atomic<BYTE> Playing;
	volatile std::atomic<BYTE> ThreadWait;
	volatile std::atomic<BYTE> ReadyForExit;
	volatile std::atomic<BYTE> EnableLevel;
	DMSPlayer(DSCreated* dsCreated, UINT32 BufferSeconds);
	void SafeSleep(INT32 nms);
public:

	static const UINT32 nAvgBytesPerSecond = 176400;
	virtual ~DMSPlayer();
	void SetVolume(INT32 Volume);
	INT32 GetVolume();
	BOOL isEnable();//if created successful
	BOOL isPlaying();
};
class DSPlayer : public DMSPlayer
{
protected:
	void PlayThread();
	static void PlayThreadCaller(LPVOID ThisPtr);
public:
	void Replay();	//stop and replay immediately
	void Play(DmspSound s); //Replace and play immediately 
	void Stop();
	void Pause();
	void Continue();
	DSPlayer(DSCreated* dsCreated, UINT32 MaximumSeconds = 10);
	virtual ~DSPlayer();
};
class DSBGMPlayer : public DMSPlayer
{
protected:
	volatile std::atomic<INT32> MCursor;
	DmspSound MCStandBy[2];
	volatile std::atomic<INT32> PauseCursor;
	DmspSound PauseBGM;
	DWORD BGMBufferSeconds;
	void PlayThread();
	static void PlayThreadCaller(LPVOID ThisPtr);
	volatile std::atomic<BYTE*> ActivedBuffer;
public:
	BOOL IsSourceBufferSafeToRelease(BYTE* Base);;
	void ChangeAndPlay(DmspSound s); //Replace and play NOT immediately 
	void Stop();	//NOT immediately
	void Pause();	//NOT immediately
	void Continue();	//NOT immediately
	DSBGMPlayer(DSCreated* dsCreated, UINT32 BufferSeconds = 1);
	virtual ~DSBGMPlayer();
};

