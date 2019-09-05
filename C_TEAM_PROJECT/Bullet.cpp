/**
*@file		Bullet.cpp
*@author	A.Yokoyama
*@brief		BulletƒNƒ‰ƒX‚ÌŽÀ‘•
*/
#include "stdafx.h"
#include <d2d1.h>
#include <math.h>
#include "Stage.h"
#include "Bullet.h"
#include "TextureLoader.h"


#define BULLET_FILE_NAME _T("res\\shot.png")
#define BULLET_DEST_FILE_NAME _T("res\\shot_destroy.png")


ID2D1Bitmap *CBullet::m_pImage = NULL;
ID2D1Bitmap *CBullet::m_pDestroyImage = NULL;
CStage *CBullet::m_pParent = NULL;
#ifdef _DEBUG
ID2D1SolidColorBrush *CBullet::m_pBrush = NULL;
#endif // _DEBUG



CBullet::CBullet(float x, float y, float angle)
{
	m_fX = x;
	m_fY = y;
	m_fVX = cos(angle);
	m_fVY = sin(angle);
	m_fAngle = angle;
	m_fRad = DEFAULT_RAD;
	m_iLifeTimer = LIFE_TIME;
	m_iDestroyAnimTimer = 12;
	m_bDamaged = false;
}


CBullet::~CBullet()
{
}


bool CBullet::move() {
	if (m_bDamaged || (m_iLifeTimer-- < 0)) {
		m_bDamaged = true;
		if (--m_iDestroyAnimTimer >= 0)
			return true;
		else
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

void CBullet::draw(ID2D1RenderTarget *pRenderTarget) {
	float playerDrawX = m_pParent->playerCoords.playerDrawX;
	float playerDrawY = m_pParent->playerCoords.playerDrawY;
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;

	D2D1_RECT_F rc, src;
	D2D1_POINT_2F	center;
	center.x = playerDrawX + (m_fX - playerX);
	center.y = playerDrawY + (m_fY - playerY);

	//	destroy animation
	if (m_bDamaged) {
		float degreeAngle = 180.f * (m_fAngle + PI * 0.5f) / PI;

		D2D1::Matrix3x2F rotation = D2D1::Matrix3x2F::Rotation(degreeAngle, center);
		pRenderTarget->SetTransform(rotation);

		int index = (11 - m_iDestroyAnimTimer) >> 2;
		src.left = 96.f * index;
		src.right = src.left + 96.f;
		src.top = 0.f;
		src.bottom = src.top + 96.f;

		rc.left = center.x - 48.f;
		rc.right = rc.left + 96.f;
		rc.top = center.y - 48.f;
		rc.bottom = rc.top + 96.f;

		pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		return;
	}

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

bool CBullet::collide(float x, float y, float r) {
	if (m_bDamaged)
		return false;

	float vx = m_fX - x;
	float vy = m_fY - y;
	float l2 = vx * vx + vy * vy;

	float d = m_fRad + r;
	d *= d;

	if (l2 < d)
		return true;

	return false;
}

bool CBullet::collide(IGameObject *pObj) {
	if (m_bDamaged)
		return false;

	return pObj->collide(m_fX, m_fY, m_fRad);

}

void CBullet::damage(float amount) {
	m_bDamaged = true;
	--m_iDestroyAnimTimer;
}

void CBullet::Restore(CStage *pStage, ID2D1RenderTarget *pTarget) {
	SAFE_RELEASE(m_pImage);
	SAFE_RELEASE(m_pDestroyImage);
	m_pParent = pStage;

	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, BULLET_FILE_NAME, &m_pImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, BULLET_DEST_FILE_NAME, &m_pDestroyImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pBrush);
	m_pBrush->SetOpacity(0.1f);
#endif // _DEBUG
}

void CBullet::Finalize() {
	SAFE_RELEASE(m_pImage);
	SAFE_RELEASE(m_pDestroyImage);
#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG
}