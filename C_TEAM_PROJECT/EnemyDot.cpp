/**
*@file		EnemyDot.cpp
*@author	A.Yokoyama
*@brief		�G�l�~�[�h�b�g�N���X�̎���
*@note		m_bDamaged �̓X�e�[�W����ύX�Am_iState�͐e�G�l�~�[����ύX
*@note		m_bDamaged �͐e�G�l�~�[���������
*/
#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include "Stage.h"
#include "EnemyDot.h"
#include "TextureLoader.h"


#define ENEMYDOT_FILE_NAME	_T("res\\enemydot.png")

//	static�����o�̏�����
ID2D1Bitmap *CEnemyDot::m_pImage = NULL;
CStage *CEnemyDot::m_pStage = NULL;
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
	m_bDamaged = false;
}


CEnemyDot::~CEnemyDot()
{
}


bool CEnemyDot::move() {
	if (!m_iState)
		return false;

	float angle = (m_pParent->GetAngle()) + 2.f * PI * ((FLOAT)m_iNumber / (FLOAT)m_iMaxDotNum);
	float parentX = m_pParent->GetX();
	float parentY = m_pParent->GetY();
	float parentRad = m_pParent->GetRad();

	m_fX = parentX + parentRad * cosf(angle);
	m_fY = parentY + parentRad * sinf(angle);

	return true;
}


void CEnemyDot::draw(ID2D1RenderTarget *pRenderTarget) {
	float playerDrawX = m_pStage->playerCoords.playerDrawX;
	float playerDrawY = m_pStage->playerCoords.playerDrawY;
	float playerX = m_pStage->playerCoords.playerX;
	float playerY = m_pStage->playerCoords.playerY;

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
*@brief	�~�Ƃ̓����蔻����s��
*@param [in] x	�Ώۂ̒��S�_x���W
*@param [in] y	�Ώۂ̒��S�_y���W
*@param [in] r	�Ώۂ̔��a
*@return	true ������ / false �O��
*/
bool CEnemyDot::collide(float x, float y, float r) {
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
*@brief	GameObject �Ƃ̓����蔻��
*@param [in] pObj	����I�u�W�F�N�g
*/
bool CEnemyDot::collide(IGameObject *pObj) {
	//	���Ă��画�肵�Ȃ�
	if (m_bDamaged)
		return false;

	float x = m_fX, y = m_fY, r = m_fRad;

	return pObj->collide(x, y, r);
}

/**
*@brief	�_���[�W���󂯂��Ƃ��̏���
*/
void CEnemyDot::damage(float amount) {
	m_bDamaged = true;
}

/**
*@brief	m_bDamaged�̒l��Ԃ�
*@return true ���S / false ����
*/
bool CEnemyDot::IsDead() {
	return m_bDamaged;
}

/**
*@brief	m_iState��0�ɂ���
*/
void CEnemyDot::SetStateZero() {
	m_iState = 0;
}


void CEnemyDot::Restore(ID2D1RenderTarget *pTarget, CStage *pStage) {
	SAFE_RELEASE(m_pImage);
	//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, ENEMYDOT_FILE_NAME, &m_pImage);
	m_pStage = pStage;

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pBrush);
#endif // _DEBUG
}

void CEnemyDot::Finalize() {
	m_pStage = NULL;
	SAFE_RELEASE(m_pImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}
