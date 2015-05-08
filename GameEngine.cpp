
#include "GameEngine.h"
#include <Digitalv.h>

GameEngine::GameEngine(HINSTANCE hInstance,
					   LPTSTR szWindowClass,
					   LPTSTR szTitle,
					   WORD wIcon,
					   WORD wSmallIcon,
					   int iWidth,
					   int iHeight,
					   int iFrameRate)
{
	m_hInstance = hInstance;
	m_hWindow = NULL;
	if (lstrlen(szWindowClass) > 0)
		lstrcpy(m_szWindowClass, szWindowClass);
	if (lstrlen(szTitle) > 0)
		lstrcpy(m_szTitle, szTitle);
	m_wIcon = wIcon;
	m_wSmallIcon = wSmallIcon;
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iFrameDelay = 1000 / iFrameRate;   // 30 FPS default
	m_bSleep = TRUE;
	m_vSprites.reserve(100);
	m_uiPlayerID = 0;
	InitJoystick();
}

BOOL GameEngine::Initialize(int iCmdShow)
{
	WNDCLASSEX    wndclass;

	// Create the window class for the main window
	wndclass.cbSize         = sizeof(wndclass);
	wndclass.style          = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc    = WndProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 0;
	wndclass.hInstance      = m_hInstance;
	wndclass.hIcon          = LoadIcon(m_hInstance,
		MAKEINTRESOURCE(GetIcon()));
	wndclass.hIconSm        = LoadIcon(m_hInstance,
		MAKEINTRESOURCE(GetSmallIcon()));
	wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName   = NULL;
	wndclass.lpszClassName  = m_szWindowClass;

	// Register the window class
	if (!RegisterClassEx(&wndclass))
	{
		// LogInfo("[ERROR] RegisterClassEx failed!");
		return FALSE;
	}

	// Calculate the window size and position based upon the game size
	int iWindowWidth = m_iWidth + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
		iWindowHeight = m_iHeight + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
		GetSystemMetrics(SM_CYCAPTION);
	if (wndclass.lpszMenuName != NULL)
		iWindowHeight += GetSystemMetrics(SM_CYMENU);
	int iXWindowPos = (GetSystemMetrics(SM_CXSCREEN) - iWindowWidth) / 2,
		iYWindowPos = (GetSystemMetrics(SM_CYSCREEN) - iWindowHeight) / 2;

	// Create the window
	m_hWindow = CreateWindow(m_szWindowClass, m_szTitle, WS_POPUPWINDOW |
		WS_CAPTION | WS_MINIMIZEBOX, iXWindowPos, iYWindowPos, iWindowWidth,
		iWindowHeight, NULL, NULL, m_hInstance, NULL);
	if (!m_hWindow)
	{
		// LogInfo("[ERROR] CreateWindow failed!");
		return FALSE;
	}

	// Show and update the window
	ShowWindow(m_hWindow, iCmdShow);
	UpdateWindow(m_hWindow);
	return TRUE;
}

GameEngine::~GameEngine() {}

void GameEngine::Run()
{
	MSG         msg;
	static int  iTickTrigger = 0;
	int         iTickCount;

	// Enter the main message loop
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Process the message
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Make sure the game engine isn't sleeping
			if (!GetSleep())
			{
				// Check the tick count to see if a game cycle has elapsed
				iTickCount = GetTickCount();
				if (iTickCount > iTickTrigger)
				{
					iTickTrigger = iTickCount + GetFrameDelay();
					HandleKeys();
					CheckJoystick();
					GameCycle();
				}
			}
		}
	}
	// End the game
	GameEnd();
}

LRESULT GameEngine::HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Route Windows messages to game engine member functions
	switch (msg)
	{
	case WM_CREATE:
		// Set the game window and start the game
		SetWindow(hWindow);
		GameStart(hWindow);
		return 0;

	case WM_ACTIVATE:
		if (wParam != WA_INACTIVE)
		{
			GameActivate(hWindow);
			SetSleep(FALSE);
		}
		else
		{
			GameDeactivate(hWindow);
			SetSleep(TRUE);
		}
		return 0;
	case WM_SETFOCUS:
		// Activate the game and update the Sleep status
		GameActivate(hWindow);
		SetSleep(FALSE);
		return 0;

	case WM_KILLFOCUS:
		// Deactivate the game and update the Sleep status
		GameDeactivate(hWindow);
		SetSleep(TRUE);
		return 0;

	case WM_PAINT:
		HDC         hDC;
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWindow, &ps);

		// Paint the game
		GamePaint(hDC);

		EndPaint(hWindow, &ps);
		return 0;

	case WM_LBUTTONDOWN:
		// Left mouse key down
		MouseButtonDown(LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_LBUTTONUP:
		// Left mouse key up
		MouseButtonUp(LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_RBUTTONDOWN:
		// Right mouse key down
		MouseButtonDown(LOWORD(lParam), HIWORD(lParam), FALSE);
		return 0;

	case WM_RBUTTONUP:
		// Right mouse key up
		MouseButtonUp(LOWORD(lParam), HIWORD(lParam), FALSE);
		return 0;

	case WM_MOUSEMOVE:
		// Mouse move
		MouseMove(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_DESTROY:
		// End the game and exit the application
		GameEnd();
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWindow, msg, wParam, lParam);
}

// Sprites Management Methods
BOOL GameEngine::CheckSpriteCollision(SpritePtr pTestSprite)
{
	BOOL bCollision = FALSE;
	for (std::vector<SpritePtr>::iterator it = m_vSprites.begin(); it != m_vSprites.end(); ++it)
	{
		if ((*it) != pTestSprite && pTestSprite->TestCollision(*it))
		{
			if (SpriteCollision((*it), pTestSprite))
			{
				bCollision = TRUE;
				break;
			}
		}
	}
	return bCollision;
}

void GameEngine::AddSprite(SpritePtr pSprite)
{
	if (pSprite != NULL)
	{
		if (m_vSprites.size() == 0)
		{
			m_vSprites.push_back(pSprite);
			return;
		}
		std::vector<SpritePtr>::iterator it = m_vSprites.begin();
		for ( ; it != m_vSprites.end(); ++it)
		{
			if (pSprite->GetZOrder() < (*it)->GetZOrder())
			{
				it = m_vSprites.insert(it, pSprite);
				return;
			}
		}
		if (it == m_vSprites.end())
		{
			m_vSprites.push_back(pSprite);
		}
	}
}

void GameEngine::DrawSprites(HDC hDC)
{
	for (std::vector<SpritePtr>::iterator it = m_vSprites.begin(); it != m_vSprites.end(); ++it)
	{
		(*it)->Draw(hDC);
	}
}

void GameEngine::UpdateSprites()
{
	for (std::vector<SpritePtr>::iterator it = m_vSprites.begin(); it != m_vSprites.end(); )
	{
		RECT oldPosition = (*it)->GetPosition();
		SPRITEACTION act = (*it)->Update();
		if (act == SA_KILL)
		{
			it = m_vSprites.erase(it);
			continue;
		}
		if (CheckSpriteCollision(*it))
		{
			(*it)->SetPosition(oldPosition);
		}
		++it;
	}
}

void GameEngine::CleanupSprites()
{
	for (std::vector<SpritePtr>::iterator it = m_vSprites.begin(); it != m_vSprites.end(); )
	{
		it = m_vSprites.erase(it);
	}
	m_vSprites.clear();
}

SpritePtr GameEngine::IsPointInSprite(int x, int y)
{
	SpritePtr ret;
	POINT pt;
	pt.x = x;
	pt.y = y;
	std::vector<SpritePtr>::iterator it = m_vSprites.begin();
	for ( ; it != m_vSprites.end(); ++it)
	{
		if (PtInRect(&(*it)->GetPosition(), pt) != 0)
		{
			ret = (*it);
		}
	}
	return ret;
}

// Joystick methods
BOOL GameEngine::InitJoystick()
{
	// Ensure there is joystick driver
	UINT uiNumJoysticks;
	if ((uiNumJoysticks = joyGetNumDevs()) == 0)
		return FALSE;

	// Ensure joystick is connected
	JOYINFO jiInfo;
	if (joyGetPos(JOYSTICKID1, &jiInfo) != JOYERR_UNPLUGGED)
		m_uiJoystickID[0] = JOYSTICKID1;
	else
		return FALSE;
	if (joyGetPos(JOYSTICKID2, &jiInfo) != JOYERR_UNPLUGGED)
		m_uiJoystickID[1] = JOYSTICKID2;

	// Calculate movements
	JOYCAPS	jcCaps;
	joyGetDevCaps(m_uiJoystickID[0], &jcCaps, sizeof(JOYCAPS));
	DWORD dwXCenter = ((DWORD)jcCaps.wXmin + jcCaps.wXmax) / 2;
	DWORD dwYCenter = ((DWORD)jcCaps.wYmin + jcCaps.wYmax) / 2;
	m_rcJoystickTrip.left = (jcCaps.wXmin + (WORD)dwXCenter) / 2;
	m_rcJoystickTrip.right = (jcCaps.wXmax + (WORD)dwXCenter) / 2;
	m_rcJoystickTrip.top = (jcCaps.wYmin + (WORD)dwYCenter) / 2;
	m_rcJoystickTrip.bottom = (jcCaps.wYmax + (WORD)dwYCenter) / 2;

	return TRUE;
}

void GameEngine::CaptureJoystick()
{
	if (m_uiJoystickID[0] == JOYSTICKID1)
		joySetCapture(m_hWindow, m_uiJoystickID[0], NULL, TRUE);
	if (m_uiJoystickID[1] == JOYSTICKID2)
		joySetCapture(m_hWindow, m_uiJoystickID[1], NULL, TRUE);
}

void GameEngine::ReleaseJoystick()
{
	if (m_uiJoystickID[0] == JOYSTICKID1)
		joyReleaseCapture(m_uiJoystickID[0]);
	if (m_uiJoystickID[1] == JOYSTICKID2)
		joyReleaseCapture(m_uiJoystickID[1]);
}

void GameEngine::CheckJoystick()
{
	if (m_uiJoystickID[0] == JOYSTICKID1)
	{
		JOYINFO jiInfo;
		JOYSTATE jsJoystickState = 0;
		if (joyGetPos(m_uiJoystickID[0], &jiInfo) == JOYERR_NOERROR)
		{
			// check x axis move
			if (jiInfo.wXpos < (WORD)m_rcJoystickTrip.left)
				jsJoystickState |= JOY_LEFT;
			else if (jiInfo.wXpos > (WORD)m_rcJoystickTrip.right)
				jsJoystickState |= JOY_RIGHT;

			// check y axis move
			if (jiInfo.wYpos < (WORD)m_rcJoystickTrip.top)
				jsJoystickState |= JOY_UP;
			else if (jiInfo.wYpos > (WORD)m_rcJoystickTrip.bottom)
				jsJoystickState |= JOY_DOWN;

			// check button
			if (jiInfo.wButtons & JOY_BUTTON1)
				jsJoystickState |= JOY_FIRE1;
			if (jiInfo.wButtons & JOY_BUTTON2)
				jsJoystickState |= JOY_FIRE2;
		}

		HandleJoystick(m_uiJoystickID[0], jsJoystickState);
	}

	if (m_uiJoystickID[1] == JOYSTICKID2)
	{
		JOYINFO jiInfo;
		JOYSTATE jsJoystickState = 0;
		if (joyGetPos(m_uiJoystickID[1], &jiInfo) == JOYERR_NOERROR)
		{
			// check x axis move
			if (jiInfo.wXpos < (WORD)m_rcJoystickTrip.left)
				jsJoystickState |= JOY_LEFT;
			else if (jiInfo.wXpos > (WORD)m_rcJoystickTrip.right)
				jsJoystickState |= JOY_RIGHT;

			// check y axis move
			if (jiInfo.wYpos < (WORD)m_rcJoystickTrip.top)
				jsJoystickState |= JOY_UP;
			else if (jiInfo.wYpos > (WORD)m_rcJoystickTrip.bottom)
				jsJoystickState |= JOY_DOWN;

			// check button
			if (jiInfo.wButtons & JOY_BUTTON1)
				jsJoystickState |= JOY_FIRE1;
			if (jiInfo.wButtons & JOY_BUTTON2)
				jsJoystickState |= JOY_FIRE2;
		}

		HandleJoystick(m_uiJoystickID[1], jsJoystickState);
	}
}

void GameEngine::CloseMusic()
{
	if (m_uiPlayerID != 0)
	{
		mciSendCommand(m_uiPlayerID, MCI_CLOSE, 0, NULL);
		m_uiPlayerID = 0;
	}
}

void GameEngine::PlayMusic(LPTSTR szFileName, BOOL bRestart)
{
	// See if the player needs to be opened
	if (m_uiPlayerID == 0)
	{
		// Judge the file name to choose device
		int iLen = lstrlen(szFileName);
		LPTSTR lpBuf = szFileName + iLen - strlen(".mp3");
		MUSICTYPE musictype = MUSIC_NONE;
		if (lstrcmp(lpBuf, ".mp3") == 0)
		{
			musictype = MUSIC_MP3;
		}
		else if (lstrcmp(lpBuf, ".mid") == 0)
		{
			musictype = MUSIC_MIDI;
		}
		else if (lstrcmp(lpBuf, ".wav") == 0)
		{
			musictype = MUSIC_WAV;
		}
		else if (lstrcmp(lpBuf, ".avi") == 0)
		{
			musictype = MUSIC_AVI;
		}

		if (musictype == MUSIC_NONE)
		{
			return;
		}

		// Open the MIDI player by specifying the device and filename
		MCI_OPEN_PARMS mciOpenParms;
		mciOpenParms.lpstrDeviceType = musictype;
		mciOpenParms.lpstrElementName = szFileName;
		if (mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
			(DWORD_PTR)&mciOpenParms) == 0)
			// Get the ID for the MIDI player
			m_uiPlayerID = mciOpenParms.wDeviceID;
		else
			// There was a problem, so just return
			return;
	}

	// Restart the MIDI song, if necessary
	if (bRestart)
	{
		MCI_SEEK_PARMS mciSeekParms;
		if (mciSendCommand(m_uiPlayerID, MCI_SEEK, MCI_SEEK_TO_START,
			(DWORD_PTR)&mciSeekParms) != 0)
			// There was a problem, so close the player
			CloseMusic();
	}

	// Play the music
	// MCI_DGV_PLAY_REPEAT means repeat play
	MCI_PLAY_PARMS mciPlayParms;
	if (mciSendCommand(m_uiPlayerID, MCI_PLAY, MCI_DGV_PLAY_REPEAT,
		(DWORD_PTR)&mciPlayParms) != 0)
		// There was a problem, so close the MIDI player
		CloseMusic();
}

void GameEngine::PauseMusic()
{
	// Pause the currently playing song, if possible
	if (m_uiPlayerID != 0)
		mciSendCommand(m_uiPlayerID, MCI_PAUSE, 0, NULL);
}

