#pragma once
//=============================================================================
//
// バレット処理 [tile.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"
#include "bullet.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define MAX_VAR_TILES	(50)
// VARTILE構造体
// in order to not break field save data, when adding new members
// ALWAYS add them to the end of the struct. member deletion is not supported
struct VARTILE
{
	int							type;
	XMFLOAT3					pos;
	XMFLOAT3					rot;
	BULLET*						bullet;
	int							cooldown;
	BOOL						frozen;
	BOOL						pressed;
};

enum {//	TILE_PLAYER = -2,
	V_MOVING,
	V_DIRECTIONAL_TRAP,

	V_MAX
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitVariableTile(void);
void UninitVariableTile(void);
void UpdateVariableTile(void);

VARTILE* GetVarTile(void);
void ResetEditorVarTile(int tileIDX);
int AddVarTile(void);
void RemoveVarTile(int idx);
void FindLastVarTileIDX(void);
