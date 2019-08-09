#include "stdafx.h"
#include <d2d1.h>
#include <dwrite.h>
#include "Selector.h"
#include "Result.h"
#include "TextureLoader.h"


CResult::CResult(CSelector *pSystem)
{
	ID2D1RenderTarget *pTarget;

	m_pSystem = pSystem;
	m_ePhase = RESULT_INIT;
	m_iTimer = 0;
	m_iFadeTimer = 0;
	//m_pImage = NULL;
	m_pBlack = NULL;

	pTarget = m_pSystem->GetRenderTarget();
	if (pTarget) {
		//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\"), &m_pImage);
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlack);
		pTarget->Release();
		pTarget = NULL;
	}

}


CResult::~CResult()
{
	SAFE_RELEASE(m_pImage);
	SAFE_RELEASE(m_pBlack);
}


/**
* @brief �A�j���[�V�������\�b�h
* @return GAMESCENE_DEFAULT:�V�[���p�� / GAMESCENE_END_OK:�V�[������
* @note GAMESCENE_DEFAULT �� GAMESCENE_END_OK ��Ԃ����ƂŁASelector �Ɏ��ɐi�ނ��ǂ����������Ă��܂�
*/
GameSceneResultCode CResult::move() {

	switch (m_ePhase) {
	case RESULT_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_ePhase = RESULT_RUN;

	case RESULT_RUN:
	{	//	���ꂪ�Ȃ��� bDone �̏������ŃG���[���ł�
		bool bDone = false;
		m_iTimer++;

		if (GetAsyncKeyState(VK_SPACE))
		{
			if (!m_bFlag) {
				bDone = true;
				m_bFlag = true;
			}
		}
		else
		{
			m_bFlag = false;
		}

		if (bDone) {
			m_iFadeTimer = 0;
			m_ePhase = RESULT_FADE;
		}
		break;
	}

	case RESULT_FADE:
		m_iFadeTimer++;
		if (m_iFadeTimer < 30)
			break;
		m_ePhase = RESULT_DONE;

	case RESULT_DONE:
		return GAMESCENE_END_OK;
	}

	return GAMESCENE_DEFAULT;
}


/**
* @brief �`�揈��
* @
*/
void CResult::draw(ID2D1RenderTarget *pRenderTarget) {
	D2D1_RECT_F rc;
	D2D1_SIZE_F screenSize, textureSize;
	screenSize = pRenderTarget->GetSize();
	//textureSize = m_pImage->GetSize();
	//rc.left  = (screenSize.width - textureSize.width) / 2;	//	�Z���^�����O
	//rc.right = rc.left + textureSize.width;
	//rc.top = (screenSize.height - textureSize.height) / 2;	//	�Z���^�����O
	//rc.bottom = rc.top + textureSize.height;
	//pRenderTarget->DrawBitmap(m_pImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);


	switch (m_ePhase) {
	case RESULT_FADE:
	case RESULT_DONE:
		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;
		m_pBlack->SetOpacity(m_iFadeTimer / 30.f);
		pRenderTarget->FillRectangle(rc, m_pBlack);
		break;
	}
}