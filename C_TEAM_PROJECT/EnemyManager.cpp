/**
*@file		EnemyManager.cpp
*@author	A.Yokoyama
*@brief		�G�l�~�[�}�l�[�W���[�N���X�̎���
*/
#include "stdafx.h"
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

#ifdef hikegami
	pObj = new CEnemy02(1400.f, 1080.f, 1.f);
#elif kmiyamoto
	pObj = new CEnemy06(1400.f, 1080.f, 1.f);
#elif tnakahara
	pObj = new CEnemy05(1400.f, 1080.f, 1.f);
#elif sugita-tadayosi
	pObj = new CEnemy04(1400.f, 1080.f, 1.f);
#elif yakihiro || ayokoyama
	pObj = new CEnemy(1400.f, 1080.f, 1.f);
#else
	pObj = new CEnemy03(1400.f, 1080.f, 1.f);
#endif
	
	m_iEnemyCount++;

	return pObj;
}


/**
*@brief	�Ǘ����Ă��邷�ׂẴG�l�~�[��Resotre���s��
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
*@brief	�Ǘ����Ă��邷�ׂẴG�l�~�[��Finalize���s��
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
	--m_iEnemyCount;
}