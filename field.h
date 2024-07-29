//=============================================================================
//
// FIELD処理 [field.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define FIELD_TILE_W				(40)
#define FIELD_TILE_H				(20)


struct FIELD
{
	int field[FIELD_TILE_H][FIELD_TILE_W];
	XMFLOAT3	pos;			// ポリゴンの座標
	float		tile_w, tile_h;	// 幅と高さ
	int			texNo;			// 使用しているテクスチャ番号

	float		scrl;			// スクロール
	XMFLOAT3	old_pos;		// １フレ前のポリゴンの座標
	float		scrl2;			// スクロール
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);

FIELD* GetField(void);
int GetTileType(XMFLOAT3 pos);
XMFLOAT3 GetGroundBelow(XMFLOAT3 pos);

