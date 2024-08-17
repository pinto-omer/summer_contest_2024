//=============================================================================
//
// �o���b�g���� [tile.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define TILE_WIDTH	(50)
#define TILE_HEIGHT	(50)
// �o���b�g�\����
struct TILE
{
	float						w, h;				// ���ƍ���
	int							countAnim;			// �A�j���[�V�����J�E���g
	int							patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int							texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	int							type;

};

enum {//	TILE_PLAYER = -2,
	TILE_EMPTY,
	TILE_GROUND,
	TILE_WALL,

	TILE_MAX
};

enum {
	AIR,
	SOLID,
	GROUND,
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTile(void);
void UninitTile(void);
void UpdateTile(void);
void DrawTile(int tileIDX, XMFLOAT3 pos, BOOL isEditTile);

TILE* GetTile(void);
void SetTile(XMFLOAT3 pos);

