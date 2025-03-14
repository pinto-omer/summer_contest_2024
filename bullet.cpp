//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#include "bullet.h"
#include "player.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"
#include "field.h"
#include "tile.h"
#include "variabletile.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_HEIGHT				(BULLET_HEIGHT)	// 
#define TEXTURE_WIDTH				(BULLET_WIDTH)	// キャラサイズ
#define TEXTURE_MAX					(1)		// テクスチャの数
#define TIP_SIZE					(TEXTURE_HEIGHT / 5)
#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void AddFrozen(BULLET* bullet);
void DelFrozen(BULLET* bullet);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[] = {
	"data/TEXTURE/arrow.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLET	g_Bullet[BULLET_MAX];	// バレット構造体
static BULLET* g_Frozen[BULLET_MAX];
static int		g_FrozenCount;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
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
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE;			// 未使用（発射されていない弾）
		g_Bullet[i].w = TEXTURE_WIDTH;
		g_Bullet[i].h = TEXTURE_HEIGHT;
		g_Bullet[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo = 0;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;
		g_Bullet[i].frozen = FALSE;
		g_Bullet[i].freezeRemaining = 0;
		g_Bullet[i].move = XMFLOAT3(BULLET_SPEED, BULLET_SPEED, 0.0f);	// 移動量を初期化
	}
	g_FrozenCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	if (g_Load == FALSE) return;
	int bulletCount = 0;				// 処理したバレットの数

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			if (!g_Bullet[i].frozen)
			{
				// アニメーション  
				g_Bullet[i].countAnim++;
				if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
				{
					// パターンの切り替え
					g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

				// バレットの移動処理
				XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
				XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
				pos += move;
				XMStoreFloat3(&g_Bullet[i].pos, pos);

				// 画面外まで進んだ？
				BG* bg = GetBG();
				if (g_Bullet[i].pos.y < (-g_Bullet[i].h / 2))		// 自分の大きさを考慮して画面外か判定している
				{
					g_Bullet[i].use = FALSE;
				}
				if (g_Bullet[i].pos.y > (bg->h + g_Bullet[i].h / 2))	// 自分の大きさを考慮して画面外か判定している
				{
					g_Bullet[i].use = FALSE;
				}

				// 当たり判定処理

				{
					//ENEMY* enemy = GetEnemy();
					//

					//// エネミーの数分当たり判定を行う
					//for (int j = 0; j < ENEMY_MAX; j++)
					//{
					//	// 生きてるエネミーと当たり判定をする
					//	if (enemy[j].use == TRUE)
					//	{
					//		BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
					//			enemy[j].pos, enemy[j].w, enemy[j].h);
					//		// 当たっている？
					//		if (ans == TRUE)
					//		{
					//			// 当たった時の処理
					//			enemy[j].use = FALSE;
					//			AddScore(100);

					//			// エフェクト発生
					//			SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
					//		}
					//	}
					//}

					// フィールドとの当たり判定
					FIELD* field = GetField();
					{

						int x = (int)(g_Bullet[i].pos.x / field->tile_w);
						int y = (int)(g_Bullet[i].pos.y / field->tile_h);
						if (g_Bullet[i].move.x != 0)
							x += g_Bullet[i].move.x > 0 ? 1 : -1;
						else
							y += g_Bullet[i].move.y > 0 ? 1 : -1;
						if (x < 0 || x >= FIELD_TILE_W || y < 0 || y > FIELD_TILE_H)
						{
							g_Bullet[i].use = FALSE;
							continue;
						}
						TILE tile = GetTile()[field->field[y][x]];
						if ((tile.type != AIR && tile.type != GOAL) &&
							CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
								XMFLOAT3(x * TILE_WIDTH + tile.w / 2.0f, y * TILE_HEIGHT + tile.h / 2.0f, 0.0f), tile.w, tile.h) == TRUE)
						{
							g_Bullet[i].use = FALSE;
							continue;
						}
					}

					// プレイヤーとの当たり判定
					if (g_Bullet[i].use)
					{
						PLAYER* player = GetPlayer();
						if (player->freeze && CollisionBC(player->freezePos, g_Bullet[i].pos, FREEZE_MAX_DIAMETER / 2.0f * player->freezeRadius, g_Bullet[i].h / 2.0f))
						{
							g_Bullet[i].frozen = TRUE;
							g_Bullet[i].freezeRemaining = FREEZE_DURATION;
							AddFrozen(&g_Bullet[i]);
							/*float y1, y2, x1, x2;
							y1 = g_Bullet[i].pos.y + 0.5f * (g_Bullet[i].rot.z == 0 || g_Bullet[i].rot.z == 3.14f ? g_Bullet[i].h : g_Bullet[i].w);
							y2 = g_Bullet[i].pos.y - 0.5f * (g_Bullet[i].rot.z == 0 || g_Bullet[i].rot.z == 3.14f ? g_Bullet[i].h : g_Bullet[i].w);
							x1 = g_Bullet[i].pos.x + 0.5f * (g_Bullet[i].rot.z == 0 || g_Bullet[i].rot.z == 3.14f ? g_Bullet[i].w : g_Bullet[i].h);
							x2 = g_Bullet[i].pos.x - 0.5f * (g_Bullet[i].rot.z == 0 || g_Bullet[i].rot.z == 3.14f ? g_Bullet[i].w : g_Bullet[i].h);
							if (GetField()->field[(int)(y1 / TILE_HEIGHT)][(int)(x1 / TILE_WIDTH)] == TILE_EMPTY)
								GetField()->field[(int)(y1 / TILE_HEIGHT)][(int)(x1 / TILE_WIDTH)] = TILE_FROZEN_ARROW;
							if (GetField()->field[(int)(y2 / TILE_HEIGHT)][(int)(x2 / TILE_WIDTH)] == TILE_EMPTY)
								GetField()->field[(int)(y2 / TILE_HEIGHT)][(int)(x2 / TILE_WIDTH)] = TILE_FROZEN_ARROW;*/
							continue;
						}
						float tipAdjust = (g_Bullet[i].h - TIP_SIZE) * 0.5f;
						XMFLOAT3 tipPos = g_Bullet[i].pos;
						if (g_Bullet[i].move.x != 0)
						{
							tipAdjust *= g_Bullet[i].move.x > 0 ? 1 : -1;
							tipPos.x += tipAdjust;

							if (CollisionBB(g_Bullet[i].pos, TIP_SIZE * 0.5f, g_Bullet[i].w * 0.8f,
								player->pos, player->w * 0.5f, player->h) == TRUE)
							{
								g_Bullet[i].use = FALSE;
								DamagePlayer(player);
							}
						}
						else
						{
							tipAdjust *= g_Bullet[i].move.y > 0 ? 1 : -1;
							tipPos.y += tipAdjust;
							if (CollisionBB(tipPos, g_Bullet[i].w * 0.8f, TIP_SIZE * 0.5f,
								player->pos, player->w * 0.5f, player->h * 0.5f) == TRUE)
							{
								g_Bullet[i].use = FALSE;
								DamagePlayer(player);
							}
						}
					}


				}
			}
			else if (--g_Bullet[i].freezeRemaining == 0)
			{
				DelFrozen(&g_Bullet[i]);
				/*float y1, y2, x1, x2;
				y1 = g_Bullet[i].pos.y + 0.5f * (g_Bullet[i].rot.z == 0 || g_Bullet[i].rot.z == 3.14f ? g_Bullet[i].h : g_Bullet[i].w);
				y2 = g_Bullet[i].pos.y - 0.5f * (g_Bullet[i].rot.z == 0 || g_Bullet[i].rot.z == 3.14f ? g_Bullet[i].h : g_Bullet[i].w);
				x1 = g_Bullet[i].pos.x + 0.5f * (g_Bullet[i].rot.z == 0 || g_Bullet[i].rot.z == 3.14f ? g_Bullet[i].w : g_Bullet[i].h);
				x2 = g_Bullet[i].pos.x - 0.5f * (g_Bullet[i].rot.z == 0 || g_Bullet[i].rot.z == 3.14f ? g_Bullet[i].w : g_Bullet[i].h);
				if (GetField()->field[(int)(y1 / TILE_HEIGHT)][(int)(x1 / TILE_WIDTH)] == TILE_FROZEN_ARROW)
					GetField()->field[(int)(y1 / TILE_HEIGHT)][(int)(x1 / TILE_WIDTH)] = TILE_EMPTY;
				if (GetField()->field[(int)(y2 / TILE_HEIGHT)][(int)(x2 / TILE_WIDTH)] == TILE_FROZEN_ARROW)
					GetField()->field[(int)(y2 / TILE_HEIGHT)][(int)(x2 / TILE_WIDTH)] = TILE_EMPTY;*/
			}

			bulletCount++;
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - bg->pos.x;	// バレットの表示位置X
			float py = g_Bullet[i].pos.y - bg->pos.y;	// バレットの表示位置Y
			float pw = g_Bullet[i].w;		// バレットの表示幅
			float ph = g_Bullet[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			if (g_Bullet[i].frozen)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

				//バレットの位置やテクスチャー座標を反映
				float px = g_Bullet[i].pos.x - bg->pos.x;	// バレットの表示位置X
				float py = g_Bullet[i].pos.y - bg->pos.y;	// バレットの表示位置Y
				float pw = g_Bullet[i].w;		// バレットの表示幅
				float ph = g_Bullet[i].h;		// バレットの表示高さ

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
				float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(0.0f, 0.0f, 1.0f, (float)g_Bullet[i].freezeRemaining / FREEZE_DURATION),
					g_Bullet[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET* GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
BULLET* SetBullet(XMFLOAT3 pos)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bullet[i].use = TRUE;			// 使用状態へ変更する
			g_Bullet[i].pos = pos;			// 座標をセット
			return &g_Bullet[i];							// 1発セットしたので終了する
		}
	}
	return NULL;
}

BULLET** GetFrozen(void)
{
	return g_Frozen;
}

int GetFrozenCount(void)
{
	return g_FrozenCount;
}

void DelAllFrozen(void)
{
	while (g_FrozenCount > 0)
		DelFrozen(g_Frozen[0]);
}

void AddFrozen(BULLET* bullet)
{
	g_Frozen[g_FrozenCount++] = bullet;
}

void DelFrozen(BULLET* bullet)
{
	BOOL removed = FALSE;
	for (int i = 0; i < g_FrozenCount; i++)
	{
		if (!removed && g_Frozen[i] == bullet)
		{
			g_Frozen[i]->frozen = FALSE;
			g_Frozen[i] = NULL;
			removed = TRUE;
		}
		if (removed && i < BULLET_MAX)
		{
			g_Frozen[i] = g_Frozen[i + 1];
		}
	}
	g_FrozenCount--;
}
