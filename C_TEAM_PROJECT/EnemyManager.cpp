/**
*@file		EnemyManager.cpp
*@author	A.Yokoyama
*@brief		エネミーマネージャークラスの実装
*/
#include "stdafx.h"
#include "Stage.h"
#include "EnemyManager.h"
#include "Enemy.h"


CEnemyManager::CEnemyManager(CStage *pStage)
{
	m_iEnemyCount = 0;
	m_pParent = pStage;
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

	if (m_iEnemyCount >= 1)
		return pObj;


	pObj = new CEnemy(1400.f, 1080.f, 1.f);
	
	m_iEnemyCount++;

	return pObj;
}


/**
*@brief	管理しているすべてのエネミーのResotreを行う
*/
void CEnemyManager::Restore(ID2D1RenderTarget *pTarget) {
	CEnemy::Restore(pTarget, m_pParent);
}

/**
*@brief	管理しているすべてのエネミーのFinalizeを行う
*/
void CEnemyManager::Finalize() {
	CEnemy::Finalize();
}