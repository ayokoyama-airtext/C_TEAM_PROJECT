/**
* @file IGameScene.h
* @brief シーンの基底クラス IGameScene の宣言
*/
#pragma once
#define __IGAMESCENE_H__

enum GameSceneResultCode;
struct ID2D1RenderTarget;

class IGameScene
{
public:
	virtual ~IGameScene() = 0;	//	純粋仮想デストラクタ
	virtual GameSceneResultCode move() = 0;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) = 0;
	//virtual void reset() = 0;
};

