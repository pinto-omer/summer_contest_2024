//=============================================================================
//
// プレイヤー処理 [editor.cpp]
// Author : 
//
//=============================================================================
#include "editor.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "tile.h"
#include "field.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// キャラサイズ
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(TILE_MAX)		// テクスチャの数

//#define TEXTURE_PATTERN_DIVIDE_X	(3)		// アニメパターンのテクスチャ内分割数（X)
//#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
//#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

#define EDITOR_DISP_X				(SCREEN_WIDTH / 2)
# define EDITOR_DISP_Y				(SCREEN_HEIGHT / 2)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/border.png",
};



static int animPatternNum[TEXTURE_MAX] = { 0 };

static BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ
static EDITOR	g_Editor;		// プレイヤー構造体



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEditor(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// プレイヤー構造体の初期化

	g_Editor.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 中心点から表示
	g_Editor.w = TEXTURE_WIDTH;
	g_Editor.h = TEXTURE_HEIGHT;
	g_Editor.texNo = 0;

	g_Editor.countAnim = 0;
	g_Editor.patternAnim = 0;

	g_Editor.move = XMFLOAT3(TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f);		// 移動量
	g_Editor.patternAnim = 0;// g_Editor.dir* TEXTURE_PATTERN_DIVIDE_X;






	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEditor(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEditor(void)
{
	if (g_Load == FALSE) return;

	XMFLOAT3 pos_old = g_Editor.pos;

	// アニメーション  

	/*g_Editor.countAnim += 1.0f;
	if (g_Editor.countAnim > ANIM_WAIT)
	{
		g_Editor.countAnim = 0.0f;

		g_Editor.patternAnim = (g_Editor.patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X;
	}*/



	// キー入力で移動 
	{
		if (GetKeyboardPress(DIK_DOWN) || IsButtonPressed(0, BUTTON_DOWN))
		{
			g_Editor.pos.y += g_Editor.move.y;
		}
		else if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_UP))
		{
			g_Editor.pos.y -= g_Editor.move.y;
		}

		else if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT))
		{
			g_Editor.pos.x += g_Editor.move.x;
		}
		else if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_LEFT))
		{
			g_Editor.pos.x -= g_Editor.move.x;
		}

		//field edit debug
		else if (GetKeyboardPress(DIK_F2))
		{
			if (GetKeyboardTrigger(DIK_1))
				SaveField(1);
			else if (GetKeyboardTrigger(DIK_2))
				SaveField(2);
			else if (GetKeyboardTrigger(DIK_3))
				SaveField(3);
		}
		else if (GetKeyboardPress(DIK_F3))
		{
			if (GetKeyboardTrigger(DIK_1))
				LoadField(1);
			else if (GetKeyboardTrigger(DIK_2))
				LoadField(2);
			else if (GetKeyboardTrigger(DIK_3))
				LoadField(3);
		}







		// MAP外チェック
		BG* bg = GetBG();

		if (g_Editor.pos.x < 0)
		{
			g_Editor.pos.x = 0;
		}

		else if (g_Editor.pos.x > bg->w - (g_Editor.w))
		{
			g_Editor.pos.x = bg->w - (g_Editor.w);
		}

		if (g_Editor.pos.y < 0)
		{
			g_Editor.pos.y = 0;
		}

		else if (g_Editor.pos.y > bg->h - (g_Editor.h))
		{
			g_Editor.pos.y = bg->h - (g_Editor.h);
		}



		// プレイヤーの立ち位置からMAPのスクロール座標を計算する
		bg->pos.x = g_Editor.pos.x - EDITOR_DISP_X;
		if (bg->pos.x < 0) bg->pos.x = 0;
		if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

		bg->pos.y = g_Editor.pos.y - EDITOR_DISP_Y;
		if (bg->pos.y < 0) bg->pos.y = 0;
		if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;




	}

#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEditor(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Editor.texNo]);

	//プレイヤーの位置やテクスチャー座標を反映
	float px = g_Editor.pos.x - bg->pos.x;	// プレイヤーの表示位置X
	float py = g_Editor.pos.y - bg->pos.y;	// プレイヤーの表示位置Y
	float pw = g_Editor.w;		// プレイヤーの表示幅
	float ph = g_Editor.h;		// プレイヤーの表示高さ

	//py += g_Editor.jumpY;		// ジャンプ中の高さを足す



	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}


//=============================================================================
// Editor構造体の先頭アドレスを取得
//=============================================================================
EDITOR* GetEditor(void)
{
	return &g_Editor;
}


