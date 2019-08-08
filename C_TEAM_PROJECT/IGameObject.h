/**
* @file IGameObject.h
* @brief �Q�[����̃I�u�W�F�N�g��\������N���X��ĉ������C���^�[�t�F�[�X�̐錾
* @note ���{�̃Q�[���ƊE�ł̓^�X�N�ƌĂԏꍇ������
*/
#pragma once
#define __IGAMEOBJECT_H__

struct ID2D1RenderTarget;

typedef struct {
	float fx;
	float fy;
	float rad;
	float angle;
}	CIRCLE_ELEMENTS;

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
protected:
	CIRCLE_ELEMENTS circle;
};


//  �}�N����`
#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }

#define PI 3.1415926f
