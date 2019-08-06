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
	bool		m_bGameOver;
};

