#pragma once

class CStage;
struct ID2D1RenderTarget;
struct ID2D1Bitmap;
struct ID2D1SolidColorBrush;
struct IDWriteTextFormat;

class CUI
{
public:
	CUI(CStage *pStage);
	virtual ~CUI();
	virtual void move();
	virtual void draw(ID2D1RenderTarget *pRenderTarget);
protected:
	CStage *m_pParent;
	ID2D1Bitmap	*m_pImage;	//	�摜�t�@�C���̓ǂݍ��ݐ�   IDWriteTextFormat    *m_pTextFormat;
	IDWriteTextFormat    *m_pTextFormat;

	UINT	m_iScore;	//	�X�R�A
	UINT	m_iTimer;	//	�o�ߎ���

	ID2D1SolidColorBrush	*m_pBrush;	//	�f�o�b�O�p�̃u���V

};

