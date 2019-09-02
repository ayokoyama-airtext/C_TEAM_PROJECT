/**
*@file		Enemy02.cpp
*@author	
*@brief		�G�l�~�[02�N���X�̎���
*/
#include "stdafx.h"
#include <d2d1.h>

#include <cmath>
#include "Stage.h"
#include "Enemy02.h"
#include "EnemyDot.h"
#include "TextureLoader.h"


#define ENEMY_FILE_NAME	_T("res\\enemy.png")
const float CEnemy02::ROTATION_SPEED = 0.025f;
const float CEnemy02::ENEMY_SPEED = 8.f;
const float CEnemy02::ENEMY_ESCAPE_SPEED = 8.f;
const float CEnemy02::ENEMY_ESCAPE_ROTATION_SPEED = 0.2f;
const float CEnemy02::ENEMY_ESCAPE_ANGLE = cosf(PI * 0.5f);
const float CEnemy02::SEARCH_ANGLE = cosf(PI * 0.5f);

float CEnemy02::m_fFieldWidth = 0.f;
float CEnemy02::m_fFieldHeight = 0.f;
ID2D1Bitmap *CEnemy02::m_pImage = NULL;
CStage *CEnemy02::m_pParent = NULL;
#ifdef _DEBUG
ID2D1SolidColorBrush *CEnemy02::m_pBrush = NULL;
ID2D1SolidColorBrush *CEnemy02::m_pRedBrush = NULL;
#endif // _DEBUG



CEnemy02::CEnemy02(float x, float y, float scale)
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


CEnemy02::~CEnemy02()
{
}


/**
*@brief	�A�j���[�V�������\�b�h
*/
bool CEnemy02::move() {
	if (m_bDamaged)
		return false;

	for (int i = 0; i < m_iDotNum; ++i) {
		if (m_pDots[i]->IsDead()) {
			m_pDots[i]->SetStateZero();
			m_pDots[i] = m_pDots.back();
			m_pDots.pop_back();
			m_iDotNum--;
		}
	}


	//************************************
	//				����
	//************************************

	//	�v���C���[�̏����擾
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	//	�v���C���[�Ƃ̑��΃x�N�g�����v�Z
	float vx = playerX - m_fX;
	float vy = playerY - m_fY;
	//	�����̌������v�Z
	float dirVX = cosf(m_fAngle);
	float dirVY = sinf(m_fAngle);

	//************************************
	//************************************


	//	m_iBehaviorFlag�ɉ����čs����ω�������
	switch (m_iBehaviorFlag) {
	case EFLAG_IDLE:	//	�ҋ@
		m_fVX = 0;
		m_fVY = 0;
		m_iTimer++;
		if (m_iTimer < 120)
			break;

		m_iBehaviorFlag = EFLAG_SEARCH;
		m_iTimer = 0;
		break;

	case EFLAG_SEARCH:	//	���G
	{
		//	�v���C���[�Ƃ̊p�x���v�Z
		float l = sqrtf(vx * vx + vy * vy);
		float cos = (dirVX * vx + dirVY * dirVY) / l;
		if (cos > SEARCH_ANGLE) {	//	���G�p�x���Ȃ�
			if (l < (FLOAT)SEARCH_LENGTH) {	//	���G�������Ȃ�
				m_iBehaviorFlag = EFLAG_CHASE;	//	�ǐՂֈڍs
				break;
			}
		}

		//	�i�s������ύX
		m_fAngle += ROTATION_SPEED;
		//	�i�s�����։���
		m_fVX = ENEMY_SPEED * dirVX;
		m_fVY = ENEMY_SPEED * dirVY;
	}

	break;

	case EFLAG_CHASE:
		m_iTimer++;
		{
			float l = 1.f / sqrtf(vx * vx + vy * vy);
			float sin = (dirVX * vy - dirVY * vx);
			float cos = (dirVX * vx + dirVY * vy) * l;
			if (sin > 0) {	//	�v���C���[�͐i�s�����E��
				if (cos < cosf(ROTATION_SPEED)) {	//	1�t���[���̉�]�\�p�x�ȓ��Ȃ�
					m_fVX = vx * l;
					m_fVY = vy * l;
					m_fAngle = atan2(m_fVY, m_fVX);
					m_fVX *= ENEMY_SPEED;
					m_fVY *= ENEMY_SPEED;
				}
				else {
					m_fVX = dirVX * ENEMY_SPEED*m_fAngle;
					m_fVY = dirVY * ENEMY_SPEED*m_fAngle;
					m_fAngle += ROTATION_SPEED;
				}
			}
			else {	//	�v���C���[�͐i�s��������
				if (cos > cosf(ROTATION_SPEED)) {	//	1�t���[���̉�]�\�p�x�ȓ��Ȃ�
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

	case EFLAG_ESCAPE:	//	�_���[�W���󂯂�ƈ�莞�ԓ�����
		if (m_iDamagedTimer < 0)
			m_iDamagedTimer--;
		if (m_iTimer++ < ESCAPE_DURATION) {
			m_iBehaviorFlag = EFLAG_SEARCH;
			m_iTimer = 0;
			break;
		}

		float l = 1.f / sqrtf(vx * vx + vy * vy);
		float sin = (dirVX * vy - dirVY * vx);
		float cos = (dirVX * vx + dirVY * vy) * l;
		if (sin > 0) {
			if (cos > ENEMY_ESCAPE_ANGLE) {
				m_fAngle += ENEMY_ESCAPE_ROTATION_SPEED;
			}
		}
		else {
			if (cos > ENEMY_ESCAPE_ANGLE) {
				m_fAngle -= ENEMY_ESCAPE_ROTATION_SPEED;
			}
		}

		m_fVX = ENEMY_ESCAPE_SPEED * cosf(m_fAngle);
		m_fVY = ENEMY_ESCAPE_SPEED * sinf(m_fAngle);
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


	//	�t�B�[���h�͈͂���o�Ȃ��悤�ɒ���
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
*@brief	�`�惁�\�b�h
*/
void CEnemy02::draw(ID2D1RenderTarget *pRenderTarget) {
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	float playerDrawX = m_pParent->playerCoords.playerDrawX;
	float playerDrawY = m_pParent->playerCoords.playerDrawY;

	float dx = playerDrawX + (m_fX - playerX);
	float dy = playerDrawY + (m_fY - playerY);

	D2D1_RECT_F rc, src;


#ifdef _DEBUG
	//	�x���g
	D2D1_ELLIPSE el;
	el.point.x = dx;
	el.point.y = dy;
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	pRenderTarget->DrawEllipse(el, m_pBrush);

	//	�h�b�g
	FLOAT angle = 0;
	/*for (int i = 0; i < m_iDotNum; ++i) {
		angle = m_fAngle + 2.f * PI * ((FLOAT)i / (FLOAT)m_iMaxDotNum);
		el.point.x = dx + m_fRad * cosf(angle);
		el.point.y = dy + m_fRad * sinf(angle);
		el.radiusX = DOT_RAD;
		el.radiusY = DOT_RAD;
		pRenderTarget->DrawEllipse(el, m_pBrush);
	}*/

	//	�R�A
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
*@brief	�~�Ƃ̓����蔻����s��
*@param [in] x	�Ώۂ̒��S�_x���W
*@param [in] y	�Ώۂ̒��S�_y���W
*@param [in] r	�Ώۂ̔��a
*@return	true ������ / false �O��
*/
bool CEnemy02::collide(float x, float y, float r) {
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
*@return	true ������ / false �O��
*/
bool CEnemy02::collide(IGameObject *pObj) {
	if (m_iDamagedTimer > 0)
		return false;

	float x = m_fX, y = m_fY, r = m_fCoreRad;

	return pObj->collide(x, y, r);
}


void CEnemy02::damage(float amount) {
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
*@brief	X���W��Ԃ�
*/
float CEnemy02::GetX() {
	return m_fX;
}
/**
*@brief	Y���W��Ԃ�
*/
float CEnemy02::GetY() {
	return m_fY;
}
/**
*@brief	Rad��Ԃ�
*/
float CEnemy02::GetRad() {
	return m_fRad;
}
/**
*@brief	Angle��Ԃ�
*/
float CEnemy02::GetAngle() {
	return m_fAngle;
}


/**
*@brief	�N���X�ŋ��L���郊�\�[�X�𐶐�����
*/
void CEnemy02::Restore(ID2D1RenderTarget *pTarget, CStage *pStage) {
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
*@brief	�N���X�ŋ��L���郊�\�[�X��j������
*/
void CEnemy02::Finalize() {
	m_pParent = NULL;
	SAFE_RELEASE(m_pImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}