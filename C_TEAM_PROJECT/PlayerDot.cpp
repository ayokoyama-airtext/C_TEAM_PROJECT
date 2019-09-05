/**
*@file		PlayerDot.cpp
*@author	A.Yokoyama
*@brief		プレイヤードットクラスの実装
*/
#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include "Stage.h"
#include "Player.h"
#include "PlayerDot.h"
#include "TextureLoader.h"


#define PLAYERDOT_FILE_NAME			_T("res\\player_dot.png")
#define PLAYERDOT_STRAW_FILE_NAME	_T("res\\straw_799x740.png")
#define PLAYERDOT_DEST_FILE_NAME	_T("res\\player_destroy.png")

//	staticメンバの初期化
ID2D1Bitmap *CPlayerDot::m_pDotImage = NULL;
ID2D1Bitmap *CPlayerDot::m_pStrawImage = NULL;
ID2D1Bitmap *CPlayerDot::m_pDestroyImage = NULL;
int	CPlayerDot::m_iCount = 0;
CStage *CPlayerDot::m_pParent = NULL;
CPlayer *CPlayerDot::m_pPlayer = NULL;
float CPlayerDot::m_pStrawTexCoord[] = { 0.f, 799.f, 1598.f, 2397.f };
#ifdef _DEBUG
ID2D1SolidColorBrush *CPlayerDot::m_pBrushBlack = NULL;
ID2D1SolidColorBrush *CPlayerDot::m_pBrushWhite = NULL;
#endif // _DEBUG



CPlayerDot::CPlayerDot()
{
	m_iNumber = m_iCount;
	m_iCount++;
	m_iTimer = 0;
	m_fRad = (FLOAT)DEFAULT_RAD;
	m_fAngle = 0;
	m_iState = 1;
	m_iDestroyAnimTimer = 12;
}


CPlayerDot::~CPlayerDot()
{
	m_iCount--;
}


bool CPlayerDot::move() {

	//	壊れてたら戻る
	if (!m_iState) {
		if (m_iDestroyAnimTimer >= 0) {
			m_iDestroyAnimTimer--;
		}
		return true;
	}

	if (false == m_pParent->playerCoords.playerIsWHmode) {
		m_iTimer++;
	}
	else {
		m_iTimer = 0;
	}

	m_fAngle = (m_pParent->playerCoords.playerAngle) + 2.f * PI * ((FLOAT)m_iNumber / (m_pParent->playerCoords.playerMaxDotNum));
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	float playerRad = m_pParent->playerCoords.playerRad;

	m_fX = playerX + playerRad * cosf(m_fAngle);
	m_fY = playerY + playerRad * sinf(m_fAngle);

	return true;
}


void CPlayerDot::draw(ID2D1RenderTarget *pRenderTarget) {
	//	壊れ終わったら描画しない
	if (m_iDestroyAnimTimer < 0)
		return;


	float playerDrawX = m_pParent->playerCoords.playerDrawX;
	float playerDrawY = m_pParent->playerCoords.playerDrawY;
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	bool  IsWHmode = m_pParent->playerCoords.playerIsWHmode;

	D2D1_RECT_F		rc, src;
	D2D1_SIZE_F		texSize;
	D2D1_POINT_2F	center;
	center.x = playerDrawX + (m_fX - playerX);
	center.y = playerDrawY + (m_fY - playerY);
	
	float degreeAngle = 180.f * (m_fAngle + PI * 0.5f) / PI;
	D2D1::Matrix3x2F rotation = D2D1::Matrix3x2F::Rotation(degreeAngle, center);
	pRenderTarget->SetTransform(rotation);

	if (!m_iState) {
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
	if (m_pParent->playerCoords.playerIsWHmode == false) {
		int texIndex = (m_iTimer % 24) / 6;
		float opacity = 1.0f;
		if (m_iTimer < 30) {
			opacity = (FLOAT)m_iTimer / 30.f;
		}

		src.left = m_pStrawTexCoord[texIndex];
		src.right = src.left + 799.f;
		src.top = 0.f;
		src.bottom = src.top + 740.f;

		rc.left -= m_fRad;
		rc.right += m_fRad;
		rc.bottom = rc.top;
		rc.top -= m_fRad * 4;

		pRenderTarget->DrawBitmap(m_pStrawImage, rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
	}

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

#ifdef _DEBUG
	D2D1_ELLIPSE el;
	el.point.x = playerDrawX + (m_fX - playerX);
	el.point.y = playerDrawY + (m_fY - playerY);
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	/*if (IsWHmode) {
		pRenderTarget->FillEllipse(el, m_pBrushWhite);
	}
	else {
		pRenderTarget->FillEllipse(el, m_pBrushBlack);
	}*/
#endif // _DEBUG

}


/**
*@brief	円との当たり判定を行う
*@param [in] x	対象の中心点x座標
*@param [in] y	対象の中心点y座標
*@param [in] r	対象の半径
*@return	true 当たり / false 外れ
*/
bool CPlayerDot::collide(float x, float y, float r) {
	if (!m_iState)
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


/**
*@brief	GameObject との当たり判定
*@param [in] pObj	相手オブジェクト
*/
bool CPlayerDot::collide(IGameObject *pObj) {
	//	壊れてたら判定しない
	if (!m_iState)
		return false;

	float x = m_fX, y = m_fY, r = m_fRad;

	return pObj->collide(x, y, r);
}

/**
*@brief	ダメージを受けたときの処理
*/
void CPlayerDot::damage(float amount) {
	m_iState = 0;
	m_pPlayer->DecreaseAliveDotNum();
	m_iDestroyAnimTimer--;
}

/**
*@brief	ドットを復活させる
*/
void CPlayerDot::ReviveDot() {
	m_iState = 1;
	m_iTimer = 0;
	m_iDestroyAnimTimer = 12;
}

/**
*@brief	stateを返す
*/
int CPlayerDot::GetState() {
	return m_iState;
}

float CPlayerDot::GetX() {
	return m_fX;
}

float CPlayerDot::GetY() {
	return m_fY;
}

void CPlayerDot::Restore(ID2D1RenderTarget *pTarget, CStage *pStage, CPlayer *pPlayer) {
	SAFE_RELEASE(m_pDotImage);
	SAFE_RELEASE(m_pStrawImage);
	SAFE_RELEASE(m_pDestroyImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, PLAYERDOT_FILE_NAME, &m_pDotImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, PLAYERDOT_STRAW_FILE_NAME, &m_pStrawImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, PLAYERDOT_DEST_FILE_NAME, &m_pDestroyImage);
	m_pParent = pStage;
	m_pPlayer = pPlayer;

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrushBlack);
	SAFE_RELEASE(m_pBrushWhite);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrushWhite);
	m_pBrushWhite->SetOpacity(0.5f);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBrushBlack);
	m_pBrushBlack->SetOpacity(0.5f);
#endif // _DEBUG
}

void CPlayerDot::Finalize() {
	m_pPlayer = NULL;
	m_pParent = NULL;
	SAFE_RELEASE(m_pDotImage);
	SAFE_RELEASE(m_pStrawImage);
	SAFE_RELEASE(m_pDestroyImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrushBlack);
	SAFE_RELEASE(m_pBrushWhite);
#endif // _DEBUG

}