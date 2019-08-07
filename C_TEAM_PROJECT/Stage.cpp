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
}


CStage::~CStage()
{
	SAFE_DELETE(m_pPlayer);
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