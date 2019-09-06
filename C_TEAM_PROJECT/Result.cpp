/**
* @file Result.cpp
* @brief クラスCResultの実装ファイル
* @author A.Yokoyama
*/

#include "stdafx.h"
#include <d2d1.h>
#include <dwrite.h>
#include "Selector.h"
#include "Result.h"
#include "TextureLoader.h"

#define BG_FILE_NAME _T("res\\bg_small.png")
#define RANK_FILE_NAME _T("res\\player_core.png")
#define DIGIT_FILE_NAME _T("res\\digit.png")
#define SCORE_FILE_NAME _T("res\\text_score.png")
#define TEXTRANK_FILE_NAME _T("res\\text_rank.png")

CResult::CResult(CSelector *pSystem)
{
	ID2D1RenderTarget *pTarget;

	m_pSystem = pSystem;
	m_ePhase = RESULT_INIT;
	m_iTimer = 0;
	m_iFadeTimer = 0;
	m_iScore = 51234;/*pSystem->GetEndScore();*/
	m_iEndTime = 3000;/*pSystem->GetEndTime();*/
	m_fBGX = 0;
	m_pBGImage = NULL;
	m_pRankImage = NULL;
	m_pDigitImage = NULL;
	m_pTextRankImage = NULL;
	m_pBlack = NULL;

	pTarget = m_pSystem->GetRenderTarget();
	if (pTarget) {
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, BG_FILE_NAME, &m_pBGImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, RANK_FILE_NAME, &m_pRankImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, DIGIT_FILE_NAME, &m_pDigitImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, SCORE_FILE_NAME, &m_pScoreImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, TEXTRANK_FILE_NAME, &m_pTextRankImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\text_time.png"), &m_pTextTimeImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\back_to_title_button3.png"), &m_pBackToTitleImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\sampletext.png"), &m_pTextBackToTitleImage);
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, _T("res\\text_result.png"), &m_pTextResultImage);




		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlack);
		pTarget->Release();
		pTarget = NULL;
	}

}


CResult::~CResult()
{
	SAFE_RELEASE(m_pTextResultImage);
	SAFE_RELEASE(m_pTextTimeImage);
	SAFE_RELEASE(m_pBackToTitleImage);
	SAFE_RELEASE(m_pTextBackToTitleImage);
	SAFE_RELEASE(m_pTextRankImage);
	SAFE_RELEASE(m_pScoreImage);
	SAFE_RELEASE(m_pDigitImage);
	SAFE_RELEASE(m_pRankImage);
	SAFE_RELEASE(m_pBGImage);
	SAFE_RELEASE(m_pBlack);
}


/**
* @brief アニメーションメソッド
* @return GAMESCENE_DEFAULT:シーン継続 / GAMESCENE_END_OK:シーン完了
* @note GAMESCENE_DEFAULT や GAMESCENE_END_OK を返すことで、Selector に次に進むかどうかを教えています
*/
GameSceneResultCode CResult::move() {
	m_fBGX = (FLOAT)(((int)m_fBGX + 2) % 1920);

	switch (m_ePhase) {
	case RESULT_INIT:
		m_iTimer = 0;
		m_bFlag = true;
		m_ePhase = RESULT_RUN;

	case RESULT_RUN:
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
			m_ePhase = RESULT_END_OK;
		}
		break;
	}

	case RESULT_END_OK:
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
* @brief 描画処理
* @
*/
void CResult::draw(ID2D1RenderTarget *pRenderTarget) {
	D2D1_RECT_F rc, src;
	D2D1_SIZE_F screenSize, textureSize;
	screenSize = pRenderTarget->GetSize();

	//	BG
	rc.left  = -m_fBGX;
	rc.right = rc.left + screenSize.width;
	rc.top = 0.f;
	rc.bottom = rc.top + screenSize.height;
	pRenderTarget->DrawBitmap(m_pBGImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

	rc.left = screenSize.width - m_fBGX;
	rc.right = rc.left + screenSize.width;
	pRenderTarget->DrawBitmap(m_pBGImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);


	//	Result
	textureSize = m_pTextResultImage->GetSize();
	rc.top = (180.f - textureSize.height); rc.bottom = rc.top + textureSize.height * 2.f;
	rc.left = (screenSize.width - textureSize.width * 2.f) * 0.5f; rc.right = rc.left + textureSize.width * 2.f;
	pRenderTarget->DrawBitmap(m_pTextResultImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

	//	Time
	{
		int sec = m_iEndTime / 60;
		int min = sec / 60;
		sec %= 60;
		min %= 60;

		rc.left = 1920 - 720.f;
		rc.top = 360.f;	rc.bottom = rc.top + 128.f;
		for (int i = 0; i < 2; ++i) {
			int digit = sec % 10;
			src.left = (digit % 5) * 64.f;	src.right = src.left + 64.f;
			src.top = (digit / 5) * 64.f;	src.bottom = src.top + 64.f;
			rc.left -= 128.f;
			rc.right = rc.left + 128.f;
			sec /= 10;
			pRenderTarget->DrawBitmap(m_pDigitImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
		}

		rc.left -= 128.f;
		rc.right = rc.left + 128.f;
		src.left = 0.f;	src.right = src.left + 64.f;
		src.top = 128.f;	src.bottom = src.top + 64.f;
		pRenderTarget->DrawBitmap(m_pDigitImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);

		for (int i = 0; i < 2; ++i) {
			int digit = min % 10;
			src.left = (digit % 5) * 64.f;	src.right = src.left + 64.f;
			src.top = (digit / 5) * 64.f;	src.bottom = src.top + 64.f;
			rc.left -= 128.f;
			rc.right = rc.left + 128.f;
			sec /= 10;
			pRenderTarget->DrawBitmap(m_pDigitImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
		}

		float h = 410.f;
		float w = 300.f;

		rc.left -= 128.f + w;
		rc.right = rc.left + w;
		rc.top = (424.f - h * 0.5f) - 10.f;
		rc.bottom = rc.top + h;

		pRenderTarget->DrawBitmap(m_pTextTimeImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

	}

	//	Score
	{

		int score = m_iScore;
		rc.left = 1920 - 720.f;
		rc.top = 560.f;	rc.bottom = rc.top + 128.f;
			
		while (score != 0) {
			int digit = score % 10;
			src.left = (digit % 5) * 64.f;	src.right = src.left + 64.f;
			src.top = (digit / 5) * 64.f;	src.bottom = src.top + 64.f;
			rc.left -= 128.f;
			rc.right = rc.left + 128.f;
			score /= 10;
			pRenderTarget->DrawBitmap(m_pDigitImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
		}

		float h = 410.f;
		float w = 300.f;

		rc.left -= 128.f + w;
		rc.right = rc.left + w;
		rc.top = (624.f - h * 0.5f) - 10.f;
		rc.bottom = rc.top + h;

		pRenderTarget->DrawBitmap(m_pScoreImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
	}

	//	Rank
	{
		int rank = m_iScore / 10000;
		rank = (rank > 5) ? 5 : rank;
		src.left = 0.f; src.right = 96.f; src.top = 0.f; src.bottom = 96.f;
		for (int i = 0; i < rank; ++i) {
			rc.left = 560.f + 288.f * i;
			rc.right = rc.left + 192.f;
			rc.top = 710.f;
			rc.bottom = rc.top + 192.f;
			pRenderTarget->DrawBitmap(m_pRankImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
		}

		float h = 410.f;
		float w = 300.f;

		rc.left = 560.f - (144.f + w);
		rc.right = rc.left + w;
		rc.top = (806.f - h * 0.5f) + 10.f;
		rc.bottom = rc.top + h;

		pRenderTarget->DrawBitmap(m_pTextRankImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
	}



	switch (m_ePhase) {
	case RESULT_END_OK:
		textureSize = m_pBackToTitleImage->GetSize();
		rc.left = (screenSize.width - textureSize.width * 0.5f) * 0.5f; rc.right = rc.left + textureSize.width * 0.5f;
		rc.top = 932.f; rc.bottom = rc.top + 89.f;
		pRenderTarget->DrawBitmap(m_pBackToTitleImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);

		textureSize = m_pTextBackToTitleImage->GetSize();
		rc.left = (screenSize.width - textureSize.width) * 0.5f; rc.right = rc.left + textureSize.width;
		rc.top = (1060 - textureSize.height * 0.5f); rc.bottom = rc.top + textureSize.height;
		pRenderTarget->DrawBitmap(m_pTextBackToTitleImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
		break;
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