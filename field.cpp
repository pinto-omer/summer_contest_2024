//=============================================================================
//
// Field処理 [bg.cpp]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#include "field.h"
#include "tile.h"
#include "bg.h"
#include "file.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// 背景サイズ
//#define TEXTURE_HEIGHT				(1080)			// (SCREEN_HEIGHT)	// 
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
static int fieldNum = 0;


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

	//memcpy(g_Field.field, field1, sizeof(g_Field.field));
	// 変数の初期化
	for (int i = 0; i < MAX_VAR_TILES; i++)
		g_Field.varTilePos[i] = XMINT2(-1, -1);
	
	g_Field.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.texNo = 0;

	g_Field.scrl = 0.0f;		// TEXスクロール
	g_Field.scrl2 = 0.0f;		// TEXスクロール
	InitTile();
	InitVariableTile();
	g_Field.tile_w = GetTile()->w;
	g_Field.tile_h = GetTile()->h;
	
	for (int i = 0; i < FIELD_TILE_H; i++)
		for (int j = 0; j < FIELD_TILE_W; j++)
			g_Field.field[i][j] = TILE_EMPTY;

	LoadField(fieldNum);
	fieldNum = 0;
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
	UninitVariableTile();
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
	UpdateVariableTile();
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
			float w, h;
			h = tiles[g_Field.field[i][j]].h;
			w = tiles[g_Field.field[i][j]].w;
			XMFLOAT3 pos = XMFLOAT3(j * w + w / 2.0f - bg->pos.x,
				i * h + h / 2.0f - bg->pos.y,
				0);
			if ((pos.y < (-h / 2.0f)) ||			// 自分の大きさを考慮して画面外か判定している
				(pos.y > (bg->h + h / 2.0f)) ||
				(pos.x < (-w / 2.0f)) ||
				(pos.x > (bg->w + w / 2.0f)))
			{
				continue;
			}
			if (tiles[g_Field.field[i][j]].isVariable == FALSE)
				DrawTile(g_Field.field[i][j], pos,FALSE,-1);
			else
				DrawTile(g_Field.field[i][j], pos, FALSE, getVarTileIDAtPos(XMINT2(i,j)));
				
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

void SetField(int fieldNumber)
{
	fieldNum = fieldNumber;
}

int GetFieldNum(void)
{
	return fieldNum;
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
	return XMFLOAT3(j * g_Field.tile_w /*- g_Field.tile_w*/,
		i * g_Field.tile_h /*- g_Field.tile_h*/,
		0);
}

int getVarTileIDAtPos(XMINT2 pos)
{
	for (int i = 0; i < MAX_VAR_TILES; i++)
	{
		if (g_Field.varTilePos[i].x == pos.x &&
			g_Field.varTilePos[i].y == pos.y)
			return i;
	}
	return -1;
}



