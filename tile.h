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

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define TILE_WIDTH	(50)
#define TILE_HEIGHT	(50)
// バレット構造体
struct TILE
{
	float						w, h;				// 幅と高さ
	int							countAnim;			// アニメーションカウント
	int							patternAnim;		// アニメーションパターンナンバー
	int							texNo;				// 何番目のテクスチャーを使用するのか
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
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTile(void);
void UninitTile(void);
void UpdateTile(void);
void DrawTile(int tileIDX, XMFLOAT3 pos, BOOL isEditTile);

TILE* GetTile(void);
void SetTile(XMFLOAT3 pos);

