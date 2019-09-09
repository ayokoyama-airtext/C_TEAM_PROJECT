/**
* @file Explain.cpp
* @brief クラスCExplainの実装ファイル
* @author A.Yokoyama
*/
#include "stdafx.h"
#include "SoundManager.h"
#include <d2d1.h>
#include <dwrite.h>
#include "Selector.h"
#include "Explain.h"
#include "TextureLoader.h"

#define SWITCH_TIME 30.f
#define FADE_TIME 60.f


CExplain::CExplain(CSelector *pSystem)
{
	ID2D1RenderTarget *pTarget;

	m_pSystem = pSystem;
	m_ePhase = EXPLAIN_INIT;
	m_iTimer = 0;
	m_iFadeTimer = 0;
	m_iPageNum = 0;
	m_iOldPageNum = 0;
	m_pBlack = NULL;
	m_bFlag = true;
	m_bPhaseDone = false;


	pTarget = m_pSystem->GetRenderTarget();
	if (pTarget) {
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\bg_small.png"), &m_pBGImage);
		
		for (int i = 0; i < _countof(m_pExplainImages); ++i) {
			CTextureLoader::CreateD2D1BitmapFromFile(pTarget, fileNames[i], &m_pExplainImages[i]);
		}
		for (int i = 0; i < _countof(m_pExplainArrowImages); ++i) {
			CTextureLoader::CreateD2D1BitmapFromFile(pTarget, arrowNames[i], &m_pExplainArrowImages[i]);
		}

		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlack);
		pTarget->Release();
		pTarget = NULL;
	}

}


CExplain::~CExplain()
{
	SAFE_RELEASE(m_pBGImage);
	for (int i = 0; i < _countof(m_pExplainArrowImages); ++i) {
		SAFE_RELEASE(m_pExplainArrowImages[i]);
	}
	for (int i = 0; i < _countof(m_pExplainImages); ++i) {
		SAFE_RELEASE(m_pExplainImages[i]);
	}
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
		m_iOldPageNum = m_iPageNum;

		if (GetAsyncKeyState(0x41))		//	Aキー : 戻る
		{
			if (!m_bFlag) {
				if (--m_iPageNum >= 0) {
					m_bPhaseDone = true;
					m_bFlag = true;
				}
				else {
					m_iPageNum = 0;
				}
			}
		}
		else if(GetAsyncKeyState(0x44)) {	//	Dキー : 進む
			if (!m_bFlag) {
				if (++m_iPageNum < _countof(m_pExplainImages)) {
					m_bPhaseDone = true;
					m_bFlag = true;
				}
				else {
					m_iPageNum = _countof(m_pExplainImages) - 1;
				}
			}
		}
		else if (GetAsyncKeyState(VK_SPACE)) {
			if (m_iPageNum == _countof(m_pExplainImages) - 1) {
				m_ePhase = EXPLAIN_FADE;
				m_iFadeTimer = 0;
				break;
			}
		}
		else
		{
			m_bFlag = false;
		}
		
		if (m_bPhaseDone) {
			m_iTimer = 0;
			m_bFlag = true;
			m_bPhaseDone = false;
			m_ePhase = EXPLAIN_RUN_2;
		}
		break;
	}

	case EXPLAIN_RUN_2:
	{
		m_iTimer++;

		if (m_iTimer >= SWITCH_TIME) {
			m_iTimer = 0;
			m_ePhase = EXPLAIN_RUN_1;
		}
		break;
	}

	case EXPLAIN_FADE:
		m_iFadeTimer++;
		CSoundManager::SetVolumeStreamSound(0, (1.f - (m_iFadeTimer / FADE_TIME)));
		if (m_iFadeTimer < FADE_TIME)
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
	case EXPLAIN_FADE:
	case EXPLAIN_DONE:
	case EXPLAIN_RUN_1:
	{
		rc.left = 0.f;	rc.right  = screenSize.width;
		rc.top  = 0.f;	rc.bottom = screenSize.height;
		pRenderTarget->DrawBitmap(m_pExplainImages[m_iPageNum], rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

		int timer = m_iTimer % 120;
		float opacity = 0.25f + ( (timer < 60) ? 0.75f * (1.f - timer / 60.f) : 0.75f * ((timer / 60.f) - 1.f) );
		textureSize = m_pExplainArrowImages[0]->GetSize();
		rc.left = (screenSize.width - textureSize.width) * 0.5f;	rc.right  = rc.left + textureSize.width;
		rc.top  = (980.f - textureSize.height * 0.5f);				rc.bottom = rc.top + textureSize.height;
		if (m_iPageNum <= 0) {
			pRenderTarget->DrawBitmap(m_pExplainArrowImages[0], rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
		}
		else if (m_iPageNum < _countof(m_pExplainImages) - 1) {
			pRenderTarget->DrawBitmap(m_pExplainArrowImages[1], rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
		}
		else {
			textureSize = m_pExplainArrowImages[2]->GetSize();
			rc.left = (screenSize.width - textureSize.width) * 0.5f;	rc.right = rc.left + textureSize.width;
			rc.top = (980.f - textureSize.height * 0.5f);				rc.bottom = rc.top + textureSize.height;
			pRenderTarget->DrawBitmap(m_pExplainArrowImages[2], rc, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
		}

		break;
	}
	case EXPLAIN_RUN_2:
		rc.left = 0.f; rc.right = screenSize.width;
		rc.top = 0.f;	rc.bottom = screenSize.height;
		float translation = 1920.f * (m_iTimer / SWITCH_TIME);

		if (m_iOldPageNum < m_iPageNum) {
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(-translation, 0.f));
			pRenderTarget->DrawBitmap(m_pExplainImages[m_iOldPageNum], rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(1920.f - translation, 0.f));
			pRenderTarget->DrawBitmap(m_pExplainImages[m_iPageNum], rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
		}
		else {
			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(translation, 0.f));
			pRenderTarget->DrawBitmap(m_pExplainImages[m_iOldPageNum], rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

			pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(-1920.f + translation, 0.f));
			pRenderTarget->DrawBitmap(m_pExplainImages[m_iPageNum], rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
		}
		
		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		break;
	}


	switch (m_ePhase) {
	case EXPLAIN_FADE:
	case EXPLAIN_DONE:
		rc.left = 0;
		rc.top = 0;
		rc.right = screenSize.width;
		rc.bottom = screenSize.height;
		m_pBlack->SetOpacity(m_iFadeTimer / FADE_TIME);
		pRenderTarget->FillRectangle(rc, m_pBlack);
		break;
	}
}


//	static field
const wchar_t CExplain::fileNames[][64] = {
	L"res\\explain_1.png",
	L"res\\explain_2.png",
	L"res\\explain_3_1.png",
	L"res\\explain_3_2.png",
	L"res\\explain_4_1.png",
	L"res\\explain_4_2.png",
	L"res\\explain_5_1.png",
	L"res\\explain_5_2.png",
	L"res\\explain_6_1.png",
	L"res\\explain_6_2.png",
	L"res\\explain_7_1.png",
	L"res\\explain_7_2.png",
	L"res\\explain_8_1.png",
	L"res\\explain_8_2.png",
};

const wchar_t CExplain::arrowNames[][64] = {
	L"res\\explain_d.png",
	L"res\\explain_ad.png",
	L"res\\explain_asp.png",
};
