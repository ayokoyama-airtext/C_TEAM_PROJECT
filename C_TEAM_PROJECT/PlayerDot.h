/**
*@file		PlayerDot.h
*@author	A.Yokoyama
*@date		09.August.2019
*@brief		プレイヤードットクラスの宣言
*/
#pragma once
#include "IGameObject.h"

struct ID2D1Bitmap;
class CStage;
class CPlayer;

class CPlayerDot :
	public IGameObject
{
public:
	CPlayerDot();
	virtual ~CPlayerDot();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	virtual int GetState() override;
	virtual float GetX() override;
	virtual float GetY() override;

	static void Restore(ID2D1RenderTarget *pTarget, CStage *pStage, CPlayer *pPlayer);
	static void Finalize();
protected:
	FLOAT	m_fX;		//	x座標
	FLOAT	m_fY;		//	y座標
	FLOAT	m_fRad;		//	半径
	INT		m_iState;	//	ドットの状態	0:壊れてる / 1:生きてる
	INT		m_iNumber;	//	ドットの番号
	INT		m_iTimer;

	//	定数
	static const int	DEFAULT_RAD = 24;	//	デフォルト半径

	//	静的メンバ(初期化を忘れずに)
	static ID2D1Bitmap	*m_pImage;
	static CStage	*m_pParent;
	static CPlayer	*m_pPlayer;
	static int	m_iCount;	//	ドットのインスタンスの数
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrushBlack;
	static ID2D1SolidColorBrush *m_pBrushWhite;
#endif
};

