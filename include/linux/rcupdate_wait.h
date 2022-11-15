/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_RCUPDATE_WAIT_H
#define _LINUX_SCHED_RCUPDATE_WAIT_H

/*
 * RCU synchronization types and methods:
 */

#include <linux/rcupdate.h>
#include <linux/completion.h>

/*
 * Structure allowing asynchronous waiting on RCU.
 */
/*xiaojin-rcu-synchronize_rcu -2.1 rcu_synchronize */
struct rcu_synchronize {
	struct rcu_head head;
	struct completion completion;
};
void wakeme_after_rcu(struct rcu_head *head);

void __wait_rcu_gp(bool checktiny, int n, call_rcu_func_t *crcu_array,
		   struct rcu_synchronize *rs_array);

/*checktiny == false 
void call_rcu(struct rcu_head *head, rcu_callback_t func)

typedef void (*call_rcu_func_t)(struct rcu_head *head, rcu_callback_t func);
*/
/*xiaojin-rcu-synchronize_rcu-2 */
#define _wait_rcu_gp(checktiny, ...) \
do {									\
	call_rcu_func_t __crcu_array[] = { __VA_ARGS__ };/*只有一个元素call_rcu*/		\
	struct rcu_synchronize __rs_array[ARRAY_SIZE(__crcu_array)];	\
	__wait_rcu_gp(checktiny, ARRAY_SIZE(__crcu_array),		\
			__crcu_array, __rs_array);			\
} while (0)

/*xiaojin-rcu-synchronize_rcu-1
... is call_rcu(__VA_ARGS__编译器标记代表...的内容)
wait_rcu_gp(call_rcu);
-- void call_rcu(struct rcu_head *head, rcu_callback_t func)*/
#define wait_rcu_gp(...) _wait_rcu_gp(false, __VA_ARGS__)

/**
 * synchronize_rcu_mult - Wait concurrently for multiple grace periods
 * @...: List of call_rcu() functions for different grace periods to wait on
 *
 * This macro waits concurrently for multiple types of RCU grace periods.
 * For example, synchronize_rcu_mult(call_rcu, call_rcu_tasks) would wait
 * on concurrent RCU and RCU-tasks grace periods.  Waiting on a given SRCU
 * domain requires you to write a wrapper function for that SRCU domain's
 * call_srcu() function, with this wrapper supplying the pointer to the
 * corresponding srcu_struct.
 *
 * The first argument tells Tiny RCU's _wait_rcu_gp() not to
 * bother waiting for RCU.  The reason for this is because anywhere
 * synchronize_rcu_mult() can be called is automatically already a full
 * grace period.
 */
#define synchronize_rcu_mult(...) \
	_wait_rcu_gp(IS_ENABLED(CONFIG_TINY_RCU), __VA_ARGS__)

#endif /* _LINUX_SCHED_RCUPDATE_WAIT_H */
