//=============================================================================
//
// バレット処理 [bullet.h]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLET_MAX		(1000)		// バレットのMax数
#define BULLET_SPEED	(6.0f)		// バレットの移動スピード
#define BULLET_HEIGHT	(50)
#define BULLET_WIDTH	(TEXTURE_HEIGHT / 3)

// バレット構造体
struct BULLET
{
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	XMFLOAT3			move;				// バレットの移動量
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか
	BOOL				frozen;
	int					freezeRemaining;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET* GetBullet(void);
BULLET* SetBullet(XMFLOAT3 pos);
BULLET** GetFrozen(void);
int		 GetFrozenCount(void);
void	DelAllFrozen(void);


