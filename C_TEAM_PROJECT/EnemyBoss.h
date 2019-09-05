/**
*@file		EnemyBoss.h
*@author	A.Yokoyama
*@brief		エネミーBossクラスの宣言
*/
#pragma once
#include "IGameObject.h"
#include <vector>

struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;
class CStage;
class CEnemyDot;
class CPlayer;


class CEnemyBoss :
	public IGameObject
{
public:
	CEnemyBoss(float x, float y, float scale);
	virtual ~CEnemyBoss();
	virtual bool move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	virtual bool collide(float x, float y, float r) override;
	virtual bool collide(IGameObject *pObj) override;
	virtual bool collideWithPlayer(CPlayer *pPlayer);
	virtual void damage(float amount) override;

	virtual float GetX() override;
	virtual float GetY() override;
	virtual float GetRad() override;
	virtual float GetAngle() override;
	virtual int GetState() override { return m_bDamaged ? 0 : 1; }
	virtual int GetScore() override { return 50000; }

	virtual void SetFlag(int flag) override;

	static void Restore(ID2D1RenderTarget *pTarget, CStage *pStage);
	static void Finalize();
protected:

	FLOAT	m_fX;			//	x座標(データ上の)
	FLOAT	m_fY;			//	y座標(データ上の)
	FLOAT	m_fVX;			//	x方向加速度
	FLOAT	m_fVY;			//	y方向加速度
	FLOAT	m_fRad;			//	ベルトの半径
	FLOAT	m_fCoreRad;		//	コア半径
	FLOAT	m_fAngle;		//	回転角度 (ワールド座標基準, ラジアン単位)->0rad:右, 0.5PIrad:下, PIrad:左, 1.75PIrad:上
	FLOAT	m_fScale;		//	大きさ(基本1.0f)
	INT		m_iDotNum;		//	生きてるドットの数
	INT		m_iMaxDotNum;	//	ドットの最大個数(成長するとこれが増える)(最大8?)
	INT		m_iRandomMoveIndex;

	std::vector<CEnemyDot*>	m_pDots;	//	保持しているドットへのポインタの配列

	bool	m_bDamaged;
	INT		m_iTimer;
	INT		m_iDamagedTimer;	//	無敵時間の管理
	INT		m_iAnimationTimer;
	INT		m_iDestroyAnimTimer;
	INT		m_iRespawnAnimTimer;
	INT		m_iAttackTimer;		//	攻撃用のタイマー

	int		m_iBehaviorFlag;	//	エネミーの行動状態を管理するフラグ
	//	m_iBehaviorFlag用定数
	static const int EFLAG_IDLE = 0x01;		//	停止
	static const int EFLAG_CHASE = 0x02;	//	追跡
	static const int EFLAG_MOVE = 0x04;	//	索敵
	static const int EFLAG_ATTACK = 0x08;	//	攻撃
	static const int EFLAG_ATTACKED = 0x10;	//	攻撃後の待ち

	//	定数
	static const int BELT_RAD = 720;				//	ベルト半径
	static const int DOT_RAD = 24;				//	ドット半径
	static const int CORE_LENGTH = 960;			//	コアの一辺の長さ

	static const float ROTATION_SPEED;			//	回転速度
	static const float ENEMY_SPEED;				//	移動速度
	static const float ENEMY_ESCAPE_SPEED;		//	逃走速度
	static const float ENEMY_ESCAPE_ROTATION_SPEED;

	static const float BULLET_ANGLE_GAP_BIG;
	static const float BULLET_ANGLE_GAP_SMALL;

	static const float ENEMY_ESCAPE_ANGLE;
	static const float SEARCH_ANGLE;			//	索敵角度

	static const int ATTACK_LENGTH = 1920;		//	攻撃距離
	static const int APPROACH_LENGTH = 1200;
	static const int MAX_CHASE_LENGTH = 800;		//	追跡時間

	static const int ESCAPE_DURATION = 90;		//	逃走時間
	static const int DAMAGED_DURATION = 30;		//	無敵時間
	static const int ATTACKED_DURATION = 180;	//	攻撃後の待ち時間
	static const int MOVE_DURATION = 180;

	static const int ATTACK_INTERVAL = 30;		//	攻撃間隔


	//	静的メンバ(初期化を忘れずに)
	static CStage			*m_pParent;
	static ID2D1Bitmap		*m_pCoreImage;
	static ID2D1Bitmap		*m_pBeltImage;
	static ID2D1Bitmap		*m_pDestroyImage;
	static FLOAT m_fFieldWidth;
	static FLOAT m_fFieldHeight;
	static FLOAT m_pRandomMove[];
	static FLOAT m_pTexCoord[];
	static const INT m_iRandomMoveSize;
#ifdef _DEBUG
	static ID2D1SolidColorBrush	*m_pBrush;
	static ID2D1SolidColorBrush	*m_pRedBrush;
#endif // _DEBUG

};