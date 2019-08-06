#include "stdafx.h"
#include <d2d1.h>
#include <list>
#include "Stage.h"
#include "TextureLoader.h"


CStage::CStage(CSelector *pSystem)
{
	m_pSystem = pSystem;
	m_bGameOver = false;
}


CStage::~CStage()
{
}

/**
* @brief �A�j���[�V����(1�t���[��)�̎��s
* @return GAMESCENE_DEFAULT: �p�� / GAMESCENE_END_FAILURE: �Q�[���I�[�o�[
*/
GameSceneResultCode CStage::move() {
	return GAMESCENE_DEFAULT;
}


/**
* @brief �`�揈��
* @
*/
void CStage::draw(ID2D1RenderTarget *pRenderTarget) {

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