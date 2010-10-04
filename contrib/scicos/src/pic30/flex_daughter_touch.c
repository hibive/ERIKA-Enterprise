// 07-06-2009 Francesco Prosperi - Retis Lab - SSSA

#include <machine.h>
#include <scicos_block4.h>

#define ADC_TO_USE 2

#include <ee.h>
#include "touch.h"

#define touch_set_dimension(a, b)       touch_1_set_dimension(a, b) 
#define touch_raw_init()                touch_1_raw_init()
#define touch_start()                   touch_1_start()
#define touch_wait_raw_position(a, b)   touch_1_wait_raw_position(a, b)
#define touch_tune(a)                   touch_1_tune(a)
#define touch_poll_s_position(a, b)     touch_1_poll_s_position(a, b)

#define ASCII_X	88
#define ASCII_Y	89

static EE_UINT8 x_already_initialized = 0;
static EE_UINT8 y_already_initialized = 0;
static EE_UINT8 touch_initialized = 0;

static void init(scicos_block *block)
{
	EE_UINT8 axis = block->ipar[0];
	EE_UINT16 range = block->ipar[1];

	if(axis != ASCII_X && axis != ASCII_Y) 
		return;
	if(axis == ASCII_X && x_already_initialized)
		return;
	if(axis == ASCII_Y && y_already_initialized)
		return;

	touch_set_dimension(axis-ASCII_X,range);

	if(axis == ASCII_X)
		x_already_initialized = 1;
	else if(axis == ASCII_Y)
		y_already_initialized = 1;
	
	if(x_already_initialized && y_already_initialized)
	{
		touch_initialized = 1;
		touch_raw_init();
		
		#if (defined __USE_LEDS__) && (defined __USE_MOTIONBOARD__)
		EE_daughter_leds_init();
		#endif
	}
}

static void inout(scicos_block *block)
{
	EE_UINT8 axis = block->ipar[0];
	EE_INT16 aux;
	float *y = block->outptr[0];

	if(!touch_initialized)
		return;
	
	if(axis != ASCII_X && axis != ASCII_Y) 
		return;
		
	//y[0] = (float)touch_get_position_s(axis-ASCII_X,);
	touch_poll_s_position(axis-ASCII_X,&aux);
	y[0] = (float)aux;

}

static void end(scicos_block *block)
{
}

void flex_daughter_touch(scicos_block *block,int flag)
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
