#include "stdafx.h"
#include <d2d1.h>
#include <list>
#include "Stage.h"
#include "Player.h"
#include "PlayerDot.h"
#include "BG.h"
#include "EnemyManager.h"
#include "EnemyDot.h"
#include "UI.h"
#include "TextureLoader.h"


const FLOAT CStage::FIELD_WIDTH = 1920.f * 3.f;
const FLOAT CStage::FIELD_HEIGHT = 1920.f * 3.f;


CStage::CStage(CSelector *pSystem)
{
	m_pSystem = pSystem;
	m_iTimer = 0;
	m_iGameFinishState = 0;
	m_ePhase = STAGE_INIT;

	ID2D1RenderTarget *pTarget = pSystem->GetRenderTarget();

	if (pTarget) {

		CPlayerDot::Restore(pTarget, this);
		m_pPlayerDots = new std::list<IGameObject*>();

		m_pPlayer = new CPlayer(this);
		m_pBG = new CBG(this);

		m_pEnemyManager = new CEnemyManager(this);
		m_pEnemyManager->Restore(pTarget);
		m_pEnemies = new std::list<IGameObject*>();

		CEnemyDot::Restore(pTarget, this);
		m_pEnemyDots = new std::list<IGameObject*>();

		m_pUI = new CUI(this);

#ifdef _DEBUG
		m_pBrush = NULL;
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
#endif // _DEBUG

		SAFE_RELEASE(pTarget);
	}

}


CStage::~CStage()
{

	//	全エネミードットの削除
	if (m_pEnemyDots) {
		std::list<IGameObject*>::iterator it = m_pEnemyDots->begin();
		while (it != m_pEnemyDots->end()) {
			delete (*it);
			it = m_pEnemyDots->erase(it);
		}
		SAFE_DELETE(m_pEnemyDots);
	}
	CEnemyDot::Finalize();

	//	全敵キャラの削除
	if (m_pEnemies) {
		std::list<IGameObject*>::iterator it = m_pEnemies->begin();
		while (it != m_pEnemies->end()) {
			SAFE_DELETE(*it);
			it = m_pEnemies->erase(it);
		}
		SAFE_DELETE(m_pEnemies);
	}
	m_pEnemyManager->Finalize();
	SAFE_DELETE(m_pEnemyManager);    //!< 敵セットシステムの削除


	//	全PlayerDotの削除
	if (m_pPlayerDots) {
		std::list<IGameObject*>::iterator it = m_pPlayerDots->begin();
		while (it != m_pPlayerDots->end()) {
			SAFE_DELETE(*it);
			it = m_pPlayerDots->erase(it);
		}
		delete m_pPlayerDots;
		m_pPlayerDots = NULL;
	}
	CPlayerDot::Finalize();


	SAFE_DELETE(m_pUI);
	SAFE_DELETE(m_pBG);
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
	m_iTimer++;

	switch (m_ePhase) {
	case STAGE_INIT:
		m_ePhase = STAGE_RUN;

	case STAGE_RUN:

		if (m_pBG)
			m_pBG->move();

		if (m_pPlayer) {
			m_pPlayer->move();
			m_pPlayer->CalcDrawCoord(&playerCoords);
		}

		//  PlayerDotの処理
		if (m_pPlayerDots) {
			std::list<IGameObject*>::iterator it = m_pPlayerDots->begin();
			int i = 0;
			while ( (i < playerCoords.playerMaxDotNum) && (it != m_pPlayerDots->end()) ) {
				(*it++)->move();
				++i;
			}
		}

		//	エネミーのリスポーンとmove
		if (m_pEnemyManager && m_pEnemies) {
			//	リスポーン
			IGameObject *pObj = NULL;
			do {
				pObj = m_pEnemyManager->CreateEnemy();
				if (pObj != NULL)
					m_pEnemies->push_back(pObj);
			} while (pObj);

			//	moveさせる
			std::list<IGameObject*>::iterator it = m_pEnemies->begin();
			while (it != m_pEnemies->end()) {
				if ( (*it)->move() ) {
					++it;
				}
				else {
					SAFE_DELETE(*it);
					it = m_pEnemies->erase(it);
					m_pEnemyManager->DecreaseEnemyCount();
					m_iScore += 1000;
				}
			}
		}

		//  EnemyDotの処理
		if (m_pEnemyDots) {
			std::list<IGameObject*>::iterator it = m_pEnemyDots->begin();
			while (it != m_pEnemyDots->end()) {
				if ( (*it)->move() ) {
					++it;
				}
				else {
					SAFE_DELETE(*it);
					it = m_pEnemyDots->erase(it);
				}
				
			}
		}

		//********************************
		//		当たり判定	
		//********************************

		//	PlayerDotとエネミー
		if (m_pPlayerDots && m_pEnemies) {
			std::list<IGameObject*>::iterator it = m_pPlayerDots->begin();
			int i = 0;
			while ((i < playerCoords.playerMaxDotNum) && (it != m_pPlayerDots->end())) {
				std::list<IGameObject*>::iterator it2 = m_pEnemies->begin();
				while (it2 != m_pEnemies->end()) {
					if ( (*it2)->collide((*it)) ) {
						(*it2)->damage(1.0f);
					}
					++it2;
				}
				++it;
				++i;
			}
		}

		//	PlayerDotとEnemyDot
		if (m_pPlayerDots && m_pEnemyDots) {
			std::list<IGameObject*>::iterator it = m_pPlayerDots->begin();
			int i = 0;
			while ((i < playerCoords.playerMaxDotNum) && (it != m_pPlayerDots->end())) {
				std::list<IGameObject*>::iterator it2 = m_pEnemyDots->begin();
				while (it2 != m_pEnemyDots->end()) {
					if ((*it2)->collide((*it))) {
						(*it2)->damage(1.0f);
					}
					++it2;
				}
				++it;
				++i;
			}
		}


		if (m_pUI)
			m_pUI->move();

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

	if (m_pBG)
		m_pBG->draw(pRenderTarget);

	if (m_pEnemies) {
		std::list<IGameObject*>::iterator it = m_pEnemies->begin();
		while (it != m_pEnemies->end()) {
			(*it++)->draw(pRenderTarget);
		}
	}

	if (m_pEnemyDots) {
		std::list<IGameObject*>::iterator it = m_pEnemyDots->begin();
		while (it != m_pEnemyDots->end()) {
			(*it++)->draw(pRenderTarget);
		}
	}

	if (m_pPlayer)
		m_pPlayer->draw(pRenderTarget);

	//  PlayerDotの処理
	if (m_pPlayerDots) {
		std::list<IGameObject*>::iterator it = m_pPlayerDots->begin();
		int i = 0;
		while ((i < playerCoords.playerMaxDotNum) && (it != m_pPlayerDots->end())) {
			(*it++)->draw(pRenderTarget);
			++i;
		}
	}

	if (m_pUI)
		m_pUI->draw(pRenderTarget);

#ifdef _DEBUG
	//if (m_pBrush) {
	//	D2D1_SIZE_F size = pRenderTarget->GetSize();

	//	D2D1_ELLIPSE el;
	//	el.point.x = size.width * 0.5f;
	//	el.point.y = size.height * 0.5f;
	//	el.radiusX = 960.f;
	//	el.radiusY = 540.f;
	//	pRenderTarget->DrawEllipse(el, m_pBrush);

	//	/*D2D1_RECT_F rc;
	//	rc.left = (size.width - 64.f) * 0.5f;
	//	rc.right = rc.left + 64.f;
	//	rc.top = (size.height - 128.f) * 0.5f;
	//	rc.bottom = rc.top + 128.f;
	//	pRenderTarget->DrawRectangle(rc, m_pBrush);*/

	//	D2D1_POINT_2F p0, p1;
	//	for (int i = 1; i < 4; ++i) {
	//		p0.x = size.width * (i / 4.f);
	//		p0.y = 0;
	//		p1.x = p0.x;
	//		p1.y = size.height;
	//		pRenderTarget->DrawLine(p0, p1, m_pBrush);
	//	}
	//}
#endif // _DEBUG

}


/***********************************************
*@method
*    生成されたPlayerDotをリンクリストに登録する
*@param in pObj  新しいPlayerDotのオブジェクト
************************************************/
void CStage::AddPlayerDot(IGameObject *pObj) {
	if (m_pPlayerDots) {
		m_pPlayerDots->push_back(pObj);
	}
}

/***********************************************
*@method
*    生成されたEnemyDotをリンクリストに登録する
*@param in pObj  新しいEnemyDotのオブジェクト
************************************************/
void CStage::AddEnemyDot(IGameObject *pObj) {
	if (m_pEnemyDots) {
		m_pEnemyDots->push_back(pObj);
	}
}


int CStage::GetScore() {
	return m_iScore;
}
int CStage::GetTimer() {
	return m_iTimer;
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