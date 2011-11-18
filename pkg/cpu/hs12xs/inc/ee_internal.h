/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation, 
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/*
 * Author: Dario Di Stefano
 */

#include "cpu/hs12xs/inc/ee_cpu.h"

#ifndef __INCLUDE_S12XS_INTERNAL_H__
#define __INCLUDE_S12XS_INTERNAL_H__

/*************************************************************************
 Functions
 *************************************************************************/

/*
 * Generic Primitives
 */

/* we should make an include file with all the registers of a S12XS CPU
   the file is typically provided by the compiler distribution */
//extern EE_FREG DISICNT;
//extern volatile EE_FREG DISICNT __attribute__((__sfr__));

/* called as _first_ function of a primitive that can be called into
   an IRQ and into a task */
__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_hal_begin_nested_primitive(void)
{
  register EE_FREG retvalue;
  retvalue = EE_READ_CCR();
  EE_hal_disableIRQ();
  
  return retvalue;
}

/* called as _last_ function of a primitive that can be called into
   an IRQ and into a task */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_end_nested_primitive(EE_FREG f)
{
  if(f & 0x10) 						// restore I bit
    EE_hal_disableIRQ();
  else
    EE_hal_enableIRQ();
}



/* 
 * Context Handling  
 */

extern EE_ADDR EE_hal_endcycle_next_thread;
extern EE_UREG EE_hal_endcycle_next_tos;


/* typically called into a generic primitive to implement preemption */
/* NOTE: s12xs_thread_tos[0]=dummy, s12xs_thread_tos[1]=thread0, ... */
#ifdef __MONO__
void EE_s12xs_hal_ready2stacked(EE_ADDR thread_addr); /* in ASM */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_ready2stacked(EE_TID thread)
{
    EE_s12xs_hal_ready2stacked(EE_hal_thread_body[thread]);
}
#endif

#ifdef __MULTI__
#ifdef __CODEWARRIOR__
 //void EE_s12xs_hal_ready2stacked(EE_ADDR thread_addr, EE_UREG tos_index); /* in ASM */
 void EE_s12xs_hal_ready2stacked(EE_UREG tos_index, EE_ADDR thread_addr); /* in ASM */
 __INLINE__ void __ALWAYS_INLINE__ EE_hal_ready2stacked(EE_TID thread)
 {
	//EE_s12xs_hal_ready2stacked(EE_hal_thread_body[thread], EE_s12xs_thread_tos[thread+1]);
	EE_s12xs_hal_ready2stacked(EE_s12xs_thread_tos[thread+1], EE_hal_thread_body[thread]);
 }
#else
 void EE_s12xs_hal_ready2stacked(EE_ADDR thread_addr, EE_UREG tos_index); /* in ASM */
 __INLINE__ void __ALWAYS_INLINE__ EE_hal_ready2stacked(EE_TID thread)
 {
	EE_s12xs_hal_ready2stacked(EE_hal_thread_body[thread], EE_s12xs_thread_tos[thread+1]);
 }
#endif
#endif


/* typically called at the end of a thread instance */
#ifdef __MONO__
__INLINE__ void __ALWAYS_INLINE__ EE_hal_endcycle_stacked(EE_TID thread)
{
  EE_hal_endcycle_next_thread = 0;
  /* TID is useless */
}
#endif

#ifdef __MULTI__
__INLINE__ void __ALWAYS_INLINE__ EE_hal_endcycle_stacked(EE_TID thread)
{
  EE_hal_endcycle_next_tos = EE_s12xs_thread_tos[thread+1];
  EE_hal_endcycle_next_thread = 0;
}
#endif



#ifdef __MONO__
__INLINE__ void __ALWAYS_INLINE__ EE_hal_endcycle_ready(EE_TID thread)
{
  EE_hal_endcycle_next_thread = EE_hal_thread_body[thread];
}
#endif

#ifdef __MULTI__
__INLINE__ void __ALWAYS_INLINE__ EE_hal_endcycle_ready(EE_TID thread)
{
  EE_hal_endcycle_next_tos = EE_s12xs_thread_tos[thread+1];
  EE_hal_endcycle_next_thread = EE_hal_thread_body[thread];
}
#endif

/* typically called at the end of an interrupt */
#define EE_hal_IRQ_stacked EE_hal_endcycle_stacked
#define EE_hal_IRQ_ready EE_hal_endcycle_ready

/* called to change the active stack, typically inside blocking primitives */
/* there is no mono version for this primitive...*/
#ifdef __MULTI__
void EE_s12xs_hal_stkchange(EE_UREG tos_index); /* in ASM */
//void EE_s12xs_hal_stkchange(EE_ADDR thread_addr, EE_UREG tos_index); /* in ASM */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_stkchange(EE_TID thread)
{
    EE_s12xs_hal_stkchange(EE_s12xs_thread_tos[thread+1]);
    //EE_s12xs_hal_stkchange(EE_hal_thread_body[thread],EE_s12xs_thread_tos[thread+1]);
}
#endif


/*
 * Nested Interrupts Handling
 */

/* can be called with interrupt enabled */
extern EE_UREG EE_IRQ_nesting_level;

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_hal_get_IRQ_nesting_level(void)
{
  return EE_IRQ_nesting_level;
}


/* 
 * OO TerminateTask related stuffs
 */

#if defined(__OO_BCC1__) || defined(__OO_BCC2__) || defined(__OO_ECC1__) || defined(__OO_ECC2__)

void EE_s12xs_terminate_savestk(EE_DADD sp, EE_ADDR realbody);
void EE_s12xs_terminate_task(EE_DADD sp);// NORETURN;

__INLINE__ void __ALWAYS_INLINE__ EE_hal_terminate_savestk(EE_TID t)
{
  EE_s12xs_terminate_savestk(&EE_terminate_data[t],
			       (EE_ADDR)EE_terminate_real_th_body[t]);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hal_terminate_task(EE_TID t)
{
  EE_s12xs_terminate_task(&EE_terminate_data[t]);
}

#endif


#endif
