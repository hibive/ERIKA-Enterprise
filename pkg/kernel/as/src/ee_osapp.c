/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
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
 * Autosar API related to OS-applications
 * Author: 2011, Bernardo  Dal Seno
 */

#include <ee_internal.h>

ApplicationType GetApplicationID(void)
{
	ApplicationType app;
	EE_UREG irqnest;
	EE_ORTI_ext_set_service_in(EE_SERVICETRACE_GETAPPLICATIONID);
	irqnest = EE_hal_get_IRQ_nesting_level();
	if (irqnest > 0U) {
		/* Inside an IRQ handler */
		ISRType irq;
		irq = EE_as_ISR_stack[irqnest - 1U].ISR_Terminated;
		app = EE_as_ISR_ROM[irq].ApplID;
	} else {
		EE_TID t = EE_stk_queryfirst();
		/* if t == INVALID_TASK the result is still correct */
		app = EE_th_app[t + 1];
	}
	EE_ORTI_ext_set_service_out(EE_SERVICETRACE_GETAPPLICATIONID);
	return app;
}


/* Only stubs here: */

ObjectAccessType
CheckObjectAccess(ApplicationType ApplID, ObjectTypeType ObjectType,
		  EE_TID ObjectID)
{
	return 0;
}

ApplicationType CheckObjectOwnership(ObjectTypeType ObjectType, EE_TID Object)
{
	return 0;
}

StatusType TerminateApplication(ApplicationType Application,
				RestartType RestartOption)
{
	return 0U;
}

StatusType AllowAccess(void)
{
	return 0U;
}

StatusType GetApplicationState(ApplicationType Application,
			       ApplicationStateRefType Value)
{
	return 0U;
}
