#include "stdafx.h"
#include <d2d1.h>
#include "Stage.h"
#include "Player.h"
#include "Shot.h"
#include "TextureLoader.h"


#define SHOT_FILE_NAME	_T("res\\shot.png")

ID2D1Bitmap *CShot::m_pImage = NULL;
CPlayer *CShot::m_pPlayer = NULL;
CStage *CShot::m_pParent = NULL;
#ifdef _DEBUG
ID2D1SolidColorBrush *CShot::m_pBrush = NULL;
#endif


CShot::CShot(float x, float y, float vx, float vy)
{
	m_fX = x;
	m_fY = y;
	m_fVX = vx;
	m_fVY = vy;
	m_fRad = DEFAULT_RAD;
	m_bDamaged = false;
}


CShot::~CShot()
{
}

bool CShot::move() {
	if (m_bDamaged) {
		return false;
	}

	m_fX += m_fVX * SHOT_SPEED;
	m_fY += m_fVY * SHOT_SPEED;

	if (m_fX < 0 - m_fRad) {
		return false;
	}
	else if (m_fX > m_pParent->FIELD_WIDTH + m_fRad) {
		return false;
	}
	if (m_fY < 0 - m_fRad) {
		return false;
	}
	else if (m_fY > m_pParent->FIELD_HEIGHT + m_fRad) {
		return false;
	}

	return true;
}

void CShot::draw(ID2D1RenderTarget *pRenderTarget) {
	float playerDrawX = m_pParent->playerCoords.playerDrawX;
	float playerDrawY = m_pParent->playerCoords.playerDrawY;
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;

	D2D1_RECT_F rc, src;
	D2D1_POINT_2F	center;
	center.x = playerDrawX + (m_fX - playerX);
	center.y = playerDrawY + (m_fY - playerY);

	src.left = 0.f;
	src.top = 0.f;
	src.right = src.left + m_fRad * 2.f;
	src.bottom = src.top + m_fRad * 2.f;

	rc.left = center.x - m_fRad;
	rc.right = rc.left + m_fRad * 2.f;
	rc.top = center.y - m_fRad;
	rc.bottom = rc.top + m_fRad * 2.f;

	pRenderTarget->DrawBitmap(m_pImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

#ifdef _DEBUG
	D2D1_ELLIPSE el;
	el.point.x = playerDrawX + m_fX - playerX;
	el.point.y = playerDrawY + m_fY - playerY;
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	pRenderTarget->FillEllipse(el, m_pBrush);
#endif
}

bool CShot::collide(float x, float y, float r) {
	float vx = m_fX - x;
	float vy = m_fY - y;
	float l2 = vx * vx + vy * vy;

	float d = m_fRad + r;
	d *= d;

	if (l2 < d)
		return true;

	return false;
}

bool CShot::collide(IGameObject *pObj) {
	
	return pObj->collide(m_fX, m_fY, m_fRad);

}

void CShot::damage(float amount) {
	m_bDamaged = true;
}

void CShot::Restore(CStage *pStage, CPlayer *pPlayer, ID2D1RenderTarget *pTarget) {
	SAFE_RELEASE(m_pImage);
	m_pParent = pStage;
	m_pPlayer = pPlayer;
	
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, SHOT_FILE_NAME, &m_pImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pBrush);
	m_pBrush->SetOpacity(0.1f);
#endif // _DEBUG
}

void CShot::Finalize() {
	SAFE_RELEASE(m_pImage);
#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG
}