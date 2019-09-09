/**
*@file		EnemyBossDot.cpp
*@author	A.Yokoyama
*@brief		エネミーBossDotクラスの実装
*/
#include "stdafx.h"
#include <d2d1.h>
#include <math.h>
#include "Stage.h"
#include "EnemyBossDot.h"


CEnemyBossDot::~CEnemyBossDot()
{
}

bool CEnemyBossDot::move() {
	if (m_iRespawnAnimTimer >= 0) {
		m_iRespawnAnimTimer--;
	}

	if (!m_iState) {
		if (--m_iDestroyAnimTimer >= 0) {
			return true;
		}
		else {
			return false;
		}
	}

	m_iTimer++;

	if (m_iLife < 2) {
		m_fRad = 24.f;
		m_iID = 1;
	}

	if (m_iDamageTimer > 0) {
		m_iDamageTimer--;
	}

	m_fAngle = (m_pParent->GetAngle()) + 2.f * PI * ((FLOAT)m_iNumber / (FLOAT)m_iMaxDotNum);
	float parentX = m_pParent->GetX();
	float parentY = m_pParent->GetY();
	float parentRad = m_pParent->GetRad();

	m_fX = parentX + (parentRad + 12.f) * cosf(m_fAngle);
	m_fY = parentY + (parentRad + 12.f) * sinf(m_fAngle);

	return true;
}

void CEnemyBossDot::draw(ID2D1RenderTarget *pRenderTarget) {
	float playerDrawX = m_pStage->playerCoords.playerDrawX;
	float playerDrawY = m_pStage->playerCoords.playerDrawY;
	float playerX = m_pStage->playerCoords.playerX;
	float playerY = m_pStage->playerCoords.playerY;

	D2D1_RECT_F		rc, src;
	D2D1_SIZE_F		texSize;
	D2D1_POINT_2F	center;
	center.x = playerDrawX + (m_fX - playerX);
	center.y = playerDrawY + (m_fY - playerY);

	float degreeAngle = 180.f * (m_fAngle + PI * 0.5f) / PI;
	D2D1::Matrix3x2F rotation = D2D1::Matrix3x2F::Rotation(degreeAngle, center);
	pRenderTarget->SetTransform(rotation);

	//	respawn animation
	if (m_iRespawnAnimTimer >= 0) {
		if (m_iRespawnAnimTimer > 11) {
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			return;
		}

		int index = (11 - m_iRespawnAnimTimer) >> 2;
		src.left = 96.f * (2 - index);
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

	//	destroy animation
	if (m_bDamaged) {
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

	//	Dot
	src.left = 0.f;
	src.top = 0.f;
	src.right = src.left + m_fRad * 2.f;
	src.bottom = src.top + m_fRad * 2.f;

	rc.left = center.x - m_fRad;
	rc.right = rc.left + m_fRad * 2.f;
	rc.top = center.y - m_fRad;
	rc.bottom = rc.top + m_fRad * 2.f;

	pRenderTarget->DrawBitmap(m_pDotImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

	//	Straw
	if (m_iID == 0) {
		int texIndex = (m_iTimer % 24) / 6;

		src.left = m_pStrawTexCoord[texIndex];
		src.right = src.left + 799.f;
		src.top = 0.f;
		src.bottom = src.top + 740.f;

		rc.left -= m_fRad;
		rc.right += m_fRad;
		rc.bottom = rc.top;
		rc.top -= m_fRad * 4;

		pRenderTarget->DrawBitmap(m_pStrawImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
	}

	if (m_iDamageTimer > 0) {
		D2D1_ELLIPSE el;
		el.point.x = center.x;
		el.point.y = center.y;
		el.radiusX = m_fRad;
		el.radiusY = m_fRad;
		m_pWhiteBrush->SetOpacity((FLOAT)m_iDamageTimer / (FLOAT)DAMAGE_DURATION);
		pRenderTarget->FillEllipse(el, m_pWhiteBrush);
	}

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

#ifdef _DEBUG
	D2D1_ELLIPSE el;
	el.point.x = playerDrawX + (m_fX - playerX);
	el.point.y = playerDrawY + (m_fY - playerY);
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	pRenderTarget->FillEllipse(el, m_pBrush);
#endif // _DEBUG

}

void CEnemyBossDot::damage(float amount) {
	if (m_iLife > 1) {
		m_iLife--;
		m_iDamageTimer = DAMAGE_DURATION;
	}
	else {
		m_bDamaged = true;
		m_iDestroyAnimTimer--;
	}
}