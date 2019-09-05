/**
*@file		EnemyBossDot.h
*@author	A.Yokoyama
*@brief		エネミーBossDotクラスの宣言
*/
#pragma once
#include "EnemyDot.h"

class CEnemyBossDot :
	public CEnemyDot
{
public:
	CEnemyBossDot(IGameObject *pParentEnemy, int dotNum, int maxDotNum) : CEnemyDot(pParentEnemy, dotNum, maxDotNum) { m_fRad = DEFAULT_RAD; m_iLife = 2; m_iDamageTimer = 0; m_iID = 0; m_iRespawnAnimTimer = 36;  };
	virtual ~CEnemyBossDot();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual void damage(float amount) override;
protected:
	int	m_iLife;
	int m_iDamageTimer;
	INT		m_iRespawnAnimTimer;
	static const int	DEFAULT_RAD = 36;		//	デフォルト半径
	static const int	DAMAGE_DURATION = 60;	//	ダメージ時間
};

