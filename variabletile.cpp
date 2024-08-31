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
//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static VARTILE	g_VarTiles[MAX_VAR_TILES];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitVariableTile(void)
{
	
		// バレット構造体の初期化
	for (int i = 0; i < MAX_VAR_TILES; i++)
	{
		g_VarTiles[i].type = V_MAX;
		g_VarTiles[i].pos = XMFLOAT3(-1.0f,-1.0f,0.0f);
		g_VarTiles[i].rot = XMFLOAT3(0.0f,0.0f,0.0f);

	}


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


//=============================================================================
// バレットの発射設定
//=============================================================================



