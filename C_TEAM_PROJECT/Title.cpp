/**
* @file Title.cpp
* @brief クラス CTitle の実装
*/
#include "stdafx.h"
#include "SoundManager.h"
#include <d2d1.h>
#include <dwrite.h>
#include "Selector.h"
#include "Title.h"
#include "TextureLoader.h"


CTitle::CTitle(CSelector *pSystem)
{
	ID2D1RenderTarget *pTarget;
	m_pSystem = pSystem;
	m_ePhase = TitlePhase::TITLE_INIT;
	m_iFadeTimer = 0;
	m_pImage = NULL;
	m_pBlack = NULL;

	pTarget = m_pSystem->GetRenderTarget();
	if (pTarget) {
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\title.tga"), &m_pImage);
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlack);
		pTarget->Release();
		pTarget = NULL;
	}
	mFrame = 0;

}


CTitle::~CTitle()
{
	SAFE_RELEASE(m_pImage);
	SAFE_RELEASE(m_pBlack);
}


/**
* @brief アニメーションメソッド
* @return GAMESCENE_DEFAULT:シーン継続 / GAMESCENE_END_OK:シーン完了
* @note GAMESCENE_DEFAULT や GAMESCENE_END_OK を返すことで、Selector に次に進むかどうかを教えています
*/
GameSceneResultCode CTitle::move() {

	switch (m_ePhase) {
	case TITLE_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_ePhase = TITLE_RUN;
		m_iFadeTimer = 0;

	case TITLE_RUN:
	{	//	これがないと bDone の初期化でエラーがでる
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
			m_ePhase = TITLE_FADE;
		}
		break;
	}

	case TITLE_FADE:
		m_iFadeTimer++;
		if (m_iFadeTimer < 30)
			break;
		m_ePhase = TITLE_DONE;

	case TITLE_DONE:
		return GAMESCENE_END_OK;
	}

	return GAMESCENE_DEFAULT;
}
float plus = 0.1f, flash = 1.0f;


/**
* @brief 描画処理
* @
*/
void CTitle::draw(ID2D1RenderTarget *pRenderTarget) {
	D2D1_RECT_F rc,rc2;
	D2D1_SIZE_F screenSize, textureSize;
	screenSize = pRenderTarget->GetSize();
	textureSize = m_pImage->GetSize();

	rc.left = 0;								//(screenSize.width - textureSize.width) /4;	//	センタリング
	rc.right = screenSize.width;				//rc.left + textureSize.width*2;
	rc.top = 0;									//(screenSize.height - textureSize.height) /4;	//	センタリング
	rc.bottom = screenSize.height*0.55f;		//rc.top + textureSize.height*2;

	mFrame = (mFrame + 1) % ANIM_FRAME;
	rc2.left = 0;
	rc2.right = textureSize.width;
	rc2.top = 0;
	rc2.bottom = textureSize.height*0.5f;
	pRenderTarget->DrawBitmap(m_pImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rc2);

	if (mFrame > ANIM_FRAME -40) {
		if (flash >= 1.0f || flash <= 0.0f)
			plus = -plus;
		flash += plus;
	}
	rc.left = 0;
	rc.right = screenSize.width;
	rc.top = screenSize.height*0.5f;
	rc.bottom = screenSize.height;

	rc2.left = 0;
	rc2.right = textureSize.width;
	rc2.top = textureSize.height*0.5f;
	rc2.bottom = textureSize.height;

	pRenderTarget->DrawBitmap(m_pImage, rc, flash, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rc2);
	
	switch (m_ePhase) {
	case TITLE_FADE:
	case TITLE_DONE:
		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;
		rc2.left = 0;
		rc2.right = screenSize.width;
		rc2.top = 0;
		rc2.bottom = screenSize.height;
		m_pBlack->SetOpacity(m_iFadeTimer / 30.f);
		pRenderTarget->FillRectangle(rc, m_pBlack);
		break;
	}
}