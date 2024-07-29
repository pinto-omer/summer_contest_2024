//=============================================================================
//
// Field処理 [bg.cpp]
// Author : 
//
//=============================================================================
#include "field.h"
#include "tile.h"
#include "bg.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(1080)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(3)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/map.png",
	"data/TEXTURE/sky000.jpg",
	"data/TEXTURE/sky001.jpg",
};


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ
static FIELD	g_Field;

static int field1[FIELD_TILE_H][FIELD_TILE_W] =
{
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_EMPTY,TILE_WALL},
	{TILE_WALL,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_GROUND,TILE_WALL},
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitField(void)
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

	memcpy(g_Field.field, field1, sizeof(g_Field.field));
	// 変数の初期化
	
	g_Field.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.texNo = 0;

	g_Field.scrl = 0.0f;		// TEXスクロール
	g_Field.scrl2 = 0.0f;		// TEXスクロール
	
	InitTile();
	g_Field.tile_w = GetTile()->w;
	g_Field.tile_h = GetTile()->h;
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitField(void)
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

	UninitTile();
	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateField(void)
{
	//g_Field.old_pos = g_Field.pos;	// １フレ前の情報を保存


	//g_Field.scrl -= 0.0f;		// 0.005f;		// スクロール

	UpdateTile();
#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawField(void)
{
	BG* bg = GetBG();
	TILE* tiles = GetTile();
	for (int i = 0; i < FIELD_TILE_H; i++)
	{
		for (int j = 0; j < FIELD_TILE_W; j++)
		{
			// 画面外まで進んだ？
			XMFLOAT3 pos = XMFLOAT3(j * tiles->w + tiles->w / 2.0f - bg->pos.x,
				i * tiles->h + tiles->h / 2.0f - bg->pos.y,
				0);
			if ((pos.y < (-tiles->h / 2.0f)) ||			// 自分の大きさを考慮して画面外か判定している
				(pos.y > (bg->h +tiles->h / 2.0f)) ||
				(pos.x < (-tiles->w / 2.0f)) ||
				(pos.x > (bg->w + tiles->w / 2.0f)))
			{
				continue;
			}
			DrawTile(g_Field.field[i][j], pos);
				
		}
	}
}


//=============================================================================
// Field構造体の先頭アドレスを取得
//=============================================================================
FIELD* GetField(void)
{
	return &g_Field;
}

int GetTileType(XMFLOAT3 pos)
{
	return GetTile()[g_Field.field[(int)(pos.y / g_Field.tile_h)][(int)(pos.x / g_Field.tile_w)]].type;
}

XMFLOAT3 GetGroundBelow(XMFLOAT3 pos)
{
	int i, j;
	i = (int)(pos.y / g_Field.tile_h);
	j = (int)(pos.x / g_Field.tile_w);
	while (i < FIELD_TILE_H && GetTile()[g_Field.field[i][j]].type != GROUND)
		i++;
	return XMFLOAT3(j * g_Field.tile_w - g_Field.tile_w,
		i * g_Field.tile_h - g_Field.tile_h,
		0);
}




