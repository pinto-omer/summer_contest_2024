//=============================================================================
//
// スコア処理 [menu.cpp]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "renderer.h"
#include "menu.h"
#include "sprite.h"
#include "player.h"
#include "fade.h"
#include "file.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(250)	// キャラサイズ
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(14)		// テクスチャの数

#define TEXTURE_WIDTH_SELECT		(50)	// キャラサイズ
#define TEXTURE_HEIGHT_SELECT		(50)	// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX + 1] = { NULL };	// テクスチャ情報

static char* g_TexturName[] = {
	"data/TEXTURE/fade_black.png",
	"data/TEXTURE/menu/back.png",
	"data/TEXTURE/menu/exit.png",
	"data/TEXTURE/menu/leveleditor.png",
	"data/TEXTURE/menu/load.png",
	"data/TEXTURE/menu/mainmenu.png",
	"data/TEXTURE/menu/plevel1.png",
	"data/TEXTURE/menu/plevel2.png",
	"data/TEXTURE/menu/plevel3.png",
	"data/TEXTURE/menu/resume.png",
	"data/TEXTURE/menu/save.png",
	"data/TEXTURE/menu/start.png",
	"data/TEXTURE/menu/tutorial.png",
	"data/TEXTURE/menu/test.png",
};

static char g_SelectTextureName[] = "data/TEXTURE/menu/select.png";


enum {
	BG,
	BACK,
	EXIT,
	LEVEL_EDITOR,
	LOAD,
	MAINMENU,
	PLEVEL1,
	PLEVEL2,
	PLEVEL3,
	RESUME,
	SAVE,
	START,
	TUTORIAL,
	TEST,
};
static int						g_ModeMenus[MODE_MAX + 1][TEXTURE_MAX];
static BOOL						isLevelSelect;						// true:使っている  false:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_Menu;					// テクスチャ番号
static int						g_MaxIndex;
static int						g_MenuIndex;					//
static BOOL						isSave;
static int						editorLevel = 0;
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
	g_Texture[TEXTURE_MAX] = NULL;
	D3DX11CreateShaderResourceViewFromFile(GetDevice(),
		g_SelectTextureName,
		NULL,
		NULL,
		&g_Texture[TEXTURE_MAX],
		NULL);

	for (int i = 0; i < MODE_MAX + 1; i++)
	{
		for (int j = 0; j < TEXTURE_MAX; j++)
		{
			g_ModeMenus[i][j] = -1;
		}
	}
	g_ModeMenus[MODE_TITLE][0] = START;
	g_ModeMenus[MODE_TITLE][1] = LEVEL_EDITOR;
	g_ModeMenus[MODE_TITLE][2] = TUTORIAL;
	g_ModeMenus[MODE_TITLE][3] = EXIT;

	g_ModeMenus[MODE_GAME][0] = RESUME;
	g_ModeMenus[MODE_GAME][1] = LOAD;
	g_ModeMenus[MODE_GAME][2] = MAINMENU;
	if (GetMode() == MODE_GAME && editorLevel != 0)
		g_ModeMenus[MODE_GAME][3] = LEVEL_EDITOR;

	g_ModeMenus[MODE_EDITOR][0] = RESUME;
	g_ModeMenus[MODE_EDITOR][1] = TEST;
	g_ModeMenus[MODE_EDITOR][2] = SAVE;
	g_ModeMenus[MODE_EDITOR][3] = LOAD;
	g_ModeMenus[MODE_EDITOR][4] = MAINMENU;

	g_ModeMenus[MODE_MAX][0] = PLEVEL1;
	g_ModeMenus[MODE_MAX][1] = PLEVEL2;
	g_ModeMenus[MODE_MAX][2] = PLEVEL3;
	g_ModeMenus[MODE_MAX][3] = BACK;




	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// プレイヤーの初期化
	isLevelSelect = FALSE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { (SCREEN_WIDTH - g_w) * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f };
	g_Menu = -1;
	g_MaxIndex = -1;
	g_MenuIndex = 0;

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

	for (int i = 0; i < TEXTURE_MAX + 1; i++)
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

	if (g_Menu == -1)
		g_Menu = GetMode();
	g_MaxIndex = -1;
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_ModeMenus[g_Menu][i] == -1) break;
		g_MaxIndex++;
	}

	if (GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_UP))
	{
		g_MenuIndex -= g_MenuIndex == 0 ? 0 : 1;
	}
	else if (GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_MenuIndex += g_MenuIndex < g_MaxIndex ? 1 : 0;
	}
	else if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A))
	{
		switch (g_ModeMenus[g_Menu][g_MenuIndex])
		{
		case BACK:
			g_Menu = GetMode();
			g_MenuIndex = 0;
			break;
		case EXIT:
			SetFade(FADE_OUT, MODE_MAX);
			break;
		case LEVEL_EDITOR:
			if (GetMode() != MODE_GAME)
				SetField(0);
			else
			{
				SetField(editorLevel);
				editorLevel = 0;
			}
			SetFade(FADE_OUT, MODE_EDITOR);
			break;
		case LOAD:
			g_Menu = MODE_MAX;
			g_MenuIndex = 0;
			isSave = FALSE;
			break;
		case MAINMENU:
			SetFade(FADE_OUT, MODE_TITLE);
			setStartPos(XMFLOAT3(-1, -1, 0));
			break;
		case PLEVEL1:
			if (isSave)
			{
				SaveField(1);
				SetField(1);
			}
			else
			{

				SetField(1);
				SetFade(FADE_OUT, GetMode());
				return;
			}
			ToggleMenu();
			break;
		case PLEVEL2:
			if (isSave)
			{
				SaveField(2);
				SetField(2);
			}
			else
			{
				SetField(2);
				SetFade(FADE_OUT, GetMode());
				return;
			}
			ToggleMenu();
			break;
		case PLEVEL3:
			if (isSave)
			{
				SaveField(3);
				SetField(3);
			}
			else
			{
				SetField(3);
				SetFade(FADE_OUT, GetMode());
				return;
			}
			ToggleMenu();
			break;
		case RESUME:
			ToggleMenu();
			break;
		case SAVE:
			g_Menu = MODE_MAX;
			g_MenuIndex = 0;
			isSave = TRUE;
			break;
		case START:
			SetField(0);
			SetFade(FADE_OUT, MODE_GAME);
			break;
		case TUTORIAL:
			SetFade(FADE_OUT, MODE_TUTORIAL);
			break;
		case TEST:
			int field = GetFieldNum();
			if (field != 0)
			{
				SetFade(FADE_OUT, MODE_GAME);
				editorLevel = field;
			}
			break;
		}
	}

	g_MaxIndex = -1;
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_ModeMenus[g_Menu][i] == -1) break;
		g_MaxIndex++;
	}
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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BG]);

	// スコアの位置やテクスチャー座標を反映
	float px = 0;	// スコアの表示位置X
	float py = 0;			// スコアの表示位置Y
	float pw = SCREEN_WIDTH;				// スコアの表示幅
	float ph = SCREEN_HEIGHT;				// スコアの表示高さ

	float tw = 1.0f;		// テクスチャの幅
	float th = 1.0f;		// テクスチャの高さ
	float tx = 0.0f;			// テクスチャの左上X座標
	float ty = 0.0f;			// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, GetMode() == MODE_GAME ? 0.55f : 0.25f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	for (int i = 0; i <= g_MaxIndex; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ModeMenus[g_Menu][i]]);

		// スコアの位置やテクスチャー座標を反映
		px = g_Pos.x;	// スコアの表示位置X
		py = g_Pos.y + i * (g_h * 1.1f);			// スコアの表示位置Y
		pw = g_w;				// スコアの表示幅
		ph = g_h;				// スコアの表示高さ

		tw = 1.0f;		// テクスチャの幅
		th = 1.0f;		// テクスチャの高さ
		tx = 0.0f;			// テクスチャの左上X座標
		ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (g_ModeMenus[g_Menu][i] == TEST && GetFieldNum() == 0) ? 0.33f : 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
		if (g_MenuIndex == i)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX]);

			// スコアの位置やテクスチャー座標を反映
			px = g_Pos.x - TEXTURE_WIDTH_SELECT * 1.1f;	// スコアの表示位置X
			py = g_Pos.y + i * (g_h * 1.1f);			// スコアの表示位置Y
			pw = TEXTURE_WIDTH_SELECT;				// スコアの表示幅
			ph = TEXTURE_HEIGHT_SELECT;				// スコアの表示高さ

			tw = 1.0f;		// テクスチャの幅
			th = 1.0f;		// テクスチャの高さ
			tx = 0.0f;			// テクスチャの左上X座標
			ty = 0.0f;			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


}




