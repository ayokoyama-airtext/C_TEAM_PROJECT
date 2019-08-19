/**
*@file		BG.cpp
*@author	A.Yokoyama
*@brief		BGƒNƒ‰ƒX‚ÌŽÀ‘•
*/
#include "stdafx.h"
#include <d2d1.h>
#include "Stage.h"
#include "BG.h"
#include "TextureLoader.h"


#define BG_FILE_NAME	_T("res\\bg.png")


CBG::CBG(CStage *pStage)
{
	m_pParent = pStage;
	m_fFieldWidth = pStage->FIELD_WIDTH;
	m_fFieldHeight = pStage->FIELD_HEIGHT;

	ID2D1RenderTarget *pTarget = pStage->GetRenderTarget();
	if (pTarget) {
		CTextureLoader::CreateD2D1BitmapFromFile(pTarget, BG_FILE_NAME, &m_pImage);
		SAFE_RELEASE(pTarget);
	}
}


CBG::~CBG()
{
	SAFE_RELEASE(m_pImage);
}


bool CBG::move() {
	return true;
}


void CBG::draw(ID2D1RenderTarget *pRenderTarget) {
	//	Stage‚©‚ç”î•ñ‚ðŽæ“¾
	float playerX = m_pParent->playerCoords.playerX;
	float playerY = m_pParent->playerCoords.playerY;
	float playerDrawX = m_pParent->playerCoords.playerDrawX;
	float playerDrawY = m_pParent->playerCoords.playerDrawY;
	DRAW_SITUATION dsX = m_pParent->playerCoords.XaxisDraw;
	DRAW_SITUATION dsY = m_pParent->playerCoords.YaxisDraw;


	D2D1_RECT_F rc, src;
	D2D1_SIZE_F size, texSize;
	size = pRenderTarget->GetSize();
	rc.left = 0;
	rc.right = rc.left + size.width;
	rc.top = 0;
	rc.bottom = rc.top + size.height;

	//	x•ûŒü
	switch (dsX) {
	case DRAW_CENTERING_PLAYER:
		src.left = playerX - (DISPLAY_RESOLUTION_WIDTH >> 1);
		src.right = src.left + DISPLAY_RESOLUTION_WIDTH;
		break;
	case DRAW_STATIC_MIN:
		src.left = 0.f;
		src.right = DISPLAY_RESOLUTION_WIDTH;
		break;
	case DRAW_STATIC_MAX:
		src.right = m_fFieldWidth;
		src.left = src.right - DISPLAY_RESOLUTION_WIDTH;
		break;
	}
	//	y•ûŒü
	switch (dsY) {
	case DRAW_CENTERING_PLAYER:
		src.top = playerY - (DISPLAY_RESOLUTION_HEIGHT >> 1);
		src.bottom = src.top + DISPLAY_RESOLUTION_HEIGHT;
		break;
	case DRAW_STATIC_MIN:
		src.top = 0.f;
		src.bottom = DISPLAY_RESOLUTION_HEIGHT;
		break;
	case DRAW_STATIC_MAX:
		src.bottom = m_fFieldHeight;
		src.top = src.bottom - DISPLAY_RESOLUTION_HEIGHT;
		break;
	}

	pRenderTarget->DrawBitmap(m_pImage, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src);
}