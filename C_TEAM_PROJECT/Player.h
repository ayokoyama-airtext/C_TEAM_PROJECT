/**
*@file		Player.h
*@author	A.Yokoyama
*@date		07.August.2019
*@brief		�v���C���[�N���X�̐錾
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
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj);
	virtual void damage(float amount) override;
protected:
	CStage	*m_pParent;
	ID2D1Bitmap	*m_pImage;
	INT	m_iTimer;

	//	�萔
	static const int PLAYER_START_X = 960;
	static const int PLAYER_START_Y = 540;
	static const int PLAYER_RAD = 16;

#ifdef _DEBUG
	ID2D1SolidColorBrush	*m_pBrush;
#endif
};
