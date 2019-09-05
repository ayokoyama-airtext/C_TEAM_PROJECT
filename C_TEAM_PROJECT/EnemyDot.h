/**
*@file		EnemyDot.h
*@author	A.Yokoyama
*@date		11.August.2019
*@brief		エネミードットクラスの宣言
*/
#pragma once
#include "IGameObject.h"

struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;
class CStage;

class CEnemyDot :
	public IGameObject
{
public:
	CEnemyDot(IGameObject *pParentEnemy, int dotNumber, int maxDotNum);
	virtual ~CEnemyDot();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual void damage(float amount) override;

	virtual int GetNumber() override;

	virtual bool IsDead();
	virtual void SetStateZero();
	virtual int GetState() override;
	virtual int GetID() override { return m_iID; }

	virtual void SetFlag(int flag) override { m_pParent->SetFlag(flag); }

	static void Restore(ID2D1RenderTarget *pTarget, CStage *pStage);
	static void Finalize();
protected:
	FLOAT	m_fX;			//	x座標
	FLOAT	m_fY;			//	y座標
	FLOAT	m_fRad;			//	半径
	FLOAT	m_fAngle;		//	親のエネミーに対する角度
	INT		m_iState;		//	ドットの状態	0:壊れてる / 1:生きてる
	INT		m_iNumber;		//	ドットの番号
	INT		m_iMaxDotNum;	//	所属しているエネミーの最大ドット数
	INT		m_iTimer;		//	アニメーションに使用
	INT		m_iDestroyAnimTimer;
	INT		m_iRespawnAnimTimer;
	INT		m_iID;			//	BossDot == 0	/	NormalDot == 1
	bool	m_bDamaged;		//	親エネミーがドットの生死判定に使用

	IGameObject	*m_pParent;	//	このドットを持っているエネミー

	//	定数
	static const int	DEFAULT_RAD = 24;	//	デフォルト半径

	//	静的メンバ(初期化を忘れずに)
	static ID2D1Bitmap	*m_pDotImage;
	static ID2D1Bitmap	*m_pStrawImage;
	static ID2D1Bitmap	*m_pDestroyImage;
	static CStage	*m_pStage;
	static float m_pStrawTexCoord[];	//	Strawテクスチャの座標
	static ID2D1SolidColorBrush	*m_pWhiteBrush;
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrush;
#endif
};


#define SET_FLG_ATTACKED 0x01
#define SET_FLG_DOT_DAMAGED 0x02