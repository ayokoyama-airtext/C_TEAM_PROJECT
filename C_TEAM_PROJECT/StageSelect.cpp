#include "stdafx.h"
#include <d2d1.h>
#include "Selector.h"
#include "StageSelect.h"
#include "TextureLoader.h"


CStageSelect::CStageSelect(CSelector *pSelector)
{
	m_pParent = pSelector;
	m_ePhase = STAGESELECT_INIT;
	m_bFlag = true;
	m_iTimer = 0;
	m_iFadeTimer = 0;

	m_pImage = NULL;
	m_pBrush = NULL;

	ID2D1RenderTarget *pTarget = pSelector->GetRenderTarget();
	{
		//CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\stage_select.png"), &m_pImage);
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBrush);

		pTarget->Release();
		pTarget = NULL;
	}
}


CStageSelect::~CStageSelect()
{
	SAFE_RELEASE(m_pImage);
	SAFE_RELEASE(m_pBrush);
}


GameSceneResultCode CStageSelect::move() {

	switch (m_ePhase) {
	case STAGESELECT_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_ePhase = STAGESELECT_RUN;

	case STAGESELECT_RUN:
		m_ePhase = STAGESELECT_FADE;

	case STAGESELECT_FADE:
		m_ePhase = STAGESELECT_DONE;

	case STAGESELECT_DONE:
		return GAMESCENE_END_OK;
	}

	return GAMESCENE_DEFAULT;
}


/**
* @brief 描画処理
* @
*/
void CStageSelect::draw(ID2D1RenderTarget *pRenderTarget) {
	D2D1_RECT_F rc;
	D2D1_SIZE_F screenSize, textureSize;
	screenSize = pRenderTarget->GetSize();
	//textureSize = m_pImage->GetSize();
	//rc.left  = (screenSize.width - textureSize.width) / 2;	//	センタリング
	//rc.right = rc.left + textureSize.width;
	//rc.top = (screenSize.height - textureSize.height) / 2;	//	センタリング
	//rc.bottom = rc.top + textureSize.height;
	//pRenderTarget->DrawBitmap(m_pImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

	switch (m_ePhase) {
	case STAGESELECT_FADE:
	case STAGESELECT_DONE:
		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;
		m_pBrush->SetOpacity(m_iFadeTimer / 30.f);
		pRenderTarget->FillRectangle(rc, m_pBrush);
		break;
	}
}