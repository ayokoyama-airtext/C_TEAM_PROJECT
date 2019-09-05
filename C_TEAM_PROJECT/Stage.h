/**
* @file Stage.h
* @brief クラスCStageの宣言ファイル
* @author A.Yokoyama
*/
#pragma once
#include "Selector.h"

#ifndef __IGAMEOBJECT_H__
#include "IGameObject.h"
#endif

//  ややトリッキーな前方宣言
namespace std {
	template<class _Ty> class allocator;
	template <class _Ty, class _Alloc = allocator<_Ty>>class list;
}

enum STAGE_PHASE {
	STAGE_INIT = 0x01,
	STAGE_RUN = 0x02,
	STAGE_DONE = 0x03,
};

enum DRAW_SITUATION {
	DRAW_STATIC_MIN			= 0x01,
	DRAW_CENTERING_PLAYER	= 0x02,
	DRAW_STATIC_MAX			= 0x04,
};

typedef struct {
	FLOAT	playerX;
	FLOAT	playerY;
	FLOAT	playerDrawX;
	FLOAT	playerDrawY;
	FLOAT	playerAngle;
	FLOAT	playerRad;
	INT		playerMaxDotNum;
	DRAW_SITUATION	XaxisDraw;
	DRAW_SITUATION	YaxisDraw;
	bool	playerIsWHmode;
}	PLAYER_COORDS;


class CPlayer;
class CBG;
class CEnemyManager;
class CEnemyBoss;
class CUI;
struct ID2D1SolidColorBrush;


class CStage : public IGameScene
{
public:
	CStage(CSelector *pSystem);
	virtual ~CStage(void);
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;

	virtual void AddPlayerDot(IGameObject *pObj);
	virtual void AddEnemyDot(IGameObject *pObj);
	virtual void AddShot(IGameObject *pObj);
	virtual void AddBullet(IGameObject *pObj);

	virtual int GetScore();
	virtual int GetTimer();

	virtual void SetBoss(CEnemyBoss *pBoss) { m_pBoss = pBoss; }
	virtual void RemoveBoss() { m_pBoss = NULL; }

	ID2D1RenderTarget *GetRenderTarget();

	//	公開メンバ変数
	PLAYER_COORDS playerCoords;

	//	フィールド幅高定数
	static const FLOAT FIELD_WIDTH;
	static const FLOAT FIELD_HEIGHT;
protected:
	ID2D1SolidColorBrush	*m_pBlackBrush;
	CSelector		*m_pSystem;
	CPlayer			*m_pPlayer;
	CEnemyBoss		*m_pBoss;
	CBG				*m_pBG;
	CEnemyManager	*m_pEnemyManager;
	CUI				*m_pUI;
	STAGE_PHASE		m_ePhase;

	std::list<IGameObject*>		*m_pPlayerDots;
	std::list<IGameObject*>		*m_pEnemies;
	std::list<IGameObject*>		*m_pEnemyDots;
	std::list<IGameObject*>		*m_pShots;
	std::list<IGameObject*>		*m_pBullets;

	UINT		m_iScore;
	UINT		m_iTimer;	//	時間制限用
	UINT		m_iGameFinishState;
	//	m_iGameFinishState 用定数
	static const int GAME_CLEAR = 0x02;
	static const int GAME_OVER = 0x04;


#ifdef _DEBUG
	ID2D1SolidColorBrush	*m_pBrush;
#endif
};


//	定数
#define DISPLAY_RESOLUTION_WIDTH	1920
#define DISPLAY_RESOLUTION_HEIGHT	1080

