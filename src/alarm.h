#ifndef ALARM_H_INCLUDED
#define ALARM_H_INCLUDED

/***********************************************************************
 * アラーム
 ************************************************************************/

typedef	void (*alarm_callback_func)(void *, void *);

extern void alarm_init(void);
extern void alarm_reset(void);
extern void alarm_add(void *alarm_id,
					  void *parm,
					  void (*callback)(void *alarm_id, void *parm),
					  int timer_ms,
					  int is_lock);
extern void alarm_remove(void *alarm_id);
extern void alarm_update(void);
extern int  alarm_is_lock(void);
extern void alarm_exit(void);

#endif /* ALARM_H_INCLUDED */
