/**
*@file		Player.cpp
*@author	A.Yokoyama
*@brief		�v���C���[�N���X�̎���
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
*@brief	�A�j���[�V�������\�b�h
*/
bool CPlayer::move() {
	return true;
}


/**
*@brief	�`�惁�\�b�h
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
*@brief	�~�Ƃ̓����蔻����s��
*@param [in] x	�Ώۂ̒��S�_x���W
*@param [in] y	�Ώۂ̒��S�_y���W
*@param [in] r	�Ώۂ̔��a
*@return	true ������ / false �O��
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
*@brief	GameObject �Ƃ̓����蔻��
*@param [in] pObj	����I�u�W�F�N�g
*/
bool CPlayer::collide(IGameObject *pObj) {
	float x = circle.fx, y = circle.fy, r = circle.rad;

	return pObj->collide(x, y, r);
}

/**
*@brief	�_���[�W���󂯂��Ƃ��̏���
*/
void CPlayer::damage(float amount) {

}