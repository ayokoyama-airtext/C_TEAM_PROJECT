/**
* @file Title.h
* @brief ÉNÉâÉX CTitle ÇÃêÈåæ
*/
#pragma once

#ifndef __IGAMESCENE_H__
#include "IGameScene.h"
#endif // !__IGAMEOBJECT_H__

enum TitlePhase {
	TITLE_INIT = 0,
	TITLE_RUN = 1,
	TITLE_FADE = 2,
	TITLE_DONE = 3
};

class CSelector;
struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;

class CTitle : public IGameScene
{
public:
	CTitle(CSelector *pSystem);
	virtual ~CTitle();
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
protected:
	CSelector	*m_pSystem;
	TitlePhase	m_ePhase;
	ID2D1Bitmap	*m_pImage;
	ID2D1SolidColorBrush	*m_pBlack;
	bool	m_bFlag;
	INT		m_iTimer;
	INT		m_iFadeTimer;
};

