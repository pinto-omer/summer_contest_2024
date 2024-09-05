//=============================================================================
//
// スコア処理 [menu.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "menu.h"
#include "sprite.h"
#include "player.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// キャラサイズ
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(11)		// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[] = {
	"data/TEXTURE/back.png",
	"data/TEXTURE/exit.png",
	"data/TEXTURE/leveleditor.png",
	"data/TEXTURE/load.png",
	"data/TEXTURE/mainmenu.png",
	"data/TEXTURE/plevel1.png",
	"data/TEXTURE/plevel2.png",
	"data/TEXTURE/plevel3.png",
	"data/TEXTURE/resume.png",
	"data/TEXTURE/save.png",
	"data/TEXTURE/select.png",
	"data/TEXTURE/start.png",
};


static bool						g_Use;						// true:使っている  false:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_Menu;					// スコア

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMenu(void)
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


	// プレイヤーの初期化
	g_Use = true;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { SCREEN_WIDTH - TEXTURE_WIDTH * 0.5f - 20.0f, TEXTURE_HEIGHT * 0.5f + 20.0f, 0.0f };
	g_TexNo = 0;

	g_Menu = 0;	// スコアの初期化

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMenu(void)
{
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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMenu(void)
{


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMenu(void)
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

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// スコアの位置やテクスチャー座標を反映
	float px = g_Pos.x;	// スコアの表示位置X
	float py = g_Pos.y;			// スコアの表示位置Y
	float pw = g_w;				// スコアの表示幅
	float ph = g_h;				// スコアの表示高さ

	float tw = 1.0f;		// テクスチャの幅
	float th = 1.0f;		// テクスチャの高さ
	float tx = 0.0f;			// テクスチャの左上X座標
	float ty = 0.0f;			// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(0.67f, 0.0f, 0.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	float percent = GetPlayer()->hp / PLAYER_MAX_HP;
	// スコアの位置やテクスチャー座標を反映
	px = g_Pos.x;	// スコアの表示位置X
	py = g_Pos.y;			// スコアの表示位置Y
	pw = g_w * percent;				// スコアの表示幅
	ph = g_h * percent;				// スコアの表示高さ

	tw = 1.0f;		// テクスチャの幅
	th = 1.0f;		// テクスチャの高さ
	tx = 0.0f;			// テクスチャの左上X座標
	ty = 0.0f;			// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(0.0f, 0.67f, 0.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

}




