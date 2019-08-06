/**
* @file IGameScene.h
* @brief �V�[���̊��N���X IGameScene �̐錾
*/
#pragma once
#define __IGAMESCENE_H__

enum GameSceneResultCode;
struct ID2D1RenderTarget;

class IGameScene
{
public:
	virtual ~IGameScene() = 0;	//	�������z�f�X�g���N�^
	virtual GameSceneResultCode move() = 0;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) = 0;
	//virtual void reset() = 0;
};

