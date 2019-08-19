/**
*@file		BG.h
*@author	A.Yokoyama
*@date		07.August.2019
*@brief		BGクラスの宣言
*/
#pragma once

#ifndef __IGAMEOBJECT_H__
#include "IGameObject.h"
#endif // !__IGAMEOBJECT_H__


struct ID2D1RenderTarget;
struct ID2D1Bitmap;
class CStage;

class CBG
{
public:
	CBG(CStage *pStage);
	virtual ~CBG();
	virtual bool move();
	virtual void draw(ID2D1RenderTarget *pRenderTarget);
protected:
	CStage	*m_pParent;
	ID2D1Bitmap	*m_pImage;

	//	フィールド幅高
	FLOAT m_fFieldWidth;
	FLOAT m_fFieldHeight;

	
};

