/**
* @file Selector.cpp
* @brief �N���XCSelector�̎����t�@�C��
*/
#include "stdafx.h"
#include <d2d1.h>
#include <dwrite.h>
#include "Selector.h"
#include "Title.h"
#include "Stage.h"
#include "GameOver.h"
#include "Explain.h"
#include "Result.h"
#include "TextureLoader.h"


CSelector::CSelector(ID2D1RenderTarget *pRenderTarget)
{
	m_pRenderTarget = pRenderTarget;
	m_pRenderTarget->AddRef();

	// �����o������
	m_iCount = 0;
	m_iWait = 0;
	m_eGamePhase = GAMEPHASE_INIT;
	m_pScene = NULL;
	

#ifdef _DEBUG
	m_pWhiteBrush = NULL;
	m_pRedBrush = NULL;
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
			20,
			L"",
			&m_pTextFormat
		);

		SAFE_RELEASE(pFactory);
	}

	// �e�L�X�g�p�u���V�̐���
	if (FAILED(m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&m_pWhiteBrush
	))) {
		SAFE_RELEASE(m_pWhiteBrush);
	}
	if (FAILED(m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&m_pRedBrush
	))) {
		SAFE_RELEASE(m_pRedBrush);
	}
#endif

}


CSelector::~CSelector()
{
	SAFE_DELETE(m_pScene);
	SAFE_RELEASE(m_pRenderTarget);
	CTextureLoader::Destroy();

#ifdef _DEBUG
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pWhiteBrush);
	SAFE_RELEASE(m_pTextFormat);
#endif
}

/**
* @brief �S�̂̃A�j���[�g���s���֐�
*
*/
void CSelector::doAnim() {
	GameSceneResultCode rc = GAMESCENE_DEFAULT;

	switch (m_eGamePhase) {
	case GAMEPHASE_INIT:
		m_eGamePhase = GAMEPHASE_RESET;


#ifdef hikegami
		m_eGamePhase = GAMEPHASE_TITLE;
		m_pScene = new CTitle(this);
#elif kmiyamoto
		m_eGamePhase = GAMEPHASE_RESULT;
		m_pScene = new CResult(this);
#elif tnakahara
		m_eGamePhase = GAMEPHASE_EXPLAIN;
		m_pScene = new CExplain(this);
#elif sugita-tadayosi
		m_eGamePhase = GAMEPHASE_GAMEOVER;
		m_pScene = new CGameOver(this);
#elif yakihiro || ayokoyama
		m_eGamePhase = GAMEPHASE_GAME;
		m_pScene = new CStage(this);
#else
		m_eGamePhase = GAMEPHASE_RESET;
#endif

		break;

	case GAMEPHASE_RESET:
		SAFE_DELETE(m_pScene);
		m_pScene = new CTitle(this);
		m_eGamePhase = GAMEPHASE_TITLE;

	case GAMEPHASE_TITLE:
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;

		SAFE_DELETE(m_pScene);
		m_pScene = new CExplain(this);
		m_eGamePhase = GAMEPHASE_EXPLAIN;

	case GAMEPHASE_EXPLAIN:
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;

		SAFE_DELETE(m_pScene);
		m_pScene = new CStage(this);
		m_eGamePhase = GAMEPHASE_GAME;

	case GAMEPHASE_GAME:
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;

		SAFE_DELETE(m_pScene);
		if (rc == GAMESCENE_END_FAILURE) {
			m_pScene = new CGameOver(this);
			m_eGamePhase = GAMEPHASE_GAMEOVER;
		}
		else {
			m_pScene = new CResult(this);
			m_eGamePhase = GAMEPHASE_RESULT;
		}
		break;

	case GAMEPHASE_GAMEOVER:
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;

		m_eGamePhase = GAMEPHASE_RESET;
		break;

	case GAMEPHASE_RESULT:
		if (m_pScene != NULL)
			rc = m_pScene->move();
		if (rc == GAMESCENE_DEFAULT)
			break;

		m_eGamePhase = GAMEPHASE_RESET;
		break;
	}

	m_iCount++;
}


/**
* @brief �S�̂̕`����s���֐�
*/
void CSelector::doDraw(ID2D1RenderTarget *pRenderTarget) {

	//	�V�[����`��
	if (m_pScene != NULL)
		m_pScene->draw(pRenderTarget);


#ifdef _DEBUG
	//	�f�o�b�O�p�\��
	TCHAR	str[256];
	D2D1_SIZE_F size;
	size = pRenderTarget->GetSize();

	_stprintf_s(str, _countof(str), _T("%08d"), m_iCount);
	D2D1_RECT_F		rc;
	rc.left = 0;
	rc.right = size.width;
	rc.top = 0;
	rc.bottom = size.height;

	if (m_pWhiteBrush) {
		pRenderTarget->DrawText(str, _tcslen(str), m_pTextFormat, &rc, m_pWhiteBrush);
	}

	rc.top = 32.f;

	switch (m_eGamePhase) {
	case GAMEPHASE_TITLE:
		_stprintf_s(str, _countof(str), _T("GAMESCENE:TITLE"));
		pRenderTarget->DrawText(str, _tcslen(str), m_pTextFormat, &rc, m_pRedBrush);
		break;
	case GAMEPHASE_EXPLAIN:
		_stprintf_s(str, _countof(str), _T("GAMESCENE:EXPLAIN"));
		pRenderTarget->DrawText(str, _tcslen(str), m_pTextFormat, &rc, m_pRedBrush);
		break;
	case GAMEPHASE_GAME:
		_stprintf_s(str, _countof(str), _T("GAMESCENE:GAME"));
		pRenderTarget->DrawText(str, _tcslen(str), m_pTextFormat, &rc, m_pRedBrush);
		break;
	case GAMEPHASE_GAMEOVER:
		_stprintf_s(str, _countof(str), _T("GAMESCENE:GAME_OVER"));
		pRenderTarget->DrawText(str, _tcslen(str), m_pTextFormat, &rc, m_pRedBrush);
		break;
	case GAMEPHASE_RESULT:
		_stprintf_s(str, _countof(str), _T("GAMESCENE:RESULT"));
		pRenderTarget->DrawText(str, _tcslen(str), m_pTextFormat, &rc, m_pRedBrush);
		break;
	}
#endif

}


/**
* @method
* @brief �Q�[����ʗp��RenderTarget ��Ԃ�
* @note	���̃��\�b�h���Ԃ��� ID2D1RenderTarget �͕K��Release ���邱��
*/
ID2D1RenderTarget *CSelector::GetRenderTarget() {
	m_pRenderTarget->AddRef();
	return m_pRenderTarget;
}

/**
* @method
* @brief	�f�o�b�O�p��TextFormat ��Ԃ�
* @note		���̃��\�b�h���Ԃ��� IDWriteTextFormat �͕K��Release ���邱��
*/
IDWriteTextFormat *CSelector::GetTextFormat() {
	m_pTextFormat->AddRef();
	return m_pTextFormat;
}