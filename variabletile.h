#pragma once
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
#include "bullet.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define MAX_VAR_TILES	(50)
// �o���b�g�\����
struct VARTILE
{
	int							type;
	XMFLOAT3					pos;
	XMFLOAT3					rot;
	BULLET*						bullet;
};

enum {//	TILE_PLAYER = -2,
	V_MOVING,
	V_DIRECTIONAL_TRAP,

	V_MAX
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitVariableTile(void);
void UninitVariableTile(void);
void UpdateVariableTile(void);

VARTILE* GetVarTile(void);
void ResetEditorVarTile(int tileIDX);
int AddVarTile(void);
void RemoveVarTile(int idx);
void FindLastVarTileIDX(void);
