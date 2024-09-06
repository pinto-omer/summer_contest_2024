//=============================================================================
//
// �t�@�C������ [file.h]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================
#pragma once

#include "main.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "field.h"
#include "variabletile.h"

/*******************************************************************************
* �}�N����`
*******************************************************************************/



/*******************************************************************************
* �\���̒�`
*******************************************************************************/
struct	SAVEDATA				// �Z�[�u�f�[�^�̍\����
{
	// �v���C���[�f�[�^��ۑ�
	PLAYER	player[PLAYER_MAX];	// �v���C���[��MAX�l�����Z�[�u����

	// �G�l�~�[�f�[�^��ۑ�
	ENEMY	enemy[ENEMY_MAX];	// �G�l�~�[��MAX�l�����Z�[�u����

	// �X�R�A�f�[�^��ۑ�
	int		score;				// �X�R�A��ۑ�����

	// ���ɂ������Z�[�u��������������΂����ɒǉ�

	int		sum;				// �`�F�b�N�T��������
};

struct SAVEFIELD
{
	int			sum;
	int			vartilesize;
	int			field[FIELD_TILE_H][FIELD_TILE_W];
	XMINT2		varTilePos[MAX_VAR_TILES];
	VARTILE		varTiles[MAX_VAR_TILES];
};

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/
void SaveData(void);
void LoadData(void);

void SaveField(int id);
void LoadField(int id);

