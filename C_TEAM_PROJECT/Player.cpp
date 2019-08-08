/**
*@file		Player.cpp
*@author	A.Yokoyama
*@brief		プレイヤークラスの実装
*/
#include "stdafx.h"
#include <d2d1.h>
#include "Stage.h"
#include "Player.h"
#include "TextureLoader.h"


#define FILE_NAME	_T("res\\player.png")


CPlayer::CPlayer(CStage *pStage)
{
	m_pParent = pStage;
	m_pImage = NULL;
	m_iTimer = 0;
	circle.fx = (FLOAT)PLAYER_START_X;
	circle.fy = (FLOAT)PLAYER_START_Y;
	circle.rad = (FLOAT)PLAYER_RAD;

	ID2D1RenderTarget *pTarget = pStage->GetRenderTarget();
	if (pTarget) {
		//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, FILE_NAME, &m_pImage);

#ifdef _DEBUG
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
		m_pBrush->SetOpacity(0.75f);
#endif

		SAFE_RELEASE(pTarget);
	}
}


CPlayer::~CPlayer()
{
	SAFE_RELEASE(m_pImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif
}


/**
*@brief	アニメーションメソッド
*/
bool CPlayer::move() {
	return true;
}


/**
*@brief	描画メソッド
*/
void CPlayer::draw(ID2D1RenderTarget *pRenderTarget) {

	D2D1_RECT_F rc, src;
	D2D1_SIZE_F size = pRenderTarget->GetSize();
	
#ifdef _DEBUG
	D2D1_ELLIPSE el;
	el.point.x = size.width * 0.5f;
	el.point.y = size.height * 0.5f;
	el.radiusX = 960.f;
	el.radiusY = 540.f;
	pRenderTarget->DrawEllipse(el, m_pBrush);

	rc.left = (size.width - 48.f) * 0.5f;
	rc.right = rc.left + 48.f;
	rc.top = (size.height - 96.f) * 0.5f;
	rc.bottom = rc.top + 96.f;
	pRenderTarget->FillRectangle(rc, m_pBrush);
#endif
}


/**
*@brief	円との当たり判定を行う
*@param [in] x	対象の中心点x座標
*@param [in] y	対象の中心点y座標
*@param [in] r	対象の半径
*@return	true 当たり / false 外れ
*/
bool CPlayer::collide(float x, float y, float r) {
	float vx = circle.fx - x;
	float vy = circle.fy - y;
	float l2 = vx * vx + vy * vy;

	float d = circle.rad + r;
	d *= d;

	if (l2 < d)
		return true;

	return false;
}


/**
*@brief	GameObject との当たり判定
*@param [in] pObj	相手オブジェクト
*/
bool CPlayer::collide(IGameObject *pObj) {
	float x = circle.fx, y = circle.fy, r = circle.rad;

	return pObj->collide(x, y, r);
}

/**
*@brief	ダメージを受けたときの処理
*/
void CPlayer::damage(float amount) {

}