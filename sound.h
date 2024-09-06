//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_TITLE,	// BGM0
	SOUND_LABEL_BGM_GAME,	// BGM1
	SOUND_LABEL_SE_HIT,		// 
	SOUND_LABEL_SE_FREEZE,		// 
	SOUND_LABEL_SE_PLACE,		// 
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

