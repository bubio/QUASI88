/***********************************************************************
 * アラーム
 ************************************************************************/
#include <stdio.h>
#include <string.h>

#include "q8tk.h"
#include "alarm.h"
#include "wait.h"

/***********************************************************************
 * ワーク
 ************************************************************************/
typedef struct {
	void	*alarm_id;
	void	*parm;
	void	(*callback)(void *id, void *parm);
	int		timer_ms;
	int		is_lock;
} t_alarm;

#define	MAX_ALARM		(16)

static t_alarm alarm_wk[ MAX_ALARM ];
static union wait_time alarm_lapse;


/***********************************************************************
 * 関数
 ************************************************************************/

/*------------------------------------------------------------------------
 * イニシャル (一度だけ呼び出される)
 *------------------------------------------------------------------------*/
void alarm_init(void)
{
	memset(&alarm_wk, 0, sizeof(alarm_wk));

	wait_get_current_time(&alarm_lapse);
}

/*------------------------------------------------------------------------
 * アラームの起点タイマー再イニシャル
 *------------------------------------------------------------------------*/
void alarm_reset(void)
{
	wait_get_current_time(&alarm_lapse);
}

/*------------------------------------------------------------------------
 * アラームの登録
 *
 * alarm_id	アラームを識別するID (NULL以外の任意のポインタ)
 *          アラームのタイムアップ時に呼び出される関数の、第一引数にもなる
 * parm     アラームのタイムアップ時に呼び出される関数の、第二引数
 * callback アラームのタイムアップ時に呼び出される関数へのポインタ
 * timer_ms タイムアップするまでの時間(ms)
 * is_lock  アラーム動作中はロック状態とさせたい場合、真をセットする
 *          この値が真であるアラームが動作中は、alarm_is_lock() 関数が真を返す
 *------------------------------------------------------------------------*/
void alarm_add(void *alarm_id,
			   void *parm,
			   void (*callback)(void *alarm_id, void *parm),
			   int timer_ms,
			   int is_lock)
{
	int i;
	if (alarm_id) {
		for (i = 0; i < MAX_ALARM; i++) {
			if (alarm_wk[i].alarm_id == NULL) {
				alarm_wk[i].alarm_id = alarm_id;
				alarm_wk[i].parm = parm;
				alarm_wk[i].callback = callback;
				alarm_wk[i].timer_ms = timer_ms;
				alarm_wk[i].is_lock = is_lock;
				return;
			}
		}
	}

	fprintf(stderr, "invalid arg or work exhoused\n");
}

/*------------------------------------------------------------------------
 * アラームの削除
 *
 * alarm 削除するアラームの識別ID
 *------------------------------------------------------------------------*/
void alarm_remove(void *alarm_id)
{
	int i;
	if (alarm_id) {
		for (i = 0; i < MAX_ALARM; i++) {
			if (alarm_wk[i].alarm_id == alarm_id) {
				alarm_wk[i].alarm_id = NULL;
			}
		}
	}
}

/*------------------------------------------------------------------------
 * 時間経過処理
 * タイムアップしたらコールバック関数を呼び出し、テーブルから削除する
 *------------------------------------------------------------------------*/
void alarm_update(void)
{
	int i;
	int time_elapsed_ms = wait_calc_elasped_time_ms(&alarm_lapse);

	if (time_elapsed_ms > 0) {

		wait_get_current_time(&alarm_lapse);

		/* 登録されたタイマー一覧より、経過時間(ms)分を順次減らす */
		for (i = 0; i < MAX_ALARM; i++) {
			if (alarm_wk[i].alarm_id) {
				alarm_wk[i].timer_ms -= time_elapsed_ms;

				if (alarm_wk[i].timer_ms <= 0) {
					/* タイムアップしたら関数を呼び出し、タイマーは削除 */
					void *alarm_id = alarm_wk[i].alarm_id;
					void *parm     = alarm_wk[i].parm;

					alarm_wk[i].alarm_id = NULL;
					(*alarm_wk[i].callback)(alarm_id, parm);
				}
				/* タイムアップしてなければ、継続 */
			}
		}
	}
}

/*------------------------------------------------------------------------
 * ロック状態のアラームが動作中であれば、真を返す
 * (Q8tk で、アラームが終了するまでイベントを受け付けたくない時などに利用)
 *------------------------------------------------------------------------*/
int alarm_is_lock(void)
{
	int i;
	for (i = 0; i < MAX_ALARM; i++) {
		if (alarm_wk[i].alarm_id) {
			if (alarm_wk[i].is_lock) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*------------------------------------------------------------------------
 * 終了
 *------------------------------------------------------------------------*/
void alarm_exit(void)
{
}
