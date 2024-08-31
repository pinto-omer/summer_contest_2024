//=============================================================================
//
// �o���b�g���� [tile.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(TILE_WIDTH)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(TILE_HEIGHT)	// 
#define TEXTURE_MAX					(TILE_MAX)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[] = {
	"",
	"data/TEXTURE/craftpix_free_green_zone/tiles/Tile_01.png",
	"data/TEXTURE/craftpix_free_green_zone/tiles/Tile_37.png",
};

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static TILE	g_Tile[TILE_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTile(void)
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateTile(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TILE_MAX; i++)
	{
		//// ��ʊO�܂Ői�񂾁H
		//BG* bg = GetBG();
		//if ((g_Tile[i].pos.y < (-g_Tile[i].h / 2)) ||			// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
		//	(g_Tile[i].pos.y > (bg->h + g_Tile[i].h / 2)) ||
		//	(g_Tile[i].pos.x < (-g_Tile[i].w / 2)) ||
		//	(g_Tile[i].pos.x > (bg->w + g_Tile[i].w / 2)))
		//{
		//	g_Tile[i].use = false;
		//	continue;
		//}
		//else
		//	g_Tile[i].use = true;

		// �A�j���[�V����  
		g_Tile[i].countAnim++;
		if ((g_Tile[i].countAnim % ANIM_WAIT) == 0)
		{
			// �p�^�[���̐؂�ւ�
			g_Tile[i].patternAnim = (g_Tile[i].patternAnim + 1) % ANIM_PATTERN_NUM;
		}

	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTile(int tileIDX, XMFLOAT3 pos, BOOL isEditTile, int varTileID)
{
	if (tileIDX < 0 || tileIDX >= TILE_MAX) return;
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

	

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Tile[tileIDX].texNo]);

	//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = pos.x;		// �o���b�g�̕\���ʒuX
	float py = pos.y;		// �o���b�g�̕\���ʒuY
	float pw = g_Tile[tileIDX].w;		// �o���b�g�̕\����
	float ph = g_Tile[tileIDX].h;		// �o���b�g�̕\������

	float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
	float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
	float tx = (float)(g_Tile[tileIDX].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
	float ty = (float)(g_Tile[tileIDX].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W
	float alpha = isEditTile == TRUE ? 0.7f : 1.0f;
	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
TILE* GetTile(void)
{
	return &g_Tile[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetTile(XMFLOAT3 pos)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	//for (int i = 0; i < TILE_MAX; i++)
	//{
	//	if (g_Tile[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
	//	{
	//		g_Tile[i].use = TRUE;			// �g�p��Ԃ֕ύX����
	//		g_Tile[i].pos = pos;			// ���W���Z�b�g
	//		return;							// 1���Z�b�g�����̂ŏI������
	//	}
	//}
}


