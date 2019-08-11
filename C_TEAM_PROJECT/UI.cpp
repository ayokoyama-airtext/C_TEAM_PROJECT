#include "stdafx.h"
#include <d2d1.h>
#include "Stage.h"
#include "UI.h"
#include "TextureLoader.h"

#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }


#define	UI_FILE_NAME	_T("res\\ui.png")


CUI::CUI(CStage *pStage)
{
	m_pParent = pStage;
	m_pImage = NULL;
	m_iScore = 0;
	m_iTimer = 0;

	ID2D1RenderTarget *pTarget = pStage->GetRenderTarget();
	if (pTarget) {
		//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, UI_FILE_NAME, &m_pImage);

#ifdef _DEBUG
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
#endif // _DEBUG

		SAFE_RELEASE(pTarget);
	}
}


CUI::~CUI()
{
	SAFE_RELEASE(m_pImage);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}


/**
* @brief �A�j���[�V����(1�t���[��)�̎��s
*/
void CUI::move() {

	m_iScore = m_pParent->GetScore();	//	�X�R�A�擾
	m_iTimer = m_pParent->GetTimer();	//	timer�擾	!<	/60 : �b, /60*60 : ��

}


/**
* @brief �`�揈��
* @
*/
void CUI::draw(ID2D1RenderTarget *pRenderTarget) {
	//	������m_iScore��m_iTimer���g���ĕ`�揈���������܂�


}