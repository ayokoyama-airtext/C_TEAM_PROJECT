/**
*@file		EnemyManager.cpp
*@author	A.Yokoyama
*@brief		エネミーマネージャークラスの実装
*/
#include "stdafx.h"
#include <time.h>
#include "Stage.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Enemy02.h"
#include "Enemy03.h"
#include "Enemy04.h"
#include "Enemy05.h"
#include "Enemy06.h"



CEnemyManager::CEnemyManager(CStage *pStage)
{
	m_iEnemyCount = 0;
	m_iWave = 0;
	m_iIndex = 0;
	m_iRespawnNum = 0;
	m_pParent = pStage;
	srand(time(NULL));
}


CEnemyManager::~CEnemyManager()
{
}


/**
*@brief	エネミーを生成する。リスポーン地点もここで決定する
*@return *pObj	生成したエネミーオブジェクト
*/
IGameObject *CEnemyManager::CreateEnemy() {
	IGameObject *pObj = NULL;



#if defined(yakihiro) || defined(ayokoyama)
	
	if (m_iEnemyCount > 0)
		return pObj;

	if (m_iIndex == m_szEnemySetDataSize) {
		m_iIndex = 0;
		m_iWave = 0;
		return pObj;
	}

	int waveNum = m_pEnemySetData[m_iIndex++] &65535;
	int enemyID = m_pEnemySetData[m_iIndex++] &65535;

	if (waveNum == m_iWave) {
		float playerX = m_pParent->playerCoords.playerX;
		float playerY = m_pParent->playerCoords.playerY;

		if (enemyID == 999) {
			enemyID = ((rand() >> 4) % 6) + 1;
		}

		float RespawnX = playerX + (-1080 + 1080 * (m_iRespawnNum % 3));
		if (RespawnX >= m_pParent->FIELD_WIDTH) {
			RespawnX -= m_pParent->FIELD_WIDTH;
		}
		else if (RespawnX <= 0) {
			RespawnX += m_pParent->FIELD_WIDTH;
		}

		float RespawnY = playerY + (-960 + 960 * (m_iRespawnNum & 1) * 2);
		if (RespawnY >= m_pParent->FIELD_HEIGHT) {
			RespawnY -= m_pParent->FIELD_HEIGHT;
		}
		else if (RespawnY <= 0) {
			RespawnY += m_pParent->FIELD_HEIGHT;
		}

		switch (enemyID) {
		case 1:
			pObj = new CEnemy(RespawnX, RespawnY, 1.f);
			break;
		case 2:
			pObj = new CEnemy02(RespawnX, RespawnY, 1.f);
			break;
		case 3:
			pObj = new CEnemy03(RespawnX, RespawnY, 1.f);
			break;
		case 4:
			pObj = new CEnemy04(RespawnX, RespawnY, 1.f);
			break;
		case 5:
			pObj = new CEnemy05(RespawnX, RespawnY, 1.f);
			break;
		case 6:
			pObj = new CEnemy06(RespawnX, RespawnY, 1.f);
			break;
		}
		m_iRespawnNum++;
		if (m_iIndex == m_szEnemySetDataSize) {
			m_iEnemyCount = m_iRespawnNum;
			m_iRespawnNum = 0;
			m_iIndex = m_szEnemySetDataSize;
		}
	}
	else {
		m_iIndex -= PITCH;
		m_iEnemyCount = m_iRespawnNum;
		m_iRespawnNum = 0;
	}

#elif defined(kmiyamoto)
	if (m_iEnemyCount >= 1)
		return pObj;
	pObj = new CEnemy06(1400.f, 1080.f, 1.f);
	m_iEnemyCount++;
#elif defined(tnakahara)
	if (m_iEnemyCount >= 1)
		return pObj;
	pObj = new CEnemy05(1400.f, 1080.f, 1.f);
	m_iEnemyCount++;
#elif defined(stadayosi)
	if (m_iEnemyCount >= 1)
		return pObj;
	pObj = new CEnemy04(1400.f, 1080.f, 1.f);
	m_iEnemyCount++;
#elif defined(hikegami)
	if (m_iEnemyCount >= 1)
		return pObj;
	pObj = new CEnemy02(1400.f, 1080.f, 1.f);
	m_iEnemyCount++;
#else
	if (m_iEnemyCount >= 1)
		return pObj;
	pObj = new CEnemy03(1400.f, 1080.f, 1.f);
	m_iEnemyCount++;
#endif
	

	return pObj;
}


/**
*@brief	管理しているすべてのエネミーのResotreを行う
*/
void CEnemyManager::Restore(ID2D1RenderTarget *pTarget) {
	CEnemy::Restore(pTarget, m_pParent);
	CEnemy02::Restore(pTarget, m_pParent);
	CEnemy03::Restore(pTarget, m_pParent);
	CEnemy04::Restore(pTarget, m_pParent);
	CEnemy05::Restore(pTarget, m_pParent);
	CEnemy06::Restore(pTarget, m_pParent);
	
}

/**
*@brief	管理しているすべてのエネミーのFinalizeを行う
*/
void CEnemyManager::Finalize() {
	CEnemy06::Finalize();
	CEnemy05::Finalize();
	CEnemy04::Finalize();
	CEnemy03::Finalize();
	CEnemy02::Finalize();
	CEnemy::Finalize();
}


void CEnemyManager::DecreaseEnemyCount() {
	if (--m_iEnemyCount == 0) {
		m_iWave++;
	}
}


//	PITCH : 2 -> Wave番号, エネミー種類(999はランダムリスポーン)
SHORT CEnemyManager::m_pEnemySetData[] = {
	0, 1,
	0, 1,
	0, 1,
	1, 1,
	1, 1,
	1, 1,
	2, 1,
	2, 1,
	2, 999,
};

size_t CEnemyManager::m_szEnemySetDataSize = sizeof(CEnemyManager::m_pEnemySetData) / sizeof(SHORT);