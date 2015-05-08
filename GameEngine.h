//-----------------------------------------------------------------
// Game Engine Object
// C++ Header - GameEngine.h
// by: WR
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include "Sprite.h"

#pragma comment(lib,"winmm.lib")

//-----------------------------------------------------------------
// Windows Function Declarations
//-----------------------------------------------------------------
int		WINAPI    WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
						  PSTR szCmdLine, int iCmdShow);
LRESULT CALLBACK  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//---------------------------------------------------------------
// Custom data structures
//---------------------------------------------------------------
typedef	WORD		JOYSTATE;
const	JOYSTATE	JOY_NONE  = 0x0000L,
	JOY_LEFT  = 0x0001L,
	JOY_RIGHT = 0x0002L,
	JOY_UP	  = 0x0004L,
	JOY_DOWN  = 0x0008L,
	JOY_FIRE1 = 0x0010L,
	JOY_FIRE2 = 0x0020L;

//-----------------------------------------------------------------
// Play Music Type
//-----------------------------------------------------------------
typedef LPCSTR		MUSICTYPE;
const	MUSICTYPE	MUSIC_NONE = "",
	MUSIC_ALL = (LPCTSTR)MCI_ALL_DEVICE_ID,
	MUSIC_MIDI = "sequencer",
	MUSIC_MP3 = "mpegvideo",
	MUSIC_AVI = "avivideo",
	MUSIC_WAV = "waveaudio";

//-----------------------------------------------------------------
// GameEngine Class
//-----------------------------------------------------------------
class GameEngine
{
protected:
	// Member Variables
	HINSTANCE				m_hInstance;
	HWND					m_hWindow;
	TCHAR					m_szWindowClass[32];
	TCHAR					m_szTitle[32];
	WORD					m_wIcon, m_wSmallIcon;
	int						m_iWidth, m_iHeight;
	int						m_iFrameDelay;		// ms
	BOOL					m_bSleep;
	UINT					m_uiJoystickID[2];  // 支持2个游戏杆
	RECT					m_rcJoystickTrip;
	std::vector<SpritePtr>	m_vSprites;			// 精灵Vector
	HDC						m_hOffscreenDC;		// 缓存画板
	HBITMAP					m_hOffscreenBitmap;
	UINT					m_uiPlayerID;   // Player设备ID

public:
	//-----------------------------------------------------------------
	// Use in WinMain
	//-----------------------------------------------------------------
	// Constructor(s)/Destructor
	GameEngine(HINSTANCE hInstance, LPTSTR szWindowClass, LPTSTR szTitle,
		WORD wIcon, WORD wSmallIcon, int iWidth = 640, int iHeight = 480, int iFrameRate = 30);
	virtual ~GameEngine();

	BOOL Initialize(int iCmdShow);

	void Run();

	//-----------------------------------------------------------------
	// Use in WndProc
	//-----------------------------------------------------------------
	LRESULT HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------
	// Game Engine Function Declarations, need to be overwrite by user
	//-----------------------------------------------------------------
	virtual void GameStart(HWND hWindow) = 0;
	virtual void GameEnd() = 0;
	virtual void GameActivate(HWND hWindow) = 0;
	virtual void GameDeactivate(HWND hWindow) = 0;
	virtual void GamePaint(HDC hDC) = 0;
	virtual void GameCycle() = 0;
	virtual void HandleKeys() = 0;
	virtual void MouseButtonDown(int x, int y, BOOL bLeft) = 0;
	virtual void MouseButtonUp(int x, int y, BOOL bLeft) = 0;
	virtual void MouseMove(int x, int y) = 0;
	virtual void HandleJoystick(UINT uiJoystickID, JOYSTATE jsJoystickState) = 0;
	virtual BOOL SpriteCollision(SpritePtr hitter, SpritePtr hittee) = 0;

	//----------------------------------------------------------------
	// Sprites Management Methods
	//----------------------------------------------------------------
	void		AddSprite(SpritePtr pSprite);
	void		DrawSprites(HDC hDC);
	void		UpdateSprites();
	void		CleanupSprites();
	SpritePtr	IsPointInSprite(int x, int y);

	//----------------------------------------------------------------
	// Joystick Methods
	//----------------------------------------------------------------
	BOOL		InitJoystick();
	void		CaptureJoystick();
	void		ReleaseJoystick();
	void		CheckJoystick();

	//----------------------------------------------------------------
	// Play Music Methods
	//----------------------------------------------------------------
	void		PlayMusic(LPTSTR szFileName = TEXT(""), BOOL bRestart = TRUE);
	void		PauseMusic();
	void		CloseMusic();

	//----------------------------------------------------------------
	// Accessor Methods
	//----------------------------------------------------------------
	HINSTANCE GetInstance()				{ return m_hInstance; };
	HWND      GetWindow()				{ return m_hWindow; };
	void      SetWindow(HWND hWindow)	{ m_hWindow = hWindow; };
	LPTSTR    GetTitle()				{ return m_szTitle; };
	WORD      GetIcon()					{ return m_wIcon; };
	WORD      GetSmallIcon()			{ return m_wSmallIcon; };
	int       GetWidth()				{ return m_iWidth; };
	int       GetHeight()				{ return m_iHeight; };
	int       GetFrameDelay()			{ return m_iFrameDelay; };
	void      SetFrameRate(int iFrameRate) { m_iFrameDelay = 1000 / iFrameRate; };
	BOOL      GetSleep()				{ return m_bSleep; };
	void      SetSleep(BOOL bSleep)		{ m_bSleep = bSleep; };

private:
	//----------------------------------------------------------------
	// Help Methods
	//----------------------------------------------------------------
	BOOL	  CheckSpriteCollision(SpritePtr pTestSprite);
};