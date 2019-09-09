/**
*@file		Bullet.h
*@author	A.Yokoyama
*@brief		Bulletクラスの宣言
*/
#pragma once
#include "IGameObject.h"

class CStage;
struct ID2D1SolidColorBrush;
struct ID2D1Bitmap;

class CBullet :
	public IGameObject
{
public:
	CBullet(float x, float y, float angle);
	virtual ~CBullet();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;

	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	static void Restore(CStage *pStage,  ID2D1RenderTarget *pRenderTarget);
	static void Finalize();
protected:
	float	m_fX;
	float	m_fY;
	float	m_fVX;
	float	m_fVY;
	float	m_fRad;
	float	m_fAngle;
	int		m_iLifeTimer;
	int		m_iDestroyAnimTimer;

	bool	m_bDamaged;

	//	定数
	static const int SHOT_SPEED = 10;
	static const int DEFAULT_RAD = 24;	//	デフォルト半径
	static const int LIFE_TIME = 180;

	//	staticメンバ(初期化を忘れずに)
	static CStage		*m_pParent;
	static ID2D1Bitmap	*m_pImage;
	static ID2D1Bitmap	*m_pDestroyImage;
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrush;
#endif
};


#define BULLET_SPEED 10.f
