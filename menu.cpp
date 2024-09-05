//=============================================================================
//
// �X�R�A���� [menu.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "menu.h"
#include "sprite.h"
#include "player.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(11)		// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[] = {
	"data/TEXTURE/back.png",
	"data/TEXTURE/exit.png",
	"data/TEXTURE/leveleditor.png",
	"data/TEXTURE/load.png",
	"data/TEXTURE/mainmenu.png",
	"data/TEXTURE/plevel1.png",
	"data/TEXTURE/plevel2.png",
	"data/TEXTURE/plevel3.png",
	"data/TEXTURE/resume.png",
	"data/TEXTURE/save.png",
	"data/TEXTURE/select.png",
	"data/TEXTURE/start.png",
};


static bool						g_Use;						// true:�g���Ă���  false:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Menu;					// �X�R�A

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMenu(void)
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


	// �v���C���[�̏�����
	g_Use = true;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { SCREEN_WIDTH - TEXTURE_WIDTH * 0.5f - 20.0f, TEXTURE_HEIGHT * 0.5f + 20.0f, 0.0f };
	g_TexNo = 0;

	g_Menu = 0;	// �X�R�A�̏�����

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMenu(void)
{
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
void UpdateMenu(void)
{


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMenu(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Pos.x;	// �X�R�A�̕\���ʒuX
	float py = g_Pos.y;			// �X�R�A�̕\���ʒuY
	float pw = g_w;				// �X�R�A�̕\����
	float ph = g_h;				// �X�R�A�̕\������

	float tw = 1.0f;		// �e�N�X�`���̕�
	float th = 1.0f;		// �e�N�X�`���̍���
	float tx = 0.0f;			// �e�N�X�`���̍���X���W
	float ty = 0.0f;			// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(0.67f, 0.0f, 0.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	float percent = GetPlayer()->hp / PLAYER_MAX_HP;
	// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_Pos.x;	// �X�R�A�̕\���ʒuX
	py = g_Pos.y;			// �X�R�A�̕\���ʒuY
	pw = g_w * percent;				// �X�R�A�̕\����
	ph = g_h * percent;				// �X�R�A�̕\������

	tw = 1.0f;		// �e�N�X�`���̕�
	th = 1.0f;		// �e�N�X�`���̍���
	tx = 0.0f;			// �e�N�X�`���̍���X���W
	ty = 0.0f;			// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(0.0f, 0.67f, 0.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}




