#include "stdafx.h"
#include <d2d1.h>
#include <list>
#include "Stage.h"
#include "Player.h"
#include "TextureLoader.h"


CStage::CStage(CSelector *pSystem)
{
	m_pSystem = pSystem;
	m_iGameFinishState = 0;
	m_ePhase = STAGE_INIT;
	m_pPlayer = new CPlayer(this);
}


CStage::~CStage()
{
	SAFE_DELETE(m_pPlayer);
}

/**
* @brief �A�j���[�V����(1�t���[��)�̎��s
* @return GAMESCENE_DEFAULT: �p�� / GAMESCENE_END_FAILURE: �Q�[���I�[�o�[
*/
GameSceneResultCode CStage::move() {

	switch (m_ePhase) {
	case STAGE_INIT:
		m_ePhase = STAGE_RUN;

	case STAGE_RUN:

		if (m_pPlayer)
			m_pPlayer->move();

		break;

	case STAGE_DONE:
		if (m_iGameFinishState & GAME_CLEAR) {
			return GAMESCENE_END_OK;
		}
		else {
			return GAMESCENE_END_FAILURE;
		}
	}

	return GAMESCENE_DEFAULT;
}


/**
* @brief �`�揈��
* @
*/
void CStage::draw(ID2D1RenderTarget *pRenderTarget) {

	if (m_pPlayer)
		m_pPlayer->draw(pRenderTarget);

}

/**
* @brief ID2D1RenderTarget ���擾���ĕԂ�
* @note ���̃��\�b�h�Ŏ󂯎���� RenderTarget �͎g�p��K��Release ���邱��
* @return ID2D1RenderTarget �I�u�W�F�N�g
*/
ID2D1RenderTarget *CStage::GetRenderTarget() {
	ID2D1RenderTarget *result = NULL;
	if (m_pSystem) {
		result = m_pSystem->GetRenderTarget();
	}
	return result;
}