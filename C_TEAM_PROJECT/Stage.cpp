/**
* @file Stage.cpp
* @brief �N���XCStage�̎����t�@�C��
* @author A.Yokoyama
*/
#include "stdafx.h"
#include <d2d1.h>
#include <list>
#include "Stage.h"
#include "Player.h"
#include "PlayerDot.h"
#include "BG.h"
#include "EnemyManager.h"
#include "EnemyDot.h"
#include "EnemyBoss.h"
#include "Shot.h"
#include "Bullet.h"
#include "UI.h"
#include "TextureLoader.h"


const FLOAT CStage::FIELD_WIDTH = 1920.f * 3.f;
const FLOAT CStage::FIELD_HEIGHT = 1920.f * 3.f;

CStage::CStage(CSelector *pSystem)
{
	m_pSystem = pSystem;
	m_iTimer = 0;
	m_iScore = 0;
	m_iGameFinishState = 0;
	m_pBoss = NULL;
	m_ePhase = STAGE_INIT;

	ID2D1RenderTarget *pTarget = pSystem->GetRenderTarget();

	if (pTarget) {

		m_pPlayerDots = new std::list<IGameObject*>();	//!< player����ɍ�邱�ƁI

		m_pPlayer = new CPlayer(this);
		m_pPlayer->CalcDrawCoord(&playerCoords);
		m_pBG = new CBG(this);

		CPlayerDot::Restore(pTarget, this, m_pPlayer);	//!< player�̌��Restore���邱��!

		m_pEnemyManager = new CEnemyManager(this);
		m_pEnemyManager->Restore(pTarget);
		m_pEnemies = new std::list<IGameObject*>();

		CEnemyDot::Restore(pTarget, this);
		m_pEnemyDots = new std::list<IGameObject*>();

		CShot::Restore(this, m_pPlayer, pTarget);
		m_pShots = new std::list<IGameObject*>();

		CBullet::Restore(this, pTarget);
		m_pBullets = new std::list<IGameObject*>();

		m_pUI = new CUI(this);

		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);

#ifdef _DEBUG
		m_pBrush = NULL;
		pTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pBrush);
		m_bPause = false;
		m_bPauseKey = true;
#endif // _DEBUG

		SAFE_RELEASE(pTarget);
	}

}


CStage::~CStage()
{
	//	�Sbullet�̍폜
	if (m_pBullets) {
		std::list<IGameObject*>::iterator it = m_pBullets->begin();
		while (it != m_pBullets->end()) {
			delete (*it);
			it = m_pBullets->erase(it);
		}
		SAFE_DELETE(m_pBullets);
	}
	CBullet::Finalize();

	//	�Sshot�̍폜
	if (m_pShots) {
		std::list<IGameObject*>::iterator it = m_pShots->begin();
		while (it != m_pShots->end()) {
			delete (*it);
			it = m_pShots->erase(it);
		}
		SAFE_DELETE(m_pShots);
	}
	CShot::Finalize();

	//	�S�G�L�����̍폜
	if (m_pEnemies) {
		std::list<IGameObject*>::iterator it = m_pEnemies->begin();
		while (it != m_pEnemies->end()) {
			SAFE_DELETE(*it);
			it = m_pEnemies->erase(it);
		}
		SAFE_DELETE(m_pEnemies);
	}
	m_pEnemyManager->Finalize();
	SAFE_DELETE(m_pEnemyManager);    //!< �G�Z�b�g�V�X�e���̍폜

	//	�S�G�l�~�[�h�b�g�̍폜
	if (m_pEnemyDots) {
		std::list<IGameObject*>::iterator it = m_pEnemyDots->begin();
		while (it != m_pEnemyDots->end()) {
			delete (*it);
			it = m_pEnemyDots->erase(it);
		}
		SAFE_DELETE(m_pEnemyDots);
	}
	CEnemyDot::Finalize();



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


	SAFE_DELETE(m_pUI);
	SAFE_DELETE(m_pBG);
	SAFE_DELETE(m_pPlayer);
	SAFE_RELEASE(m_pBlackBrush);

#ifdef _DEBUG
	SAFE_RELEASE(m_pBrush);
#endif // _DEBUG

}

/**
* @brief �A�j���[�V����(1�t���[��)�̎��s
* @return GAMESCENE_DEFAULT: �p�� / GAMESCENE_END_FAILURE: �Q�[���I�[�o�[
*/
GameSceneResultCode CStage::move() {
	m_iTimer++;

	switch (m_ePhase) {
	case STAGE_INIT:
		m_ePhase = STAGE_RUN;

	case STAGE_RUN:

#ifdef _DEBUG
		if (GetAsyncKeyState(VK_UP)) {
			if (!m_bPauseKey) {
				m_bPause = (m_bPause == true) ? false : true;
				m_bPauseKey = true;
			}
		}
		else {
			m_bPauseKey = false;
		}
#endif

		if (m_bPause)
			return GAMESCENE_DEFAULT;

		if (m_pBG)
			m_pBG->move();

		if (m_pPlayer) {
			if (m_pPlayer->move()) {
				m_pPlayer->CalcDrawCoord(&playerCoords);
			}
			else {
				m_ePhase = STAGE_DONE;
				m_pSystem->SetScoreAndTime(m_iTimer, m_iScore);
				m_iTimer = 0;
				break;
			}
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

		//  EnemyDot�̏���
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

		//	Shot�̏���
		if (m_pShots) {
			std::list<IGameObject*>::iterator it = m_pShots->begin();
			while (it != m_pShots->end()) {
				if ((*it)->move()) {
					++it;
				}
				else {
					SAFE_DELETE(*it);
					it = m_pShots->erase(it);
				}
			}
		}

		//	Bullet�̏���
		if (m_pBullets) {
			std::list<IGameObject*>::iterator it = m_pBullets->begin();
			while (it != m_pBullets->end()) {
				if ((*it)->move()) {
					++it;
				}
				else {
					SAFE_DELETE(*it);
					it = m_pBullets->erase(it);
				}
			}
		}

		//********************************
		//		�����蔻��	
		//********************************

		//	Boss �Ɓ@�v���C���[
		if (m_pBoss && m_pPlayer) {
			if (m_pBoss->collideWithPlayer(m_pPlayer)) {
				float x = m_pBoss->GetX();
				float y = m_pBoss->GetY();
				float r = m_pBoss->GetRad();
				m_pPlayer->setPos(x, y, r);
				m_pPlayer->CalcDrawCoord(&playerCoords);
			}
		}


		//	��EnemyDot(�擪�ɂ��Ă��� + Boss��Life2�h�b�g)�ƃv���C���[&�v���C���[�h�b�g
		if (m_pPlayerDots && m_pEnemyDots) {
			std::list<IGameObject*>::iterator eIt = m_pEnemyDots->begin();
			std::list<IGameObject*>::iterator pIt;
			while (eIt != m_pEnemyDots->end()) {
				if (0 == (*eIt)->GetID()) {
					if ((*eIt)->collide(m_pPlayer)) {
						m_pPlayer->damage(1.0f);
					}
					pIt = m_pPlayerDots->begin();
					while (pIt != m_pPlayerDots->end()) {
						if ((*eIt)->collide(*pIt)) {
							if (playerCoords.playerIsWHmode == false) {
								(*eIt)->damage(1.f);
								(*eIt)->SetFlag(SET_FLG_DOT_DAMAGED);
								(*pIt)->damage(1.f);
							}
							else {
								(*pIt)->damage(1.f);
								(*eIt)->SetFlag(SET_FLG_ATTACKED);
							}
						}
						++pIt;
					}
				}
				++eIt;
			}	//	--while�I���--
		}



		//	PlayerDot��EnemyDot
		if (m_pPlayerDots && m_pEnemyDots) {
			std::list<IGameObject*>::iterator pIt = m_pPlayerDots->begin();
			std::list<IGameObject*>::iterator eIt;
			int i = 0;
			while ((i < playerCoords.playerMaxDotNum) && (pIt != m_pPlayerDots->end())) {
				eIt = m_pEnemyDots->begin();
				while (eIt != m_pEnemyDots->end()) {
					if ((*eIt)->collide((*pIt))) {
						if (playerCoords.playerIsWHmode == false) {
							(*eIt)->damage(1.0f);
							(*eIt)->SetFlag(SET_FLG_DOT_DAMAGED);
						}
						else {
							(*pIt)->damage(1.0f);
							(*eIt)->SetFlag(SET_FLG_ATTACKED);
						}
					}
					++eIt;
				}
				++pIt;
				++i;
			}
		}


		//	PlayerDot�ƃG�l�~�[
		if (m_pPlayerDots && m_pEnemies && playerCoords.playerIsWHmode == false) {
			std::list<IGameObject*>::iterator it = m_pPlayerDots->begin();
			std::list<IGameObject*>::iterator it2;
			int i = 0;
			while ((i < playerCoords.playerMaxDotNum) && (it != m_pPlayerDots->end())) {
					it2 = m_pEnemies->begin();
					while (it2 != m_pEnemies->end()) {
						if ( (*it2)->collide((*it)) ) {
							(*it2)->damage(1.0f);
							if (0 == (*it2)->GetState()) {
								m_pPlayer->ReviveDot();
								m_iScore += (*it2)->GetScore();
							}
						}
						++it2;
					}
				++it;
				++i;
			}
		}

		

		//	Shot �Ɓ@�G�l�~�[�h�b�g/�G�l�~�[�R�A
		if (m_pShots && m_pEnemyDots &&m_pEnemies) {
			std::list<IGameObject*>::iterator sIt = m_pShots->begin();
			std::list<IGameObject*>::iterator eIt;

			//	shot and dot
			while (sIt != m_pShots->end()) {
				eIt = m_pEnemyDots->begin();
				bool IsHit = false;
				while (eIt != m_pEnemyDots->end() && IsHit == false) {
					if ((*sIt)->collide(*eIt)) {
						(*eIt)->damage(1.f);
						(*eIt)->SetFlag(SET_FLG_DOT_DAMAGED);
						(*sIt)->damage(1.f);
						IsHit = true;
					}
					eIt++;
				}
				sIt++;
			}

			//	shot and core
			sIt = m_pShots->begin();
			while (sIt != m_pShots->end()) {
				eIt = m_pEnemies->begin();
				while (eIt != m_pEnemies->end()) {
					if ((*eIt)->collide(*sIt)) {
						(*sIt)->damage(1.f);
					}
					eIt++;
				}
				sIt++;
			}
		}

		//	shot �� bullet
		if (m_pShots && m_pBullets) {
			std::list<IGameObject*>::iterator sIt = m_pShots->begin();
			std::list<IGameObject*>::iterator bIt;
			while (sIt != m_pShots->end()) {
				bIt = m_pBullets->begin();
				while (bIt != m_pBullets->end()) {
					if ((*bIt)->collide(*sIt)) {
						(*bIt)->damage(1.f);
						(*sIt)->damage(1.f);
						bIt = m_pBullets->end();
					}
					else {
						bIt++;
					}
				}
				sIt++;
			}
		}

		//	bullet �� player/player dot
		if (m_pPlayer && m_pPlayerDots && m_pBullets) {
			//	bullet �� �h�b�g
			std::list<IGameObject*>::iterator dIt = m_pPlayerDots->begin();
			std::list<IGameObject*>::iterator bIt;
			while (dIt != m_pPlayerDots->end()) {
				bIt = m_pBullets->begin();
				while (bIt != m_pBullets->end()) {
					if ((*bIt)->collide(*dIt)) {
						(*bIt)->damage(1.f);
						if (playerCoords.playerIsWHmode == true) {
							(*dIt)->damage(1.f);
						}
						bIt = m_pBullets->end();
					}
					else {
						bIt++;
					}
				}
				dIt++;
			}

			//	player �� bullet
			bIt = m_pBullets->begin();
			while (bIt != m_pBullets->end()) {
				if ((*bIt)->collide(m_pPlayer)) {
					(*bIt)->damage(1.f);
				}
				bIt++;
			}

		}


		if (m_pUI)
			m_pUI->move();

		break;

	case STAGE_DONE:
		if (m_iTimer < 120)
			break;

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

	if (m_pEnemyDots) {
		std::list<IGameObject*>::iterator it = m_pEnemyDots->begin();
		while (it != m_pEnemyDots->end()) {
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

	if (m_pShots) {
		std::list<IGameObject*>::iterator it = m_pShots->begin();
		while (it != m_pShots->end()) {
			(*it++)->draw(pRenderTarget);
		}
	}

	if (m_pBullets) {
		std::list<IGameObject*>::iterator it = m_pBullets->begin();
		while (it != m_pBullets->end()) {
			(*it++)->draw(pRenderTarget);
		}
	}

	if (m_pUI)
		m_pUI->draw(pRenderTarget);

	if (m_ePhase == STAGE_DONE) {
		D2D1_SIZE_F screen = pRenderTarget->GetSize();
		D2D1_RECT_F rc;
		rc.left = 0.f;
		rc.top = 0.f;
		rc.right = screen.width;
		rc.bottom = screen.height;
		float opacity = m_iTimer / 120.f;
		m_pBlackBrush->SetOpacity(opacity);
		pRenderTarget->FillRectangle(rc, m_pBlackBrush);
	}

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
*    �������ꂽPlayerDot�������N���X�g�ɓo�^����
*@param in pObj  �V����PlayerDot�̃I�u�W�F�N�g
************************************************/
void CStage::AddPlayerDot(IGameObject *pObj) {
	if (m_pPlayerDots) {
		m_pPlayerDots->push_back(pObj);
	}
}

/***********************************************
*@method
*    �������ꂽEnemyDot�������N���X�g�ɓo�^����
*@param in pObj  �V����EnemyDot�̃I�u�W�F�N�g
************************************************/
void CStage::AddEnemyDot(IGameObject *pObj) {
	if (m_pEnemyDots) {
		m_pEnemyDots->push_back(pObj);
	}
}

/***********************************************
*@method
*    �������ꂽShot�������N���X�g�ɓo�^����
*@param in pObj  �V����Shot�̃I�u�W�F�N�g
************************************************/
void CStage::AddShot(IGameObject *pObj) {
	if (m_pShots) {
		m_pShots->push_back(pObj);
	}
}

/***********************************************
*@method
*    �������ꂽBullet�������N���X�g�ɓo�^����
*@param in pObj  �V����Bullet�̃I�u�W�F�N�g
************************************************/
void CStage::AddBullet(IGameObject *pObj) {
	if (m_pBullets) {
		m_pBullets->push_back(pObj);
	}
}

int CStage::GetScore() {
	return m_iScore;
}
int CStage::GetTimer() {
	return m_iTimer;
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