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
 * Author: 2002 Paolo Gai
 * CVS: $Id: ee_altick.c,v 1.5 2006/06/08 20:40:42 pj Exp $
 */

#include "ee_internal.h"

#if defined(RTDRUID_CONFIGURATOR_NUMBER) \
 && (RTDRUID_CONFIGURATOR_NUMBER >= RTDRUID_CONFNUM_NO_ORTI_VARS)

/* ORTI variables */

#ifdef __OO_ORTI_ALARMTIME__
EE_TYPETICK EE_ORTI_alarmtime[EE_MAX_ALARM];
#endif

#endif /* RTDRUID_CONFIGURATOR_NUMBER */



#ifndef __PRIVATE_COUNTER_TICK__

void EE_oo_alarm_insert(AlarmType AlarmID, TickType increment)
{
  register AlarmType current, previous;
  
#ifdef __OO_ORTI_ALARMTIME__
  EE_ORTI_alarmtime[AlarmID] = increment + 
    EE_counter_RAM[EE_alarm_ROM[AlarmID].c].value;
#endif /* __OO_ORTI_ALARMTIME__ */

  current = EE_counter_RAM[EE_alarm_ROM[AlarmID].c].first;

  if (current == (AlarmType)-1) {
    /* the alarm becomes the first into the delta queue */
    EE_counter_RAM[EE_alarm_ROM[AlarmID].c].first = AlarmID;
  } else if (EE_alarm_RAM[current].delta > increment) {
    EE_counter_RAM[EE_alarm_ROM[AlarmID].c].first = AlarmID;
    EE_alarm_RAM[current].delta -= increment;
  } else {
    /* the alarm is not the first into the delta queue */

    /* follow the delta chain until I reach the right place */
    do {
      increment -= EE_alarm_RAM[current].delta;
      previous = current;
      current = EE_alarm_RAM[current].next;
    } while((current != (AlarmType)-1) &&
            (EE_alarm_RAM[current].delta <= increment));

    /* insert the alarm between previous and current */
    if (current != (AlarmType)-1) {
      EE_alarm_RAM[current].delta -= increment;
    }
    EE_alarm_RAM[previous].next = AlarmID;
  }

  EE_alarm_RAM[AlarmID].delta = increment;
  EE_alarm_RAM[AlarmID].next = current;
}

static void EE_handle_alarm_action_task(CounterType CounterID,
  AlarmType current)
{
/* activate the task; NOTE: no preemption at all... 
   This code was directly copied from ActivateTask */
  register TaskType TaskID;

  TaskID = EE_alarm_ROM[current].TaskID;

#ifdef __RN_TASK__
  if (EE_IS_TID_REMOTE(TaskID)) {
    register EE_TYPERN_PARAM par;
    par.pending = 1U;
    /* forward the request to another CPU whether the thread do
       not become to the current CPU */
    (void)EE_rn_send((EE_SREG)EE_UNMARK_REMOTE_TID(TaskID), EE_RN_TASK, par);
    return;
  }
#endif

#ifdef __OO_EXTENDED_STATUS__    
  /* check if the task Id is valid */
  if ((TaskID < 0) || (TaskID >= EE_MAX_TASK)) {
    EE_ORTI_set_lasterror(E_OS_ID);
    EE_oo_notify_error_IncrementCounter(CounterID, current, TaskID, 
      EE_alarm_ROM[current].action, E_OS_ID);
    return;
  }
#endif

  /* check for pending activations */
  if (EE_th_rnact[TaskID] == (EE_UREG)0U) {
    EE_ORTI_set_lasterror(E_OS_LIMIT);
    EE_oo_notify_error_IncrementCounter(CounterID, current, TaskID, 
      EE_alarm_ROM[current].action, E_OS_LIMIT);
    return;
  }

  EE_th_rnact[TaskID]--;

  EE_oo_set_th_status_ready(TaskID);

  /* insert the task in the ready queue */
  EE_rq_insert(TaskID);
}

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
static void EE_handle_alarm_action_event(CounterType CounterID,
  AlarmType current)
{
  /* set an event for a task... NOTE: no preemption at all... 
     This code was directly copied from SetEvent */
  register TaskType TaskID;
  register EventMaskType Mask;

  TaskID = EE_alarm_ROM[current].TaskID;
  Mask = EE_alarm_ROM[current].Mask;

#ifdef __RN_EVENT__
  if (EE_IS_TID_REMOTE(TaskID)) {
    register EE_TYPERN_PARAM par;
    par.ev = Mask;
    /* forward the request to another CPU whether the thread do
       not become to the current CPU */
    (void)EE_rn_send((EE_SREG)EE_UNMARK_REMOTE_TID(TaskID), EE_RN_EVENT, par);

    return;
  }
#endif /* __RN_EVENT__ */

#ifdef __OO_EXTENDED_STATUS__    
  /* check if the task Id is valid */
  if ((TaskID < 0) || (TaskID >= EE_MAX_TASK)) {
    EE_ORTI_set_lasterror(E_OS_ID);
    EE_oo_notify_error_with_mask_IncrementCounter(CounterID, current,
      TaskID, Mask, EE_alarm_ROM[current].action, E_OS_ID);
    return;
  }

  if (EE_th_is_extended[TaskID] == 0U) {
    EE_ORTI_set_lasterror(E_OS_ACCESS);
    EE_oo_notify_error_with_mask_IncrementCounter(CounterID, current,
      TaskID, Mask, EE_alarm_ROM[current].action, E_OS_ACCESS);
    return;
  }
#endif /* __OO_EXTENDED_STATUS__ */

  if (EE_th_status[TaskID] == SUSPENDED) {
    EE_ORTI_set_lasterror(E_OS_STATE);
    EE_oo_notify_error_with_mask_IncrementCounter(CounterID, current,
      TaskID, Mask, EE_alarm_ROM[current].action, E_OS_STATE);
    return;
  }

  /* set the event mask */
  EE_th_event_active[TaskID] |= Mask;

  /* check if the task was waiting for an event we just set
   *
   * WARNING:
   * the test with status==WAITING is FUNDAMENTAL to avoid double
   * insertion of the task in the ready queue!!! Example, when I call
   * two times the same setevent... the first time the task must go in
   * the ready queue, the second time NOT!!!
   */
  if (((EE_th_event_waitmask[TaskID] & Mask) != 0U) &&
      (EE_th_status[TaskID] == WAITING))
  {
    /* if yes, the task must go back into the READY state */
    EE_th_status[TaskID] = READY;
    /* insert the task in the ready queue */
    EE_rq_insert(TaskID);
  }
}
#endif /* __OO_ECC1__ || __OO_ECC2__ */

static void EE_oo_counter_tick_Impl(CounterType CounterID)
{
  register AlarmType current;
  /* Increment the counter value or reset it when overcome maxallowedvalue.
     I need this behaviour for AS services GetCounterValue and GetElapsedValue
   */
  EE_counter_RAM[CounterID].value += 1U;
  if(EE_counter_RAM[CounterID].value >
      EE_counter_ROM[CounterID].maxallowedvalue)
  {
    EE_counter_RAM[CounterID].value = 0U;
  }

  /* if there are queued alarms */
  if (EE_counter_RAM[CounterID].first != (EE_SREG)-1) {
    /* decrement first queued alarm delta */
    EE_alarm_RAM[EE_counter_RAM[CounterID].first].delta--;

    /* execute all the alarms with counter 0 */
    current = EE_counter_RAM[CounterID].first;
    while (EE_alarm_RAM[current].delta == 0U) {
      /* execute it */
      switch (EE_alarm_ROM[current].action) {

        case  EE_ALARM_ACTION_TASK:
          /* activate the task */
          EE_handle_alarm_action_task(CounterID, current);
          break;

        case EE_ALARM_ACTION_CALLBACK:
          (EE_alarm_ROM[current].f)();
          break;

        case EE_ALARM_ACTION_COUNTER:
          /* recursive call */
          EE_oo_counter_tick_Impl(EE_alarm_ROM[current].inccount);
          break;

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
        case EE_ALARM_ACTION_EVENT:
          /* set an event for a task */
          EE_handle_alarm_action_event(CounterID, current);
          break;
#endif /* defined(__OO_ECC1__) || defined(__OO_ECC2__) */

        default:
          /* Invalid action: this should never happen, as `action' is
             initialized by RT-Druid */
          break;
      }

      /* remove the current entry */
      EE_counter_RAM[CounterID].first = EE_alarm_RAM[current].next;

      /* the alarm is cyclic? */
      if (EE_alarm_RAM[current].cycle > 0U) {
        /* enqueue it again 
           note: this can modify EE_counter_RAM[CounterID].first!!! see (*)
         */
        EE_oo_alarm_insert(current,EE_alarm_RAM[current].cycle);
      } else {
        /* alarm no more used! */
        EE_alarm_RAM[current].used = 0U;
      }
      /* (*) here we need EE_counter_RAM[CounterID].first again... */
      current = EE_counter_RAM[CounterID].first;
      if (current == (AlarmType)-1) {
        break;
      }
    }
  }
}
/* Internal primitive */
void EE_oo_counter_tick(CounterType CounterID)
{
  register EE_FREG flag;

  flag = EE_hal_begin_nested_primitive();
  EE_oo_counter_tick_Impl(CounterID);
  EE_hal_end_nested_primitive(flag);
}
#endif /* __PRIVATE_COUNTER_TICK__ */

#ifndef __PRIVATE_INCREMENTCOUNTER__

/*
  OS399: IncrementCounter
*/
static void EE_oo_IncrementCounter_Impl(CounterType CounterID)
{
  register EE_FREG flag;
  flag = EE_hal_begin_nested_primitive();

  /* Call to function that actually increment the counter */
  EE_oo_counter_tick_Impl(CounterID);

  /* After all counter updates check if I'm not in a ISR2 and then 
     execute rescheduling.
   */
  if(EE_hal_get_IRQ_nesting_level() == 0U)
  {
    EE_oo_preemption_point();
  }
  EE_hal_end_nested_primitive(flag);
}

#ifdef __OO_EXTENDED_STATUS__
StatusType EE_oo_IncrementCounter(CounterType CounterID)
{
  register AlarmType current;
  register EE_FREG flag;

  EE_ORTI_set_service_in(EE_SERVICETRACE_INCREMENTCOUNTER);

  /* OS285: If the input parameter CounterID in a call of IncrementCounter() is
      not valid OR the counter is a hardware counter, IncrementCounter() shall
      return E_OS_ID.
  */
  if ((CounterID < 0) || (CounterID >= EE_MAX_COUNTER)) {
    EE_ORTI_set_lasterror(E_OS_ID);
    current = EE_counter_RAM[CounterID].first;

    flag = EE_hal_begin_nested_primitive();
    EE_oo_notify_error_IncrementCounter(CounterID, current, 
      EE_alarm_ROM[current].TaskID, EE_alarm_ROM[current].action, E_OS_ID);
    EE_hal_end_nested_primitive(flag);

    EE_ORTI_set_service_out(EE_SERVICETRACE_INCREMENTCOUNTER);

    return E_OS_ID;
  }

  EE_oo_IncrementCounter_Impl(CounterID);

  EE_ORTI_set_service_out(EE_SERVICETRACE_INCREMENTCOUNTER);
  return E_OK;
}
#else /* __OO_EXTENDED_STATUS__ */
StatusType EE_oo_IncrementCounter(CounterType CounterID)
{
  EE_ORTI_set_service_in(EE_SERVICETRACE_INCREMENTCOUNTER);

  EE_oo_IncrementCounter_Impl(CounterID);

  EE_ORTI_set_service_out(EE_SERVICETRACE_INCREMENTCOUNTER);
  return E_OK;
}
#endif /* __OO_EXTENDED_STATUS__ */

#endif /* __PRIVATE_INCREMENTCOUNTER__ */

