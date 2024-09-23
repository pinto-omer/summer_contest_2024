//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "player.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(6)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define ANIM_WAIT					(4)
#define TEXTURE_PATTERN_DIVIDE_X	(5)
#define PLAYER_TEXTURE_WIDTH		(128)
#define PLAYER_TEXTURE_HEIGHT		(128)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/menu/gameclear.png",
	"data/TEXTURE/menu/gameover.png",
	"data/TEXTURE/clearbg.png",
	"data/TEXTURE/gameoverbg.png",
	"data/TEXTURE/Enchantress/Dead.png",
};

enum {
	CLEAR,
	OVER,
	CLEAR_BG,
	OVER_BG,
	DEAD,
};

static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static float					g_countAnim;
static int						g_patternAnim;
static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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


	// 変数の初期化
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { g_w / 2.0f, g_h * 0.33f, 0.0f };
	g_TexNo = 0;

	g_patternAnim = 0;
	g_countAnim = 0.0f;
	// BGM再生


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	if (GetGameOverStatus() == GAME_OVER && g_patternAnim < TEXTURE_PATTERN_DIVIDE_X - 1)
	{
		g_countAnim += 0.33f;
		if (g_countAnim >= ANIM_WAIT)
		{
			g_patternAnim++;
			g_countAnim = 0;
		}
	}
	else
	{
		if (GetKeyboardTrigger(DIK_SPACE) || GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_A))
		{// Enter押したら、ステージを切り替える
			SetFade(FADE_OUT, MODE_TITLE);
		}

	}


#ifdef _DEBUG	// デバッグ情報を表示する

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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

	// リザルトの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GetGameOverStatus() == GAME_CLEAR ? CLEAR_BG : OVER_BG]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// リザルトのロゴを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GetGameOverStatus() == GAME_CLEAR ? CLEAR : OVER]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	if (GetGameOverStatus() == GAME_OVER)
	{
		//バレットの位置やテクスチャー座標を反映
		float px = g_Pos.x;		// バレットの表示位置X
		float py = g_Pos.y * 2.0f;		// バレットの表示位置Y
		float pw = PLAYER_TEXTURE_WIDTH;		// バレットの表示幅
		float ph = PLAYER_TEXTURE_HEIGHT;		// バレットの表示高さ

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f; 	// テクスチャの高さ
		float tx = g_patternAnim * tw;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[DEAD]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}




