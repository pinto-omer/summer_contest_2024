//=============================================================================
//
// FIELD���� [field.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define FIELD_TILE_W				(40)
#define FIELD_TILE_H				(20)


struct FIELD
{
	int field[FIELD_TILE_H][FIELD_TILE_W];
	XMFLOAT3	pos;			// �|���S���̍��W
	float		tile_w, tile_h;	// ���ƍ���
	int			texNo;			// �g�p���Ă���e�N�X�`���ԍ�

	float		scrl;			// �X�N���[��
	XMFLOAT3	old_pos;		// �P�t���O�̃|���S���̍��W
	float		scrl2;			// �X�N���[��
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);

FIELD* GetField(void);
int GetTileType(XMFLOAT3 pos);
XMFLOAT3 GetGroundBelow(XMFLOAT3 pos);

