/**
* @file IGameObject.h
* @brief ゲーム上のオブジェクトを表現するクラスを汎化したインターフェースの宣言
* @note 日本のゲーム業界ではタスクと呼ぶ場合がある
*/
#pragma once
#define __IGAMEOBJECT_H__

#include "SoundManager.h"

//	前方宣言
struct ID2D1RenderTarget;



class IGameObject
{
public:
	virtual ~IGameObject() = 0;
	virtual bool move() = 0;
	virtual void draw(ID2D1RenderTarget *pRenderTarget) = 0;

	virtual bool collide(float x, float y, float r) {
		return false;
	}
	virtual bool collide(IGameObject *pObj) {
		return false;
	}
	virtual void damage(float amount) {};

	virtual float GetX() { return 0.f; }
	virtual float GetY() { return 0.f; }
	virtual float GetRad() { return 0.f; }
	virtual float GetAngle() { return 0.f; }
	virtual int GetNumber() { return -1; }
	virtual int GetState() { return -1; }
	virtual int GetID() { return -1; }
	virtual int GetScore() { return 0; }

	virtual void SetFlag(int flag) {};
};


//  マクロ定義
#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }

#define PI 3.1415926f
