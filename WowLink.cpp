#include "WowLink.h"
#include "Resource.h"
#include <queue>
#include <random>


void WowLink::InitIcons()
{
	m_iMakePair = 0;
	m_iScore = 0;
	m_iNumLives = 3;
	for (int i = 0; i < ROW; ++i)
		for (int j = 0; j < COL; ++j)
			m_matrix[i][j] = -1;
	m_ptClick1.x = m_ptClick1.y = -1;
	m_ptClick2.x = m_ptClick2.y = -1;
	m_ptTurnPoint1.x = m_ptTurnPoint1.y = m_ptTurnPoint2.x = m_ptTurnPoint2.y = -1;

	// add icons
	int puticon = 0;         // 记录放置的图标数
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::uniform_int_distribution<> distx(1, ROW-2);
	std::uniform_int_distribution<> disty(1, COL-2);
	std::uniform_int_distribution<> disticon(0, ICONCOUNT-1);
	int ix = distx(rng);
	int iy = disty(rng);
	int randicon = disticon(rng);
	int addcount = 0;
	while (puticon < m_iTotalNum)
	{
		if (addcount == 2)
		{
			randicon = disticon(rng);
			addcount = 0;
		}
		++addcount;
		while(m_matrix[ix][iy] != -1)
		{
			ix = distx(rng);
			iy = disty(rng);
		}
		m_matrix[ix][iy] = randicon;
		puticon += 1;
	}
}

bool WowLink::CheckDead()
{
	POINT p1;
	POINT p2;
	bool dead = true;
	for (int i = 1; i < ROW - 1; ++i)
		for (int j = 1; j < COL - 1; ++j)
		{
			p1.x = i;
			p1.y = j;
			for (int m = 1; m < ROW - 1; ++m)
				for (int n = 1; n < COL - 1; ++n)
				{
					p2.x = m;
					p2.y = n;
					if (CheckMatch(p1, p2))
					{
						m_ptPair1.x = p1.x;
						m_ptPair1.y = p1.y;
						m_ptPair2.x = p2.x;
						m_ptPair2.y = p2.y;
						dead = false;
						return dead;
					}
				}
		}
	return dead;
}

void WowLink::RandIcons()
{
	int matrix[ROW][COL] = {0};
	for (int i = 0; i < ROW; ++i)
		for (int j = 0; j < COL; ++j)
			matrix[i][j] = -1;

	std::random_device rd;
	std::default_random_engine rng(rd());
	std::uniform_int_distribution<> distx(1, ROW-2);
	std::uniform_int_distribution<> disty(1, COL-2);
	int ix = 0;
	int iy = 0;
	for (int i = 1; i < ROW - 1; ++i)
		for (int j = 1; j < COL - 1; ++j)
		{
			if (m_matrix[i][j] != -1)
			{
				ix = distx(rng);
				iy = disty(rng);
				while (matrix[ix][iy] != -1)
				{
					ix = distx(rng);
					iy = disty(rng);
				}
				matrix[ix][iy] = m_matrix[i][j];
			}
		}

	for (int i = 1; i < ROW - 1; ++i)
		for (int j = 1; j < COL - 1; ++j)
		{
			m_matrix[i][j] = matrix[i][j];
		}

	return;
}

void WowLink::DrawRect(HDC hDC, POINT& pt)
{
	RECT rc;
	rc.left = pt.y * m_pWowBitmap[0]->GetWidth();
	rc.top = pt.x * m_pWowBitmap[0]->GetHeight();
	rc.right = rc.left + m_pWowBitmap[0]->GetWidth();
	rc.bottom = rc.top + m_pWowBitmap[0]->GetHeight();
	HPEN hPen = CreatePen(PS_SOLID, 5, RGB(192, 255, 62));
	SelectObject(hDC, hPen);
	SelectObject(hDC, GetStockObject(NULL_BRUSH));
	Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
	DeleteObject(hPen);
}

void WowLink::DrawConnLine(HDC hDC)
{
	// 画连接线
	HPEN hPen = CreatePen(PS_SOLID, 5, RGB(192, 255, 62));
	SelectObject(hDC, hPen);
	MoveToEx(hDC, (m_ptClick1.y+0.5) * m_pWowBitmap[0]->GetWidth(),
		(m_ptClick1.x + 0.5) * m_pWowBitmap[0]->GetHeight(), NULL);
	if (m_ptTurnPoint1.x != -1)
	{
		LineTo(hDC, (m_ptTurnPoint1.y + 0.5) * m_pWowBitmap[0]->GetWidth(),
			(m_ptTurnPoint1.x + 0.5) * m_pWowBitmap[0]->GetHeight());
	}
	if (m_ptTurnPoint2.x != -1)
	{
		LineTo(hDC, (m_ptTurnPoint2.y + 0.5) * m_pWowBitmap[0]->GetWidth(),
			(m_ptTurnPoint2.x + 0.5) * m_pWowBitmap[0]->GetHeight());
	}
	LineTo(hDC, (m_ptClick2.y + 0.5) * m_pWowBitmap[0]->GetWidth(),
		(m_ptClick2.x + 0.5) * m_pWowBitmap[0]->GetHeight());
	DeleteObject(hPen);
}

void WowLink::GameStart(HWND hWindow)
{
	m_hOffscreenDC = CreateCompatibleDC(GetDC(hWindow));
	m_hOffscreenBitmap = CreateCompatibleBitmap(GetDC(hWindow), GetWidth(), GetHeight());
	SelectObject(m_hOffscreenDC, m_hOffscreenBitmap);

	// load bitmap
	HDC hDC = GetDC(hWindow);
	m_pBackgroundBitmap = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_BACKGROUND, m_hInstance));
	m_pCopyrightBitmap = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_COPYRIGHT, m_hInstance));
	m_pSlashBitmap = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_SLASH, m_hInstance));
	m_pWowBitmap[0] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW1, m_hInstance));
	m_pWowBitmap[1] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW2, m_hInstance));
	m_pWowBitmap[2] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW3, m_hInstance));
	m_pWowBitmap[3] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW4, m_hInstance));
	m_pWowBitmap[4] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW5, m_hInstance));
	m_pWowBitmap[5] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW6, m_hInstance));
	m_pWowBitmap[6] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW7, m_hInstance));
	m_pWowBitmap[7] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW8, m_hInstance));
	m_pWowBitmap[8] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW9, m_hInstance));
	m_pWowBitmap[9] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW10, m_hInstance));
	m_pWowBitmap[10] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW11, m_hInstance));
	m_pWowBitmap[11] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW12, m_hInstance));
	m_pWowBitmap[12] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW13, m_hInstance));
	m_pWowBitmap[13] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW14, m_hInstance));
	m_pWowBitmap[14] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW15, m_hInstance));
	m_pWowBitmap[15] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW16, m_hInstance));
	m_pWowBitmap[16] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW17, m_hInstance));
	m_pWowBitmap[17] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW18, m_hInstance));
	m_pWowBitmap[18] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW19, m_hInstance));
	m_pWowBitmap[19] = std::shared_ptr<Bitmap>(new Bitmap(hDC, IDB_WOW20, m_hInstance));

	// Initialize
	m_bGameOver = FALSE;
	m_iTotalNum = (ROW - 2) * (COL - 2);
	m_Matched = false;
	m_bSlash = true;
	m_v1.reserve(20);
	m_v2.reserve(20);
	m_ptPair1.x = m_ptPair1.y = m_ptPair2.x = m_ptPair2.y = -1;
	InitIcons();

	// dead ?
	while (CheckDead())
	{
		RandIcons();
	}

	// show copyright
	m_pCopyrightBitmap->Draw(hDC, 0, 0);
	Sleep(3000);

	// play background music
	PlayMusic(TEXT("Song_of_Elune.mp3"));
}

void WowLink::GameEnd()
{
	DeleteObject(m_hOffscreenBitmap);
	DeleteDC(m_hOffscreenDC);

	CleanupSprites();
	// Close music
	CloseMusic();
}

void WowLink::GameActivate(HWND hWindow)
{
	// Continue music
	if (!m_bSlash)
		PlayMusic(TEXT(""), FALSE);
}

void WowLink::GameDeactivate(HWND hWindow)
{
	// Pause music
	if (!m_bSlash)
		PauseMusic();
}

void WowLink::GamePaint(HDC hDC)
{
	// Draw background
	m_pBackgroundBitmap->Draw(hDC, 0, 0);

	// Draw icons
	int iWidth = m_pWowBitmap[0]->GetWidth();
	int iHeight = m_pWowBitmap[0]->GetHeight();
	for (int i = 1; i < ROW - 1; ++i)
	{
		for (int j = 1; j < COL - 1; ++j)
		{
			if (m_matrix[i][j] != -1)
			{
				m_pWowBitmap[m_matrix[i][j]]->Draw(hDC, j * iWidth, i * iHeight, TRUE);
			}
		}
	}

	// Draw Rectangle on selected icon and connect line
	if (!m_Matched)
	{
		if (m_ptClick1.x != -1)
			DrawRect(hDC, m_ptClick1);
		if (m_ptClick2.x != -1)
			DrawRect(hDC, m_ptClick2);
	}
	else {
		m_dwEndTime = GetTickCount();
		if (m_dwEndTime - m_dwBeginTime < 1000)
		{
			if (m_ptClick1.x != -1)
				DrawRect(hDC, m_ptClick1);
			if (m_ptClick2.x != -1)
				DrawRect(hDC, m_ptClick2);
			DrawConnLine(hDC);
		}
	}
}

void WowLink::GameCycle()
{
	// if (!m_bGameOver)
	{
		// Get device context
		HWND hWindow = GetWindow();
		HDC hDC = GetDC(hWindow);

		// Paint on off screen device context
		GamePaint(m_hOffscreenDC);

		// Send off screen to game screen
		BitBlt(hDC, 0, 0, GetWidth(), GetHeight(),
			m_hOffscreenDC, 0, 0, SRCCOPY);

		// Clean up
		ReleaseDC(hWindow, hDC);
	}
}

void WowLink::HandleKeys()
{
	if (m_bGameOver && GetAsyncKeyState(VK_RETURN) < 0)
	{
		m_bGameOver = FALSE;
		m_Matched = false;
		InitIcons();
	}
}

void WowLink::HandleJoystick(UINT uiJoystickID, JOYSTATE jsJoystickState)
{
}

void WowLink::MouseButtonDown(int x, int y, BOOL bLeft)
{
	if (x < m_pWowBitmap[0]->GetWidth() || x > (COL - 1) * m_pWowBitmap[0]->GetWidth()
		|| y < m_pWowBitmap[0]->GetHeight() || y > (ROW - 1) * m_pWowBitmap[0]->GetHeight())
		return;

	int col = x / m_pWowBitmap[0]->GetWidth();
	int row = y / m_pWowBitmap[0]->GetHeight();

	if (m_Matched)
	{
		m_Matched = false;
		m_ptClick1.x = m_ptClick1.y = m_ptClick2.x = m_ptClick2.y = -1;
		m_ptTurnPoint1.x = m_ptTurnPoint1.y = m_ptTurnPoint2.x = m_ptTurnPoint2.y = -1;
	}

	if (m_matrix[row][col] != -1)
	{
		if (m_ptClick1.x == -1)  // 1 is empty
		{
			PlaySound((LPCSTR)IDW_CLICK, m_hInstance, SND_ASYNC | SND_RESOURCE);
			m_ptClick1.x = row;
			m_ptClick1.y = col;
		}
		else if ((row != m_ptClick1.x) || (col != m_ptClick1.y))   // not 1 again
		{
			if (m_ptClick2.x == -1)  // 2 is empty
			{
				PlaySound((LPCSTR)IDW_CLICK, m_hInstance, SND_ASYNC | SND_RESOURCE);
				m_ptClick2.x = row;
				m_ptClick2.y = col;
				if (CheckMatch(m_ptClick1, m_ptClick2))
				{
					PlaySound((LPCSTR)IDW_MAKEPAIR, m_hInstance, SND_ASYNC | SND_RESOURCE);
					m_Matched = true;
					m_dwBeginTime = GetTickCount();
					m_iScore += 10;
					m_iMakePair += 2;
					m_matrix[m_ptClick1.x][m_ptClick1.y] = -1;
					m_matrix[m_ptClick2.x][m_ptClick2.y] = -1;

					// end?
					if (m_iMakePair == m_iTotalNum)
					{
						PlaySound((LPCSTR)IDW_WIN, m_hInstance, SND_ASYNC | SND_RESOURCE);
						InvalidateRect(GetWindow(), NULL, FALSE);
						MessageBox(GetWindow(), TEXT("You win!\nPress Enter to restart."), TEXT("WOW Link"), MB_OK);
						m_bGameOver = TRUE;
					}

					// if dead?
					if (m_ptPair1.x == -1 || m_ptPair2.x == -1)
					{
						if (CheckDead())
						{
							RandIcons();
						}
					}
				}
				else
				{
					PlaySound((LPCSTR)IDW_MISMATCH, m_hInstance, SND_ASYNC | SND_RESOURCE);
					m_ptClick1.x = m_ptClick1.y = m_ptClick2.x = m_ptClick2.y = -1;
				}
			}
			else    // 2 is not empty
			{
				m_ptClick1.x = m_ptClick1.y = m_ptClick2.x = m_ptClick2.y = -1;
			}
		}
	}
}

void WowLink::MouseButtonUp(int x, int y, BOOL bLeft)
{
}

void WowLink::MouseMove(int x, int y)
{
}

BOOL WowLink::SpriteCollision(SpritePtr pSpriteHitter, SpritePtr pSpriteHittee)
{
	return FALSE;
}


bool WowLink::CheckMatch(POINT& p1, POINT& p2)
{
	if (p1.x == -1 || p2.x == -1)
		return false;

	if (m_matrix[p1.x][p1.y] != m_matrix[p2.x][p2.y])
		return false;

	// 直连？
	if (p1.x == p2.x)
	{
		bool clean = true;
		for (int i = min(p1.y, p2.y) + 1; i < max(p1.y, p2.y); ++i)
		{
			if (m_matrix[p1.x][i] != -1)
			{
				clean = false;
				break;
			}
		}
		if (clean)
			return true;
	}
	if (p1.y == p2.y)
	{
		bool clean = true;
		for (int i = min(p1.x, p2.x) + 1; i < max(p1.x, p2.x); ++i)
		{
			if (m_matrix[i][p1.y] != -1)
			{
				clean = false;
				break;
			}
		}
		if (clean)
			return true;
	}

	// 直连不通
	m_v1.clear();
	m_v2.clear();
	POINT tmp;
	if (p1.y != p2.y) // 纵向延伸
	{
		m_v1.push_back(p1);
		for (int i = p1.x - 1; i >= 0; --i)
		{
			if (m_matrix[i][p1.y] != -1)
				break;
			tmp.x = i;
			tmp.y = p1.y;
			m_v1.push_back(tmp);
		}
		for (int i = p1.x + 1; i < ROW; ++i)
		{
			if (m_matrix[i][p1.y] != -1)
				break;
			tmp.x = i;
			tmp.y = p1.y;
			m_v1.push_back(tmp);
		}

		m_v2.push_back(p2);
		for (int i = p2.x - 1; i >= 0; --i)
		{
			if (m_matrix[i][p2.y] != -1)
				break;
			tmp.x = i;
			tmp.y = p2.y;
			m_v2.push_back(tmp);
		}
		for (int i = p2.x + 1; i < ROW; ++i)
		{
			if (m_matrix[i][p2.y] != -1)
				break;
			tmp.x = i;
			tmp.y = p2.y;
			m_v2.push_back(tmp);
		}
		// 横向对比
		for (size_t i = 0; i < m_v1.size(); ++i)
		{
			for (size_t j = 0; j < m_v2.size(); ++j)
			{
				if (m_v1[i].x == m_v2[j].x)
				{
					bool clean = true;
					for (int k = min(m_v1[i].y, m_v2[j].y) + 1; k < max(m_v1[i].y, m_v2[j].y); ++k)
					{
						if (m_matrix[m_v1[i].x][k] != -1)
						{
							clean = false;
							break;
						}
					}
					if (clean)
					{
						m_ptTurnPoint1.x = m_v1[i].x;
						m_ptTurnPoint1.y = m_v1[i].y;
						m_ptTurnPoint2.x = m_v2[j].x;
						m_ptTurnPoint2.y = m_v2[j].y;
						return true;
					}
				}
			}
		}
	}

	m_v1.clear();
	m_v2.clear();
	if (p1.x != p2.x)   // 横向延伸
	{
		m_v1.push_back(p1);
		for (int i = p1.y - 1; i >= 0; --i)
		{
			if (m_matrix[p1.x][i] != -1)
				break;
			tmp.x = p1.x;
			tmp.y = i;
			m_v1.push_back(tmp);
		}
		for (int i = p1.y + 1; i < COL; ++i)
		{
			if (m_matrix[p1.x][i] != -1)
				break;
			tmp.x = p1.x;
			tmp.y = i;
			m_v1.push_back(tmp);
		}
		m_v2.push_back(p2);
		for (int i = p2.y - 1; i >= 0; --i)
		{
			if (m_matrix[p2.x][i] != -1)
				break;
			tmp.x = p2.x;
			tmp.y = i;
			m_v2.push_back(tmp);
		}
		for (int i = p2.y + 1; i < COL; ++i)
		{
			if (m_matrix[p2.x][i] != -1)
				break;
			tmp.x = p2.x;
			tmp.y = i;
			m_v2.push_back(tmp);
		}
		// 纵向对比
		for (size_t i = 0; i < m_v1.size(); ++i)
		{
			for (size_t j = 0; j < m_v2.size(); ++j)
			{
				if (m_v1[i].y == m_v2[j].y)
				{
					bool clean = true;
					for (int k = min(m_v1[i].x, m_v2[j].x) + 1; k < max(m_v1[i].x, m_v2[j].x); ++k)
					{
						if (m_matrix[k][m_v1[i].y] != -1)
						{
							clean = false;
							break;
						}
					}
					if (clean)
					{
						m_ptTurnPoint1.x = m_v1[i].x;
						m_ptTurnPoint1.y = m_v1[i].y;
						m_ptTurnPoint2.x = m_v2[j].x;
						m_ptTurnPoint2.y = m_v2[j].y;
						return true;
					}
				}
			}
		}
	}
	return false;
}