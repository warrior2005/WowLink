#pragma once

#include <windows.h>
#include <memory>
#include "Bitmap.h"

typedef WORD		BOUNDSACTION;
const BOUNDSACTION	BA_STOP		= 0,
					BA_WRAP		= 1,
					BA_BOUNCE	= 2,
					BA_DIE		= 3;

typedef WORD		SPRITEACTION;
const SPRITEACTION	SA_NONE		= 0x0000L,
					SA_KILL		= 0x0001L;

class Sprite
{
protected:
	// Member variables
	BitmapPtr		m_pBitmap;
	RECT			m_rcPosition;
	POINT			m_ptVelocity;
	int				m_iZOrder;
	RECT			m_rcBounds;
	BOUNDSACTION	m_baBoundsAction;
	BOOL			m_bHidden;
	RECT			m_rcCollision;

public:
	// Constructors & Destructor
	Sprite(BitmapPtr pBitmap);
	Sprite(BitmapPtr pBitmap, RECT& rcBounds, BOUNDSACTION baBoundsAction = BA_STOP);
	Sprite(BitmapPtr pBitmap, POINT ptPosition, POINT ptVelocity, int iZOrder,
		RECT& rcBounds, BOUNDSACTION baBoundsAction = BA_STOP);
	virtual ~Sprite();

	// Functions
	virtual SPRITEACTION	Update();
	void			Draw(HDC hDC);
	BOOL			IsPointInside(int x, int y);

	// Accessor
	const RECT&	GetPosition()				{ return m_rcPosition; }
	void		SetPosition(int x, int y);
	void		SetPosition(POINT ptPosition);
	void		SetPosition(RECT& rcPosition)	{ CopyRect(&m_rcPosition, &rcPosition); }
	void		OffsetPosition(int x, int y);
	const POINT	GetVelocity() const 		{ return m_ptVelocity; }
	void		SetVelocity(int x, int y);
	void		SetVelocity(POINT ptVelocity);
	const int	GetZOrder() const 			{ return m_iZOrder; }
	void		SetZOrder(int iZOrder)		{ m_iZOrder = iZOrder; }
	void		SetBounds(RECT& rcBounds)	{ CopyRect(&m_rcBounds, &rcBounds); }
	void		SetBoundsAction(BOUNDSACTION ba)	{ m_baBoundsAction = ba; }
	BOOL		IsHidden()					{ return m_bHidden; }
	const int	GetWidth() const 			{ return m_pBitmap->GetWidth(); }
	const int	GetHeight() const 			{ return m_pBitmap->GetHeight(); }
	const RECT&	GetCollision() 				{ return m_rcCollision; }
	virtual void CalcCollisionRec();
	BOOL		TestCollision(std::shared_ptr<Sprite> pTestSprite);
};
typedef std::shared_ptr<Sprite> SpritePtr;