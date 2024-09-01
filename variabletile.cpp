//=============================================================================
//
// バレット処理 [tile.cpp]
// Author : 
//
//=============================================================================
#include "variabletile.h"
#include "tile.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "editor.h"
#include "field.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
int GetVarType(int tileType);

//*****************************************************************************
// グローバル変数
//*****************************************************************************

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static VARTILE	g_VarTiles[MAX_VAR_TILES + 1];	// バレット構造体
static int		lastIDX;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitVariableTile(void)
{

	// バレット構造体の初期化
	for (int i = 0; i < MAX_VAR_TILES + 1; i++)
	{
		g_VarTiles[i].type = V_MAX;
		g_VarTiles[i].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		g_VarTiles[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}

	lastIDX = -1;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitVariableTile(void)
{
	if (g_Load == FALSE) return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateVariableTile(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_VAR_TILES; i++)
	{
		// アニメーション  
		if (g_VarTiles[i].type == V_MAX) break;

	}


}

//=============================================================================
// 描画処理
//=============================================================================


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
VARTILE* GetVarTile(void)
{
	return g_VarTiles;
}

void ResetEditorVarTile(int tileType)
{
	g_VarTiles[MAX_VAR_TILES].type = GetVarType(tileType);
	g_VarTiles[MAX_VAR_TILES].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

int AddVarTile(void)
{
	if (lastIDX == MAX_VAR_TILES) return -1;
	return ++lastIDX;
}

void RemoveVarTile(int idx)
{
	if (idx > lastIDX || idx < 0) return;
	FIELD* field = GetField();
	for (int i = idx; i < lastIDX; i++)
	{
		g_VarTiles[i] = g_VarTiles[i + 1];
		field->varTilePos[i] = field->varTilePos[i + 1];
	}
	g_VarTiles[lastIDX].type = V_MAX;
	g_VarTiles[lastIDX].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	g_VarTiles[lastIDX].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	field->varTilePos[lastIDX] = XMINT2(-1, -1);
	lastIDX--;
}

int GetVarType(int tileType)
{
	switch (tileType)
	{
	case TILE_TRAP:
		return V_DIRECTIONAL;
	default:
		return V_MAX;
	}

}

void FindLastVarTileIDX(void)
{
	lastIDX = -1;
	for (int i = 0; i < MAX_VAR_TILES; i++)
	{
		if (g_VarTiles[i].type == V_MAX) break;
		lastIDX++;
	}
}
//=============================================================================
// バレットの発射設定
//=============================================================================



