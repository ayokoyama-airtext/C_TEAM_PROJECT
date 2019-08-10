/**
*@file		EnemyManager.h
*@author	A.Yokoyama
*@date		10.August.2019
*@brief		�G�l�~�[�}�l�[�W���[�N���X�̐錾
*@note		�G�l�~�[�͑S�Ă��̃N���X��include����
*/
#pragma once
#ifndef __IGAMEOBJECT_H__
#include "IGameObject.h"
#endif // !__IGAMEOBJECT_H__

class CStage;

class CEnemyManager
{
public:
	CEnemyManager(CStage *pStage);
	virtual ~CEnemyManager();
	virtual IGameObject *CreateEnemy();
	virtual void Restore(ID2D1RenderTarget *pTarget);
	virtual void Finalize();
protected:
	CStage	*m_pParent;
	int		m_iEnemyCount;	//	���݂̃G�l�~�[�̐�
};

