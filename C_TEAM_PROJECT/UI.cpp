#include "stdafx.h"
#include <d2d1.h>
#include "Stage.h"
#include "UI.h"
#include "TextureLoader.h"
#include <dwrite.h>

#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }


#define	UI_FILE_NAME	_T("res\\ui.png")


CUI::CUI(CStage *pStage)
{
	// Direct Write ������
	{
		HRESULT hr;
		IDWriteFactory *pFactory;
		// DirectWrite�t�@�N�g���[����
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pFactory),
			reinterpret_cast<IUnknown **>(&pFactory)
		);
		// �e�L�X�g�t�H�[�}�b�g����
		hr = pFactory->CreateTextFormat(
			_T("consolas"),
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			50,
			L"",
			&m_pTextFormat
		);
		

		SAFE_RELEASE(pFactory);
	}

	

	m_pParent = pStage;
	m_pImage = NULL;
	m_iScore = 0;
	m_iTimer = 0;

	ID2D1RenderTarget *pTarget = pStage->GetRenderTarget();
	if (pTarget) {
		//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, UI_FILE_NAME, &m_pImage);

		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);

		SAFE_RELEASE(pTarget);
	}
}


CUI::~CUI()
{
	SAFE_RELEASE(m_pImage);

	SAFE_RELEASE(m_pBrush);

	SAFE_RELEASE(m_pTextFormat);
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
	
	
	
	D2D1_SIZE_F size = pRenderTarget->GetSize();

	//  �l�p�`�̏���
	//D2D1_SIZE_F size = pRenderTarget->GetSize();
	D2D1_RECT_F rc;
	rc.left = size.width * 0.8;
	rc.top = 0;
	rc.right = size.width;
	rc.bottom = size.height * 0.75f;

	D2D1_RECT_F rc2;
	rc2.left = size.width * 0.8;
	rc2.top = size.height * 0.05f;
	rc2.right = size.width;
	rc2.bottom = size.height * 0.80f;

	WCHAR    tmpStr[MAX_PATH];
	int len;
	int   h, m, s, f;  //  ��,��,�b,�t���[��
	f = m_iTimer;
	s = f / 60;
	m = s / 60;
	
	f %= 60;
	s %= 60;
	m %= 60;
	_stprintf_s(tmpStr, MAX_PATH, L"TIME :%02d:%02d", m, s);
	len = wcslen(tmpStr);
	m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER);
	
	if (m_pBrush) {
		pRenderTarget->DrawText(tmpStr, len, m_pTextFormat, &rc, m_pBrush);  //  TIME�`��
	}

	//�`�ʂ���X�R�A
	WCHAR tmpStr2[MAX_PATH];
	_stprintf_s(tmpStr2, MAX_PATH, L"SCORE:%5d", m_iScore);
	if (m_pBrush) {
		pRenderTarget->DrawText(tmpStr2, len, m_pTextFormat, &rc2, m_pBrush);  //  SCORE�`��
	}

	
	
	
	
	
}