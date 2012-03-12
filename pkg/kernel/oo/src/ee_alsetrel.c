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
 * CVS: $Id: ee_alsetrel.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

#include "ee_internal.h"

/* SetRelAlarm
   - This function occupies the AlarmID setting an alarm after increment ticks.
   - if increment=0, E_OS_VALUE is returned.
   - the Alarm must not be already in use

   returns the relative value in ticks before the alarm expires
   - it returns E_OK if all OK
     E_OS_STATE if the Alarm is already used
     E_OS_ID if the alarm is invalid (only Extended status)
     E_OS_VALUE if the parameters are incorrect
*/

#ifndef __PRIVATE_SETRELALARM__
StatusType EE_oo_SetRelAlarm(AlarmType AlarmID, 
          TickType increment, 
          TickType cycle)
{
  register EE_FREG flag;
  EE_ORTI_set_service_in(EE_SERVICETRACE_SETRELALARM);

#ifdef __OO_EXTENDED_STATUS__
  if ((AlarmID < 0) || (AlarmID >= EE_MAX_ALARM)) {
    EE_ORTI_set_lasterror(E_OS_ID);

    flag = EE_hal_begin_nested_primitive();
    EE_oo_notify_error_SetRelAlarm(AlarmID, increment, cycle, E_OS_ID);
    EE_hal_end_nested_primitive(flag);

    EE_ORTI_set_service_out(EE_SERVICETRACE_SETRELALARM);

    return E_OS_ID;
  }
#endif /* __OO_EXTENDED_STATUS__ */

  /* New feature: you can configure alarm increment value and alarm cycle value
   * inside the conf.oil and then call the function in this way:
   * SetRelAlarm(alarm_id, EE_STATIC_ALARM_TIME, EE_STATIC_CYCLE_TIME);
   */
#ifdef __OO_AUTOSTART_ALARM__
  if(increment == EE_STATIC_ALARM_TIME) {
    increment = EE_oo_autostart_alarm_increment[AlarmID];
  }
  if(cycle == EE_STATIC_CYCLE_TIME) {
    cycle = EE_oo_autostart_alarm_cycle[AlarmID];
  }
#endif /* __OO_AUTOSTART_ALARM__ */

  /* OS304: If in a call to SetRelAlarm() the parameter “increment” is set to
     zero, the service shall return E_OS_VALUE in standard and extended status
   */
  if(increment == (TickType)0U) {
    EE_ORTI_set_lasterror(E_OS_VALUE);

    flag = EE_hal_begin_nested_primitive();
    EE_oo_notify_error_SetRelAlarm(AlarmID, increment, cycle, E_OS_VALUE);
    EE_hal_end_nested_primitive(flag);

    EE_ORTI_set_service_out(EE_SERVICETRACE_SETRELALARM);

    return E_OS_VALUE;
  }

  /* let the system still work if the increment parameter is 0 note that 0
   * is still an invalid value, so I decided arbitrarily to let it
   * fire the next tick.
   */
  /* commented for OS304 */
  /* if (increment == (TickType)0U) {
    increment = 1U;
  } */

#ifdef __OO_EXTENDED_STATUS__
  if ((increment > EE_counter_ROM[EE_alarm_ROM[AlarmID].c].maxallowedvalue)
      || 
      ((cycle != 0U) && 
       ((cycle < EE_counter_ROM[EE_alarm_ROM[AlarmID].c].mincycle) ||
        (cycle > EE_counter_ROM[EE_alarm_ROM[AlarmID].c].maxallowedvalue)))
      ) {
    EE_ORTI_set_lasterror(E_OS_VALUE);

    flag = EE_hal_begin_nested_primitive();
    EE_oo_notify_error_SetRelAlarm(AlarmID, increment, cycle, E_OS_VALUE);
    EE_hal_end_nested_primitive(flag);

    EE_ORTI_set_service_out(EE_SERVICETRACE_SETRELALARM);

    return E_OS_VALUE;
  }
#endif /* __OO_EXTENDED_STATUS__ */

  flag = EE_hal_begin_nested_primitive();

  if (EE_alarm_RAM[AlarmID].used) {

    EE_ORTI_set_lasterror(E_OS_STATE);

    EE_oo_notify_error_SetRelAlarm(AlarmID, increment, cycle, E_OS_STATE);

    EE_hal_end_nested_primitive(flag);
    EE_ORTI_set_service_out(EE_SERVICETRACE_SETRELALARM);

    return E_OS_STATE;
  }

  /* first, use the alarm and set the cycle */
  EE_alarm_RAM[AlarmID].used = 1U;
  EE_alarm_RAM[AlarmID].cycle = cycle;

  /* then, insert the task into the delta queue with a value = interval */
  EE_oo_alarm_insert(AlarmID,increment);

  EE_hal_end_nested_primitive(flag);
  EE_ORTI_set_service_out(EE_SERVICETRACE_SETRELALARM);

  return E_OK;
}
#endif /* !__PRIVATE_SETRELALARM__ */
