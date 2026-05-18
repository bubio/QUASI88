#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

/*----------------------------------------------------------------------
 * デバッグ用
 *----------------------------------------------------------------------*/
#ifdef DEBUGPRINTF
void debugprintf(const char *format, ...);
#define XPRINTF		debugprintf
#else
#define XPRINTF		if(1){}else printf
#endif

extern int show_statistics;

extern int pio_debug;
extern int fdc_debug;
extern int main_debug;
extern int sub_debug;

#ifdef DEBUGLOG
void debuglog_init(void);
void debuglog_sync(void);
void debuglog_exit(void);

void logpio(const char *format, ...);
void logfdc(const char *format, ...);
void logz80(const char *format, ...);
void logz80_target(int debug_flag);
#else

#define debuglog_init()
#define debuglog_sync()
#define debuglog_exit()
#define logpio  if(1){}else printf
#define logfdc  if(1){}else printf
#define logz80  if(1){}else printf
#define logz80_target(x)
#endif


extern int debug_profiler;
extern char *file_profiler;

#ifdef PROFILER
enum {
	PROF_LAPSE_SKIP,
	PROF_LAPSE_START,
	PROF_LAPSE_EVENT,
	PROF_LAPSE_UI,
	PROF_LAPSE_INPUT,
	PROF_LAPSE_CPU,
	PROF_LAPSE_PCM,
	PROF_LAPSE_AUDIO,
	PROF_LAPSE_BLIT,
	PROF_LAPSE_VIDEO,
	PROF_LAPSE_IDLE,
	PROF_LAPSE_END
};
void profiler_init(void);
void profiler_lapse(int type);
void profiler_start(void);
void profiler_stop(void);
void profiler_exit(void);
void profiler_video_output(int timing, int skip, int drawn);
#else
#define profiler_init()
#define profiler_lapse(i)
#define profiler_start()
#define profiler_stop()
#define profiler_exit()
#define profiler_video_output(t,s,d)
#endif


#endif /* DEBUG_H_INCLUDED */
