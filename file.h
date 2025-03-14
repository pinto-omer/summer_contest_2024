//=============================================================================
//
// ファイル処理 [file.h]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#pragma once

#include "main.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "field.h"
#include "variabletile.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/



/*******************************************************************************
* 構造体定義
*******************************************************************************/
struct	SAVEDATA				// セーブデータの構造体
{
	// プレイヤーデータを保存
	PLAYER	player[PLAYER_MAX];	// プレイヤーのMAX人数分セーブする

	// エネミーデータを保存
	ENEMY	enemy[ENEMY_MAX];	// エネミーのMAX人数分セーブする

	// スコアデータを保存
	int		score;				// スコアを保存する

	// 他にも何かセーブしたい物があればここに追加

	int		sum;				// チェックサムを入れる
};

struct SAVEFIELD
{
	int			sum;
	int			vartilesize;
	int			field[FIELD_TILE_H][FIELD_TILE_W];
	XMINT2		varTilePos[MAX_VAR_TILES];
	VARTILE		varTiles[MAX_VAR_TILES];
};

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
void SaveData(void);
void LoadData(void);

void SaveField(int id);
void LoadField(int id);

