/**
*@file		Player.cpp
*@author	A.Yokoyama
*@brief		プレイヤークラスの実装
*/
#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include <vector>
#include "Stage.h"
#include "Player.h"
#include "PlayerDot.h"
#include "Shot.h"
#include "TextureLoader.h"


#define PLAYER_FILE_NAME	_T("res\\player.png")
const float CPlayer::ROTATION_SPEED = 0.05f;
const float CPlayer::PLAYER_SPEED = 10.f;

#define DECREASE_SPEED 0.05f
#define BRAKE_SPEED	0.5f


CPlayer::CPlayer(CStage *pStage)
{
	m_pParent = pStage;
	m_pImage = NULL;
	m_iTimer = 0;
	m_iDamagedTimer = 0;
	m_iShotTimer = 0;
	m_fX = (FLOAT)PLAYER_START_X;
	m_fY = (FLOAT)PLAYER_START_Y;
	m_fVX = 0;
	m_fVY = 0;
	m_fRad = (FLOAT)BELT_RAD;
	m_fCoreRad = (FLOAT)(CORE_LENGTH >> 1);
	m_fAngle = -PI * 0.5f;
	m_fScale = 1.0f;
	m_iDotNum = START_DOT_NUM;
	m_iMaxDotNum = START_MAX_DOT_NUM;

	m_fDecreaseCos = 0.f;
	m_fDecreaseSin = 0.f;

	m_bDamaged = false;
	m_bIsWhiteHallMode = false;
	m_bIsRkeyPress = true;

	m_fFieldWidth = m_pParent->FIELD_WIDTH;
	m_fFieldHeight = m_pParent->FIELD_HEIGHT;

	//	Dotを生成
	for (int i = 0; i < m_iDotNum; ++i) {
		CPlayerDot *pObj = new CPlayerDot();
		if (pObj) {
			m_pDots.push_back(pObj);
			pStage->AddPlayerDot(pObj);
		}
	}

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
	float cos = cosf(m_fAngle);
	float sin = sinf(m_fAngle);

	
	if (m_bDamaged) {
		return false;
	}


	//	自動減速
	if (m_fVX > 0) {
		m_fVX += -DECREASE_SPEED * m_fDecreaseCos;
		if (m_fVX <= 0)
			m_fVX = 0;
	}
	else if (m_fVX < 0) {
		m_fVX += -DECREASE_SPEED * m_fDecreaseCos;
		if (m_fVX >= 0)
			m_fVX = 0;
	}
	if (m_fVY > 0) {
		m_fVY += -DECREASE_SPEED * m_fDecreaseSin;
		if (m_fVY <= 0)
			m_fVY = 0;
	}
	else if (m_fVY < 0) {
		m_fVY += -DECREASE_SPEED * m_fDecreaseSin;
		if (m_fVY >= 0)
			m_fVY = 0;
	}
	
	if (m_iDamagedTimer > 0) {
		m_iDamagedTimer--;
	}
	if (m_iShotTimer > 0) {
		m_iShotTimer--;
	}


	//********************************************************
	//						モード切替
	//********************************************************
	if (GetAsyncKeyState(RKEY_CODE)) {
		if (!m_bIsRkeyPress) {
			if (m_bIsWhiteHallMode) {
				m_bIsWhiteHallMode = false;
			}
			else {
				m_bIsWhiteHallMode = true;
			}
			m_bIsRkeyPress = true;
		}
	}
	else {
		m_bIsRkeyPress = false;
	}


	//********************************************************
	//					移動処理
	//********************************************************
	
	//	前進
	if (GetAsyncKeyState(VK_UP)) {
		m_fVX = PLAYER_SPEED * cos;
		m_fVY = PLAYER_SPEED * sin;
		m_fDecreaseCos = cos;
		m_fDecreaseSin = sin;
	}

	//	ブレーキ
	if (GetAsyncKeyState(VK_DOWN)) {
		if (m_fVX > 0) {
			m_fVX += -BRAKE_SPEED * m_fDecreaseCos;
			if (m_fVX <= 0)
				m_fVX = 0;
		}
		else if (m_fVX < 0) {
			m_fVX += -BRAKE_SPEED * m_fDecreaseCos;
			if (m_fVX >= 0)
				m_fVX = 0;
		}
		if (m_fVY > 0) {
			m_fVY += -BRAKE_SPEED * m_fDecreaseSin;
			if (m_fVY <= 0)
				m_fVY = 0;
		}
		else if (m_fVY < 0) {
			m_fVY += -BRAKE_SPEED * m_fDecreaseSin;
			if (m_fVY >= 0)
				m_fVY = 0;
		}
	}

	//	右回転
	if (GetAsyncKeyState(VK_RIGHT)) {
		m_fAngle += ROTATION_SPEED;
		if (m_fAngle > 2.f * PI) {
			m_fAngle -= 2.f * PI;
		}
	}

	//	左回転
	if (GetAsyncKeyState(VK_LEFT)) {
		m_fAngle -= ROTATION_SPEED;
		if (m_fAngle < -2.f * PI) {
			m_fAngle += 2.f * PI;
		}
	}

	//********************************************************
	//				弾発射
	//********************************************************
	if (GetAsyncKeyState(VK_SPACE)) {
		if (m_iShotTimer == 0 && m_bIsWhiteHallMode == true) {
			IGameObject *pObj;
			for (int i = 0; i < m_iMaxDotNum; ++i) {
				pObj = NULL;
				if (m_pDots[i]->GetState()) {
					float x = m_pDots[i]->GetX(), y = m_pDots[i]->GetY();
					pObj = new CShot(x, y, cos, sin);
					if (pObj) {
						m_pParent->AddShot(pObj);
					}
				}
			}	//	for 
			m_iShotTimer = SHOT_INTERVAL;
		}	//	if (m_iShotTimer == 0)
	}
	//********************************************************
	//********************************************************

	m_fX += m_fVX;
	m_fY += m_fVY;


	//	フィールド範囲から出ないように調整
	if (m_fX < m_fRad) {
		m_fX = m_fRad;
	}
	else if (m_fX > m_fFieldWidth - m_fRad) {
		m_fX = m_fFieldWidth - m_fRad;
	}

	if (m_fY < m_fRad) {
		m_fY = m_fRad;
	}
	else if (m_fY > m_fFieldHeight - m_fRad) {
		m_fY = m_fFieldHeight - m_fRad;
	}


	//	デバッグ用操作
#ifdef _DEBUG
	////	最大ドット数を増やす
	//if (GetAsyncKeyState(INC_MAXDOT_KEY)) {
	//	if (!INC_MAXDOT_FLAG) {
	//		m_iMaxDotNum++;
	//		INC_MAXDOT_FLAG = true;
	//	}
	//}
	//else {
	//	INC_MAXDOT_FLAG = false;
	//}


#endif // _DEBUG



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
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	pRenderTarget->DrawEllipse(el, m_pBrush);

	//	ドット
	FLOAT angle = 0;
	/*for (int i = 0; i < m_iDotNum; ++i) {
		angle = m_fAngle + 2.f * PI * ((FLOAT)i / (FLOAT)m_iMaxDotNum);
		el.point.x = m_fDrawX + m_fRad * cosf(angle);
		el.point.y = m_fDrawY + m_fRad * sinf(angle);
		el.radiusX = DOT_RAD;
		el.radiusY = DOT_RAD;
		pRenderTarget->DrawEllipse(el, m_pBrush);
	}*/

	//	コア
	D2D1_POINT_2F point;
	angle = 180.f * (m_fAngle + PI * 0.5f) / PI;
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
*@brief	プレイヤーの描画座標を計算 + 諸情報を渡す。Stageから呼ぶメソッド
*@param [in/out] *playerCoords		プレイヤーのデータ上の座標と描画上の座標をまとめた構造体
*@note	
*/
void CPlayer::CalcDrawCoord(PLAYER_COORDS *playerCoords) {

	playerCoords->playerX = m_fX;
	playerCoords->playerY = m_fY;
	playerCoords->playerAngle = m_fAngle;
	playerCoords->playerRad = m_fRad;
	playerCoords->playerMaxDotNum = m_iMaxDotNum;
	playerCoords->playerIsWHmode = m_bIsWhiteHallMode;

	//	x座標
	if ( (m_fX >= (DISPLAY_RESOLUTION_WIDTH >> 1)) && (m_fX <= (m_fFieldWidth - (DISPLAY_RESOLUTION_WIDTH >> 1))) ) {
		m_fDrawX = DISPLAY_RESOLUTION_WIDTH >> 1;
		playerCoords->playerDrawX = DISPLAY_RESOLUTION_WIDTH >> 1;
		playerCoords->XaxisDraw = DRAW_CENTERING_PLAYER;
	}
	else if(m_fX > (m_fFieldWidth - (DISPLAY_RESOLUTION_WIDTH >> 1)) ){
		m_fDrawX = m_fX - (m_fFieldWidth - DISPLAY_RESOLUTION_WIDTH);
		playerCoords->playerDrawX = m_fX - (m_fFieldWidth - DISPLAY_RESOLUTION_WIDTH);
		playerCoords->XaxisDraw = DRAW_STATIC_MAX;;

	}
	else {
		m_fDrawX = m_fX;
		playerCoords->playerDrawX = m_fX;
		playerCoords->XaxisDraw = DRAW_STATIC_MIN;
	}

	//	y座標
	if ( (m_fY >= (DISPLAY_RESOLUTION_HEIGHT >> 1)) && (m_fY <= (m_fFieldHeight - (DISPLAY_RESOLUTION_HEIGHT >> 1))) ) {
		m_fDrawY = (DISPLAY_RESOLUTION_HEIGHT >> 1);
		playerCoords->playerDrawY = (DISPLAY_RESOLUTION_HEIGHT >> 1);
		playerCoords->YaxisDraw = DRAW_CENTERING_PLAYER;

	}
	else if(m_fY > (m_fFieldHeight - (DISPLAY_RESOLUTION_HEIGHT >> 1)) ){
		m_fDrawY = m_fY - (m_fFieldHeight - DISPLAY_RESOLUTION_HEIGHT);
		playerCoords->playerDrawY = m_fY - (m_fFieldHeight - DISPLAY_RESOLUTION_HEIGHT);
		playerCoords->YaxisDraw = DRAW_STATIC_MAX;
	}
	else {
		m_fDrawY = m_fY;
		playerCoords->playerDrawY = m_fY;
		playerCoords->YaxisDraw = DRAW_STATIC_MIN;
	}

}


/**
*@brief	プレイヤーの持つ生存ドット数を一つ減らす
*@note	プレイヤードットが死んだときに呼ぶ関数
*/
void CPlayer::DecreaseAliveDotNum() {
	m_iDotNum--;
}


/**
*@brief	円との当たり判定を行う
*@param [in] x	対象の中心点x座標
*@param [in] y	対象の中心点y座標
*@param [in] r	対象の半径
*@return	true 当たり / false 外れ
*/
bool CPlayer::collide(float x, float y, float r) {
	if (m_iDamagedTimer > 0)
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
bool CPlayer::collide(IGameObject *pObj) {
	if (m_iDamagedTimer > 0)
		return false;

	float x = m_fX, y = m_fY, r = m_fCoreRad;

	return pObj->collide(x, y, r);
}

/**
*@brief	ダメージを受けたときの処理
*/
void CPlayer::damage(float amount) {
	m_iDotNum = 0;
	for (int i = 0; i < m_iMaxDotNum; ++i) {
		if (1 == m_pDots[i]->GetState()) {
			m_iDotNum++;
		}
	}

	if (m_iDotNum > 0) {
		for (int i = m_iMaxDotNum; i > 0; --i) {
			if (0 != m_pDots[i - 1]->GetState()) {
				m_pDots[i - 1]->damage(1.f);
				m_iDotNum--;
				i = -1;
			}
		}
		m_iDamagedTimer = DAMAGED_DURATION;
	}
	else {
		m_bDamaged = true;
	}
}