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
	// Direct Write 初期化
	{
		HRESULT hr;
		IDWriteFactory *pFactory;
		// DirectWriteファクトリー生成
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(pFactory),
			reinterpret_cast<IUnknown **>(&pFactory)
		);
		// テキストフォーマット生成
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
* @brief アニメーション(1フレーム)の実行
*/
void CUI::move() {

	m_iScore = m_pParent->GetScore();	//	スコア取得
	m_iTimer = m_pParent->GetTimer();	//	timer取得	!<	/60 : 秒, /60*60 : 分

}


/**
* @brief 描画処理
* @
*/
void CUI::draw(ID2D1RenderTarget *pRenderTarget) {
	//	ここにm_iScoreやm_iTimerを使って描画処理を書きます
	
	
	
	D2D1_SIZE_F size = pRenderTarget->GetSize();

	//  四角形の準備
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
	int   h, m, s, f;  //  時,分,秒,フレーム
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
		pRenderTarget->DrawText(tmpStr, len, m_pTextFormat, &rc, m_pBrush);  //  TIME描写
	}

	//描写するスコア
	WCHAR tmpStr2[MAX_PATH];
	_stprintf_s(tmpStr2, MAX_PATH, L"SCORE:%5d", m_iScore);
	if (m_pBrush) {
		pRenderTarget->DrawText(tmpStr2, len, m_pTextFormat, &rc2, m_pBrush);  //  SCORE描写
	}

	
	
	
	
	
}