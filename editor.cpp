//=============================================================================
//
// �v���C���[���� [editor.cpp]
// Author : 
//
//=============================================================================
#include "editor.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "file.h"
#include "tile.h"
#include "field.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(TILE_MAX)		// �e�N�X�`���̐�

//#define TEXTURE_PATTERN_DIVIDE_X	(3)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
//#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
//#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define EDITOR_DISP_X				(SCREEN_WIDTH / 2)
# define EDITOR_DISP_Y				(SCREEN_HEIGHT / 2)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/border.png",
};



static int animPatternNum[TEXTURE_MAX] = { 0 };

static BOOL		g_Load = FALSE;				// ���������s�������̃t���O
static EDITOR	g_Editor;		// �v���C���[�\����



//=============================================================================
// ����������
//=============================================================================
HRESULT InitEditor(void)
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


	// �v���C���[�\���̂̏�����

	g_Editor.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
	g_Editor.w = TEXTURE_WIDTH;
	g_Editor.h = TEXTURE_HEIGHT;
	g_Editor.texNo = 0;

	g_Editor.countAnim = 0;
	g_Editor.patternAnim = 0;

	g_Editor.move = XMFLOAT3(TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f);		// �ړ���
	g_Editor.patternAnim = 0;// g_Editor.dir* TEXTURE_PATTERN_DIVIDE_X;






	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEditor(void)
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
// �X�V����
//=============================================================================
void UpdateEditor(void)
{
	if (g_Load == FALSE) return;

	XMFLOAT3 pos_old = g_Editor.pos;

	// �A�j���[�V����  

	/*g_Editor.countAnim += 1.0f;
	if (g_Editor.countAnim > ANIM_WAIT)
	{
		g_Editor.countAnim = 0.0f;

		g_Editor.patternAnim = (g_Editor.patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X;
	}*/



	// �L�[���͂ňړ� 
	{
		if (GetKeyboardPress(DIK_DOWN) || IsButtonPressed(0, BUTTON_DOWN))
		{
			g_Editor.pos.y += g_Editor.move.y;
		}
		else if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_UP))
		{
			g_Editor.pos.y -= g_Editor.move.y;
		}

		else if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT))
		{
			g_Editor.pos.x += g_Editor.move.x;
		}
		else if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_LEFT))
		{
			g_Editor.pos.x -= g_Editor.move.x;
		}

		//field edit debug
		else if (GetKeyboardPress(DIK_F2))
		{
			if (GetKeyboardTrigger(DIK_1))
				SaveField(1);
			else if (GetKeyboardTrigger(DIK_2))
				SaveField(2);
			else if (GetKeyboardTrigger(DIK_3))
				SaveField(3);
		}
		else if (GetKeyboardPress(DIK_F3))
		{
			if (GetKeyboardTrigger(DIK_1))
				LoadField(1);
			else if (GetKeyboardTrigger(DIK_2))
				LoadField(2);
			else if (GetKeyboardTrigger(DIK_3))
				LoadField(3);
		}







		// MAP�O�`�F�b�N
		BG* bg = GetBG();

		if (g_Editor.pos.x < 0)
		{
			g_Editor.pos.x = 0;
		}

		else if (g_Editor.pos.x > bg->w - (g_Editor.w))
		{
			g_Editor.pos.x = bg->w - (g_Editor.w);
		}

		if (g_Editor.pos.y < 0)
		{
			g_Editor.pos.y = 0;
		}

		else if (g_Editor.pos.y > bg->h - (g_Editor.h))
		{
			g_Editor.pos.y = bg->h - (g_Editor.h);
		}



		// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
		bg->pos.x = g_Editor.pos.x - EDITOR_DISP_X;
		if (bg->pos.x < 0) bg->pos.x = 0;
		if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

		bg->pos.y = g_Editor.pos.y - EDITOR_DISP_Y;
		if (bg->pos.y < 0) bg->pos.y = 0;
		if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;




	}

#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEditor(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Editor.texNo]);

	//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Editor.pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
	float py = g_Editor.pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
	float pw = g_Editor.w;		// �v���C���[�̕\����
	float ph = g_Editor.h;		// �v���C���[�̕\������

	//py += g_Editor.jumpY;		// �W�����v���̍����𑫂�



	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}


//=============================================================================
// Editor�\���̂̐擪�A�h���X���擾
//=============================================================================
EDITOR* GetEditor(void)
{
	return &g_Editor;
}


