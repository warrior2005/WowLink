#pragma once

#include <windows.h>
#include <memory>
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"

const int COL = 18;
const int ROW = 12;
const int ICONCOUNT = 20;

class WowLink : public GameEngine
{
public:
	WowLink(HINSTANCE hInstance, LPTSTR szWindowClass, LPTSTR szTitle,
		WORD wIcon, WORD wSmallIcon, int iWidth, int iHeight, int iFrameRate)
		: GameEngine(hInstance, szWindowClass, szTitle,
		wIcon, wSmallIcon, iWidth, iHeight, iFrameRate) {}

	virtual void GameStart(HWND hWindow);
	virtual void GameEnd();
	virtual void GameActivate(HWND hWindow);
	virtual void GameDeactivate(HWND hWindow);
	virtual void GamePaint(HDC hDC);
	virtual void GameCycle();
	virtual void HandleKeys();
	virtual void MouseButtonDown(int x, int y, BOOL bLeft);
	virtual void MouseButtonUp(int x, int y, BOOL bLeft);
	virtual void MouseMove(int x, int y);
	virtual void HandleJoystick(UINT uiJoystickID, JOYSTATE jsJoystickState);
	virtual BOOL SpriteCollision(SpritePtr pSpriteHitter, SpritePtr pSpriteHittee);

private:
	bool CheckMatch(POINT& p1, POINT& p2);
	void InitIcons();					// 布局
	bool CheckDead();					// 检查死局
	void RandIcons();					// 随机分配图标
	void DrawRect(HDC hDC, POINT& pt);  // 画选中方框
	void DrawConnLine(HDC hDC);			// 画连接线

	int m_matrix[ROW][COL];
	BitmapPtr	m_pWowBitmap[ICONCOUNT];
	BitmapPtr	m_pBackgroundBitmap;
	BitmapPtr	m_pCopyrightBitmap;
	BitmapPtr	m_pSlashBitmap;
	POINT		m_ptClick1;
	POINT		m_ptClick2;
	POINT		m_ptTurnPoint1;
	POINT		m_ptTurnPoint2;
	int			m_iNumLives;
	int			m_iScore;
	int			m_iMakePair;
	int			m_iTotalNum;
	BOOL		m_bGameOver;
	DWORD		m_dwBeginTime;		// 选中框和连接线存在计时
	DWORD		m_dwEndTime;
	bool		m_Matched;			// 配对成功标识
	bool		m_bSlash;			// 闪屏标识
	std::vector<POINT> m_v1, m_v2;  // 存储用于对比的临时点
	POINT		m_ptPair1;			// 未被消除的配对
	POINT		m_ptPair2;			// 用于指示活局的存在

};
typedef std::shared_ptr<WowLink> WowLinkPtr;