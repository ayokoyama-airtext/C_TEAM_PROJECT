/**
*@file		EnemyDot.cpp
*@author	A.Yokoyama
*@brief		エネミードットクラスの実装
*@note		m_bDamaged はステージから変更、m_iStateは親エネミーから変更
*@note		m_bDamaged は親エネミーが見るもの
*/
#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include "Stage.h"
#include "EnemyDot.h"
#include "TextureLoader.h"


#define ENEMYDOT_FILE_NAME	_T("res\\enemy_dot.png")
#define STRAW_FILE_NAME		_T("res\\straw_799x740.png")
#define DESTROY_FILE_NAME	_T("res\\enemy_destroy.png")

//	staticメンバの初期化
ID2D1Bitmap *CEnemyDot::m_pDotImage = NULL;
ID2D1Bitmap *CEnemyDot::m_pStrawImage = NULL;
ID2D1Bitmap *CEnemyDot::m_pDestroyImage = NULL;
CStage *CEnemyDot::m_pStage = NULL;
float CEnemyDot::m_pStrawTexCoord[] = { 0.f, 799.f, 1598.f, 2397.f };
ID2D1SolidColorBrush *CEnemyDot::m_pWhiteBrush = NULL;
#ifdef _DEBUG
ID2D1SolidColorBrush *CEnemyDot::m_pBrush = NULL;
#endif // _DEBUG


CEnemyDot::CEnemyDot(IGameObject *pParentEnemy, int dotNum, int maxDotNum)
{
	m_pParent = pParentEnemy;
	m_fRad = DEFAULT_RAD;
	m_iNumber = dotNum;
	m_iMaxDotNum = maxDotNum;
	m_iState = 1;
	m_iTimer = 0;
	m_iRespawnAnimTimer = 24;
	m_iDestroyAnimTimer = 12;
	if (m_iNumber == 0) {
		m_iID = 0;
	}
	else {
		m_iID = 1;
	}
	m_fAngle = (m_pParent->GetAngle()) + 2.f * PI * ((FLOAT)m_iNumber / (FLOAT)m_iMaxDotNum);
	m_bDamaged = false;
}


CEnemyDot::~CEnemyDot()
{
}


bool CEnemyDot::move() {
	if (m_bDamaged)
		m_iDestroyAnimTimer--;

	if (!m_iState) {
		if (m_iDestroyAnimTimer >= 0)
			return true;
		else
			return false;
	}

	if (m_iRespawnAnimTimer >= 0) {
		m_iRespawnAnimTimer--;
	}

	m_iTimer++;

	m_fAngle = (m_pParent->GetAngle()) + 2.f * PI * ((FLOAT)m_iNumber / (FLOAT)m_iMaxDotNum);
	float parentX = m_pParent->GetX();
	float parentY = m_pParent->GetY();
	float parentRad = m_pParent->GetRad();

	m_fX = parentX + parentRad * cosf(m_fAngle);
	m_fY = parentY + parentRad * sinf(m_fAngle);

	return true;
}


void CEnemyDot::draw(ID2D1RenderTarget *pRenderTarget) {
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

	//	リスポーンanimation
	if (m_iRespawnAnimTimer >= 0) {
		if (m_iRespawnAnimTimer > 11) {
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			return;
		}

		int index = (11 - m_iRespawnAnimTimer) >> 2;
		src.left = 96.f * (2 -index);
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


/**
*@brief	円との当たり判定を行う
*@param [in] x	対象の中心点x座標
*@param [in] y	対象の中心点y座標
*@param [in] r	対象の半径
*@return	true 当たり / false 外れ
*/
bool CEnemyDot::collide(float x, float y, float r) {
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


/**
*@brief	GameObject との当たり判定
*@param [in] pObj	相手オブジェクト
*/
bool CEnemyDot::collide(IGameObject *pObj) {
	//	壊れてたら判定しない
	if (m_bDamaged)
		return false;

	float x = m_fX, y = m_fY, r = m_fRad;

	return pObj->collide(x, y, r);
}

/**
*@brief	ダメージを受けたときの処理
*/
void CEnemyDot::damage(float amount) {
	m_bDamaged = true;
	m_iDestroyAnimTimer--;
}

/**
*@brief	ドットの番号を返す
*/
int CEnemyDot::GetNumber() {
	return m_iNumber;
}

/**
*@brief	m_bDamagedの値を返す
*@return true 死亡 / false 生存
*/
bool CEnemyDot::IsDead() {
	return m_bDamaged;
}

/**
*@brief	m_iStateを0にする
*/
void CEnemyDot::SetStateZero() {
	m_iState = 0;
	m_bDamaged = true;
}

int CEnemyDot::GetState() {
	return m_iState;
}


void CEnemyDot::Restore(ID2D1RenderTarget *pTarget, CStage *pStage) {
	SAFE_RELEASE(m_pDotImage);
	SAFE_RELEASE(m_pStrawImage);
	SAFE_RELEASE(m_pWhiteBrush);
	SAFE_RELEASE(m_pDestroyImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, ENEMYDOT_FILE_NAME, &m_pDotImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, STRAW_FILE_NAME, &m_pStrawImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, DESTROY_FILE_NAME, &m_pDestroyImage);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pWhiteBrush);
	m_pStage = pStage;

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pBrush);
	m_pBrush->SetOpacity(0.1f);
#endif // _DEBUG
}

void CEnemyDot::Finalize() {
	m_pStage = NULL;
	SAFE_RELEASE(m_pDotImage);
	SAFE_RELEASE(m_pStrawImage);
	SAFE_RELEASE(m_pWhiteBrush);
	SAFE_RELEASE(m_pDestroyImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}
