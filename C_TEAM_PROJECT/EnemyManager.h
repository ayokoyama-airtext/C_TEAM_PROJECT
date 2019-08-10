/**
*@file		EnemyManager.h
*@author	A.Yokoyama
*@date		10.August.2019
*@brief		エネミーマネージャークラスの宣言
*@note		エネミーは全てこのクラスにincludeする
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
	int		m_iEnemyCount;	//	現在のエネミーの数
};

