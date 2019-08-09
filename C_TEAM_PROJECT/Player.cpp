/**
*@file		Player.cpp
*@author	A.Yokoyama
*@brief		プレイヤークラスの実装
*/
#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include "Stage.h"
#include "Player.h"
#include "TextureLoader.h"


#define PLAYER_FILE_NAME	_T("res\\player.png")
const float CPlayer::ROTATION_SPEED = 0.05f;
const float CPlayer::PLAYER_SPEED = 10.f;


CPlayer::CPlayer(CStage *pStage)
{
	m_pParent = pStage;
	m_pImage = NULL;
	m_iTimer = 0;
	m_stStats.fx = (FLOAT)PLAYER_START_X;
	m_stStats.fy = (FLOAT)PLAYER_START_Y;
	m_stStats.fVX = 0;
	m_stStats.fVY = 0;
	m_stStats.rad = (FLOAT)PLAYER_RAD;
	m_stStats.angle = -PI * 0.5f;
	m_stStats.scale = 1.0f;
	m_stStats.coreHP = 1;

	m_fFieldWidth = m_pParent->FIELD_WIDTH;
	m_fFieldHeight = m_pParent->FIELD_HEIGHT;

	ID2D1RenderTarget *pTarget = pStage->GetRenderTarget();
	if (pTarget) {
		//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, PLAYER_FILE_NAME, &m_pImage);

#ifdef _DEBUG
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
		m_pBrush->SetOpacity(0.75f);
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pRedBrush);
#endif

		SAFE_RELEASE(pTarget);
	}
}


CPlayer::~CPlayer()
{
	SAFE_RELEASE(m_pImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pBrush);
#endif
}


/**
*@brief	アニメーションメソッド
*/
bool CPlayer::move() {
	m_stStats.fVX = 0;
	m_stStats.fVY = 0;


	if (GetAsyncKeyState(VK_UP)) {
		m_stStats.fVX = PLAYER_SPEED * cosf(m_stStats.angle);
		m_stStats.fVY = PLAYER_SPEED * sinf(m_stStats.angle);
	}

	if (GetAsyncKeyState(VK_RIGHT)) {
		m_stStats.angle += ROTATION_SPEED;
		if (m_stStats.angle > 2.f * PI) {
			m_stStats.angle -= 2.f * PI;
		}
	}
	if (GetAsyncKeyState(VK_LEFT)) {
		m_stStats.angle -= ROTATION_SPEED;
		if (m_stStats.angle < -2.f * PI) {
			m_stStats.angle += 2.f * PI;
		}
	}


	m_stStats.fx += m_stStats.fVX;
	m_stStats.fy += m_stStats.fVY;

	if (m_stStats.fx < m_stStats.rad) {
		m_stStats.fx = m_stStats.rad;
	}
	else if (m_stStats.fx > m_fFieldWidth - m_stStats.rad) {
		m_stStats.fx = m_fFieldWidth - m_stStats.rad;
	}

	if (m_stStats.fy < m_stStats.rad) {
		m_stStats.fy = m_stStats.rad;
	}
	else if (m_stStats.fy > m_fFieldHeight - m_stStats.rad) {
		m_stStats.fy = m_fFieldHeight - m_stStats.rad;
	}

	return true;
}


/**
*@brief	描画メソッド
*/
void CPlayer::draw(ID2D1RenderTarget *pRenderTarget) {

	D2D1_RECT_F rc, src;
	D2D1_SIZE_F size = pRenderTarget->GetSize();
	
#ifdef _DEBUG
	//	ベルト
	D2D1_ELLIPSE el;
	el.point.x = m_fDrawX;
	el.point.y = m_fDrawY;
	el.radiusX = m_stStats.rad;
	el.radiusY = m_stStats.rad;
	pRenderTarget->DrawEllipse(el, m_pBrush);

	//	ドット
	FLOAT angle = 0;
	for (int i = 0; i < 5; ++i) {
		angle = m_stStats.angle + PI * (2.f / 5.f) * i;
		el.point.x = m_fDrawX + m_stStats.rad * cosf(angle);
		el.point.y = m_fDrawY + m_stStats.rad * sinf(angle);
		el.radiusX = BELT_RAD;
		el.radiusY = BELT_RAD;
		pRenderTarget->DrawEllipse(el, m_pBrush);
	}

	//	コア
	D2D1_POINT_2F point;
	angle = 180.f * (m_stStats.angle + PI * 0.5f) / PI;
	rc.left = m_fDrawX - CORE_LENGTH * 0.5f;
	rc.right = rc.left + CORE_LENGTH;
	rc.top = m_fDrawY - CORE_LENGTH * 0.5f;
	rc.bottom = rc.top + CORE_LENGTH;
	point.x = rc.left + CORE_LENGTH * 0.5f;
	point.y = rc.top + CORE_LENGTH * 0.5f;
	D2D1::Matrix3x2F rotate = D2D1::Matrix3x2F::Rotation(angle, point);
	pRenderTarget->SetTransform(rotate);

	pRenderTarget->DrawRectangle(rc, m_pBrush);

	rc.bottom = rc.top + CORE_LENGTH * 0.2f;
	pRenderTarget->DrawRectangle(rc, m_pRedBrush);

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
#endif
}


/**
*@brief	プレイヤーの描画座標を計算。主にStageから呼ぶメソッド
*@param [in/out] *playerCoords		プレイヤーのデータ上の座標と描画上の座標をまとめた構造体
*@note	当たり判定などを終えて位置が確定してから呼ぶこと
*/
void CPlayer::CalcDrawCoord(PLAYER_COORDS *playerCoords) {

	playerCoords->playerX = m_stStats.fx;
	playerCoords->playerY = m_stStats.fy;

	//	x座標
	if ( (m_stStats.fx >= (DISPLAY_RESOLUTION_WIDTH >> 1)) && (m_stStats.fx <= (m_fFieldWidth - (DISPLAY_RESOLUTION_WIDTH >> 1))) ) {
		m_fDrawX = DISPLAY_RESOLUTION_WIDTH >> 1;
		playerCoords->playerDrawX = DISPLAY_RESOLUTION_WIDTH >> 1;
		playerCoords->XaxisDraw = DRAW_CENTERING_PLAYER;
	}
	else if( m_stStats.fx > (m_fFieldWidth - (DISPLAY_RESOLUTION_WIDTH >> 1)) ){
		m_fDrawX = m_stStats.fx - (m_fFieldWidth - DISPLAY_RESOLUTION_WIDTH);
		playerCoords->playerDrawX = m_stStats.fx - (m_fFieldWidth - DISPLAY_RESOLUTION_WIDTH);
		playerCoords->XaxisDraw = DRAW_STATIC_MAX;;

	}
	else {
		m_fDrawX = m_stStats.fx;
		playerCoords->playerDrawX = m_stStats.fx;
		playerCoords->XaxisDraw = DRAW_STATIC_MIN;
	}

	//	y座標
	if ( (m_stStats.fy >= (DISPLAY_RESOLUTION_HEIGHT >> 1)) && (m_stStats.fy <= (m_fFieldHeight - (DISPLAY_RESOLUTION_HEIGHT >> 1))) ) {
		m_fDrawY = (DISPLAY_RESOLUTION_HEIGHT >> 1);
		playerCoords->playerDrawY = (DISPLAY_RESOLUTION_HEIGHT >> 1);
		playerCoords->YaxisDraw = DRAW_CENTERING_PLAYER;

	}
	else if( m_stStats.fy > (m_fFieldHeight - (DISPLAY_RESOLUTION_HEIGHT >> 1)) ){
		m_fDrawY = m_stStats.fy - (m_fFieldHeight - DISPLAY_RESOLUTION_HEIGHT);
		playerCoords->playerDrawY = m_stStats.fy - (m_fFieldHeight - DISPLAY_RESOLUTION_HEIGHT);
		playerCoords->YaxisDraw = DRAW_STATIC_MAX;
	}
	else {
		m_fDrawY = m_stStats.fy;
		playerCoords->playerDrawY = m_stStats.fy;
		playerCoords->YaxisDraw = DRAW_STATIC_MIN;
	}

}


/**
*@brief	円との当たり判定を行う
*@param [in] x	対象の中心点x座標
*@param [in] y	対象の中心点y座標
*@param [in] r	対象の半径
*@return	true 当たり / false 外れ
*/
bool CPlayer::collide(float x, float y, float r) {
	float vx = m_stStats.fx - x;
	float vy = m_stStats.fy - y;
	float l2 = vx * vx + vy * vy;

	float d = m_stStats.rad + r;
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
	float x = m_stStats.fx, y = m_stStats.fy, r = m_stStats.rad;

	return pObj->collide(x, y, r);
}

/**
*@brief	ダメージを受けたときの処理
*/
void CPlayer::damage(float amount) {

}