//=============================================================================
//
// バレット処理 [tile.cpp]
// Author : 
//
//=============================================================================
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
#define TEXTURE_WIDTH				(TILE_WIDTH)	// キャラサイズ
#define TEXTURE_HEIGHT				(TILE_HEIGHT)	// 
#define TEXTURE_MAX					(TILE_MAX)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[] = {
	"",
	"data/TEXTURE/craftpix_free_green_zone/tiles/Tile_01.png",
	"data/TEXTURE/craftpix_free_green_zone/tiles/Tile_37.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static TILE	g_Tile[TILE_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTile(void)
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


	// バレット構造体の初期化
	for (int i = 0; i < TILE_MAX; i++)
	{
		g_Tile[i].w = TEXTURE_WIDTH;
		g_Tile[i].h = TEXTURE_HEIGHT;
		g_Tile[i].texNo = i;
		g_Tile[i].countAnim = 0;
		g_Tile[i].patternAnim = 0;
		g_Tile[i].type = SOLID;
	}

	g_Tile[TILE_EMPTY].type = AIR;
	g_Tile[TILE_GROUND].type = GROUND;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTile(void)
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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTile(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TILE_MAX; i++)
	{
		//// 画面外まで進んだ？
		//BG* bg = GetBG();
		//if ((g_Tile[i].pos.y < (-g_Tile[i].h / 2)) ||			// 自分の大きさを考慮して画面外か判定している
		//	(g_Tile[i].pos.y > (bg->h + g_Tile[i].h / 2)) ||
		//	(g_Tile[i].pos.x < (-g_Tile[i].w / 2)) ||
		//	(g_Tile[i].pos.x > (bg->w + g_Tile[i].w / 2)))
		//{
		//	g_Tile[i].use = false;
		//	continue;
		//}
		//else
		//	g_Tile[i].use = true;

		// アニメーション  
		g_Tile[i].countAnim++;
		if ((g_Tile[i].countAnim % ANIM_WAIT) == 0)
		{
			// パターンの切り替え
			g_Tile[i].patternAnim = (g_Tile[i].patternAnim + 1) % ANIM_PATTERN_NUM;
		}

	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTile(int tileIDX, XMFLOAT3 pos, BOOL isEditTile, int varTileID)
{
	if (tileIDX < 0 || tileIDX >= TILE_MAX) return;
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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Tile[tileIDX].texNo]);

	//バレットの位置やテクスチャー座標を反映
	float px = pos.x;		// バレットの表示位置X
	float py = pos.y;		// バレットの表示位置Y
	float pw = g_Tile[tileIDX].w;		// バレットの表示幅
	float ph = g_Tile[tileIDX].h;		// バレットの表示高さ

	float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
	float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
	float tx = (float)(g_Tile[tileIDX].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
	float ty = (float)(g_Tile[tileIDX].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標
	float alpha = isEditTile == TRUE ? 0.7f : 1.0f;
	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
TILE* GetTile(void)
{
	return &g_Tile[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetTile(XMFLOAT3 pos)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	//for (int i = 0; i < TILE_MAX; i++)
	//{
	//	if (g_Tile[i].use == FALSE)		// 未使用状態のバレットを見つける
	//	{
	//		g_Tile[i].use = TRUE;			// 使用状態へ変更する
	//		g_Tile[i].pos = pos;			// 座標をセット
	//		return;							// 1発セットしたので終了する
	//	}
	//}
}


