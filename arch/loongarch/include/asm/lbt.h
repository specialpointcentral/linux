/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Author: Qi Hu <huqi@loongson.cn>
 * Copyright (C) 2020-2022 Loongson Technology Corporation Limited
 */
#ifndef _ASM_LBT_H
#define _ASM_LBT_H

#include <linux/sched.h>
#include <linux/sched/task_stack.h>
#include <linux/ptrace.h>
#include <linux/thread_info.h>
#include <linux/bitops.h>

#include <asm/cpu.h>
#include <asm/cpu-features.h>
#include <asm/current.h>
#include <asm/loongarch.h>
#include <asm/processor.h>
#include <asm/ptrace.h>

#ifdef CONFIG_CPU_HAS_LBT

extern void _save_lbt(struct thread_struct *);
extern void _restore_lbt(struct thread_struct *);

static inline int is_lbt_enabled(void)
{
	if (!cpu_has_lbt)
		return 0;
	return (csr_read32(LOONGARCH_CSR_EUEN) & CSR_EUEN_LBTEN) ?
		1 : 0;
}

static inline int thread_lbt_context_live(void)
{
	if (!cpu_has_lbt)
		return 0;
	return test_thread_flag(TIF_LBT_CTX_LIVE);
}

#define enable_lbt()		set_csr_euen(CSR_EUEN_LBTEN)
#define disable_lbt()		clear_csr_euen(CSR_EUEN_LBTEN)

static inline int is_lbt_owner(void)
{
	return test_thread_flag(TIF_USEDLBT);
}

static inline void __own_lbt(void)
{
	enable_lbt();
	set_thread_flag(TIF_USEDLBT);
	KSTK_EUEN(current) |= CSR_EUEN_LBTEN;
}

static inline void own_lbt_inatomic(int restore)
{
	if (cpu_has_lbt && !is_lbt_owner()) {
		__own_lbt();
		if (restore)
			_restore_lbt(&current->thread);
	}
}

static inline void own_lbt(int restore)
{
	preempt_disable();
	own_lbt_inatomic(restore);
	preempt_enable();
}

static inline void __lose_lbt(struct task_struct *tsk)
{
	disable_lbt();
	clear_tsk_thread_flag(tsk, TIF_USEDLBT);
	KSTK_EUEN(tsk) &= ~(CSR_EUEN_LBTEN);
}

static inline void lose_lbt_inatomic(int save, struct task_struct *tsk)
{
	if (cpu_has_lbt && is_lbt_owner()) {
		if (save)
			_save_lbt(&tsk->thread);
		__lose_lbt(tsk);
	}
}

static inline void lose_lbt(int save)
{
	preempt_disable();
	lose_lbt_inatomic(save, current);
	preempt_enable();
}

static inline void init_lbt(void)
{
	set_thread_flag(TIF_LBT_CTX_LIVE);
	__own_lbt();
}

/* these functions are not used yet */
static inline void save_lbt(struct task_struct *tsk)
{
	if (cpu_has_lbt)
		_save_lbt(&tsk->thread);
}

static inline void restore_lbt(struct task_struct *tsk)
{
	if (cpu_has_lbt)
		_restore_lbt(&tsk->thread);
}
#else
static inline void own_lbt_inatomic(int restore)
{}
static inline void lose_lbt_inatomic(int save, struct task_struct *tsk)
{}
static inline void own_lbt(int restore)
{}
static inline void lose_lbt(int save)
{}
#endif

#endif /* _ASM_LBT_H */
