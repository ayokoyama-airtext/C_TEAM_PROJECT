/**
* @file GameOver.cpp
* @brief クラス CGameOVer の実装
*/
#include "stdafx.h"
#include <d2d1.h>
#include <dwrite.h>
#include "Selector.h"
#include "GameOver.h"
#include "TextureLoader.h"



CGameOver::CGameOver(CSelector *pSystem)
{
	m_pSystem = pSystem;
	m_pImage = NULL;
	m_pBlack = NULL;
	m_iTimer = 0;
	m_ePhase = GAMEOVER_INIT;

	ID2D1RenderTarget *pTarget = NULL;
	pTarget = m_pSystem->GetRenderTarget();
	if (pTarget) {
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\gameover.tga"), &m_pImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\sampletext.png"), &m_pImage2);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\sampletext2.png"), &m_pImage3);
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlack);
		pTarget->Release();
		pTarget = NULL;
	}

}


CGameOver::~CGameOver()
{
	SAFE_RELEASE(m_pBlack);
	SAFE_RELEASE(m_pImage);
	SAFE_RELEASE(m_pImage2);
	SAFE_RELEASE(m_pImage3);
}


/**
* @brief アニメーションメソッド
* @return GAMESCENE_DEFAULT:シーン継続 / GAMESCENE_END_OK:シーン完了
* @note GAMESCENE_DEFAULT や GAMESCENE_END_OK を返すことで、Selector に次に進むかどうかを教えています
*/
GameSceneResultCode CGameOver::move() {
	switch (m_ePhase) {
	case GAMEOVER_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_ePhase = GAMEOVER_RUN;

	case GAMEOVER_RUN:
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
			m_ePhase = GAMEOVER_FADE;
		}
		break;
	}

	case GAMEOVER_FADE:
		m_iFadeTimer++;
		if (m_iFadeTimer < 30)
			break;
		m_ePhase = GAMEOVER_DONE;

	case GAMEOVER_DONE:
		return GAMESCENE_END_OK;
	}

	return GAMESCENE_DEFAULT;
}


/**
* @brief 描画処理
* @
*/
void CGameOver::draw(ID2D1RenderTarget *pRenderTarget) {
	D2D1_RECT_F rc, rc2, rc3;
	D2D1_SIZE_F screenSize, textureSize;
	screenSize = pRenderTarget->GetSize();
	textureSize = m_pImage->GetSize();

	rc.left  = (screenSize.width - textureSize.width) / 4;	//	センタリング
	rc.right = rc.left + textureSize.width * 2.5;
	rc.top = (screenSize.height - textureSize.height) / 4;	//	センタリング
	rc.bottom = rc.top + textureSize.height * 2.5;

	pRenderTarget->DrawBitmap(m_pImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);


	rc2.left = screenSize.width / 8;	//	センタリング
	rc2.right = textureSize.width * 3.5;
	rc2.top = screenSize.height * 1.2;	//	センタリング
	rc2.bottom = textureSize.height * 2;

	pRenderTarget->DrawBitmap(m_pImage2, rc2, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

	rc3.left = screenSize.width / 8;
	rc3.right = textureSize.width * 3.5;
	rc3.top = screenSize.height * 1.35;
	rc3.bottom = textureSize.height * 2;

	pRenderTarget->DrawBitmap(m_pImage3, rc3, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

	switch (m_ePhase) {
	case GAMEOVER_FADE:
	case GAMEOVER_DONE:

		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;

		rc2.left = 0;
		rc2.top = 0;
		rc2.right = screenSize.width;
		rc2.bottom = screenSize.height;

		rc3.left = 0;
		rc3.top = 0;
		rc3.right = screenSize.width;
		rc3.bottom = screenSize.height;

		m_pBlack->SetOpacity(m_iFadeTimer / 30.f);
		pRenderTarget->FillRectangle(rc, m_pBlack);
		pRenderTarget->FillRectangle(rc2, m_pBlack);
		pRenderTarget->FillRectangle(rc3, m_pBlack);

		break;
	}
}
