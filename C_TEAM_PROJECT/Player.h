/**
*@file		Player.h
*@author	A.Yokoyama
*@date		07.August.2019
*@brief		プレイヤークラスの宣言
*@note		Stage.h の後に#include すること
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
	
	INT		m_iTimer;
	FLOAT	m_fDrawX;		//	x座標(描画上の)
	FLOAT	m_fDrawY;		//	y座標(描画上の)
	FLOAT	m_fX;			//	x座標(データ上の)
	FLOAT	m_fY;			//	y座標(データ上の)
	FLOAT	m_fVX;			//	x方向加速度
	FLOAT	m_fVY;			//	y方向加速度
	FLOAT	m_fRad;			//	ベルトの半径
	FLOAT	m_fCoreRad;	//	コアの半径
	FLOAT	m_fAngle;		//	回転角度 (ワールド座標基準, ラジアン単位)->0rad:右, 0.5PIrad:下, PIrad:左, 1.75PIrad:上
	FLOAT	m_fScale;		//	大きさ(基本1.0f)
	INT		m_iDotNum;		//	生きてるドットの数
	INT		m_iMaxDotNum;	//	ドットの最大個数(成長するとこれが増える)(最大8?)

	bool	m_bDamaged;

	//	フィールド幅高
	FLOAT m_fFieldWidth;
	FLOAT m_fFieldHeight;
	

	//	定数
	static const int PLAYER_START_X = 960;
	static const int PLAYER_START_Y = 540;
	static const int BELT_RAD = 96;		//	ベルト半径
	static const int DOT_RAD = 24;			//	ドット半径
	static const int CORE_LENGTH = 96;		//	コアの一辺の長さ
	static const float ROTATION_SPEED;		//	回転速度
	static const float PLAYER_SPEED;		//	移動速度

	//	デバッグ用
#ifdef _DEBUG
	ID2D1SolidColorBrush	*m_pBrush;
	ID2D1SolidColorBrush	*m_pRedBrush;

	//	デバッグ用定数
	static const int INC_MAXDOT_KEY = 0x46;	//	Fキー
	bool	INC_MAXDOT_FLAG = true;
	static const int DEC_MAXDOT_KEY = 0x47;	//	Gキー
#endif
};



