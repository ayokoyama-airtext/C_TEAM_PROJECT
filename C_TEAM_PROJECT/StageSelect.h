#pragma once
#include "IGameScene.h"

struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;
class  CSelector;

enum STAGESELECT_PHASE {
	STAGESELECT_INIT = 0x01,
	STAGESELECT_RUN	 = 0x02,
	STAGESELECT_FADE = 0x03,
	STAGESELECT_DONE = 0x04,
};

class CStageSelect :
	public IGameScene
{
public:
	CStageSelect(CSelector *pSelector);
	virtual ~CStageSelect();
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
protected:
	CSelector	*m_pParent;
	STAGESELECT_PHASE	m_ePhase;
	ID2D1Bitmap	*m_pImage;
	ID2D1SolidColorBrush	*m_pBrush;
	bool	m_bFlag;
	INT		m_iTimer;
	INT		m_iFadeTimer;
};

