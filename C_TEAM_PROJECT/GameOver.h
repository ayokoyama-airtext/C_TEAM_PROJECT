/**
* @file GameOver.h
* @brief ÉNÉâÉX CGameOVer ÇÃêÈåæ
*/
#pragma once

#ifndef __IGAMESCENE_H__
#include "IGameScene.h"
#endif // !__IGAMEOBJECT_H__

enum GameOverPhase {
	GAMEOVER_INIT = 0,
	GAMEOVER_RUN = 1,
	GAMEOVER_FADE = 2,
	GAMEOVER_DONE = 3
};

class CSelector;
struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;

class CGameOver : public IGameScene
{
public:
	CGameOver(CSelector *pSystem);
	virtual ~CGameOver();
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget *pRendetTarget);
protected:
	CSelector		*m_pSystem;
	GameOverPhase	m_ePhase;
	ID2D1Bitmap		*m_pImage;
	ID2D1SolidColorBrush	*m_pBlack;
	INT m_iTimer;
	INT m_iFadeTimer;
	bool m_bFlag;
};

