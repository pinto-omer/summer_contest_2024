//=============================================================================
//
// Field���� [bg.cpp]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#include "field.h"
#include "tile.h"
#include "bg.h"
#include "file.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// �w�i�T�C�Y
//#define TEXTURE_HEIGHT				(1080)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(3)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/map.png",
	"data/TEXTURE/sky000.jpg",
	"data/TEXTURE/sky001.jpg",
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static FIELD	g_Field;
static int fieldNum = 0;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitField(void)
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

	//memcpy(g_Field.field, field1, sizeof(g_Field.field));
	// �ϐ��̏�����
	for (int i = 0; i < MAX_VAR_TILES; i++)
		g_Field.varTilePos[i] = XMINT2(-1, -1);
	
	g_Field.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.texNo = 0;

	g_Field.scrl = 0.0f;		// TEX�X�N���[��
	g_Field.scrl2 = 0.0f;		// TEX�X�N���[��
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateField(void)
{
	//g_Field.old_pos = g_Field.pos;	// �P�t���O�̏���ۑ�


	//g_Field.scrl -= 0.0f;		// 0.005f;		// �X�N���[��

	UpdateTile();
	UpdateVariableTile();
#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawField(void)
{
	BG* bg = GetBG();
	TILE* tiles = GetTile();
	for (int i = 0; i < FIELD_TILE_H; i++)
	{
		for (int j = 0; j < FIELD_TILE_W; j++)
		{
			// ��ʊO�܂Ői�񂾁H
			float w, h;
			h = tiles[g_Field.field[i][j]].h;
			w = tiles[g_Field.field[i][j]].w;
			XMFLOAT3 pos = XMFLOAT3(j * w + w / 2.0f - bg->pos.x,
				i * h + h / 2.0f - bg->pos.y,
				0);
			if ((pos.y < (-h / 2.0f)) ||			// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
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
// Field�\���̂̐擪�A�h���X���擾
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



