//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "tile.h"
#include "field.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(128)	// キャラサイズ
#define TEXTURE_HEIGHT				(70)	// 
#define TEXTURE_MAX					(MISC_TEXTURE_MAX)		// テクスチャの数

#define FREEZE_FRAME_COUNT			(30)
//#define TEXTURE_PATTERN_DIVIDE_X	(3)		// アニメパターンのテクスチャ内分割数（X)
//#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
//#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

#define PLAYER_INITIAL_X			(80.0f)
#define PLAYER_INITIAL_Y			(930.0f)

// ジャンプ処理
#define	PLAYER_JUMP_CNT_MAX			(45)		// 30フレームで着地する
#define	PLAYER_JUMP_Y_MAX			(150.0f)	// ジャンプの高さ
#define PLAYER_JUMP_GRAVITY			(0.5f)
#define PLAYER_MAX_FALL_SPEED		(15.0f)

#define PLAYER_HIT_IFRAMES			(60)
#define PLAYER_REGEN_START			(240)
#define PLAYER_REGEN_AMOUNT			(0.2f)

enum
{
	SHADOW = CHAR_DIR_MAX,
	FREEZE,

	MISC_TEXTURE_MAX
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enchantress/walk_right.png",
	"data/TEXTURE/Enchantress/walk_left.png",
	"data/TEXTURE/Enchantress/idle.png",
	"data/TEXTURE/shadow000.jpg",
	"data/TEXTURE/circle.png"
};

static int texturePatternDivideX[TEXTURE_MAX] = {
	8,8,5
};

static int texturePatternDivideY[TEXTURE_MAX] = {
	1,1,1
};

static int animPatternNum[TEXTURE_MAX] = { 0 };

static BOOL		g_Load = FALSE;				// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];		// プレイヤー構造体
static int		g_PlayerCount = PLAYER_MAX;	// 生きてるプレイヤーの数

static int      g_jumpCnt = 0;
static int		g_jump[PLAYER_JUMP_CNT_MAX] =
{
	-15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5,-4,-3,-2,-1,
	  1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11,12,13,14,15
};

static int		gameOverStatus;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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

		animPatternNum[i] = texturePatternDivideX[i] * texturePatternDivideY[i];
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	g_PlayerCount = 0;						// 生きてるプレイヤーの数

	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_PlayerCount++;
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(PLAYER_INITIAL_X, PLAYER_INITIAL_Y, 0.0f);	// 中心点から表示
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = CHAR_DIR_IDLE;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

		g_Player[i].dir = CHAR_DIR_IDLE;					// 下向きにしとくか
		g_Player[i].moving = FALSE;							// 移動中フラグ
		g_Player[i].patternAnim = 0;// g_Player[i].dir* TEXTURE_PATTERN_DIVIDE_X;

		// ジャンプの初期化
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;
		g_Player[i].fallSpeed = 0.0f;

		// 分身用
		g_Player[i].dash = FALSE;
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
		g_Player[i].hp = PLAYER_MAX_HP;
		g_Player[i].framesSinceHit = 0;
	}
	gameOverStatus = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	if (g_Load == FALSE || GetMode() == MODE_EDITOR) return;
	g_PlayerCount = 0;				// 生きてるプレイヤーの数

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{
			g_PlayerCount++;		// 生きてるプレイヤーの数

			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Player[i].pos;
			if (g_Player[i].hp <= 0)
			{
				gameOverStatus = GAME_OVER;
				return;
			}
			else if (g_Player[i].hp > PLAYER_MAX_HP)
			{
				g_Player[i].hp = PLAYER_MAX_HP;
			}
			else if (g_Player[i].hp < PLAYER_MAX_HP && g_Player[i].framesSinceHit >= PLAYER_REGEN_START)
			{
				g_Player[i].hp += PLAYER_REGEN_AMOUNT;
			}

			if (g_Player[i].framesSinceHit < PLAYER_REGEN_START)
				g_Player[i].framesSinceHit++;
			// 分身用
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			g_Player[i].offset[0] = pos_old;

			// アニメーション  
			if (g_Player[i].moving == TRUE || g_Player[i].dir == CHAR_DIR_IDLE)
			{
				if (g_Player[i].dir == CHAR_DIR_IDLE)
					g_Player[i].countAnim += 0.33f;
				else
					g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;
					// パターンの切り替え
					//g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
					g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % texturePatternDivideX[g_Player[i].dir];
				}
			}
			else if (g_Player[i].jump == FALSE && g_Player[i].dir != CHAR_DIR_IDLE)
			{
				g_Player[i].dir = CHAR_DIR_IDLE;
				g_Player[i].texNo = g_Player[i].dir;
				g_Player[i].countAnim = 0.0f;
				g_Player[i].patternAnim = 0;
			}

			// キー入力で移動 
			{
				float speed = g_Player[i].move.x;

				g_Player[i].moving = FALSE;
				g_Player[i].dash = FALSE;



				/*	if (GetKeyboardPress(DIK_DOWN))
					{
						g_Player[i].pos.y += speed;
						g_Player[i].dir = CHAR_DIR_DOWN;
						g_Player[i].moving = TRUE;
					}
					else if (GetKeyboardPress(DIK_UP))
					{
						g_Player[i].pos.y -= speed;
						g_Player[i].dir = CHAR_DIR_UP;
						g_Player[i].moving = TRUE;
					}*/

				if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, BUTTON_RIGHT))
				{
					BOOL collided = FALSE;
					BOOL goal = FALSE;
					float height = g_Player[i].h;
					float y = g_Player[i].pos.y - height / 2.0f;
					while (height > 0)
					{
						int tileType = GetTileType(XMFLOAT3(g_Player[i].pos.x + g_Player[i].w / 4.0f, y, 0));
						if (tileType == GOAL)
							goal = TRUE;
						else if (tileType != AIR)
						{
							collided = TRUE;
							break;
						}
						height -= TILE_HEIGHT;
						y += TILE_HEIGHT;
					}
					int fCount = GetFrozenCount();
					for (int j = 0; j < fCount && !collided; j++)
					{
						BULLET* frozenB = GetFrozen()[j];
						if (CollisionBBRight(XMFLOAT3(g_Player[i].pos.x+ g_Player[i].w * 0.01f,g_Player[i].pos.y,0), g_Player[i].w * 0.5f, g_Player[i].h,
							frozenB->pos, frozenB->w, frozenB->h,
							g_Player[i].rot.z, frozenB->rot.z))
							collided = TRUE;

					}
					if (!collided && !goal)
					{
						g_Player[i].pos.x += speed;
						g_Player[i].dir = CHAR_DIR_RIGHT;
						g_Player[i].texNo = g_Player[i].dir;
						g_Player[i].moving = TRUE;
					}
					else if (!collided)
					{
						gameOverStatus = GAME_CLEAR;
						return;
					}
				}
				else if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_LEFT))
				{
					BOOL collided = FALSE;
					BOOL goal = FALSE;
					float height = g_Player[i].h;
					float y = g_Player[i].pos.y - height / 2.0f;
					while (height > 0)
					{
						int tileType = GetTileType(XMFLOAT3(g_Player[i].pos.x - g_Player[i].w / 4.0f, y, 0));
						if (tileType == GOAL)
							goal = TRUE;
						else if (tileType != AIR)
						{
							collided = TRUE;
							break;
						}
						height -= TILE_HEIGHT;
						y += TILE_HEIGHT;
					}
					int fCount = GetFrozenCount();
					for (int j = 0; j < fCount && !collided; j++)
					{
						BULLET* frozenB = GetFrozen()[j];
						if (CollisionBBRight(XMFLOAT3(g_Player[i].pos.x - g_Player[i].w * 0.05f, g_Player[i].pos.y, 0), g_Player[i].w * 0.25f, g_Player[i].h,
							frozenB->pos, frozenB->w, frozenB->h,
							g_Player[i].rot.z, frozenB->rot.z))
							collided = TRUE;

					}
					if (!collided && !goal)
					{
						g_Player[i].pos.x -= speed;
						g_Player[i].dir = CHAR_DIR_LEFT;
						g_Player[i].texNo = g_Player[i].dir;
						g_Player[i].moving = TRUE;
					}
					else if (!collided)
					{
						gameOverStatus = GAME_CLEAR;
						return;
					}
				}
				else if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0,BUTTON_START))
				{
					ToggleMenu();
				}
#ifdef _DEBUG
				//field edit debug
				if (GetKeyboardTrigger(DIK_F5))
				{
					SetMode(MODE_EDITOR);
				}
				else if (GetKeyboardPress(DIK_F3))
				{
					if (GetKeyboardTrigger(DIK_1))
					{
						SetField(1);
						SetMode(MODE_GAME);
						return;
					}
					else if (GetKeyboardTrigger(DIK_2))
					{
						SetField(2);
						SetMode(MODE_GAME);
						return;
					}
					else if (GetKeyboardTrigger(DIK_3))
					{
						SetField(3);
						SetMode(MODE_GAME);
						return;
					}
				}
#endif
				if (GetTileType(XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y + g_Player[i].h / 2.0f, 0)) == AIR &&
					g_Player[i].jump == FALSE)
				{
					g_Player[i].jump = TRUE;
					g_Player[i].jumpCnt = PLAYER_JUMP_CNT_MAX;
					g_Player[i].jumpY = 0;// g_Player[i].pos.y + PLAYER_JUMP_Y_MAX;
				}


				// ジャンプ処理中？
				if (g_Player[i].jump == TRUE)
				{

					float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player[i].jumpCnt;
					float y = g_Player[i].jumpYMax * cosf(XM_PI / 2 + angle);

					if (g_Player[i].jumpCnt <= PLAYER_JUMP_CNT_MAX / 2)
					{
						g_Player[i].pos.y += y - g_Player[i].jumpY;
						g_Player[i].jumpY = y;
						XMFLOAT3 pPos = XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y - g_Player[i].h / 2.0f, 0);
						int type = GetTileType(pPos);
						if (type != AIR)
						{
							if (type == GOAL)
							{
								gameOverStatus = GAME_CLEAR;
								return;
							}
							if (type != TYPE_ARROW)
								g_Player[i].pos.y = (int)(g_Player[i].pos.y / GetTile()->h) * GetTile()->h + g_Player[i].h / 2.0f;
							else
							{
								BULLET* bullet = GetBullet();
								for (int j = 0; j < BULLET_MAX; j++)
								{
									if (!bullet[j].use || !bullet[j].frozen) continue;
									else if (bullet[j].frozen)
										if ((bullet[j].rot.z == 0.0f || bullet[j].rot.z == 3.14f) &&
											CollisionBB(pPos, g_Player[i].w * 0.5f, 1.0f, bullet[j].pos, bullet[j].w * 0.5f, bullet[j].h))
											g_Player[i].pos.y = bullet[j].pos.y + bullet[j].w * 0.5f + g_Player[j].h * 0.5f;
										else if ((bullet[j].rot.z != 0.0f && bullet[j].rot.z != 3.14f) &&
											CollisionBB(pPos, g_Player[i].w * 0.5f, 1.0f, bullet[j].pos, bullet[j].h, bullet[j].w * 0.5f))
											g_Player[i].pos.y = bullet[j].pos.y + bullet[j].h * 0.5f + g_Player[j].h * 0.5f;
								}
							}
							g_Player[i].jumpCnt = PLAYER_JUMP_CNT_MAX;
							g_Player[i].jumpY = 0;
						}
					}
					else
					{
						g_Player[i].pos.y += g_Player[i].fallSpeed;
						if (g_Player[i].fallSpeed < PLAYER_MAX_FALL_SPEED)
							g_Player[i].fallSpeed += PLAYER_JUMP_GRAVITY;

					}

					XMFLOAT3 pPos = XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y + g_Player[i].h / 2.0f, 0);
					int type = GetTileType(pPos);
					if (g_Player[i].jumpCnt > PLAYER_JUMP_CNT_MAX / 2 &&
						(type == GROUND ||
							type == TYPE_ARROW ||
							type == GOAL))
					{
						if (type == GROUND)
							g_Player[i].pos.y = GetGroundBelow(g_Player[i].pos).y - g_Player[i].h / 2.0f;
						else if (type == GOAL)
						{
							gameOverStatus = GAME_CLEAR;
							return;
						}
						else
						{
							BULLET* bullet = GetBullet();
							for (int j = 0; j < BULLET_MAX; j++)
							{
								if (!bullet[j].use || !bullet[j].frozen) continue;
								else if (bullet[j].frozen &&
									CollisionBB(pPos, g_Player[i].w * 0.5f, 1.0f, bullet[j].pos, bullet[j].w * 0.33f, bullet[j].h))
									g_Player[i].pos.y = bullet[j].pos.y - bullet[j].w * 0.33f - g_Player[i].h * 0.5f;
							}
						}
						g_Player[i].jump = FALSE;
						g_Player[i].jumpCnt = 0;
						g_Player[i].jumpY = 0.0f;
						g_Player[i].fallSpeed = 0.0f;
					}
					else
					{
						if (g_Player[i].jumpCnt < PLAYER_JUMP_CNT_MAX)
							g_Player[i].jumpCnt++;
						if (g_Player[i].jumpCnt >= PLAYER_JUMP_CNT_MAX)
							g_Player[i].jumpY = 0;
					}

				}
				// ジャンプボタン押した？
				else if (g_Player[i].jump == FALSE && (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0,BUTTON_A)))
				{
					g_Player[i].jump = TRUE;
					g_Player[i].jumpCnt = 1;
					g_Player[i].jumpY = 0;// g_Player[i].pos.y;
				}

				if (!g_Player[i].moving && !g_Player[i].jump && !g_Player[i].freeze)
				{
					if (GetKeyboardTrigger(DIK_F) || IsButtonTriggered(0,BUTTON_X))
					{
						g_Player[i].freeze = TRUE;
						g_Player[i].freezePos = g_Player[i].pos;
						g_Player[i].freezeRadius = 1.0f / FREEZE_FRAME_COUNT;
						PlaySound(SOUND_LABEL_SE_FREEZE);
					}
				}
				else if (g_Player[i].freeze)
				{
					if (g_Player[i].freezeRadius < 1.0f)
						g_Player[i].freezeRadius += 1.0f / FREEZE_FRAME_COUNT;
					else
						g_Player[i].freeze = FALSE;

				}
				// MAP外チェック
				BG* bg = GetBG();

				if (g_Player[i].pos.x < g_Player[i].w / 2.0f)
				{
					g_Player[i].pos.x = g_Player[i].w / 2.0f;
				}

				else if (g_Player[i].pos.x > bg->w - (g_Player[i].w / 2.0f))
				{
					g_Player[i].pos.x = bg->w - (g_Player[i].w / 2.0f);
				}

				if (g_Player[i].pos.y < g_Player[i].h / 2.0f)
				{
					g_Player[i].pos.y = g_Player[i].h / 2.0f;
				}

				else if (g_Player[i].pos.y > bg->h - (g_Player[i].h / 2.0f))
				{
					g_Player[i].hp = 0.0f;
					gameOverStatus = GAME_OVER;
					return;
				}



				// プレイヤーの立ち位置からMAPのスクロール座標を計算する
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes
			if (GetMode() == MODE_GAME)
			{	// 影表示
				SetBlendState(BLEND_MODE_SUBTRACT);	// 減算合成

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SHADOW]);

				float px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				float py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				if (g_Player[i].jump == TRUE)
					py = GetGroundBelow(g_Player[i].pos).y /*+ GetField()->tile_h*/ - bg->pos.y;
				else
					py += g_Player[i].h / 2.0f;		// 足元に表示
				float pw = g_Player[i].w;		// プレイヤーの表示幅
				float ph = g_Player[i].h / 4;		// プレイヤーの表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す

			}

			if (g_Player[i].freeze)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[FREEZE]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = g_Player[i].freezePos.x - bg->pos.x;	// プレイヤーの表示位置X
				float py = g_Player[i].freezePos.y - bg->pos.y;	// プレイヤーの表示位置Y
				float pw = FREEZE_MAX_DIAMETER * g_Player[i].freezeRadius;	// プレイヤーの表示幅
				float ph = FREEZE_MAX_DIAMETER * g_Player[i].freezeRadius;		// プレイヤーの表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 0.45f),
					0.0f);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			//// プレイヤーの分身を描画
			//if (g_Player[i].dash)
			//{	// ダッシュ中だけ分身処理
			//	DrawPlayerOffset(i);
			//}
			if (g_Player[i].framesSinceHit > PLAYER_HIT_IFRAMES ||
				g_Player[i].framesSinceHit % 5 < 3)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
				float py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
				float pw = g_Player[i].w;		// プレイヤーの表示幅
				float ph = g_Player[i].h;		// プレイヤーの表示高さ

				//py += g_Player[i].jumpY;		// ジャンプ中の高さを足す

				// アニメーション用
				float tw = 1.0f / texturePatternDivideX[g_Player[i].dir];	// テクスチャの幅
				float th = 1.0f / texturePatternDivideY[g_Player[i].dir];	// テクスチャの高さ
				float tx = (float)(g_Player[i].patternAnim % texturePatternDivideX[g_Player[i].dir]) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_Player[i].patternAnim / texturePatternDivideX[g_Player[i].dir]) * th;	// テクスチャの左上Y座標

				//float tw = 1.0f;	// テクスチャの幅
				//float th = 1.0f;	// テクスチャの高さ
				//float tx = 0.0f;	// テクスチャの左上X座標
				//float ty = 0.0f;	// テクスチャの左上Y座標
				float alpha = GetMode() == MODE_GAME ? 1.0f : 0.5f;
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
					g_Player[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}
	}


}


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


// 生きてるエネミーの数
int GetPlayerCount(void)
{
	return g_PlayerCount;
}

void DamagePlayer(PLAYER* player)
{
	if (player->framesSinceHit < PLAYER_HIT_IFRAMES) return;
	player->hp -= 30.0f;
	if (player->hp < 0.0f)
		player->hp = 0.0f;
	player->framesSinceHit = 0;
	PlaySound(SOUND_LABEL_SE_HIT);

}


//=============================================================================
// プレイヤーの分身を描画
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[no].offset[j].x - bg->pos.x;	// プレイヤーの表示位置X
		float py = g_Player[no].offset[j].y - bg->pos.y;	// プレイヤーの表示位置Y
		float pw = g_Player[no].w;		// プレイヤーの表示幅
		float ph = g_Player[no].h;		// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f / texturePatternDivideX[g_Player[no].dir];	// テクスチャの幅
		float th = 1.0f / texturePatternDivideY[g_Player[no].dir];	// テクスチャの高さ
		float tx = (float)(g_Player[no].patternAnim % texturePatternDivideX[g_Player[no].dir]) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Player[no].patternAnim / texturePatternDivideX[g_Player[no].dir]) * th;	// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}

int GetGameOverStatus(void)
{
	return gameOverStatus;
}

