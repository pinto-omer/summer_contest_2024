//=============================================================================
//
// �o���b�g���� [bullet.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_HEIGHT				(BULLET_HEIGHT)	// 
#define TEXTURE_WIDTH				(BULLET_WIDTH)	// �L�����T�C�Y
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�
#define TIP_SIZE					(TEXTURE_HEIGHT / 5)
#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void AddFrozen(BULLET* bullet);
void DelFrozen(BULLET* bullet);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[] = {
	"data/TEXTURE/arrow.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLET	g_Bullet[BULLET_MAX];	// �o���b�g�\����
static BULLET* g_Frozen[BULLET_MAX];
static int		g_FrozenCount;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Bullet[i].w = TEXTURE_WIDTH;
		g_Bullet[i].h = TEXTURE_HEIGHT;
		g_Bullet[i].pos = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo = 0;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;
		g_Bullet[i].frozen = FALSE;
		g_Bullet[i].freezeRemaining = 0;
		g_Bullet[i].move = XMFLOAT3(BULLET_SPEED, BULLET_SPEED, 0.0f);	// �ړ��ʂ�������
	}
	g_FrozenCount = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateBullet(void)
{
	if (g_Load == FALSE) return;
	int bulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			if (!g_Bullet[i].frozen)
			{
				// �A�j���[�V����  
				g_Bullet[i].countAnim++;
				if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
				{
					// �p�^�[���̐؂�ւ�
					g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

				// �o���b�g�̈ړ�����
				XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
				XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
				pos += move;
				XMStoreFloat3(&g_Bullet[i].pos, pos);

				// ��ʊO�܂Ői�񂾁H
				BG* bg = GetBG();
				if (g_Bullet[i].pos.y < (-g_Bullet[i].h / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_Bullet[i].use = FALSE;
				}
				if (g_Bullet[i].pos.y > (bg->h + g_Bullet[i].h / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_Bullet[i].use = FALSE;
				}

				// �����蔻�菈��

				{
					//ENEMY* enemy = GetEnemy();
					//

					//// �G�l�~�[�̐��������蔻����s��
					//for (int j = 0; j < ENEMY_MAX; j++)
					//{
					//	// �����Ă�G�l�~�[�Ɠ����蔻�������
					//	if (enemy[j].use == TRUE)
					//	{
					//		BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
					//			enemy[j].pos, enemy[j].w, enemy[j].h);
					//		// �������Ă���H
					//		if (ans == TRUE)
					//		{
					//			// �����������̏���
					//			enemy[j].use = FALSE;
					//			AddScore(100);

					//			// �G�t�F�N�g����
					//			SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);
					//		}
					//	}
					//}

					// �t�B�[���h�Ƃ̓����蔻��
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

					// �v���C���[�Ƃ̓����蔻��
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
// �`�揈��
//=============================================================================
void DrawBullet(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Bullet[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Bullet[i].w;		// �o���b�g�̕\����
			float ph = g_Bullet[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			if (g_Bullet[i].frozen)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

				//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Bullet[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
				float py = g_Bullet[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
				float pw = g_Bullet[i].w;		// �o���b�g�̕\����
				float ph = g_Bullet[i].h;		// �o���b�g�̕\������

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(0.0f, 0.0f, 1.0f, (float)g_Bullet[i].freezeRemaining / FREEZE_DURATION),
					g_Bullet[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET* GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
BULLET* SetBullet(XMFLOAT3 pos)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Bullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Bullet[i].pos = pos;			// ���W���Z�b�g
			return &g_Bullet[i];							// 1���Z�b�g�����̂ŏI������
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
