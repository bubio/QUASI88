/***********************************************************************
 * デバッグ用
 ************************************************************************/
#include <stdio.h>

#include "quasi88.h"
#include "debug.h"

int show_statistics;

/*----------------------------------------------------------------------
 * デバッグ用 printf
 *----------------------------------------------------------------------*/
#ifdef  DEBUGPRINTF
#include <stdarg.h>
void    debugprintf(const char *format, ...)
{
	va_list  list;

	va_start(list, format);
	vfprintf(stdout, format, list);
	va_end(list);

	fflush(stdout);
}
#endif

/*----------------------------------------------------------------------
 * デバッグ用 ログ
 *----------------------------------------------------------------------*/
/* bit0 が 1 なら、標準出力、 bit1 が 1 なら "quasi88.log" に出力 */
int pio_debug = 0;
int fdc_debug = 0;
int main_debug = 0;
int sub_debug = 0;


#ifdef DEBUGLOG
#include <stdarg.h>
static FILE *LOG = NULL;
static FILE *debuglog_fp[2];

void debuglog_init(void)
{
	LOG = fopen("quasi88.log", "w");

	debuglog_fp[0] = stdout;
	debuglog_fp[1] = LOG;

	if (verbose_proc) {
		printf("+ Support debug logging.\n");
	}
}

void debuglog_sync(void)
{
	fflush(LOG);
}

void debuglog_exit(void)
{
	fclose(LOG);
}

static void logprintf(int output_flag, const char *format, va_list list)
{
	int i;
	for (i = 0; i < COUNTOF(debuglog_fp); i++) {
		if (output_flag & (1 << i)) {
			vfprintf(debuglog_fp[i], format, list);
		}
	}
}

void logpio(const char *format, ...)
{
	va_list  list;
	va_start(list, format);
	logprintf(pio_debug, format, list);
	va_end(list);
}

void logfdc(const char *format, ...)
{
	va_list  list;
	va_start(list, format);
	logprintf(fdc_debug, format, list);
	va_end(list);
}

static int z80_debug_wk;
void logz80_target(int debug_flag)
{
	z80_debug_wk = debug_flag;
}
void logz80(const char *format, ...)
{
	va_list  list;
	va_start(list, format);
	logprintf(z80_debug_wk, format, list);
	va_end(list);
}
#endif

/*----------------------------------------------------------------------
 * 処理時間 区間計測
 *----------------------------------------------------------------------*/
int debug_profiler;						/* bit0: 区間ラップをファイル出力
										 * bit1: 1秒毎に描画状況を表示
										 */
#define FILE_PROFILER			"quasi88.lap"
char *file_profiler;

#ifdef PROFILER
#if defined(HAVE_GETTIMEOFDAY) || defined(QUASI88_SDL2) || defined(QUASI88_SDL)

#include "initval.h"
#include "intr.h"

#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>			/* gettimeofday */
#define TIMEVAL					struct timeval
#define TIMEVAL_RESO_MS			(double)(1000.0/1000000)
#define TIMEVAL_NOW(a)			gettimeofday(& a, 0);
#define	TIMEVAL_ADD(a, b, c)	timeradd(& a, & b, & c)
#define	TIMEVAL_SUB(a, b, c)	timersub(& a, & b, & c)
#define	TIMEVAL_GT(a, b)		timercmp(& a, & b, >)
#define	TIMEVAL_CLEAR(a)		timerclear(& a)
#define	TIMEVAL_ISSET(a)		timerisset(& a)
#define	TIMEVAL_TO_MS(a)		((double)(a.tv_sec*1000000+a.tv_usec)/1000.0)
#endif

#ifdef QUASI88_SDL2
#include <SDL.h>
#define TIMEVAL					Uint64
#define TIMEVAL_RESO_MS			(double)(1000.0/SDL_GetPerformanceCounter())
#define TIMEVAL_NOW(a)			a = SDL_GetPerformanceCounter()
#define	TIMEVAL_ADD(a, b, c)	c = a + b
#define	TIMEVAL_SUB(a, b, c)	c = a - b
#define	TIMEVAL_GT(a, b)		a > b
#define	TIMEVAL_CLEAR(a)		a = 0
#define	TIMEVAL_ISSET(a)		a != 0
#define	TIMEVAL_TO_MS(a)		((double)(a*1000)/SDL_GetPerformanceFrequency())
#endif

#ifdef QUASI88_SDL
#include <SDL.h>
#define TIMEVAL					Uint32
#define TIMEVAL_RESO_MS			(double)(1000.0/1000)
#define TIMEVAL_NOW(a)			a = SDL_GetTicks()
#define	TIMEVAL_ADD(a, b, c)	c = a + b
#define	TIMEVAL_SUB(a, b, c)	c = a - b
#define	TIMEVAL_GT(a, b)		a > b
#define	TIMEVAL_CLEAR(a)		a = 0
#define	TIMEVAL_ISSET(a)		a != 0
#define	TIMEVAL_TO_MS(a)		((double)(a))
#endif

static int prof_exec;
static const char *prof_label[PROF_LAPSE_END] = {
	"SKIP", "START", "EVENT", "UI", "INPUT", "CPU",
	"PCM", "AUDIO", "BLIT", "VIDEO", "IDLE",
};
static FILE           *prof_lap_fp;
static int prof_type;
static TIMEVAL prof_list[ PROF_LAPSE_END ];
static TIMEVAL whole_tv;
static TIMEVAL parts_tv;
static struct {
	TIMEVAL max;						/* 最大時間 */
	TIMEVAL total;						/* 累計時間 */
	int     count;						/* 累計回数 */
} prof_lap[ PROF_LAPSE_END ];

void profiler_init(void)
{
	int i;

	if (verbose_proc) {
		printf("+ Support profiler logging.\n");
	}

	if (debug_profiler & 1) {
		prof_exec = TRUE;

		if (file_profiler == NULL) {
			file_profiler = FILE_PROFILER;
		}

		printf("Start profiling ... \"%s\" (freq = %.9f[ms], vsync = %f[ms])\n",
			   file_profiler,
			   TIMEVAL_RESO_MS,
			   (double)(1000.0 / (CONST_VSYNC_FREQ * wait_rate / 100)));

		prof_lap_fp = fopen(file_profiler, "w");

		if (prof_lap_fp == NULL) {
			fprintf(stderr, "profiling output file \"%s\" open failed.\n", file_profiler);

		} else {
			for (i = 1; i < PROF_LAPSE_END; i++) {
				fprintf(prof_lap_fp, "%s,", prof_label[i]);
			}
			fprintf(prof_lap_fp, "\n");
		}
	}
}

void profiler_lapse(int type)
{
	TIMEVAL now_tv, diff_tv;
	int i;
	if (prof_exec == FALSE) {
		return;
	}

	TIMEVAL_NOW(now_tv);

	/* 前回 profiler_lapse(...) 呼び出し時からの経過時間 */
	if (TIMEVAL_ISSET(parts_tv)) {
		TIMEVAL_SUB(now_tv, parts_tv, diff_tv);
		prof_list[ prof_type ] = diff_tv;
			
		TIMEVAL_ADD(prof_lap[ prof_type ].total, diff_tv,
					prof_lap[ prof_type ].total);
		prof_lap[ prof_type ].count ++;

		if (TIMEVAL_GT(diff_tv, prof_lap[ prof_type ].max)) {
			prof_lap[ prof_type ].max = diff_tv;
		}
	}

	if ((type == PROF_LAPSE_SKIP) ||
		(type == PROF_LAPSE_START)) {
		TIMEVAL_CLEAR(parts_tv);
	} else {
		parts_tv = now_tv;
	}

	/* 前回 profiler_lapse(PROF_LAPSE_START) 呼び出し時からの経過時間 */
	if (type == PROF_LAPSE_START) {
		if (TIMEVAL_ISSET(whole_tv)) {
			TIMEVAL_SUB(now_tv, whole_tv, diff_tv);
			prof_list[ PROF_LAPSE_START ] = diff_tv;

			TIMEVAL_ADD(prof_lap[ PROF_LAPSE_START ].total, diff_tv,
						prof_lap[ PROF_LAPSE_START ].total);
			prof_lap[ PROF_LAPSE_START ].count ++;

			if (TIMEVAL_GT(diff_tv, prof_lap[ PROF_LAPSE_START ].max)) {
				prof_lap[ PROF_LAPSE_START ].max = diff_tv;
			}

			if (prof_lap_fp) {
				for (i = 1; i < PROF_LAPSE_END; i++) {
					fprintf(prof_lap_fp, "%f,", TIMEVAL_TO_MS(prof_list[i]));
				}
				fprintf(prof_lap_fp, "\n");
			}
		}
		whole_tv = now_tv;
	}

	prof_type = type;
}

void profiler_start(void)
{
	if (debug_profiler & 1) {
		prof_exec = TRUE;
	}
}

void profiler_stop(void)
{
	prof_exec = FALSE;
}

void profiler_exit(void)
{
	int i;
	double d;

	if (debug_profiler & 1) {
		for (i = 1; i < PROF_LAPSE_END; i++) {
			d = TIMEVAL_TO_MS(prof_lap[i].total);
			printf("%-6s%5d[times], %12.6f[ms] (ave %10.6f, max %10.6f)\n",
				   prof_label[i], prof_lap[i].count,
				   d, d / prof_lap[i].count, TIMEVAL_TO_MS(prof_lap[i].max));
		}
	}

	if (prof_lap_fp) {
		fclose(prof_lap_fp);
		prof_lap_fp = NULL;
	}
}

#else
void profiler_init(void) {}
void profiler_lapse(int type) {}
void profiler_start(void) {}
void profiler_stop(void) {}
void profiler_exit(void) {}
#endif

void profiler_video_output(int timing, int skip, int drawn)
{
	static int n;

	if (debug_profiler & 2) {
		if (timing) {
			if (skip == FALSE) {
				if (drawn) {
					printf("@");	/* 画像処理の結果、更新が必要だった */
				} else {
					printf("o");	/* 画像処理の結果、更新は不要だった */
				}
			} else {
				printf("-");		/* 時間がないので、スキップした */
			}
		} else {
			printf(" ");			/* 今回は、画像処理しなかった */
		}

		if (++n > 56) {
			n = 0;
			printf("\n");
			fflush(stdout);
		}
	}
}

#endif
