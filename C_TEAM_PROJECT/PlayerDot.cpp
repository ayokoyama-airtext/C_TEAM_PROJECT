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


#define PLAYERDOT_FILE_NAME	_T("res\\playerdot.png")

//	staticメンバの初期化
ID2D1Bitmap *CPlayerDot::m_pImage = NULL;
int	CPlayerDot::m_iCount = 0;
CStage *CPlayerDot::m_pParent = NULL;
CPlayer *CPlayerDot::m_pPlayer = NULL;
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
	m_iState = 1;
}


CPlayerDot::~CPlayerDot()
{
	m_iCount--;
}


bool CPlayerDot::move() {
	float angle = (m_pParent->playerCoords.playerAngle) + 2.f * PI * ((FLOAT)m_iNumber / (m_pParent->playerCoords.playerMaxDotNum));
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	float playerRad = m_pParent->playerCoords.playerRad;

	m_fX = playerX + playerRad * cosf(angle);
	m_fY = playerY + playerRad * sinf(angle);

	return true;
}


void CPlayerDot::draw(ID2D1RenderTarget *pRenderTarget) {
	//	壊れてたら描画しない
	if (!m_iState)
		return;

	float playerDrawX = m_pParent->playerCoords.playerDrawX;
	float playerDrawY = m_pParent->playerCoords.playerDrawY;
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	bool  IsWHmode = m_pParent->playerCoords.playerIsWHmode;

#ifdef _DEBUG
	D2D1_ELLIPSE el;
	el.point.x = playerDrawX + (m_fX - playerX);
	el.point.y = playerDrawY + (m_fY - playerY);
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	if (IsWHmode) {
		pRenderTarget->FillEllipse(el, m_pBrushWhite);
	}
	else {
		pRenderTarget->FillEllipse(el, m_pBrushBlack);
	}
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
	SAFE_RELEASE(m_pImage);
	//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, PLAYERDOT_FILE_NAME, &m_pImage);
	m_pParent = pStage;
	m_pPlayer = pPlayer;

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrushBlack);
	SAFE_RELEASE(m_pBrushWhite);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrushWhite);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBrushBlack);
#endif // _DEBUG
}

void CPlayerDot::Finalize() {
	m_pPlayer = NULL;
	m_pParent = NULL;
	SAFE_RELEASE(m_pImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrushBlack);
	SAFE_RELEASE(m_pBrushWhite);
#endif // _DEBUG

}