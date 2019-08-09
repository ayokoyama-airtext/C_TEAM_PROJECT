/**
*@file		Player.h
*@author	A.Yokoyama
*@date		07.August.2019
*@brief		プレイヤークラスの宣言
*/
#pragma once
#include "IGameObject.h"

class CStage;
struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;

class CPlayer :
	public IGameObject
{
public:
	CPlayer(CStage *pStage);
	virtual ~CPlayer();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	void CalcDrawCoord(PLAYER_COORDS *playerCoords);

	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj);
	virtual void damage(float amount) override;
protected:
	CStage	*m_pParent;
	ID2D1Bitmap	*m_pImage;
	INT	m_iTimer;
	GAMEOBJECT_STATS	m_stStats;
	FLOAT m_fDrawX;
	FLOAT m_fDrawY;

	//	フィールド幅高
	FLOAT m_fFieldWidth;
	FLOAT m_fFieldHeight;
	

	//	定数
	static const int PLAYER_START_X = 960;
	static const int PLAYER_START_Y = 540;
	static const int PLAYER_RAD = 96;
	static const int BELT_RAD = 24;
	static const int CORE_LENGTH = 96;
	static const float ROTATION_SPEED;
	static const float PLAYER_SPEED;

#ifdef _DEBUG
	ID2D1SolidColorBrush	*m_pBrush;
	ID2D1SolidColorBrush	*m_pRedBrush;
#endif
};

