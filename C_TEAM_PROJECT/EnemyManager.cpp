/**
*@file		EnemyManager.cpp
*@author	A.Yokoyama
*@brief		�G�l�~�[�}�l�[�W���[�N���X�̎���
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
*@brief	�G�l�~�[�𐶐�����B���X�|�[���n�_�������Ō��肷��
*@return *pObj	���������G�l�~�[�I�u�W�F�N�g
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
*@brief	�Ǘ����Ă��邷�ׂẴG�l�~�[��Resotre���s��
*/
void CEnemyManager::Restore(ID2D1RenderTarget *pTarget) {
	CEnemy::Restore(pTarget, m_pParent);
}

/**
*@brief	�Ǘ����Ă��邷�ׂẴG�l�~�[��Finalize���s��
*/
void CEnemyManager::Finalize() {
	CEnemy::Finalize();
}