/**
* @file Explain.cpp
* @brief クラスCExplainの実装ファイル
* @author A.Yokoyama
*/
#include "stdafx.h"
#include <d2d1.h>
#include <dwrite.h>
#include "Selector.h"
#include "Explain.h"
#include "TextureLoader.h"


CExplain::CExplain(CSelector *pSystem)
{
	ID2D1RenderTarget *pTarget;

	m_pSystem = pSystem;
	m_ePhase = EXPLAIN_INIT;
	m_iTimer = 0;
	m_iFadeTimer = 0;
	m_pBlack = NULL;
	m_bFlag = true;
	m_bPhaseDone = false;

	pTarget = m_pSystem->GetRenderTarget();
	if (pTarget) {
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\explain_1.png"), &m_pExplainImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\bg_small.png"), &m_pBGImage);
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlack);
		pTarget->Release();
		pTarget = NULL;
	}

}


CExplain::~CExplain()
{
	SAFE_RELEASE(m_pBGImage);
	SAFE_RELEASE(m_pExplainImage);
	SAFE_RELEASE(m_pBlack);
}


/**
* @brief アニメーションメソッド
* @return GAMESCENE_DEFAULT:シーン継続 / GAMESCENE_END_OK:シーン完了
* @note GAMESCENE_DEFAULT や GAMESCENE_END_OK を返すことで、Selector に次に進むかどうかを教えています
*/
GameSceneResultCode CExplain::move() {

	switch (m_ePhase) {
	case EXPLAIN_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_bPhaseDone = false;
		m_ePhase = EXPLAIN_RUN_1;

	case EXPLAIN_RUN_1:
	{	
		m_iTimer++;

		if (!m_bPhaseDone) {
			if (GetAsyncKeyState(VK_RETURN))
			{
				if (!m_bFlag) {
					m_bPhaseDone = true;
					m_bFlag = true;
					m_iTimer = 0;
				}
			}
			else
			{
				m_bFlag = false;
			}
		}
		
		if (m_bPhaseDone) {
			if (m_iTimer > 30) {
				m_iTimer = 0;
				m_bFlag = true;
				m_bPhaseDone = false;
				m_ePhase = EXPLAIN_RUN_2;
			}
		}
		break;
	}

	case EXPLAIN_RUN_2:
	{
		m_iTimer++;

		if (!m_bPhaseDone) {
			if (GetAsyncKeyState(VK_RETURN))
			{
				if (!m_bFlag) {
					m_bPhaseDone = true;
					m_bFlag = true;
					m_iTimer = 0;
				}
			}
			else
			{
				m_bFlag = false;
			}
		}

		if (m_bPhaseDone) {
			if (m_iTimer > 60) {
				m_iTimer = 0;
				m_bFlag = true;
				m_bPhaseDone = false;
				m_ePhase = EXPLAIN_FADE;
			}
		}
		break;
	}

	case EXPLAIN_FADE:
		m_iFadeTimer++;
		if (m_iFadeTimer < 30)
			break;
		m_ePhase = EXPLAIN_DONE;

	case EXPLAIN_DONE:
		return GAMESCENE_END_OK;
	}

	return GAMESCENE_DEFAULT;
}


/**
* @brief 描画処理
* @
*/
void CExplain::draw(ID2D1RenderTarget *pRenderTarget) {
	D2D1_RECT_F rc;
	D2D1_SIZE_F screenSize, textureSize;
	screenSize = pRenderTarget->GetSize();
	
	switch (m_ePhase) {
	case EXPLAIN_RUN_1:
		rc.left = 0.f; rc.right = screenSize.width;
		rc.top = 0.f;	rc.bottom = screenSize.height;
		pRenderTarget->DrawBitmap(m_pExplainImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

		if (m_bPhaseDone) {
			m_pBlack->SetOpacity((FLOAT)(m_iTimer) / 30.f);
			pRenderTarget->FillRectangle(rc, m_pBlack);
		}
		break;
	case EXPLAIN_RUN_2:
		rc.left = 0.f; rc.right = screenSize.width;
		rc.top = 0.f;	rc.bottom = screenSize.height;
		pRenderTarget->DrawBitmap(m_pBGImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

		break;
	}


	switch (m_ePhase) {
	case EXPLAIN_FADE:
	case EXPLAIN_DONE:
		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;
		m_pBlack->SetOpacity(m_iFadeTimer / 30.f);
		pRenderTarget->FillRectangle(rc, m_pBlack);
		break;
	}
}