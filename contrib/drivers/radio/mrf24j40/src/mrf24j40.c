/**
* @file mrf24j40_compiler.h
* @brief MRF24J40 Abstract Compiler
* @author Gianluca Franchino
* @author Christian Nastasi
* @author Mauro Nino Marinoni
* @date 2009-03-24
*
* File Contents: a set of functions to manage the Microchip MRF24J40 radio transceiver.
*
* \todo Reformat the following text using doxygen
*
* Author: Gianluca Franchino.
* Affiliation: Retis Lab. Scuola Superiore Sant'Anna. Pisa (Italy).
* Contacts: g.franchino@sssup.it; gianluca@evidence.eu.com
* Date: 02/27/2008.
*
 */

#include "mrf24j40.h"


/*chris: TODO: Choose a proper policy for the 'is_initialized' status variable,
		we have to use it in some function!!
*/
static uint8_t radio_initialized = 0;
static void (*rx_callback)(void) = NULL;

void mrf24j40_set_rx_callback(void (*func)(void)) 
{
	rx_callback = func;
}

/**
* @brief Init transceiver
*
* This routine initializes the radio transceiver
*
* @return 0 if the initialization goes well, -1 otherwise.
*/

int8_t mrf24j40_init(uint8_t int_setup, uint8_t ch)
{  
	uint8_t i;

	/*chris: do I need this chek? This currently avoid the init func to 
		 be called twice!
	*/
	if (radio_initialized) 
		return -2;

	/* init hal-specific things */
	mrf24j40_hal_init();

	MRF24J40_RESETn = 0;
	mrf24j40_delay_us(2500); 
    
	MRF24J40_RESETn = 1;
	mrf24j40_delay_us(2500);

	/**
	 * Software reset:  
	 * 7:3 = '00'  = Reserved
	 * 2:0   = '111' = Reset MAC, baseband  
	 * and power management circuitries
	 */
	mrf24j40_set_short_add_mem(MRF24J40_SOFTRST, 0x07);

	/**
	* wait until the radio reset is completed
	*/
	do
	{
		i = mrf24j40_get_short_add_mem(MRF24J40_SOFTRST);
	}
	while((i & 0x07) != 0);   

	mrf24j40_delay_us(2500);
	
	mrf24j40_set_short_add_mem(MRF24J40_PACON2, 0x98);

	/* 
	 * Read back to value just written.
	 * This trick is used to verify if the radio is connected.
	 */
	if (mrf24j40_get_short_add_mem(MRF24J40_PACON2) != 0x98)
		return -1;
	/**
	* Reset RF state machine
	*/         
	mrf24j40_set_short_add_mem(MRF24J40_RFCTL, 0x04);        
	mrf24j40_set_short_add_mem(MRF24J40_RFCTL, 0x00);
    
	/** flush RX fifo */
	mrf24j40_set_short_add_mem(MRF24J40_RXFLUSH, 0x01);
    
	/** program the RF and Baseband Register */
	//mrf24j40_long_add_mem(RFCTRL4,0x02);
    /** Enable the RX */
	//mrf24j40_long_add_mem(RFRXCTRL,0x01);

	/** setup */
	mrf24j40_set_channel(ch); //set channel    
	mrf24j40_set_long_add_mem(MRF24J40_RFCON1, 0x01); //program the RF and Baseband Register as
						//suggested by the datasheet
	mrf24j40_set_long_add_mem(MRF24J40_RFCON2, 0x80); //enable PLL
    mrf24j40_set_long_add_mem(MRF24J40_RFCON3, 0x00); //set maximum power 0dBm

	/** set up RFCON6
	* 7 = 1 = as suggested by the datasheet
	* 6:5 = '00' = Reserved
	* 4 = '1' recovery from sleep 1 usec
	* 3 = '0' = battery monitor disabled
	* 2:0 = '00' = Reserved
	*/
	mrf24j40_set_long_add_mem(MRF24J40_RFCON6, 0x90);
	
	mrf24j40_set_long_add_mem(MRF24J40_RFCON7, 0x80); //sleep clock = 100kHz
	
	mrf24j40_set_long_add_mem(MRF24J40_RFCON8, 0x10); //as suggested by the datasheet

	mrf24j40_set_long_add_mem(MRF24J40_SLPCON1, 0x21);

	/** Program CCA mode using RSSI */
	mrf24j40_set_short_add_mem(MRF24J40_BBREG2, 0x80);
    
	#ifdef ADD_RSSI_AND_LQI_TO_PACKET
	/** Enable the packet RSSI */
	mrf24j40_set_short_add_mem(MRF24J40_BBREG6, 0x40);
    #endif

	/** Program CCA, RSSI threshold values */
	mrf24j40_set_short_add_mem(MRF24J40_CCAEDTH, 0x60);

	/**
	* wait until the radio state machine is not on rx mode
	*/
	do
	{
        	i = mrf24j40_get_long_add_mem(MRF24J40_RFSTATE);
	}
	while((i&0xA0) != 0xA0);
    
	/**
	*
	*Set interrupts.
	*/
	/**
	* set INTCON
	* 7 = '1' = Disables the Sleep alert interrupt
	* 6 = '1' = Disables the wake-up alert interrupt
	* 5 = '1' = Disables the half symbol timer interrupt
	* 4 = '1' = Disables the security key request interrupt
	* 3 = '0' = Enables the RX FIFO reception interrupt
	* 2 = '1' = Disables the TX GTS2 FIFO transmission interrupt
	* 1 = '1' = Disables the TX GTS1 FIFO transmission interrupt
	* 0 = '0' = Enables the TX Normal FIFO transmission interrupt	
	*/
	//mrf24j40_set_short_add_mem(INTCON,0xF6);

	mrf24j40_set_short_add_mem(MRF24J40_INTCON, int_setup);

	#ifdef INT_POLARITY_HIGH
	/* Set interrupt edge polarity high */
	mrf24j40_set_long_add_mem(MRF24J40_SLPCON0, 0x02);
	#endif
	/**
	Disables automatic Acknowledgement response.
	Receive all packet types with good CRC.
	*/
	//mrf24j40_set_short_add_mem(RXMCR,0x21);
	
	i = 0;

	#ifdef MRF24J40_AUTOMATIC_ACK
		i = i | 0b00100000	
	#endif

	#ifdef MRF24J40_PAN_COORDINATOR
		i = i | 0b00001000;
	#endif
	
	#ifdef MRF24J40_COORDINATOR
		i = i | 0b00000100;
	#endif
	
	#ifdef MRF24J40_ACCEPT_WRONG_CRC_PKT
		i = i | 0b00000010;
	#endif
	
	#ifdef MRF24J40_PROMISCUOUS_MODE
		i = i | 0b00000001;
	#endif	
	
	/*
	 * Set the RXMCR register.
	 * Default setting i=0x00, which means:
	 * - Automatic ACK;
	 * - Device is not a PAN coordinator;
	 * - Device is not a coordinator;
	 * - Accept only packets with good CRC
	 * - Discard packet when there is a MAC address mismatch, 
	 *   illegal frame type, dPAN/sPAN or MAC short address mismatch.
	 * See the datasheet for further information.
	 */
	mrf24j40_set_short_add_mem(MRF24J40_RXMCR, i);
	
	#ifndef MRF24J40_DISABLE_CSMA
	
	#ifdef MRF24J40_BEACON_ENABLED_MODE
	
	#ifdef MRF24J40_BATT_LIFE_EXT
		/*
		 *Set the TXMCR register.
		 * bit 7 = '0';  Enable No Carrier Sense Multiple Access (CSMA) 
		 * Algorithm.
		 * bit 6 = '1';  Enable Battery Life Extension Mode bit.
		 * bit 5 = '1';  Enable Slotted CSMA-CA Mode bit.
		 * bit 4-3 = '11'; MAC Minimum Backoff Exponent bits (macMinBE).
		 * bit 2-0 = '100';  CSMA Backoff bits (macMaxCSMABackoff)
		 */
		mrf24j40_set_short_add_mem(MRF24J40_TXMCR, 0x7C);
	#else
		/*
		 *Set the TXMCR register.
		 * bit 7 = '0';  Enable No Carrier Sense Multiple Access (CSMA) 
		 * Algorithm.
		 * bit 6 = '0';  Disable Battery Life Extension Mode bit.
		 * bit 5 = '1';  Enable Slotted CSMA-CA Mode bit.
		 * bit 4-3 = '11'; MAC Minimum Backoff Exponent bits (macMinBE).
		 * bit 2-0 = '100';  CSMA Backoff bits (macMaxCSMABackoff)
		 */
		mrf24j40_set_short_add_mem(MRF24J40_TXMCR, 0x3C);
	#endif

	#else
	
	#ifdef MRF24J40_BATT_LIFE_EXT
		/*
		 *Set the TXMCR register.
		 * bit 7 = '0';  Enable No Carrier Sense Multiple Access (CSMA) 
		 * Algorithm.
		 * bit 6 = '1';  Enable Battery Life Extension Mode bit.
		 * bit 5 = '0';  Disable Slotted CSMA-CA Mode bit.
		 * bit 4-3 = '11'; MAC Minimum Backoff Exponent bits (macMinBE).
		 * bit 2-0 = '100';  CSMA Backoff bits (macMaxCSMABackoff)
		 */
		mrf24j40_set_short_add_mem(MRF24J40_TXMCR, 0x5C);
	#else
		/*
		 *Set the TXMCR register.
		 * bit 7 = '0';  Enable No Carrier Sense Multiple Access (CSMA) 
		 * Algorithm.
		 * bit 6 = '0';  Disable Battery Life Extension Mode bit.
		 * bit 5 = '0';  Disable Slotted CSMA-CA Mode bit.
		 * bit 4-3 = '11'; MAC Minimum Backoff Exponent bits (macMinBE).
		 * bit 2-0 = '100';  CSMA Backoff bits (macMaxCSMABackoff)
		 */
		mrf24j40_set_short_add_mem(MRF24J40_TXMCR, 0x1C);

	#endif
		
	#endif
	
	#else
		mrf24j40_set_short_add_mem(MRF24J40_TXMCR, 0x80); /* Disable CSMA alg. */
	#endif

	/**
	 * Set TX turn around time as defined by IEEE802.15.4 standard
	 */
	mrf24j40_set_short_add_mem(MRF24J40_TXTIME, 0x30);
	mrf24j40_set_short_add_mem(MRF24J40_TXSTBL, 0x95);
	
	/* 
	 * if TURBO_MODE is defined, the transceiver works
	 * at 625 Kbit/sec.
	 */
	#ifdef TURBO_MODE
	mrf24j40_set_short_add_mem(MRF24J40_BBREG0, 0x01);
    mrf24j40_set_short_add_mem(MRF24J40_BBREG3, 0x38);
    mrf24j40_set_short_add_mem(MRF24J40_BBREG4, 0x5C);
	#endif
	
	/**
	* Reset RF state machine
	*/         
	mrf24j40_set_short_add_mem(MRF24J40_RFCTL, 0x04);        
	mrf24j40_set_short_add_mem(MRF24J40_RFCTL, 0x00);

	return 0;
}


/**
* @brief Store message
*
* This routine stores a buffer of buf_length bytes in the TX_FIFO buffer of the 
* MRF24J40.
*
* @param[in] *buf 	The message pointer
* @param[in] len 	The message lenght
*/
int8_t mrf24j40_store_norm_txfifo(uint8_t* buf, uint8_t len)
{
	uint8_t i;

	if (len > 127 || len <= 0)
		return -1;

	mrf24j40_set_long_add_mem(MRF24J40_NORMAL_TX_FIFO, 1);
	mrf24j40_set_long_add_mem(MRF24J40_NORMAL_TX_FIFO+1, len);

	for (i=0;i<len;i++)
		mrf24j40_set_long_add_mem(MRF24J40_NORMAL_TX_FIFO + 2 + i, buf[i]);
	
	return 0;
}
/**
* @brief Store message
*
* This routine stores a buffer of buf_length bytes in the TX_FIFO buffer of the 
* MRF24J40.
*
* @param[in] *buf 	The message pointer
* @param[in] buf_legth 	The message lenght
*/
uint8_t mrf24j40_get_norm_txfifo(uint8_t pos)
{
	if (pos > 127)
		return 0;
	
	return mrf24j40_get_long_add_mem(MRF24J40_NORMAL_TX_FIFO + pos);
}


/**
* @brief Get message
*
* This routine is used to retrieve a message store in the RX_FIFO
*
* @param[in] *msg 	The buffer where to store the message from the RX_FIFO
*
* @return the lenght of the received message
*
* Note: the last two bytes, representing the CRC code, are discarded.
 */
uint8_t mrf24j40_get_fifo_msg(uint8_t *msg)
{
	uint8_t i, len;

	#ifdef MRF24J40_PROMISCUOUS_MODE
 	/* 
	* Flush RX FIFO as suggested by the work around 1 in 
	* MRF24J40 Silicon Errata.
	*/
	mrf24j40_set_short_add_mem(MRF24J40_RXFLUSH,0x01);
	#endif

	/*Disable packet reception*/
	mrf24j40_set_short_add_mem(MRF24J40_BBREG1, 0x04);
	
	/* Get packet length */
	len = mrf24j40_get_long_add_mem(MRF24J40_RX_FIFO);

	#ifndef MRF24J40_ADD_RSSI_AND_LQI_TO_PACKET
	/* 
	 * Note: the last two bytes of the message represent 
	 * CRC, hence, we discard them.
	 */
	len -= 2;
	#endif

	/* Get the packet */
	for (i=0;i < len; i++)
		msg[i] = mrf24j40_get_long_add_mem(MRF24J40_RX_FIFO + 1 + i);
	 
	/*Enable packet reception*/
	mrf24j40_set_short_add_mem(MRF24J40_BBREG1, 0x00);
	
	return len;
}


/**
* @brief Start sleep
*
* This routine puts the radio in sleep mode.
*
*
*/
void mrf24j40_put_to_sleep() 
{
	/* Prepare wake pin */
	#ifdef MRF24J40_USE_WAKE_PIN
	MRF24J40_WAKE = 0;
	#else
	mrf24j40_set_short_add_mem(MRF24J40_RXFLUSH, 0x60);
	mrf24j40_set_short_add_mem(MRF24J40_WAKECON, 0x80);
	#endif
	
	/* Put to sleep*/
	mrf24j40_set_short_add_mem(MRF24J40_SOFTRST, 0x04);
	mrf24j40_set_short_add_mem(MRF24J40_SLPACK, 0x80);
}
/**
* @brief Awake the radio
*
* This routine turns on a sets the radio on receiving mode.
*
* Note:   After performing this routine the radio is in the receiving  state.
*
* Warning:      If no radio is present the system is blocked in a infinite loop!
*/
void mrf24j40_wake() 
{
	#ifdef MRF24J40_USE_WAKE_PIN
	MRF24J40_WAKE = 1;
	#else
	mrf24j40_set_short_add_mem(MRF24J40_WAKECON, 0xC0);
	mrf24j40_set_short_add_mem(MRF24J40_WAKECON, 0x80);
	#endif
	mrf24j40_delay_us(2000); /* Delay for 2 ms */

	/* Wait until the transceiver is not on receiving mode */
	while (mrf24j40_get_long_add_mem(MRF24J40_RFSTATE) != 0xA0 );
}

COMPILER_ISR(MRF24J40_INTERRUPT_NAME)
{
	MRF24J40_INTERRUPT_FLAG = 0;
	if (rx_callback != NULL) 
		rx_callback();
}
