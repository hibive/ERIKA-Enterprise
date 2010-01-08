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
 * Author: 2006 Paolo Gai
 * CVS: $Id: code.c,v 1.4 2007/03/16 14:21:17 nino Exp $
 */

#include "ee.h"
#include "kernel/sem/inc/ee_sem.h"        							

/* This example has 2 semaphores, used by the Task1 and by the Task2 tasks. */

/* This semaphore is initialized automatically */
SemType P = STATICSEM(1);

/* This semaphore is initialized inside the Background Task */
SemType V;

volatile int dummit_counter = 0;
volatile int taskp_counter = 0;
volatile int taskc_counter = 0;

TASK(Task1)
{
	int i;
	taskp_counter++;
	
	ActivateTask(Task2);
	
	EE_leds_on();
	PostSem(&V); 
}

TASK(Task2)
{
	int i;
	taskc_counter++;
	
	WaitSem(&V);
	EE_leds_off();
}

int main(void)
{
	/* Initialization of the second semaphore of the example; the first
	semaphore is initialized inside the definition */
	InitSem(V,0);
  
	/* Init the led and turn it off */
	EE_leds_init();
	EE_leds_off();

	ActivateTask(Task1);
	//ActivateTask(Task2);
  
	/* Forever loop: background activities (if any) should go here.
	Please note that in this example the code never reach this point... */
	for (;;);
  
	return 0;
}