/**
* @file Stage.h
* @brief �N���X CStage �̐錾
*/
#pragma once
#include "Selector.h"

#ifndef __IGAMEOBJECT_H__
#include "IGameObject.h"
#endif

//  ���g���b�L�[�ȑO���錾
namespace std {
	template<class _Ty> class allocator;
	template <class _Ty, class _Alloc = allocator<_Ty>>class list;
}

enum STAGE_PHASE {
	STAGE_INIT = 0x01,
	STAGE_RUN = 0x02,
	STAGE_DONE = 0x03,
};


class CPlayer;


class CStage : public IGameScene
{
public:
	CStage(CSelector *pSystem);
	virtual ~CStage(void);
	virtual GameSceneResultCode move() override;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) override;
	ID2D1RenderTarget *GetRenderTarget();
protected:
	CSelector	*m_pSystem;
	CPlayer		*m_pPlayer;
	STAGE_PHASE	m_ePhase;
	UINT		m_iGameFinishState;

	//	m_iGameFinishState �p�萔
	static const int GAME_CLEAR = 0x02;
	static const int GAME_OVER = 0x04;
};


