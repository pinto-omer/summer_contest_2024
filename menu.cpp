//=============================================================================
//
// �X�R�A���� [menu.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(250)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(14)		// �e�N�X�`���̐�

#define TEXTURE_WIDTH_SELECT		(50)	// �L�����T�C�Y
#define TEXTURE_HEIGHT_SELECT		(50)	// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX + 1] = { NULL };	// �e�N�X�`�����

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
static BOOL						isLevelSelect;						// true:�g���Ă���  false:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_Menu;					// �e�N�X�`���ԍ�
static int						g_MaxIndex;
static int						g_MenuIndex;					//
static BOOL						isSave;
static int						editorLevel = 0;
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




	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �v���C���[�̏�����
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
// �I������
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
// �X�V����
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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BG]);

	// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = 0;	// �X�R�A�̕\���ʒuX
	float py = 0;			// �X�R�A�̕\���ʒuY
	float pw = SCREEN_WIDTH;				// �X�R�A�̕\����
	float ph = SCREEN_HEIGHT;				// �X�R�A�̕\������

	float tw = 1.0f;		// �e�N�X�`���̕�
	float th = 1.0f;		// �e�N�X�`���̍���
	float tx = 0.0f;			// �e�N�X�`���̍���X���W
	float ty = 0.0f;			// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, GetMode() == MODE_GAME ? 0.55f : 0.25f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	for (int i = 0; i <= g_MaxIndex; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ModeMenus[g_Menu][i]]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		px = g_Pos.x;	// �X�R�A�̕\���ʒuX
		py = g_Pos.y + i * (g_h * 1.1f);			// �X�R�A�̕\���ʒuY
		pw = g_w;				// �X�R�A�̕\����
		ph = g_h;				// �X�R�A�̕\������

		tw = 1.0f;		// �e�N�X�`���̕�
		th = 1.0f;		// �e�N�X�`���̍���
		tx = 0.0f;			// �e�N�X�`���̍���X���W
		ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (g_ModeMenus[g_Menu][i] == TEST && GetFieldNum() == 0) ? 0.33f : 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
		if (g_MenuIndex == i)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX]);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			px = g_Pos.x - TEXTURE_WIDTH_SELECT * 1.1f;	// �X�R�A�̕\���ʒuX
			py = g_Pos.y + i * (g_h * 1.1f);			// �X�R�A�̕\���ʒuY
			pw = TEXTURE_WIDTH_SELECT;				// �X�R�A�̕\����
			ph = TEXTURE_HEIGHT_SELECT;				// �X�R�A�̕\������

			tw = 1.0f;		// �e�N�X�`���̕�
			th = 1.0f;		// �e�N�X�`���̍���
			tx = 0.0f;			// �e�N�X�`���̍���X���W
			ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


}




