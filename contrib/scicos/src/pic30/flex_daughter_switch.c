/* ###*B*###
 * Copyright (C) Roberto Bucher (SUPSI- Lugano)
 *               Simone Mannori (Scilab / INRIA / DIGITEO)
 *
 * Copyright (C) METALAU Project (INRIA)
 * ###*E*### */
 
 
//** 9 Feb 2008 : Revision notes by Simone Mannori 
//**

#include <machine.h>
#include <scicos_block4.h>

#include <ee.h>

/* Buttons bit allocation 

Scicos	Function	dsPIC
Pin=1	Button1	RD4
Pin=2	Button2	RD5
Pin=3	Button3	RD6
Pin=4	Button4	RD15
*/

static void init(scicos_block *block)
{
	unsigned int pin = block->ipar[0];
	
	if( (pin < 1) || (pin > 8) )
		return;

	EE_switch_init(); // Enable switches 
}

static void inout(scicos_block *block)
{
	float * y = block->outptr[0];

	unsigned int pin = block->ipar[0];
	
	if ((pin < 1) || (pin > 8))
		return; //** return if outside the allowed range
	
	switch(pin) {
		case 1:
			if (EE_switch_1_get())
				y[0] = 1.0; //** the output of a input bit is "1.0"  
			else
				y[0] = 0.0; //** "0.0" (float)
			break;
		case 2:
			if (EE_switch_2_get())
				y[0] = 1.0; //** the output of a input bit is "1.0"  
			else
				y[0] = 0.0; //** "0.0" (float)
			break;
		case 3:
			if (EE_switch_3_get())
				y[0] = 1.0; //** the output of a input bit is "1.0"  
			else
				y[0] = 0.0; //** "0.0" (float)
			break;
		case 4:
			if (EE_switch_4_get())
				y[0] = 1.0; //** the output of a input bit is "1.0"  
			else
				y[0] = 0.0; //** "0.0" (float)
			break;
		case 5:
			if (EE_switch_5_get())
				y[0] = 1.0; //** the output of a input bit is "1.0"  
			else
				y[0] = 0.0; //** "0.0" (float)
			break;
		case 6:
			if (EE_switch_6_get())
				y[0] = 1.0; //** the output of a input bit is "1.0"  
			else
				y[0] = 0.0; //** "0.0" (float)
			break;
		case 7:
			if (EE_switch_7_get())
				y[0] = 1.0; //** the output of a input bit is "1.0"  
			else
				y[0] = 0.0; //** "0.0" (float)
			break;
		case 8:
			if (EE_switch_8_get())
				y[0] = 1.0; //** the output of a input bit is "1.0"  
			else
				y[0] = 0.0; //** "0.0" (float)
			break;

}
}

static void end(scicos_block *block)
{
}

void flex_daughter_switch(scicos_block *block, int flag)
{
 switch (flag) {
    case OutputUpdate:  /* set output */
      inout(block);
      break;

    case StateUpdate: /* get input */
      break;

    case Initialization:  /* initialisation */
      init(block);
      break;

    case Ending:  /* ending */
      end(block);
      break;
  }
}