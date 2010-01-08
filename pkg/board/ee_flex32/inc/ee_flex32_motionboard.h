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


#ifndef __INCLUDE_EE_FLEX32_MOTIONBOARD_H__
#define __INCLUDE_EE_FLEX32_MOTIONBOARD_H__

#ifdef __USE_MOTIONBOARD__


/******************************************************************************/
/*                                    Leds                                    */
/******************************************************************************/
#ifdef __USE_LEDS__

__INLINE__ void __ALWAYS_INLINE__ EE_daughter_leds_init(void) 
{
	/* set LEDs drive state low */
	LATDbits.LATD0 = 0;
	LATDbits.LATD2 = 0;
	/* set LEDs pin as output */
	TRISDbits.TRISD0 = 0;
	TRISDbits.TRISD2 = 0;
}
__INLINE__ void __ALWAYS_INLINE__ EE_led_0_on(void)   
{ 
	LATDbits.LATD0 = 1;
}
__INLINE__ void __ALWAYS_INLINE__ EE_led_0_off(void)  
{ 
	LATDbits.LATD0 = 0;
}
__INLINE__ void __ALWAYS_INLINE__ EE_led_1_on(void)   
{ 
	LATDbits.LATD2 = 1;
}
__INLINE__ void __ALWAYS_INLINE__ EE_led_1_off(void)  
{ 
	LATDbits.LATD2 = 0;
}
__INLINE__ void __ALWAYS_INLINE__ EE_leds_on(void)   
{ 
	LATDbits.LATD0 = 1;
	LATDbits.LATD2 = 1;
}
__INLINE__ void __ALWAYS_INLINE__ EE_leds_off(void)  
{ 
	LATDbits.LATD0 = 0;
	LATDbits.LATD2 = 0;
}

#endif

/******************************************************************************/
/*                                   Buttons                                  */
/******************************************************************************/
#ifdef __USE_BUTTONS__
union cn_st {
	EE_UINT8 status;
	struct a_bits {
		EE_UINT8 s1: 1;
		EE_UINT8 s2: 1;
	} bits;
};

extern void (*EE_button_callback)(void);
extern EE_UINT8 EE_button_mask;
extern union cn_st cn_st_old;

__INLINE__ void __ALWAYS_INLINE__ EE_buttons_init(void(*isr_callback)(void), 
						  EE_UINT8 mask) 
{
	/* set BUTTON pins as inputs */
	TRISDbits.TRISD9  = 1;
	TRISDbits.TRISD11 = 1;
	/* TODO: unfeasible due to missing CN, IC must be used.*/
	///* Enable Interrupt */
	//if (isr_callback != NULL) {
	//	if (mask & 0x01) {
	//		CNEN1bits.CN13IE = 1;	// S1/RD4
	//		//cn_st_old.bits.s1 = PORTDbits.RD4;
	//	}
	//	if (mask & 0x02) {
	//		CNEN1bits.CN14IE = 1;	// S2/RD5
	//		//cn_st_old.bits.s2 = PORTDbits.RD5;
	//	}
	//	IFS1bits.CNIF = 0;
	//	IEC1bits.CNIE = 1;
	//}
	///* Save callback */
	//EE_button_callback = isr_callback;
}

__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_button_get_S1(void) 
{
	return !(PORTDbits.RD9);
}
__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_button_get_S2(void) 
{
	return !(PORTDbits.RD11);
}

#endif

/******************************************************************************/
/*                                   Switch                                   */
/******************************************************************************/
#ifdef __USE_SWITCH__

__INLINE__ void __ALWAYS_INLINE__ EE_switch_init(void) 
{
	/* set SWITCH pins as inputs */
	TRISEbits.TRISE7  = 1;
	TRISEbits.TRISE6  = 1;
	TRISEbits.TRISE5  = 1;
	TRISGbits.TRISG13 = 1;
	TRISAbits.TRISA7  = 1;
	TRISGbits.TRISG14 = 1;
	TRISDbits.TRISD4  = 1;
	TRISDbits.TRISD1  = 1;
}

__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_1_get(void) 
{
	return !(PORTEbits.RE7);
}
__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_2_get(void) 
{
	return !(PORTEbits.RE6);
}
__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_3_get(void) 
{
	return !(PORTEbits.RE5);
}
__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_4_get(void) 
{
	return !(PORTGbits.RG13);
}
__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_5_get(void) 
{
	return !(PORTAbits.RA7);
}
__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_6_get(void) 
{
	return !(PORTGbits.RG14);
}
__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_7_get(void) 
{
	return !(PORTDbits.RD4);
}
__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_8_get(void) 
{
	return !(PORTDbits.RD1);
}

__INLINE__ EE_UINT8 __ALWAYS_INLINE__ EE_switch_get_all(void) 
{

	return !(((EE_UINT8) PORTEbits.RE7      ) |
		((EE_UINT8) PORTEbits.RE6  << 1 ) |
		((EE_UINT8) PORTEbits.RE5  << 2 ) |
		((EE_UINT8) PORTGbits.RG13 << 3 ) |
		((EE_UINT8) PORTAbits.RA7  << 4 ) |
		((EE_UINT8) PORTGbits.RG14 << 5 ) |
		((EE_UINT8) PORTDbits.RD4  << 6 ) |
		((EE_UINT8) PORTDbits.RD1  << 7 ));
}
#endif /* __USE_SWITCH__ */




/******************************************************************************/
/*                                   PWM Out                                  */
/******************************************************************************/
#ifdef __USE_PWM__

#define EE_PWM_PORT1 1
#define EE_PWM_PORT2 2

#define EE_PWM_ZERO_DUTY 1500

extern EE_UINT8 t_pre_scaler;

void EE_pwm_init(EE_UINT8 chan, EE_UINT32 pwm_period, EE_UINT32 init_pw);

//__INLINE__ void __ALWAYS_INLINE__ EE_pwm_set_duty_f( EE_UINT8 chan , float duty )
//{
//	/* The computed duty cycle*/
//	float duty_out ;
//
//	/* Get period from Timer2 period register PR2 */
//	EE_UINT16 period = PR2;
//
//	if (duty <= 0.0)
//		duty_out = 0; //** for negative values assume zero
//	else if(duty >= 1.0)
//		duty_out = 1; //** for exessive values assume one
//	else
//		duty_out = duty; //** for the correct values ...
//
//	// Computer register valure
//	switch (chan) {
//		case EE_PWM_PORT1:
//			OC8RS = duty_out * (period+1);
//			break;
//		case EE_PWM_PORT2:
//			OC7RS = duty_out * (period+1);
//			break;
//	}
//}

void EE_pwm_set_duty_f(EE_UINT8 chan , float duty);

__INLINE__ void __ALWAYS_INLINE__ EE_pwm_set_duty(EE_UINT8 chan, EE_UINT32 duty)
{
	/* Compute the Current PulseWidth  to set */
	duty = ( (duty * 40) >> t_pre_scaler ) - 1;  
	if (duty > PR2) 
		return;
	duty = PR2 - duty;
	switch(chan) {
	case EE_PWM_PORT1:
		OC8RS = (EE_UINT16)duty; /* Load OCRS: current pwm duty cycle */
		break;
	case EE_PWM_PORT2:
		OC7RS = (EE_UINT16)duty; /* Load OCRS: current pwm duty cycle */
	  	break;
	}
	return;
}

__INLINE__ void __ALWAYS_INLINE__ EE_pwm_close(EE_UINT8 chan)
{
	switch (chan) {
	case EE_PWM_PORT1:	/** Close PWM1 **/
		OC8RS  = 0;
		OC8CON = 0;
		break;
	case EE_PWM_PORT2: /** Close PWM2 **/
		OC7RS  = 0;
		OC7CON = 0;
		break;
	}
}

#endif // __USE_PWM__


/******************************************************************************/
/*                                   Encoder                                  */
/******************************************************************************/
#ifdef __USE_ENCODER__

/* TODO: Add second channel */

#define QEI_TICK_PER_REV	500
#define	QEI_MAX_CNT_PER_REV	0xffff

__INLINE__ void __ALWAYS_INLINE__ EE_encoder_init(void)
{
	/* set encoder bits as digital input */
 	AD1PCFGLbits.PCFG3 = 1;
 	AD1PCFGLbits.PCFG4 = 1;
 	AD1PCFGLbits.PCFG5 = 1;
 	AD2PCFGLbits.PCFG3 = 1;
 	AD2PCFGLbits.PCFG4 = 1;
 	AD2PCFGLbits.PCFG5 = 1;

	// Disable QEI Module
	QEICONbits.QEIM = 0;

	// Clear any count errors
	QEICONbits.CNTERR = 0;

	// Count error interrupts disabled
	DFLTCONbits.CEID = 1;

	// Reset position counter
	POSCNT = 0;

	// Set bound value
	MAXCNT = QEI_MAX_CNT_PER_REV;

	// X4 mode with position counter reset by MAXCNT
	QEICONbits.QEIM = 7;
}

__INLINE__ void __ALWAYS_INLINE__ EE_encoder_close(void)
{
	// Disable QEI Module
	QEICONbits.QEIM = 0;
}

__INLINE__ EE_INT6 __ALWAYS_INLINE__ EE_encoder_get_ticks(void)
{
	return ((EE_INT16)POSCNT);
}

//__INLINE__ EE_UINT16 __ALWAYS_INLINE__ EE_encoder_get_position(void)
__INLINE__ float __ALWAYS_INLINE__ EE_encoder_get_position(void)
{
	EE_INT16 POSCNTcopy = 0;

	POSCNTcopy = (EE_INT16)POSCNT;

	if (POSCNTcopy < 0)
		POSCNTcopy = -POSCNTcopy;

	//AngPos[1] = AngPos[0];
	//AngPos[0] = (unsigned int)(((unsigned long)POSCNTcopy * 2048)/125);
	// 0 <= POSCNT <= 1999 to 0 <= AngPos <= 32752

	//return (unsigned int)(((unsigned long)POSCNTcopy * 2048)/125);
	return (float)(((EE_UINT32)POSCNTcopy*500)/360);
}

#endif


/* ************************************************************************* */
/* USB Communication - TODO!!! */
/* ************************************************************************* */

#if defined __USE_USB__
void EE_usb_init(void);
EE_INT16 EE_usb_write(EE_UINT8 *buf, EE_UINT16 len);
EE_INT16 EE_usb_read(EE_UINT8 *buf, EE_UINT16 len);
#endif

/* ************************************************************************* */

/* ************************************************************************* */

#endif

#endif