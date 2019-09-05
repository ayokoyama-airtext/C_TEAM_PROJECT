/**
*@file		Enemy.cpp
*@author	A.Yokoyama
*@brief		�G�l�~�[�N���X�̎���
*/
#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include <time.h>
#include "Stage.h"
#include "Enemy.h"
#include "EnemyDot.h"
#include "TextureLoader.h"


#define ENEMY_FILE_NAME	_T("res\\enemy_core.png")
#define BELT_FILE_NAME	_T("res\\belt.png")
#define ENEMY_DEST_FILE_NAME	_T("res\\enemy_destroy.png")

const float CEnemy::ROTATION_SPEED = 0.025f;
const float CEnemy::ENEMY_SPEED = 5.f;
const float CEnemy::ENEMY_ESCAPE_SPEED = 8.f;
const float CEnemy::ENEMY_ESCAPE_ROTATION_SPEED = 0.2f;
const float CEnemy::ENEMY_ESCAPE_ANGLE = cosf(PI * 0.5f);
const float CEnemy::SEARCH_ANGLE = cosf(PI * 0.5f);


FLOAT CEnemy::m_pRandomMove[] = { 0.f, -1.f, 1.f, -1.f, 1.f };
FLOAT CEnemy::m_pTexCoord[] = { 0.f, 96.f, 192.f };
const INT CEnemy::m_iRandomMoveSize = _countof(CEnemy::m_pRandomMove);
FLOAT CEnemy::m_fFieldWidth = 0.f;
FLOAT CEnemy::m_fFieldHeight = 0.f;
ID2D1Bitmap *CEnemy::m_pCoreImage = NULL;
ID2D1Bitmap *CEnemy::m_pBeltImage = NULL;
ID2D1Bitmap *CEnemy::m_pDestroyImage = NULL;
CStage *CEnemy::m_pParent = NULL;
#ifdef _DEBUG
ID2D1SolidColorBrush *CEnemy::m_pBrush = NULL;
ID2D1SolidColorBrush *CEnemy::m_pRedBrush = NULL;
#endif // _DEBUG



CEnemy::CEnemy(float x, float y, float scale, int dotNum)
{
	m_fX = x;
	m_fY = y;
	m_fVX = 0;
	m_fVY = 0;
	m_fRad = BELT_RAD;
	m_fCoreRad = (FLOAT)(CORE_LENGTH >> 1);
	m_fScale = scale;
	m_iMaxDotNum = dotNum;
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
	m_iAnimationTimer = 0;
	m_iDamagedTimer = 0;
	m_iDestroyAnimTimer = 12;
	m_iRespawnAnimTimer = 12;
	m_bDamaged = false;
}


CEnemy::~CEnemy()
{
}


/**
*@brief	�A�j���[�V�������\�b�h
*@return	true : ���� / false : ���S
*/
bool CEnemy::move() {

	if (m_iRespawnAnimTimer >= 0) {
		m_iRespawnAnimTimer--;
	}

	//	m_bDamaged == true �Ȃ� �h�b�g���c���Ă�����State��0�ɐݒ肵�Ă���false ��Ԃ�
	if (m_bDamaged) {
		if (m_iDotNum > 0) {
			for (int i = 0; i < m_iDotNum; ++i) {
				m_pDots[i]->SetStateZero();
			}
			m_iDotNum = 0;
		}
		if (--m_iDestroyAnimTimer >= 0) {
			return true;
		}
		
		return false;
	}

	//	�h�b�g������ł�����State��0�ɂ��āA�Ǘ�����O��
	for (int i = 0; i < m_iDotNum; ++i) {
		if (m_pDots[i]->IsDead()) {
			m_pDots[i]->SetStateZero();
			m_pDots[i] = m_pDots.back();
			m_pDots.pop_back();
			m_iDotNum--;
			i--;
		}
	}


	m_iAnimationTimer = (m_iAnimationTimer + 1) % 54;

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
			float cos = (dirVX * vx + dirVY * vy) / l;
			if (cos > SEARCH_ANGLE) {	//	���G�p�x���Ȃ�
				if (l < (FLOAT)SEARCH_LENGTH) {	//	���G�������Ȃ�
					m_iBehaviorFlag = EFLAG_CHASE;	//	�ǐՂֈڍs
					m_iTimer = 0;
					break;
				}
			}

			if (l > APPROACH_LENGHT) {	//	���ꂷ���Ă���߂Â�
				l = 1.0f / l;
				float sin = (dirVX * vy - dirVY * vx);
				if (sin > 0) {	//	�v���C���[�͐i�s�����E��
					if (cos > cosf(ROTATION_SPEED)) {	//	1�t���[���̉�]�\�p�x�ȓ��Ȃ�
						m_fVX = vx * l;
						m_fVY = vy * l;
						m_fAngle = atan2(m_fVY, m_fVX);
						m_fVX *= ENEMY_SPEED;
						m_fVY *= ENEMY_SPEED;
					}
					else {
						m_fVX = 0;
						m_fVY = 0;
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
						m_fVX = 0;
						m_fVY = 0;
						m_fAngle -= ROTATION_SPEED;
					}
				}
			}
			else {
				if (m_iTimer == 0) {
					m_iRandomMoveIndex = (rand() >> 4) % m_iRandomMoveSize;
					m_iTimer = 180;
				}
				//	�i�s������ύX
				m_fAngle += ROTATION_SPEED * m_pRandomMove[m_iRandomMoveIndex];
				//	�i�s�����։���
				m_fVX = ENEMY_SPEED * dirVX;
				m_fVY = ENEMY_SPEED * dirVY;
				m_iTimer--;
			}

		}

		break;

	case EFLAG_CHASE:
		{
			float l = sqrtf(vx * vx + vy * vy);

			//	���ꂷ������ǐՒ��~
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
			if (sin > 0) {	//	�v���C���[�͐i�s�����E��
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
void CEnemy::draw(ID2D1RenderTarget *pRenderTarget) {
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	float playerDrawX = m_pParent->playerCoords.playerDrawX;
	float playerDrawY = m_pParent->playerCoords.playerDrawY;

	float angle = 180.f * (m_fAngle + PI * 0.5f) / PI;
	float opacity = 1.0f;
	if (m_iDamagedTimer > 0) {
		opacity = 0.25f;
	}
	int texIndex = (m_iAnimationTimer % 18) / 6;

	D2D1_RECT_F rc, src;
	D2D1_POINT_2F	center;
	center.x = playerDrawX + (m_fX - playerX);
	center.y = playerDrawY + (m_fY - playerY);

	D2D1::Matrix3x2F rotation = D2D1::Matrix3x2F::Rotation(angle, center);
	pRenderTarget->SetTransform(rotation);

	//	���X�|�[���A�j���[�V����
	if (m_iRespawnAnimTimer >= 0) {
		int index = (11 - m_iRespawnAnimTimer) >> 2;
		src.left = 96.f * (2 - index);
		src.right = src.left + 96.f;
		src.top = 0.f;
		src.bottom = src.top + 96.f;

		rc.left = center.x - 96.f - 24.f;
		rc.right = rc.left + 192.f;
		rc.top = center.y - 96.f;
		rc.bottom = rc.top + 192.f;
		pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

		rc.left = center.x - 96.f + 24.f;
		rc.right = rc.left + 192.f;

		pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		return;
	}

	//	���S�A�j���[�V����
	if (m_bDamaged) {
		int index = (11 - m_iDestroyAnimTimer) >> 2;
		src.left = 96.f * index;
		src.right = src.left + 96.f;
		src.top = 0.f;
		src.bottom = src.top + 96.f;

		rc.left = center.x - 96.f - 24.f;
		rc.right = rc.left + 192.f;
		rc.top = center.y - 96.f;
		rc.bottom = rc.top + 192.f;

		pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

		rc.left = center.x - 96.f + 24.f;
		rc.right = rc.left + 192.f;

		pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		return;
	}
	
	//	Core
	src.left = m_pTexCoord[texIndex];
	src.top = 0.f;
	src.right = src.left + CORE_LENGTH;
	src.bottom = src.top + CORE_LENGTH;

	rc.left = center.x - CORE_LENGTH * 0.5f;
	rc.right = rc.left + CORE_LENGTH;
	rc.top = center.y - CORE_LENGTH * 0.5f;
	rc.bottom = rc.top + CORE_LENGTH;

	pRenderTarget->DrawBitmap(m_pCoreImage, rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

	//	Belt
	if (m_iDotNum > 0) {
		src.left = 0.f;
		src.top = 0.f;
		src.right = src.left + (BELT_RAD << 1);
		src.bottom = src.top + (BELT_RAD << 1);

		rc.left = center.x - BELT_RAD;
		rc.right = rc.left + (BELT_RAD << 1);
		rc.top = center.y - BELT_RAD;
		rc.bottom = rc.top + (BELT_RAD << 1);

		pRenderTarget->DrawBitmap(m_pBeltImage, rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
	}

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

#ifdef _DEBUG
	//	�x���g
	/*D2D1_ELLIPSE el;
	el.point.x = center.x;
	el.point.y = center.y;
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	pRenderTarget->DrawEllipse(el, m_pBrush);*/

	//	�h�b�g
	/*for (int i = 0; i < m_iDotNum; ++i) {
		angle = m_fAngle + 2.f * PI * ((FLOAT)i / (FLOAT)m_iMaxDotNum);
		el.point.x = dx + m_fRad * cosf(angle);
		el.point.y = dy + m_fRad * sinf(angle);
		el.radiusX = DOT_RAD;
		el.radiusY = DOT_RAD;
		pRenderTarget->DrawEllipse(el, m_pBrush);
	}*/

	////	�R�A
	//D2D1_POINT_2F point;
	//angle = 180.f * (m_fAngle + PI * 0.5f) / PI;
	//rc.left = center.x - CORE_LENGTH * 0.5f;
	//rc.right = rc.left + CORE_LENGTH;
	//rc.top = center.y - CORE_LENGTH * 0.5f;
	//rc.bottom = rc.top + CORE_LENGTH;
	//point.x = rc.left + CORE_LENGTH * 0.5f;
	//point.y = rc.top + CORE_LENGTH * 0.5f;
	//D2D1::Matrix3x2F rotate = D2D1::Matrix3x2F::Rotation(angle, point);
	//pRenderTarget->SetTransform(rotate);

	//pRenderTarget->DrawRectangle(rc, m_pBrush);

	//rc.bottom = rc.top + CORE_LENGTH * 0.2f;
	//pRenderTarget->DrawRectangle(rc, m_pRedBrush);

	//pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
#endif // _DEBUG


}

/**
*@brief	�~�Ƃ̓����蔻����s��
*@param [in] x	�Ώۂ̒��S�_x���W
*@param [in] y	�Ώۂ̒��S�_y���W
*@param [in] r	�Ώۂ̔��a
*@return	true ������ / false �O��
*/
bool CEnemy::collide(float x, float y, float r) {
	if (m_iDamagedTimer > 0 || m_bDamaged)
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
*@brief	GameObject �Ƃ̓����蔻��
*@param [in] pObj	����I�u�W�F�N�g
*@return	true ������ / false �O��
*/
bool CEnemy::collide(IGameObject *pObj) {
	if (m_iDamagedTimer > 0 || m_bDamaged)
		return false;

	float x = m_fX, y = m_fY, r = m_fCoreRad;

	return pObj->collide(x, y, r);
}


void CEnemy::damage(float amount) {
	if (m_iRespawnAnimTimer >= 0)
		return;

	if (m_iDotNum > 0) {
		m_pDots[--m_iDotNum]->SetStateZero();
		m_pDots.pop_back();
		m_iBehaviorFlag = EFLAG_ESCAPE;
		m_iDamagedTimer = DAMAGED_DURATION;
		m_iTimer = 0;
	}
	else {
		m_bDamaged = true;
		m_iDestroyAnimTimer--;
	}
}

/**
*@brief	X���W��Ԃ�
*/
float CEnemy::GetX() {
	return m_fX;
}
/**
*@brief	Y���W��Ԃ�
*/
float CEnemy::GetY() {
	return m_fY;
}
/**
*@brief	Rad��Ԃ�
*/
float CEnemy::GetRad() {
	return m_fRad;
}
/**
*@brief	Angle��Ԃ�
*/
float CEnemy::GetAngle() {
	return m_fAngle;
}


/**
*@brief	�t���O��ݒ肷��
*@note	flag 1: �U������
*			 2: �h�b�g���_���[�W���󂯂�
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
*@brief	�N���X�ŋ��L���郊�\�[�X�𐶐�����
*/
void CEnemy::Restore(ID2D1RenderTarget *pTarget, CStage *pStage) {
	SAFE_RELEASE(m_pCoreImage);
	SAFE_RELEASE(m_pBeltImage);
	SAFE_RELEASE(m_pDestroyImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, ENEMY_FILE_NAME, &m_pCoreImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, BELT_FILE_NAME, &m_pBeltImage);
	CTextureLoader::CreateD2D1BitmapFromFile(pTarget, ENEMY_DEST_FILE_NAME, &m_pDestroyImage);
	m_pParent = pStage;

	m_fFieldWidth = pStage->FIELD_WIDTH;
	m_fFieldHeight = pStage->FIELD_HEIGHT;

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pBrush);
	m_pBrush->SetOpacity(0.1f);
	SAFE_RELEASE(m_pRedBrush);
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pRedBrush);
	m_pRedBrush->SetOpacity(0.1f);
#endif // _DEBUG
}

/**
*@brief	�N���X�ŋ��L���郊�\�[�X��j������
*/
void CEnemy::Finalize() {
	m_pParent = NULL;
	SAFE_RELEASE(m_pCoreImage);
	SAFE_RELEASE(m_pBeltImage);
	SAFE_RELEASE(m_pDestroyImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}