//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"
#include "tile.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX			(1)		// プレイヤーのMax人数

#define	PLAYER_OFFSET_CNT	(16)	// 16分身

#define PLAYER_MAX_HP		(100.0f)
#define FREEZE_MAX_DIAMETER	(TILE_WIDTH * 10.0f)
#define	FREEZE_DURATION		(300)	//freeze duration in frames
enum
{
	//CHAR_DIR_UP,
	CHAR_DIR_RIGHT,
	//CHAR_DIR_DOWN,
	CHAR_DIR_LEFT,
	CHAR_DIR_IDLE,

	CHAR_DIR_MAX
};

enum
{
	GAME_NOT_OVER,
	GAME_OVER,
	GAME_CLEAR,
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	
	int			dir;			// 向き（0:上 1:右 2:下 3:左）
	BOOL		moving;			// 移動中フラグ

	BOOL		dash;			// ダッシュ中フラグ
	XMFLOAT3	move;			// 移動速度
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標

	BOOL		jump;			// ジャンプフラグ
	float		jumpY;			// ジャンプの高さ
	int			jumpCnt;		// ジャンプ中のカウント
	float		jumpYMax;		// 
	float		fallSpeed;

	BOOL		freeze;
	XMFLOAT3	freezePos;
	float		freezeRadius;

	float		hp;
	int			framesSinceHit;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

int GetPlayerCount(void);
void DamagePlayer(PLAYER* player);

int GetGameOverStatus(void);