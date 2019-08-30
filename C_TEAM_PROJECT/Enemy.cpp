/**
*@file		Enemy.cpp
*@author	A.Yokoyama
*@brief		エネミークラスの実装
*/
#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include <time.h>
#include "Stage.h"
#include "Enemy.h"
#include "EnemyDot.h"
#include "TextureLoader.h"


#define ENEMY_FILE_NAME	_T("res\\enemy.png")
const float CEnemy::ROTATION_SPEED = 0.025f;
const float CEnemy::ENEMY_SPEED = 5.f;
const float CEnemy::ENEMY_ESCAPE_SPEED = 8.f;
const float CEnemy::ENEMY_ESCAPE_ROTATION_SPEED = 0.2f;
const float CEnemy::ENEMY_ESCAPE_ANGLE = cosf(PI * 0.5f);
const float CEnemy::SEARCH_ANGLE = cosf(PI * 0.5f);


FLOAT CEnemy::m_pRandomMove[] = { 0.f, -1.f, 1.f, -1.f, 1.f };
const INT CEnemy::m_iRandomMoveSize = _countof(CEnemy::m_pRandomMove);
FLOAT CEnemy::m_fFieldWidth = 0.f;
FLOAT CEnemy::m_fFieldHeight = 0.f;
ID2D1Bitmap *CEnemy::m_pImage = NULL;
CStage *CEnemy::m_pParent = NULL;
#ifdef _DEBUG
ID2D1SolidColorBrush *CEnemy::m_pBrush = NULL;
ID2D1SolidColorBrush *CEnemy::m_pRedBrush = NULL;
#endif // _DEBUG



CEnemy::CEnemy(float x, float y, float scale)
{
	m_fX = x;
	m_fY = y;
	m_fVX = 0;
	m_fVY = 0;
	m_fRad = BELT_RAD;
	m_fCoreRad = (FLOAT)(CORE_LENGTH >> 1);
	m_fScale = scale;
	m_iMaxDotNum = 5;
	m_iDotNum = m_iMaxDotNum;
	m_fAngle = PI * 0.5f;
	m_iRandomMoveIndex = 0;
	srand(time(NULL));


	for (int i = 0; i < m_iMaxDotNum; ++i) {
		CEnemyDot *pObj = NULL;
		pObj = new CEnemyDot(this, i, m_iMaxDotNum);
		if (pObj) {
			m_pDots.push_back(pObj);
			m_pParent->AddEnemyDot(pObj);
		}
	}

	m_iBehaviorFlag = EFLAG_IDLE;

	m_iTimer = 0;
	m_iDamagedTimer = 0;
	m_bDamaged = false;
}


CEnemy::~CEnemy()
{
}


/**
*@brief	アニメーションメソッド
*/
bool CEnemy::move() {
	if (m_bDamaged) {
		if (m_iDotNum > 0) {
			for (int i = 0; i < m_iDotNum; ++i) {
				m_pDots[i]->SetStateZero();
			}
		}
		return false;
	}

	for (int i = 0; i < m_iDotNum; ++i) {
		if (m_pDots[i]->IsDead()) {
			m_pDots[i]->SetStateZero();
			m_pDots[i] = m_pDots.back();
			m_pDots.pop_back();
			m_iDotNum--;
		}
	}


	//************************************
	//				準備
	//************************************

	//	プレイヤーの情報を取得
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	//	プレイヤーとの相対ベクトルを計算
	float vx = playerX - m_fX;
	float vy = playerY - m_fY;
	//	自分の向きを計算
	float dirVX = cosf(m_fAngle);
	float dirVY = sinf(m_fAngle);

	//************************************
	//************************************


	//	m_iBehaviorFlagに応じて行動を変化させる
	switch (m_iBehaviorFlag) {
	case EFLAG_IDLE:	//	待機
		m_fVX = 0;
		m_fVY = 0;
		m_iTimer++;
		if (m_iTimer < 120)
			break;

		m_iBehaviorFlag = EFLAG_SEARCH;
		m_iTimer = 0;
		break;

	case EFLAG_SEARCH:	//	索敵
		{
			//	プレイヤーとの角度を計算
			float l = sqrtf(vx * vx + vy * vy);
			float cos = (dirVX * vx + dirVY * dirVY) / l;
			if (cos > SEARCH_ANGLE) {	//	索敵角度内なら
				if (l < (FLOAT)SEARCH_LENGTH) {	//	索敵距離内なら
					m_iBehaviorFlag = EFLAG_CHASE;	//	追跡へ移行
					m_iTimer = 0;
					break;
				}
			}

			if (m_iTimer == 0) {
				m_iRandomMoveIndex = (rand() >> 4) % m_iRandomMoveSize;
				m_iTimer = 180;
			}
			//	進行方向を変更
			m_fAngle += ROTATION_SPEED * m_pRandomMove[m_iRandomMoveIndex];
			//	進行方向へ加速
			m_fVX = ENEMY_SPEED * dirVX;
			m_fVY = ENEMY_SPEED * dirVY;
			m_iTimer--;
		}

		break;

	case EFLAG_CHASE:
		{
			float l = sqrtf(vx * vx + vy * vy);

			//	離れすぎたら追跡中止
			if (l > MAX_CHASE_LENGTH) {
				if (m_iTimer++ <= ATTACKED_DURATION) {
					float speedDecline = 0.5f * (FLOAT)(ATTACKED_DURATION - m_iTimer) / (FLOAT)ATTACKED_DURATION;
					m_fVX = dirVX * ENEMY_SPEED * speedDecline;
					m_fVY = dirVY * ENEMY_SPEED * speedDecline;
				}
				else {
					m_iTimer = 0;
					m_iBehaviorFlag = EFLAG_SEARCH;
				}
				break;
			}

			m_iTimer = 0;
			l = 1.0f / l;
			float sin = (dirVX * vy - dirVY * vx);
			float cos = (dirVX * vx + dirVY * vy) * l;
			if (sin > 0) {	//	プレイヤーは進行方向右側
				if (cos > cosf(ROTATION_SPEED)) {	//	1フレームの回転可能角度以内なら
					m_fVX = vx * l;
					m_fVY = vy * l;
					m_fAngle = atan2(m_fVY, m_fVX);
					m_fVX *= ENEMY_SPEED;
					m_fVY *= ENEMY_SPEED;
				}
				else {
					m_fVX = dirVX * ENEMY_SPEED;
					m_fVY = dirVY * ENEMY_SPEED;
					m_fAngle += ROTATION_SPEED;
				}
			}
			else {	//	プレイヤーは進行方向左側
				if (cos > cosf(ROTATION_SPEED)) {	//	1フレームの回転可能角度以内なら
					m_fVX = vx * l;
					m_fVY = vy * l;
					m_fAngle = atan2(m_fVY, m_fVX);
					m_fVX *= ENEMY_SPEED;
					m_fVY *= ENEMY_SPEED;
				}
				else {
					m_fVX = dirVX * ENEMY_SPEED;
					m_fVY = dirVY * ENEMY_SPEED;
					m_fAngle -= ROTATION_SPEED;
				}
			}
		}
		
		break;

	case EFLAG_ESCAPE:	//	ダメージを受けると一定時間逃げる
		if (m_iDamagedTimer > 0)
			m_iDamagedTimer--;
		if (m_iTimer++ > ESCAPE_DURATION) {
			m_iBehaviorFlag = EFLAG_SEARCH;
			m_iTimer = 0;
			break;
		}
		{
			float l = 1.f / sqrtf(vx * vx + vy * vy);
			float sin = (dirVX * vy - dirVY * vx);
			float cos = (dirVX * vx + dirVY * vy) * l;
			if (sin > 0) {
				if (cos > ENEMY_ESCAPE_ANGLE) {
					m_fAngle -= ENEMY_ESCAPE_ROTATION_SPEED;
				}
			}
			else {
				if (cos > ENEMY_ESCAPE_ANGLE) {
					m_fAngle += ENEMY_ESCAPE_ROTATION_SPEED;
				}
			}
		}

		m_fVX = ENEMY_ESCAPE_SPEED * cosf(m_fAngle);
		m_fVY = ENEMY_ESCAPE_SPEED * sinf(m_fAngle);
		break;

	case EFLAG_ATTACKED:
		m_iTimer++;
		if (m_iTimer >= ATTACKED_DURATION) {
			m_iBehaviorFlag = EFLAG_IDLE;
			m_iTimer = 0;
		}
		float speedDecline = 0.5f * (FLOAT)(ATTACKED_DURATION - m_iTimer) / (FLOAT)ATTACKED_DURATION;
		m_fVX = dirVX * ENEMY_SPEED * speedDecline;
		m_fVY = dirVY * ENEMY_SPEED * speedDecline;
		break;
	}

	
	m_fX += m_fVX;
	m_fY += m_fVY;

	if (m_fAngle > 2.f * PI) {
		m_fAngle -= 2.f * PI;
	}
	else if (m_fAngle < -2.f * PI) {
		m_fAngle += 2.f * PI;
	}


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

	return true;
}


/**
*@brief	描画メソッド
*/
void CEnemy::draw(ID2D1RenderTarget *pRenderTarget) {
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	float playerDrawX = m_pParent->playerCoords.playerDrawX;
	float playerDrawY = m_pParent->playerCoords.playerDrawY;

	float dx = playerDrawX + (m_fX - playerX);
	float dy = playerDrawY + (m_fY - playerY);

	D2D1_RECT_F rc, src;
	

#ifdef _DEBUG
	//	ベルト
	D2D1_ELLIPSE el;
	el.point.x = dx;
	el.point.y = dy;
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	pRenderTarget->DrawEllipse(el, m_pBrush);

	//	ドット
	FLOAT angle = 0;
	/*for (int i = 0; i < m_iDotNum; ++i) {
		angle = m_fAngle + 2.f * PI * ((FLOAT)i / (FLOAT)m_iMaxDotNum);
		el.point.x = dx + m_fRad * cosf(angle);
		el.point.y = dy + m_fRad * sinf(angle);
		el.radiusX = DOT_RAD;
		el.radiusY = DOT_RAD;
		pRenderTarget->DrawEllipse(el, m_pBrush);
	}*/

	//	コア
	D2D1_POINT_2F point;
	angle = 180.f * (m_fAngle + PI * 0.5f) / PI;
	rc.left = dx - CORE_LENGTH * 0.5f;
	rc.right = rc.left + CORE_LENGTH;
	rc.top = dy - CORE_LENGTH * 0.5f;
	rc.bottom = rc.top + CORE_LENGTH;
	point.x = rc.left + CORE_LENGTH * 0.5f;
	point.y = rc.top + CORE_LENGTH * 0.5f;
	D2D1::Matrix3x2F rotate = D2D1::Matrix3x2F::Rotation(angle, point);
	pRenderTarget->SetTransform(rotate);

	pRenderTarget->DrawRectangle(rc, m_pBrush);

	rc.bottom = rc.top + CORE_LENGTH * 0.2f;
	pRenderTarget->DrawRectangle(rc, m_pRedBrush);

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
#endif // _DEBUG


}

/**
*@brief	円との当たり判定を行う
*@param [in] x	対象の中心点x座標
*@param [in] y	対象の中心点y座標
*@param [in] r	対象の半径
*@return	true 当たり / false 外れ
*/
bool CEnemy::collide(float x, float y, float r) {
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
*@return	true 当たり / false 外れ
*/
bool CEnemy::collide(IGameObject *pObj) {
	if (m_iDamagedTimer > 0)
		return false;

	float x = m_fX, y = m_fY, r = m_fCoreRad;

	return pObj->collide(x, y, r);
}


void CEnemy::damage(float amount) {
	if (m_iDotNum > 0) {
		m_pDots[--m_iDotNum]->SetStateZero();
		m_pDots.pop_back();
		m_iBehaviorFlag = EFLAG_ESCAPE;
		m_iDamagedTimer = DAMAGED_DURATION;
		m_iTimer = 0;
	}
	else {
		m_bDamaged = true;
	}
}

/**
*@brief	X座標を返す
*/
float CEnemy::GetX() {
	return m_fX;
}
/**
*@brief	Y座標を返す
*/
float CEnemy::GetY() {
	return m_fY;
}
/**
*@brief	Radを返す
*/
float CEnemy::GetRad() {
	return m_fRad;
}
/**
*@brief	Angleを返す
*/
float CEnemy::GetAngle() {
	return m_fAngle;
}

/**
*@brief	フラグを設定する
*@note	flag 1: 攻撃命中
*			 2: ドットがダメージを受けた
*/
void CEnemy::SetFlag(int flag) {

	switch (flag) {
	case 1:
		m_iTimer = 0;
		m_iBehaviorFlag = EFLAG_ATTACKED;
		break;
	case 2:
		if (m_iBehaviorFlag != EFLAG_ESCAPE) {
			m_iTimer = 0;
			m_iBehaviorFlag = EFLAG_ESCAPE;
		}
		break;
	default:
		break;
	}
}


/**
*@brief	クラスで共有するリソースを生成する
*/
void CEnemy::Restore(ID2D1RenderTarget *pTarget, CStage *pStage) {
	SAFE_RELEASE(m_pImage);
	//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, ENEMY_FILE_NAME, &m_pImage);
	m_pParent = pStage;

	m_fFieldWidth = pStage->FIELD_WIDTH;
	m_fFieldHeight = pStage->FIELD_HEIGHT;

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pBrush);
	SAFE_RELEASE(m_pRedBrush);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pRedBrush);
#endif // _DEBUG
}

/**
*@brief	クラスで共有するリソースを破棄する
*/
void CEnemy::Finalize() {
	m_pParent = NULL;
	SAFE_RELEASE(m_pImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}