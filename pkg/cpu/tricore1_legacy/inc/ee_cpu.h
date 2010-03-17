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
 * Author: 2005 Paolo Gai
 * CVS: $Id: ee_cpu.h,v 1.5 2006/04/08 21:08:54 pj Exp $
 */


#include "eecfg.h"
#include "cpu/tricore1/inc/ee_cpudefs.h"
#include "cpu/tricore1/inc/ee_compiler.h"

#ifndef __INCLUDE_TRICORE1_CPU_H__
#define __INCLUDE_TRICORE1_CPU_H__



/*************************************************************************
 HAL Constants
 *************************************************************************/


/* invalid pointer */
#ifndef NULL
#define NULL 0
#endif

/*************************************************************************
 HAL Types
 *************************************************************************/


/* Primitive data types */
/* TODO: it is worth for THUMB?!? */

typedef unsigned char EE_BIT;
typedef unsigned char EE_UINT8;
typedef unsigned short EE_UINT16;
typedef unsigned int EE_UINT32;
typedef signed char EE_INT8;
typedef signed short EE_INT16;
typedef signed int EE_INT32;

typedef EE_UINT32 EE_UREG;
typedef EE_INT32  EE_SREG;
typedef EE_UINT32 EE_FREG;

/* Thread IDs */
typedef EE_UINT32 EE_TID;

/* Addresses (that have the same size of a pointer) */
typedef EE_UINT32 *EE_ADDR;

/* EE_TYPEIRQ is defined inside the MCU */

/*
 * This structure is used by the Multistack HAL to contain the
 * information about a "stack", that is composed by a user stack
 * (SYS-mode) and a system stack (IRQ-mode). This type is
 * used internally by the HAL and is not used by the Kernels.
 */
#ifdef __MULTI__
struct EE_TOS {
    EE_ADDR SYS_tos;
    EE_ADDR IRQ_tos;
};
#endif

/*************************************************************************
 Application dependent data types
 *************************************************************************/

#ifdef __HAS_TYPES_H__
#include "types.h"
#endif


/*************************************************************************
 HAL Variables
 *************************************************************************/

/* Thread function body pointer */
extern const EE_ADDR EE_hal_thread_body[];


#ifdef __MULTI__

/* each task use a system (IRQ) stack and a user (SYS) stack */
extern struct EE_TOS EE_tc1_system_tos[];

/* tc1_system_tos[] index that point to the thread tos (one for each thread) */
extern EE_UREG EE_tc1_thread_tos[];

/* tc1_system_tos[] index that point to the active thread tos */
extern EE_UREG EE_tc1_active_tos;

/* stack used by IRQ handlers 
 * mi sa che questo va sostituito con la gestione dell'irq stack di tricore
 */
#ifdef __IRQ_STACK_NEEDED__
extern struct EE_TOS EE_tc1_IRQ_tos;
#endif

#endif /* __MULTI__ */

#if defined(__OO_BCC1__) || defined(__OO_BCC2__) || defined(__OO_ECC1__) || defined(__OO_ECC2__)

/* this is a safe place to put sp_sys when EE_hal_terminate_savestk
   is called into EE_oo_thread_stub */
extern EE_UINT32 EE_terminate_data[];

/* this is the real thread body that is called if the thread use the
   TerminateTask function */
extern const EE_ADDR EE_terminate_real_th_body[];

/* this is the stub that have to be put into the EE_th_body array */
extern void EE_oo_thread_stub(void);

#endif



/*********************************************************************
 TRICORE interrupt disabling/enabling
 *********************************************************************/

__INLINE__ void __ALWAYS_INLINE__ EE_tc1_enableIRQ(void) { __asm("enable"); }
__INLINE__ void __ALWAYS_INLINE__ EE_tc1_disableIRQ(void){ __asm("disable");}


/*************************************************************************
 Functions
 *************************************************************************/

/* 
 * Interrupt Handling
 */

/* Disable/Enable Interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_enableIRQ(void)
{
    EE_tc1_enableIRQ();
}

__INLINE__ void __ALWAYS_INLINE__ EE_hal_disableIRQ(void)
{
    EE_tc1_disableIRQ();
}

#endif