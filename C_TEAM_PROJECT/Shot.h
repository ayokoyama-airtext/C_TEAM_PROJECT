/**
*@file		Shot.h
*@author	A.Yokoyama
*@brief		Shotクラスの宣言
*/
#pragma once
#include "IGameObject.h"

class CPlayer;
class CStage;
struct ID2D1SolidColorBrush;
struct ID2D1Bitmap;

class CShot :
	public IGameObject
{
public:
	CShot(float x, float y, float vx, float vy);
	virtual ~CShot();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;

	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	static void Restore(CStage *pStage ,CPlayer *pPlayer, ID2D1RenderTarget *pRenderTarget);
	static void Finalize();
protected:
	float	m_fX;
	float	m_fY;
	float	m_fVX;
	float	m_fVY;
	float	m_fRad;
	int		m_iLifeTimer;
	int		m_iDestroyAnimTimer;

	bool	m_bDamaged;

	//	定数
	static const int SHOT_SPEED = 20;
	static const int DEFAULT_RAD = 24;	//	デフォルト半径
	static const int LIFE_TIME = 60;

	//	staticメンバ(初期化を忘れずに)
	static CPlayer		*m_pPlayer;
	static CStage		*m_pParent;
	static ID2D1Bitmap	*m_pImage;
	static ID2D1Bitmap	*m_pDestroyImage;
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrush;
#endif
};

