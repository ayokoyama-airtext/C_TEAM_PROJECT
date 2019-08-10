#include "stdafx.h"
#include <d2d1.h>
#include <list>
#include "Stage.h"
#include "Player.h"
#include "PlayerDot.h"
#include "BG.h"
#include "EnemyManager.h"
#include "TextureLoader.h"


const FLOAT CStage::FIELD_WIDTH = 1920.f * 3.f;
const FLOAT CStage::FIELD_HEIGHT = 1920.f * 3.f;


CStage::CStage(CSelector *pSystem)
{
	m_pSystem = pSystem;
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

#ifdef _DEBUG
		m_pBrush = NULL;
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
#endif // _DEBUG

		SAFE_RELEASE(pTarget);
	}

}


CStage::~CStage()
{

	//	�S�G�L�����̍폜
	if (m_pEnemies) {
		std::list<IGameObject*>::iterator it = m_pEnemies->begin();
		while (it != m_pEnemies->end()) {
			SAFE_DELETE(*it);
			it = m_pEnemies->erase(it);
		}
		delete m_pEnemies;
	}
	m_pEnemyManager->Finalize();
	SAFE_DELETE(m_pEnemyManager);    //!< �G�Z�b�g�V�X�e���̍폜


	//	�SPlayerDot�̍폜
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


	SAFE_DELETE(m_pBG);
	SAFE_DELETE(m_pPlayer);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}

/**
* @brief �A�j���[�V����(1�t���[��)�̎��s
* @return GAMESCENE_DEFAULT: �p�� / GAMESCENE_END_FAILURE: �Q�[���I�[�o�[
*/
GameSceneResultCode CStage::move() {

	switch (m_ePhase) {
	case STAGE_INIT:
		m_ePhase = STAGE_RUN;

	case STAGE_RUN:

		if (m_pPlayer) {
			m_pPlayer->move();
			m_pPlayer->CalcDrawCoord(&playerCoords);
		}

		//  PlayerDot�̏���
		if (m_pPlayerDots) {
			std::list<IGameObject*>::iterator it = m_pPlayerDots->begin();
			int i = 0;
			while ( (i < playerCoords.playerMaxDotNum) && (it != m_pPlayerDots->end()) ) {
				(*it++)->move();
				++i;
			}
		}

		//	�G�l�~�[�̃��X�|�[����move
		if (m_pEnemyManager && m_pEnemies) {
			//	���X�|�[��
			IGameObject *pObj = NULL;
			do {
				pObj = m_pEnemyManager->CreateEnemy();
				if (pObj != NULL)
					m_pEnemies->push_back(pObj);
			} while (pObj);

			//	move������
			std::list<IGameObject*>::iterator it = m_pEnemies->begin();
			while (it != m_pEnemies->end()) {
				(*it++)->move();
			}
		}



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
* @brief �`�揈��
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

	if (m_pPlayer)
		m_pPlayer->draw(pRenderTarget);

	//  PlayerDot�̏���
	if (m_pPlayerDots) {
		std::list<IGameObject*>::iterator it = m_pPlayerDots->begin();
		int i = 0;
		while ((i < playerCoords.playerMaxDotNum) && (it != m_pPlayerDots->end())) {
			(*it++)->draw(pRenderTarget);
			++i;
		}
	}

#ifdef _DEBUG
	if (m_pBrush) {
		D2D1_SIZE_F size = pRenderTarget->GetSize();

		D2D1_ELLIPSE el;
		el.point.x = size.width * 0.5f;
		el.point.y = size.height * 0.5f;
		el.radiusX = 960.f;
		el.radiusY = 540.f;
		pRenderTarget->DrawEllipse(el, m_pBrush);

		/*D2D1_RECT_F rc;
		rc.left = (size.width - 64.f) * 0.5f;
		rc.right = rc.left + 64.f;
		rc.top = (size.height - 128.f) * 0.5f;
		rc.bottom = rc.top + 128.f;
		pRenderTarget->DrawRectangle(rc, m_pBrush);*/

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


/***********************************************
*@method
*    �������ꂽPlayerDot�������N���X�g�ɓo�^����
*@param in pObj  �V����PlayerDot�̃I�u�W�F�N�g
************************************************/
void CStage::AddPlayerDot(IGameObject *pObj) {
	if (m_pPlayerDots) {
		m_pPlayerDots->push_back(pObj);
	}
}


/**
* @brief ID2D1RenderTarget ���擾���ĕԂ�
* @note ���̃��\�b�h�Ŏ󂯎���� RenderTarget �͎g�p��K��Release ���邱��
* @return ID2D1RenderTarget �I�u�W�F�N�g
*/
ID2D1RenderTarget *CStage::GetRenderTarget() {
	ID2D1RenderTarget *result = NULL;
	if (m_pSystem) {
		result = m_pSystem->GetRenderTarget();
	}
	return result;
}