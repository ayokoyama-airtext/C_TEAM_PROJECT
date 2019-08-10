/**
*@file		Enemy.h
*@author	A.Yokoyama
*@date		10.August.2019
*@brief		エネミークラスの宣言
*/
#pragma once
#include "IGameObject.h"

struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;
class CStage;

class CEnemy :
	public IGameObject
{
public:
	CEnemy(float x, float y, float scale);
	virtual ~CEnemy();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	static void Restore(ID2D1RenderTarget *pTarget, CStage *pStage);
	static void Finalize();
protected:

	FLOAT	m_fX;			//	x座標(データ上の)
	FLOAT	m_fY;			//	y座標(データ上の)
	FLOAT	m_fVX;			//	x方向加速度
	FLOAT	m_fVY;			//	y方向加速度
	FLOAT	m_fRad;			//	ベルトの半径
	FLOAT	m_fAngle;		//	回転角度 (ワールド座標基準, ラジアン単位)->0rad:右, 0.5PIrad:下, PIrad:左, 1.75PIrad:上
	FLOAT	m_fScale;		//	大きさ(基本1.0f)
	INT		m_iDotNum;		//	生きてるドットの数
	INT		m_iMaxDotNum;	//	ドットの最大個数(成長するとこれが増える)(最大8?)

	bool	m_bDamaged;



	//	定数
	static const int BELT_RAD = 96;		//	ベルト半径
	static const int DOT_RAD = 24;			//	ドット半径
	static const int CORE_LENGTH = 96;		//	コアの一辺の長さ
	static const float ROTATION_SPEED;		//	回転速度
	static const float ENEMY_SPEED;		//	移動速度


	//	静的メンバ(初期化を忘れずに)
	static CStage			*m_pParent;
	static ID2D1Bitmap		*m_pImage;
	static FLOAT m_fFieldWidth;
	static FLOAT m_fFieldHeight;
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrush;
	static ID2D1SolidColorBrush	*m_pRedBrush;
#endif // _DEBUG
};

