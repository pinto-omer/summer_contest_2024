//=============================================================================
//
// ファイル処理 [file.cpp]
// Author : GP11B132 31 PINTO OMER
//
//=============================================================================

/******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "file.h"
#include "score.h"
#include "field.h"
/*******************************************************************************
* マクロ定義
*******************************************************************************/



/*******************************************************************************
* 構造体定義
*******************************************************************************/



/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/



/*******************************************************************************
マクロ定義
*******************************************************************************/



/*******************************************************************************
* グローバル変数
*******************************************************************************/
SAVEDATA	save;		// セーブデータ作成場所
SAVEFIELD	sField;

/*******************************************************************************
関数名:	void SaveData( void )
引数:	void
戻り値:	void
説明:	セーブデータを作成し、ファイルへ出力する
*******************************************************************************/
void SaveData(void)
{
	{	// プレイヤーデータをセーブする
		PLAYER *player = GetPlayer();

		// プレイヤーの人数分セーブする
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			save.player[i] = player[i];
		}
	}

	{	// エネミーデータをセーブする
		ENEMY *enemy = GetEnemy();

		// エネミーの人数分セーブする
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			save.enemy[i] = enemy[i];
		}
	}


	// スコアデータをセーブする
	save.score = GetScore();


	// セーブデータのチェックサムを求める
	{
		char *adr = (char *)&save;	// デーブデータの先頭アドレスをadrに入れる
		int  sum = 0;				// 計算するチェックサム

		save.sum = 0;				// セーブデータ側のチェックサムを０でクリアしていく

		for (int i = 0; i < sizeof(SAVEDATA); i++)
		{
			sum += adr[i];
		}

		save.sum = sum;
	}



	// SAVEDATA構造体ごと全部をファイルに出力する
	FILE *fp;

	printf("\nセーブ開始・・・");
	fp = fopen("savedata.bin", "wb");			// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{	
		fwrite(&save, 1, sizeof(SAVEDATA), fp);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}

}


/*******************************************************************************
関数名:	void LoadData( void )
引数:	void
戻り値:	void
説明:	セーブデータをファイルから読み込む
*******************************************************************************/
void LoadData(void)
{
	PLAYER *player = GetPlayer();	// プレイヤーのアドレスを取得する
	ENEMY  *enemy  = GetEnemy();	// エネミーのアドレスを取得する

	// ファイルからセーブデータを読み込む
	FILE* fp;

	printf("\nロード開始・・・");
	fp = fopen("savedata.bin", "rb");	// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
	{
		fread(&save, 1, sizeof(SAVEDATA), fp);	// 指定したアドレスへ指定したバイト数分ファイルから読み込む
		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}


	// セーブデータのチェックサムが合っているか調べる
	{
		char *adr = (char *)&save;	// デーブデータの先頭アドレスをadrに入れる
		int  sum = 0;				// 計算するチェックサム
		int  org = save.sum;		// セーブデータ内のチェックサム

		save.sum = 0;				// セーブデータ側のチェックサムを０でクリアしていく

		for (int i = 0; i < sizeof(SAVEDATA); i++)
		{
			sum += adr[i];
		}

		// 元々のチェックサムと再計算したチェックサムが同じか調べている
		if (sum != org)
		{
			// データが改ざんされている！
			return;
		}
	}



	// プレイヤーの人数分、playerワークへ戻す
	{	// プレイヤーデータをロードする
		PLAYER *player = GetPlayer();

		// プレイヤーの人数分ロードする
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			player[i] = save.player[i];
		}
	}

	{	// エネミーデータをロードする
		ENEMY *enemy = GetEnemy();

		// エネミーの人数分ロードする
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			enemy[i] = save.enemy[i];
		}
	}

	// スコアデータをロードする
	SetScore(save.score);


}

void SaveField(int id)
{

	{	
		FIELD* field = GetField();
		memcpy(sField.field, field->field, sizeof(sField.field));
		memcpy(sField.varTilePos, field->varTilePos, sizeof(sField.varTilePos));
		memcpy(sField.varTiles, GetVarTile(), sizeof(sField.varTiles));
		sField.vartilesize = sizeof(VARTILE);
	}

	// セーブデータのチェックサムを求める
	{
		char* adr = (char*)&sField;	// デーブデータの先頭アドレスをadrに入れる
		int  sum = 0;				// 計算するチェックサム

		sField.sum = 0;				// セーブデータ側のチェックサムを０でクリアしていく

		for (int i = 0; i < sizeof(SAVEFIELD); i++)
		{
			sum += adr[i];
		}

		sField.sum = sum;
	}



	// SAVEDATA構造体ごと全部をファイルに出力する
	FILE* fp;
	static char fileName[12] = "field01.bin";
	fileName[6] = '0' + id;
	printf("\nセーブ開始・・・");
	fp = fopen(fileName, "wb");			// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{
		fwrite(&sField, 1, sizeof(SAVEFIELD), fp);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}

}

void LoadField(int id)
{

	// ファイルからセーブデータを読み込む
	FILE* fp;
	static char fileName[12] = "field01.bin";
	fileName[6] = '0' + id;
	printf("\nロード開始・・・");

	fp = fopen(fileName, "rb");	// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
	{
		fread(&sField, 1, sizeof(SAVEFIELD), fp);	// 指定したアドレスへ指定したバイト数分ファイルから読み込む
		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}


	// セーブデータのチェックサムが合っているか調べる
	{
		char* adr = (char*)&sField;	// デーブデータの先頭アドレスをadrに入れる
		int  sum = 0;				// 計算するチェックサム
		int  org = sField.sum;		// セーブデータ内のチェックサム

		sField.sum = 0;				// セーブデータ側のチェックサムを０でクリアしていく

		for (int i = 0; i < sizeof(SAVEFIELD); i++)
		{
			sum += adr[i];
		}

		// 元々のチェックサムと再計算したチェックサムが同じか調べている
#ifndef _DEBUG
		if (sum != org)
		{
			// データが改ざんされている！
			return;
		}
#endif
	}

	{	
		
		FIELD* field = GetField();
		memcpy(field->field, sField.field, sizeof(sField.field));
		memcpy(field->varTilePos, sField.varTilePos, sizeof(sField.varTilePos));
		// future proofing saves in case of adding more members to VARTILE
		if (sField.vartilesize < sizeof(VARTILE))
		{
			char* adr = (char*)GetVarTile();
			char* sAdr = (char*)sField.varTiles;
			for (int i = 0; i < MAX_VAR_TILES; i++)
			{
				for (int j = 0; j < sField.vartilesize; j++)
				{
					*(adr++) = *(sAdr++);
				}
				adr += sizeof(VARTILE) - sField.vartilesize;
			}
		}
		else
			memcpy(GetVarTile(), sField.varTiles, sizeof(sField.varTiles));
		FindLastVarTileIDX();
	}

	

}



