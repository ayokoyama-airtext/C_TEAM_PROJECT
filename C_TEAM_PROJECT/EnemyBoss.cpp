/**
*@file		EnemyBoss.cpp
*@author	A.Yokoyama
*@brief		�G�l�~�[Boss�N���X�̎���
*/
#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include <time.h>
#include "Stage.h"
#include "EnemyBoss.h"
#include "EnemyBossDot.h"
#include "Player.h"
#include "Bullet.h"
#include "TextureLoader.h"


#define ENEMY_FILE_NAME	_T("res\\boss_core.png")
#define BELT_FILE_NAME	_T("res\\boss_belt.png")
#define ENEMY_DEST_FILE_NAME	_T("res\\enemy_destroy.png")

const float CEnemyBoss::ROTATION_SPEED = 0.005f;
const float CEnemyBoss::ENEMY_SPEED = 1.5f;
const float CEnemyBoss::ENEMY_ESCAPE_SPEED = 8.f;
const float CEnemyBoss::ENEMY_ESCAPE_ROTATION_SPEED = 0.2f;
const float CEnemyBoss::ENEMY_ESCAPE_ANGLE = cosf(PI * 0.5f);
const float CEnemyBoss::SEARCH_ANGLE = cosf(PI * 0.5f);
const float CEnemyBoss::BULLET_ANGLE_GAP_BIG = PI / 6.f;
const float CEnemyBoss::BULLET_ANGLE_GAP_SMALL = PI / 24.f;


FLOAT CEnemyBoss::m_pRandomMove[] = { 0.f, -1.f, 1.f, -1.f, 1.f };
FLOAT CEnemyBoss::m_pTexCoord[] = { 0.f, 480.f, 960.f };
const INT CEnemyBoss::m_iRandomMoveSize = _countof(CEnemyBoss::m_pRandomMove);
FLOAT CEnemyBoss::m_fFieldWidth = 0.f;
FLOAT CEnemyBoss::m_fFieldHeight = 0.f;
ID2D1Bitmap *CEnemyBoss::m_pCoreImage = NULL;
ID2D1Bitmap *CEnemyBoss::m_pBeltImage = NULL;
ID2D1Bitmap *CEnemyBoss::m_pDestroyImage = NULL;
CStage *CEnemyBoss::m_pParent = NULL;
#ifdef _DEBUG
ID2D1SolidColorBrush *CEnemyBoss::m_pBrush = NULL;
ID2D1SolidColorBrush *CEnemyBoss::m_pRedBrush = NULL;
#endif // _DEBUG



CEnemyBoss::CEnemyBoss(float x, float y, float scale)
{
	m_fX = x;
	m_fY = y;
	m_fVX = 0;
	m_fVY = 0;
	m_fRad = BELT_RAD;
	m_fCoreRad = (FLOAT)(CORE_LENGTH >> 1);
	m_fScale = scale;
	m_iMaxDotNum = 24;
	m_iDotNum = m_iMaxDotNum;
	m_fAngle = PI * 0.5f;
	m_iRandomMoveIndex = 0;
	srand(time(NULL));


	for (int i = 0; i < m_iMaxDotNum; ++i) {
		CEnemyBossDot *pObj = NULL;
		pObj = new CEnemyBossDot(this, i, m_iMaxDotNum);
		if (pObj) {
			m_pDots.push_back(pObj);
			m_pParent->AddEnemyDot(pObj);
		}
	}

	m_iBehaviorFlag = EFLAG_IDLE;

	m_iTimer = 0;
	m_iAnimationTimer = 0;
	m_iDamagedTimer = 0;
	m_iAttackTimer = 0;
	m_iDestroyAnimTimer = 24;
	m_iRespawnAnimTimer = 24;
	m_bDamaged = false;

	m_pParent->SetBoss(this);
}


CEnemyBoss::~CEnemyBoss()
{
	m_pParent->RemoveBoss();
}


/**
*@brief	�A�j���[�V�������\�b�h
*/
bool CEnemyBoss::move() {
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
		else {
			return false;
		}

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
		if (m_iRespawnAnimTimer >= 0) {
			m_iRespawnAnimTimer--;
		}
		m_iTimer++;
		if (m_iTimer < 60)
			break;

		m_iBehaviorFlag = EFLAG_CHASE;
		m_iTimer = 0;
		break;

	case EFLAG_CHASE:	//	��苗���ȏ㗣��Ă���ߊ��
	{
		//	�v���C���[�Ƃ̊p�x���v�Z
		float l = sqrtf(vx * vx + vy * vy);
		if (l <= (FLOAT)APPROACH_LENGTH) {	//	�U���������Ȃ�
			m_iTimer = MOVE_DURATION;
			m_iBehaviorFlag = EFLAG_MOVE;	//	�U���s��
			break;
		}

		//	�v���C���[�����ֈړ�
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

	case EFLAG_MOVE:
	{
		float l = sqrtf(vx * vx + vy * vy);

		if (l > (FLOAT)ATTACK_LENGTH) {	//	�U�������O�Ȃ�
			m_iBehaviorFlag = EFLAG_CHASE;	//	�ǐՍs��
			m_iTimer = 0;
			break;
		}
		
		l = 1.0f / l;
		float sin = (dirVX * vy - dirVY * vx) * l;
		float cos = (dirVX * vx + dirVY * vy) * l;
		if (m_iTimer == MOVE_DURATION) {
			m_iRandomMoveIndex = (rand() >> 4) % m_iRandomMoveSize;
		}
		//	�i�s������ύX
		m_fAngle += ROTATION_SPEED * m_pRandomMove[m_iRandomMoveIndex];
		//	�i�s�����։���
		m_fVX = ENEMY_SPEED * dirVX;
		m_fVY = ENEMY_SPEED * dirVY;
		m_iTimer--;

		if (m_iTimer <= 0) {
			m_iAttackTimer = 0;
			m_iBehaviorFlag = EFLAG_ATTACK;
		}
	}

	break;

	case EFLAG_ATTACK:	//	�U��
	{
		if (m_iAttackTimer % ATTACK_INTERVAL == 0) {
			float l = 1.f / sqrtf(vx * vx + vy * vy);
			float sin = vy * l;
			float cos = vx* l;
			float angle = atan2(sin, cos);

			int attackNum = (m_iAttackTimer / ATTACK_INTERVAL);
			switch (attackNum) {
			case 0:
				{
					CBullet *bullet = NULL;
					bullet = new CBullet(m_fX, m_fY, angle);
					m_pParent->AddBullet(bullet);
					for (int i = 0; i < 2; ++i) {
						float bulletAngle = angle + BULLET_ANGLE_GAP_SMALL * (1 - i * 2);
						bullet = new CBullet(m_fX, m_fY, bulletAngle);
						m_pParent->AddBullet(bullet);
					}
				}
			break;

			case 1:
				{
					CBullet *bullet = NULL;
					bullet = new CBullet(m_fX, m_fY, angle);
					m_pParent->AddBullet(bullet);
					for (int i = 0; i < 2; ++i) {
						float bulletAngle = angle + BULLET_ANGLE_GAP_BIG * 2.f + BULLET_ANGLE_GAP_SMALL * (1 - i * 2);
						bullet = new CBullet(m_fX, m_fY, bulletAngle);
						m_pParent->AddBullet(bullet);

						bulletAngle = angle - BULLET_ANGLE_GAP_BIG * 2.f + BULLET_ANGLE_GAP_SMALL * (1 - i * 2);
						bullet = new CBullet(m_fX, m_fY, bulletAngle);
						m_pParent->AddBullet(bullet);
					}
				}
				break;

			case 2:
				{
					CBullet *bullet = NULL;
					bullet = new CBullet(m_fX, m_fY, angle);
					m_pParent->AddBullet(bullet);
					for (int i = 0; i < 2; ++i) {
						float bulletAngle = angle + BULLET_ANGLE_GAP_BIG + BULLET_ANGLE_GAP_SMALL * (1 - i * 2);
						bullet = new CBullet(m_fX, m_fY, bulletAngle);
						m_pParent->AddBullet(bullet);

						bulletAngle = angle - BULLET_ANGLE_GAP_BIG + BULLET_ANGLE_GAP_SMALL * (1 - i * 2);
						bullet = new CBullet(m_fX, m_fY, bulletAngle);
						m_pParent->AddBullet(bullet);
					}
				}
				break;

			}	//switch (attackNum)

			CSoundManager::PlayOneShot(0, 0.75f);
		}	//	if (m_iAttackTimer % ATTACK_INTERVAL == 0) 

		if (++m_iAttackTimer > ATTACK_INTERVAL * 2) {
			m_iTimer = MOVE_DURATION + 120;
			m_iBehaviorFlag = EFLAG_MOVE;
		}
	}
	break;

	}	//	switch (m_iBehaviorFlag)


	m_fX += m_fVX;
	m_fY += m_fVY;

	if (m_fAngle > 2.f * PI) {
		m_fAngle -= 2.f * PI;
	}
	else if (m_fAngle < -2.f * PI) {
		m_fAngle += 2.f * PI;
	}


	//	�t�B�[���h�͈͂���o�Ȃ��悤�ɒ���
	if (m_fX < m_fRad * 1.5f) {
		m_fX = m_fRad * 1.5f;
	}
	else if (m_fX > m_fFieldWidth - m_fRad * 1.5f) {
		m_fX = m_fFieldWidth - m_fRad * 1.5f;
	}
	if (m_fY < m_fRad * 1.5f) {
		m_fY = m_fRad * 1.5f;
	}
	else if (m_fY > m_fFieldHeight - m_fRad * 1.5f) {
		m_fY = m_fFieldHeight - m_fRad * 1.5f;
	}

	return true;
}


/**
*@brief	�`�惁�\�b�h
*/
void CEnemyBoss::draw(ID2D1RenderTarget *pRenderTarget) {
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

	//	���X�|�[��animation
	if (m_iRespawnAnimTimer >= 0) {
		int tex = ((23 - m_iRespawnAnimTimer) % 12) >> 2;

		src.left = 96.f * (2 - tex);
		src.right = src.left + 96.f;
		src.top = 0.f;
		src.bottom = src.top + 96.f;

		for (int i = 0; i < 6; ++i) {
			rc.left = (center.x + 160 * cosf(PI * (0.5f + 0.33333f * i))) - 96.f;
			rc.right = rc.left + 192.f;
			rc.top = (center.y + 160 * sinf(PI * (0.5f + 0.33333f * i))) - 96.f;
			rc.bottom = rc.top + 192.f;
			pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
		}

		for (int i = 0; i < 6; ++i) {
			rc.left = (center.x + 320 * cosf(PI * (0.5f + 0.33333f * i))) - 96.f;
			rc.right = rc.left + 192.f;
			rc.top = (center.y + 320 * sinf(PI * (0.5f + 0.33333f * i))) - 96.f;
			rc.bottom = rc.top + 192.f;
			pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
		}

		for (int i = 0; i < 6; ++i) {
			rc.left = (center.x + 480 * cosf(PI * (0.5f + 0.33333f * i))) - 96.f;
			rc.right = rc.left + 192.f;
			rc.top = (center.y + 480 * sinf(PI * (0.5f + 0.33333f * i))) - 96.f;
			rc.bottom = rc.top + 192.f;
			pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
		}

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		return;
	}


	//	���S�A�j���[�V����
	if (m_bDamaged) {
		int timer = (23 - m_iDestroyAnimTimer);
		int tex;
		int rad;

		if (0 <= timer && timer <= 11) {
			tex = timer >> 2;

			src.left = 96.f * tex;
			src.right = src.left + 96.f;
			src.top = 0.f;
			src.bottom = src.top + 96.f;

			for (int i = 0; i < 6; ++i) {
				rc.left = (center.x + 160 * cosf(PI * (0.5f + 0.33333f * i))) - 96.f;
				rc.right = rc.left + 192.f;
				rc.top = (center.y + 160 * sinf(PI * (0.5f + 0.33333f * i))) - 96.f;
				rc.bottom = rc.top + 192.f;
				pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
			}
		}

		if (6 <= timer && timer <= 17) {
			tex = (timer - 6) >> 2;

			src.left = 96.f * tex;
			src.right = src.left + 96.f;
			src.top = 0.f;
			src.bottom = src.top + 96.f;

			for (int i = 0; i < 6; ++i) {
				rc.left = (center.x + 320 * cosf(PI * (0.5f + 0.33333f * i))) - 96.f;
				rc.right = rc.left + 192.f;
				rc.top = (center.y + 320 * sinf(PI * (0.5f + 0.33333f * i))) - 96.f;
				rc.bottom = rc.top + 192.f;
				pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
			}
		}

		if (12 <= timer && timer <= 23) {
			tex = (timer - 12) >> 2;

			src.left = 96.f * tex;
			src.right = src.left + 96.f;
			src.top = 0.f;
			src.bottom = src.top + 96.f;

			for (int i = 0; i < 6; ++i) {
				rc.left = (center.x + 480 * cosf(PI * (0.5f + 0.33333f * i))) - 96.f;
				rc.right = rc.left + 192.f;
				rc.top = (center.y + 480 * sinf(PI * (0.5f + 0.33333f * i))) - 96.f;
				rc.bottom = rc.top + 192.f;
				pRenderTarget->DrawBitmap(m_pDestroyImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
			}
		}


		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		return;
	}

	//	Core
	src.left = m_pTexCoord[texIndex];
	src.top = 0.f;
	src.right = src.left + 480.f;
	src.bottom = src.top + 480.f;

	rc.left = center.x - CORE_LENGTH * 0.5f;
	rc.right = rc.left + CORE_LENGTH;
	rc.top = center.y - CORE_LENGTH * 0.5f;
	rc.bottom = rc.top + CORE_LENGTH;

	pRenderTarget->DrawBitmap(m_pCoreImage, rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

	if (m_iDotNum > 0) {
		//	Belt
		src.left = 0.f;
		src.top = 0.f;
		src.right = src.left + (BELT_RAD << 1);
		src.bottom = src.top + (BELT_RAD << 1);

		rc.left = center.x - BELT_RAD;
		rc.right = rc.left + (BELT_RAD << 1);
		rc.top = center.y - BELT_RAD;
		rc.bottom = rc.top + (BELT_RAD << 1);

		pRenderTarget->DrawBitmap(m_pBeltImage, rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
	}

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

#ifdef _DEBUG
	//	�x���g
	D2D1_ELLIPSE el;
	el.point.x = center.x;
	el.point.y = center.y;
	el.radiusX = m_fRad;
	el.radiusY = m_fRad;
	pRenderTarget->DrawEllipse(el, m_pBrush);

	//	�h�b�g
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
	rc.left = center.x - CORE_LENGTH * 0.5f;
	rc.right = rc.left + CORE_LENGTH;
	rc.top = center.y - CORE_LENGTH * 0.5f;
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
bool CEnemyBoss::collide(float x, float y, float r) {
	if (m_iDamagedTimer > 0 || m_bDamaged)
		return false;

	float vx = m_fX - x;
	float vy = m_fY - y;
	float l2 = vx * vx + vy * vy;
	float d;

	if (m_iDotNum > 0) {
		d = m_fRad + r;
	}
	else {
		d = m_fCoreRad + r;
	}

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
bool CEnemyBoss::collide(IGameObject *pObj) {
	if (m_iDamagedTimer > 0 || m_bDamaged)
		return false;

	float x = m_fX, y = m_fY, r;

	if (m_iDotNum > 0) {
		r = m_fRad;
	}
	else {
		r = m_fCoreRad;
	}

	return pObj->collide(x, y, r);
}


bool CEnemyBoss::collideWithPlayer(CPlayer *pPlayer) {
	if (m_iDotNum > 0) {
		return pPlayer->collideWithBoss(m_fX, m_fY, m_fRad);
	}
	else {
		return false;
	}
}

void CEnemyBoss::damage(float amount) {
	if (m_iDotNum > 0) {
		/*m_pDots[--m_iDotNum]->SetStateZero();
		m_pDots.pop_back();
		m_iBehaviorFlag = EFLAG_ESCAPE;
		m_iDamagedTimer = DAMAGED_DURATION;
		m_iTimer = 0;*/
	}
	else {
		m_bDamaged = true;
		m_iDestroyAnimTimer--;
	}
}

/**
*@brief	X���W��Ԃ�
*/
float CEnemyBoss::GetX() {
	return m_fX;
}
/**
*@brief	Y���W��Ԃ�
*/
float CEnemyBoss::GetY() {
	return m_fY;
}
/**
*@brief	Rad��Ԃ�
*/
float CEnemyBoss::GetRad() {
	return m_fRad;
}
/**
*@brief	Angle��Ԃ�
*/
float CEnemyBoss::GetAngle() {
	return m_fAngle;
}

/**
*@brief	�t���O��ݒ肷��
*@note	flag 1: �U������
*			 2: �h�b�g���_���[�W���󂯂�
*/
void CEnemyBoss::SetFlag(int flag) {

	switch (flag) {
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
}


/**
*@brief	�N���X�ŋ��L���郊�\�[�X�𐶐�����
*/
void CEnemyBoss::Restore(ID2D1RenderTarget *pTarget, CStage *pStage) {
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
void CEnemyBoss::Finalize() {
	m_pParent = NULL;
	SAFE_RELEASE(m_pCoreImage);
	SAFE_RELEASE(m_pBeltImage);
	SAFE_RELEASE(m_pDestroyImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}