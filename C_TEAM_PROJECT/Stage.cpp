#include "stdafx.h"
#include <d2d1.h>
#include <list>
#include "Stage.h"
#include "TextureLoader.h"


CStage::CStage(CSelector *pSystem)
{
	m_pSystem = pSystem;
	m_bGameOver = false;
}


CStage::~CStage()
{
}

/**
* @brief アニメーション(1フレーム)の実行
* @return GAMESCENE_DEFAULT: 継続 / GAMESCENE_END_FAILURE: ゲームオーバー
*/
GameSceneResultCode CStage::move() {
	return GAMESCENE_DEFAULT;
}


/**
* @brief 描画処理
* @
*/
void CStage::draw(ID2D1RenderTarget *pRenderTarget) {

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