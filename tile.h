//=============================================================================
//
// �o���b�g���� [tile.h]
// Author : GP11B132 31 PINTO OMER
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
	BOOL						isVariable;
};

enum {//	TILE_PLAYER = -2,
	TILE_EMPTY,
	TILE_GROUND,
	TILE_WALL,
	TILE_TRAP,
	TILE_FROZEN_ARROW,
	TILE_GOAL,
	TILE_GROUND_2,
	TILE_GROUND_3,
	TILE_GROUND_4,
	TILE_GROUND_5,
	TILE_GROUND_6,
	TILE_GROUND_7,
	TILE_MAX
};

enum {
	AIR,
	SOLID,
	GROUND,
	MOVING,
	DIRECTIONAL,
	TYPE_ARROW,
	GOAL,
	TILE_TYPE_MAX
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTile(void);
void UninitTile(void);
void UpdateTile(void);
void DrawTile(int tileIDX, XMFLOAT3 pos, BOOL isEditTile, int varTileID);

TILE* GetTile(void);
void SetTile(XMFLOAT3 pos);

