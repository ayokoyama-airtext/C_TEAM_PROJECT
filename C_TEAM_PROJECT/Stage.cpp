#include "stdafx.h"
#include <d2d1.h>
#include <list>
#include "Stage.h"
#include "Player.h"
#include "TextureLoader.h"


CStage::CStage(CSelector *pSystem)
{
	m_pSystem = pSystem;
	m_iGameFinishState = 0;
	m_ePhase = STAGE_INIT;
	m_pPlayer = new CPlayer(this);

#ifdef _DEBUG
	m_pBrush = NULL;
	ID2D1RenderTarget *pTarget = pSystem->GetRenderTarget();
	pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
	SAFE_RELEASE(pTarget);
#endif // _DEBUG

}


CStage::~CStage()
{
	SAFE_DELETE(m_pPlayer);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}

/**
* @brief アニメーション(1フレーム)の実行
* @return GAMESCENE_DEFAULT: 継続 / GAMESCENE_END_FAILURE: ゲームオーバー
*/
GameSceneResultCode CStage::move() {

	switch (m_ePhase) {
	case STAGE_INIT:
		m_ePhase = STAGE_RUN;

	case STAGE_RUN:

		if (m_pPlayer)
			m_pPlayer->move();

		break;

	case STAGE_DONE:
		if (m_iGameFinishState & GAME_CLEAR) {
			return GAMESCENE_END_OK;
		}
		else {
			return GAMESCENE_END_FAILURE;
		}
	}

	return GAMESCENE_DEFAULT;
}


/**
* @brief 描画処理
* @
*/
void CStage::draw(ID2D1RenderTarget *pRenderTarget) {

	if (m_pPlayer)
		m_pPlayer->draw(pRenderTarget);

#ifdef _DEBUG
	if (m_pBrush) {
		D2D1_SIZE_F size = pRenderTarget->GetSize();

		D2D1_ELLIPSE el;
		el.point.x = size.width * 0.5f;
		el.point.y = size.height * 0.5f;
		el.radiusX = 960.f;
		el.radiusY = 540.f;
		pRenderTarget->DrawEllipse(el, m_pBrush);

		D2D1_RECT_F rc;
		rc.left = (size.width - 48.f) * 0.5f;
		rc.right = rc.left + 48.f;
		rc.top = (size.height - 96.f) * 0.5f;
		rc.bottom = rc.top + 96.f;
		pRenderTarget->DrawRectangle(rc, m_pBrush);

		D2D1_POINT_2F p0, p1;
		for (int i = 1; i < 4; ++i) {
			p0.x = size.width * (i / 4.f);
			p0.y = 0;
			p1.x = p0.x;
			p1.y = size.height;
			pRenderTarget->DrawLine(p0, p1, m_pBrush);
		}
	}
#endif // _DEBUG

}

/**
* @brief ID2D1RenderTarget を取得して返す
* @note このメソッドで受け取った RenderTarget は使用後必ずRelease すること
* @return ID2D1RenderTarget オブジェクト
*/
ID2D1RenderTarget *CStage::GetRenderTarget() {
	ID2D1RenderTarget *result = NULL;
	if (m_pSystem) {
		result = m_pSystem->GetRenderTarget();
	}
	return result;
}