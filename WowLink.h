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
	void InitIcons();					// ����
	bool CheckDead();					// �������
	void RandIcons();					// �������ͼ��
	void DrawRect(HDC hDC, POINT& pt);  // ��ѡ�з���
	void DrawConnLine(HDC hDC);			// ��������

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
	DWORD		m_dwBeginTime;		// ѡ�п�������ߴ��ڼ�ʱ
	DWORD		m_dwEndTime;
	bool		m_Matched;			// ��Գɹ���ʶ
	bool		m_bSlash;			// ������ʶ
	std::vector<POINT> m_v1, m_v2;  // �洢���ڶԱȵ���ʱ��
	POINT		m_ptPair1;			// δ�����������
	POINT		m_ptPair2;			// ����ָʾ��ֵĴ���

};
typedef std::shared_ptr<WowLink> WowLinkPtr;