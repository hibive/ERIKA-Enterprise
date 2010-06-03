/*********************************************************************
 *
 *            ENC28J60 registers/bits
 *
 *********************************************************************
 * FileName:        ENC28J60.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		06/01/04	Original
 * Howard Schlunder		06/29/04	Fixed byte boundary problems on a 
 *									couple of PHY register structs.
 * Howard Schlunder		09/29/04	Matched with data sheet
 * Howard Schlunder		01/04/06	Matched with new data sheet
 * Howard Schlunder		06/29/06	Changed MACON3.PHDRLEN to PHDREN
 * Howard Schlunder		07/21/06	Several bits removed to match now 
 *									reserved bits in rev. B data sheet 
 *									(DS39662B)
 ********************************************************************/

#ifndef __ENC28J60_H
#define __ENC28J60_H

#include "enc28j60_compiler.h"
#include "enc28j60_hal.h"


/* Generic Type definitions (#include "GenericTypeDefs.h", not allowed...) */	

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;	// Undefined size

#ifndef NULL
#define NULL    	0 //((void *)0)
#endif

#define	PUBLIC                                  // Function attributes
#define PROTECTED
#define PRIVATE   	static

#if(0)
typedef uint64_t	QWORD;				// 64-bit unsigned
typedef int8_t		CHAR;				// 8-bit signed
typedef int16_t		SHORT;				// 16-bit signed
typedef int32_t		LONG;				// 32-bit signed
typedef int64_t		LONGLONG;			// 64-bit signed
/* Alternate definitions */
typedef void		VOID;
typedef int8_t		CHAR8;
typedef uint8_t		UCHAR8;
/* Processor & Compiler independent, size specific definitions */
// To Do:  We need to verify the sizes on each compiler.  These
//         may be compiler specific, we should either move them
//         to "compiler.h" or #ifdef them for compiler type.
typedef signed int          INT;
typedef signed char         INT8;
typedef signed short int    INT16;
typedef signed long int     INT32;
typedef signed long long    INT64;
typedef unsigned int        UINT;
typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;
typedef unsigned long int   UINT32;  // other name for 32-bit integer
typedef unsigned long long  UINT64;
#endif

typedef uint8_t		BYTE;				// 8-bit unsigned
typedef uint16_t	WORD;				// 16-bit unsigned
typedef uint32_t	DWORD;				// 32-bit unsigned

#ifdef BIG_ENDIAN
/* BYTE_VAL */
typedef union _BYTE_VAL
{
    BYTE Val;
    struct
    {
        unsigned char b7:1;
        unsigned char b6:1;
        unsigned char b5:1;
        unsigned char b4:1;
        unsigned char b3:1;
        unsigned char b2:1;
        unsigned char b1:1;
        unsigned char b0:1;
    } bits;
} BYTE_VAL;
#else
typedef union _BYTE_VAL
{
    BYTE Val;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
    } bits;
} BYTE_VAL, BYTE_BITS;
#endif

#ifdef BIG_ENDIAN
/* WORD_VAL */
typedef union _WORD_VAL
{
    WORD Val;
    BYTE v[2];
    struct
    {
        BYTE HB;
        BYTE LB;
    } byte;
    struct
    {
        unsigned char b15:1;
        unsigned char b14:1;
        unsigned char b13:1;
        unsigned char b12:1;
        unsigned char b11:1;
        unsigned char b10:1;
        unsigned char b9:1;
        unsigned char b8:1;
        unsigned char b7:1;
        unsigned char b6:1;
        unsigned char b5:1;
        unsigned char b4:1;
        unsigned char b3:1;
        unsigned char b2:1;
        unsigned char b1:1;
        unsigned char b0:1;
    } bits;
} WORD_VAL;
#else
typedef union _WORD_VAL
{
    WORD Val;
    BYTE v[2];
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
    } bits;
} WORD_VAL, WORD_BITS;
#endif

/* DWORD_VAL */
typedef union _DWORD_VAL
{
    DWORD Val;
	WORD w[2];
    BYTE v[4];
} DWORD_VAL;

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
/* WRAPPER */
#define RESERVED_SSL_MEMORY 	0ul		// ATT!!! this macro is server dependent... (es. STACK_USE_SSL for Microchip TCPIP stack)
#define RESERVED_HTTP_MEMORY 	0ul		// ATT!!! this macro is server dependent... (es. STACK_USE_HTTP2_SERVER for Microchip TCPIP stack)
#define RAMSIZE					(8*1024ul)
#define TXSTART 				(RAMSIZE - (1ul+1518ul+7ul) - TCP_ETH_RAM_SIZE - RESERVED_HTTP_MEMORY - RESERVED_SSL_MEMORY)
#define RXSTART					(0ul)						// Should be an even memory address; must be 0 for errata
#define	RXSTOP					((TXSTART-2ul) | 0x0001ul)	// Odd for errata workaround
#define RXSIZE					(RXSTOP-RXSTART+1ul)
#define TCP_ETH_RAM_SIZE 		0u
#define ENC_MAX_SPI_FREQ    	(20000000ul)	// SPI max Hz
#define IP_ADDR					DWORD_VAL
#define SetLEDConfig(NewConfig)	WritePHYReg(PHLCON, NewConfig)
#define MAC_IP      			(0x00u)
#define MAC_UNKNOWN 			(0xFFu)
typedef struct __attribute__((__packed__))
{
    BYTE v[6];
} MAC_ADDR;
typedef struct  __attribute__((aligned(2), packed))		// A generic structure representing the
{														// Ethernet header starting all Ethernet frames
	MAC_ADDR        DestMACAddr;
	MAC_ADDR        SourceMACAddr;
	WORD_VAL        Type;
} ETHER_HEADER;
typedef struct __attribute__((__packed__)) 
{
	IP_ADDR		MyIPAddr;
	IP_ADDR		MyMask;
	IP_ADDR		MyGateway;
	IP_ADDR		PrimaryDNSServer;
	IP_ADDR		SecondaryDNSServer;
	IP_ADDR		DefaultIPAddr;
	IP_ADDR		DefaultMask;
	BYTE		NetBIOSName[16];
	struct
	{
		unsigned char : 5;
        unsigned char bConfigureAutoIP : 1;
		unsigned char bIsDHCPEnabled : 1;
		unsigned char bInConfigMode : 1;
	} Flags;
	MAC_ADDR	MyMACAddr;

#if defined(ZG_CS_TRIS)
	BYTE		MySSID[32];
#endif
	
#if defined(STACK_USE_SNMP_SERVER)
	// SNMPv2C Read community names
	// SNMP_COMMUNITY_MAX_LEN (8) + 1 null termination byte
	BYTE readCommunity[SNMP_MAX_COMMUNITY_SUPPORT][SNMP_COMMUNITY_MAX_LEN+1]; 

	// SNMPv2C Write community names
	// SNMP_COMMUNITY_MAX_LEN (8) + 1 null termination byte
	BYTE writeCommunity[SNMP_MAX_COMMUNITY_SUPPORT][SNMP_COMMUNITY_MAX_LEN+1];
#endif

} APP_CONFIG;


/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */




/** D E F I N I T I O N S ****************************************************/
// IMPORTANT SPI NOTE: The code in this file expects that the SPI interrupt
//      flag (ENC_SPI_IF) be clear at all times.  If the SPI is shared with
//      other hardware, the other code should clear the ENC_SPI_IF when it is
//      done using the SPI.
// Since the ENC28J60 doesn't support auto-negotiation, full-duplex mode is
// not compatible with most switches/routers.  If a dedicated network is used
// where the duplex of the remote node can be manually configured, you may
// change this configuration.  Otherwise, half duplex should always be used.
#define HALF_DUPLEX
//#define FULL_DUPLEX
//#define LEDB_DUPLEX
// Pseudo Functions
#define LOW(a)               	((a) & 0xFF)
#define HIGH(a)              	(((a)>>8) & 0xFF)
// ENC28J60 Opcodes (to be ORed with a 5 bit address)
#define ENC28J60_WCR (0x2<<5)           // Write Control Register command
#define ENC28J60_BFS (0x4<<5)           // Bit Field Set command
#define ENC28J60_BFC (0x5<<5)           // Bit Field Clear command
#define ENC28J60_RCR (0x0<<5)           // Read Control Register command
#define ENC28J60_RBM ((0x1<<5) | 0x1A)  // Read Buffer Memory command
#define ENC28J60_WBM ((0x3<<5) | 0x1A) 	// Write Buffer Memory command
#define ENC28J60_SR  ((0x7<<5) | 0x1F)  // System Reset command does not use an address.
										// It requires 0x1F, however.
#define ETHER_IP    			(0x00u)
#define ETHER_ARP   			(0x06u)

#ifdef BIG_ENDIAN
typedef union {
	BYTE v[7];
	struct
    {
        BYTE HB;
		BYTE B6;
		BYTE B5;
		BYTE B4;
		BYTE B3;
		BYTE B2;
        BYTE LB;
    } byte;
	struct {
		unsigned char	Zeros:4;
		unsigned char	VLANTaggedFrame:1;
		unsigned char	BackpressureApplied:1;
		unsigned char	PAUSEControlFrame:1;
		unsigned char	ControlFrame:1;
		WORD 	 		BytesTransmittedOnWire;
		unsigned char	Underrun:1;
		unsigned char	Giant:1;
		unsigned char	LateCollision:1;
		unsigned char	MaximumCollisions:1;
		unsigned char	ExcessiveDefer:1;
		unsigned char	PacketDefer:1;
		unsigned char	Broadcast:1;
		unsigned char	Multicast:1;
		unsigned char	Done:1;
		unsigned char	LengthOutOfRange:1;
		unsigned char	LengthCheckError:1;
		unsigned char	CRCError:1;
		unsigned char	CollisionCount:4;
		WORD	 		ByteCount;
	} bits;
} TXSTATUS;

typedef union {
	BYTE v[4];
	struct
    {
        BYTE HB;
		BYTE B3;
		BYTE B2;
        BYTE LB;
    } byte;
	struct {
		unsigned char	Zero:1;
		unsigned char	VLANType:1;
		unsigned char	UnsupportedOpcode:1;
		unsigned char	PauseControlFrame:1;
		unsigned char	ControlFrame:1;
		unsigned char	DribbleNibble:1;
		unsigned char	Broadcast:1;
		unsigned char	Multicast:1;
		unsigned char	ReceiveOk:1;
		unsigned char	LengthOutOfRange:1;
		unsigned char	LengthCheckError:1;
		unsigned char	CRCError:1;
		unsigned char	CodeViolation:1;
		unsigned char	CarrierPreviouslySeen:1;
		unsigned char	RXDCPreviouslySeen:1;
		unsigned char	PreviouslyIgnored:1;
		WORD	 		ByteCount;
	} bits;
} RXSTATUS;

#else	//#ifdef BIG_ENDIAN

typedef union {
	BYTE v[7];
	struct
    {
        BYTE LB;
		BYTE B2;
		BYTE B3;
		BYTE B4;
		BYTE B5;
		BYTE B6;
        BYTE HB;
    } byte;
	struct {
		WORD	 		ByteCount;
		unsigned char	CollisionCount:4;
		unsigned char	CRCError:1;
		unsigned char	LengthCheckError:1;
		unsigned char	LengthOutOfRange:1;
		unsigned char	Done:1;
		unsigned char	Multicast:1;
		unsigned char	Broadcast:1;
		unsigned char	PacketDefer:1;
		unsigned char	ExcessiveDefer:1;
		unsigned char	MaximumCollisions:1;
		unsigned char	LateCollision:1;
		unsigned char	Giant:1;
		unsigned char	Underrun:1;
		WORD 	 		BytesTransmittedOnWire;
		unsigned char	ControlFrame:1;
		unsigned char	PAUSEControlFrame:1;
		unsigned char	BackpressureApplied:1;
		unsigned char	VLANTaggedFrame:1;
		unsigned char	Zeros:4;
	} bits;
} TXSTATUS;

typedef union {
	BYTE v[4];
	struct
    {
        BYTE LB;
		BYTE B2;
		BYTE B3;
        BYTE HB;
    } byte;
	struct {
		WORD	 		ByteCount;
		unsigned char	PreviouslyIgnored:1;
		unsigned char	RXDCPreviouslySeen:1;
		unsigned char	CarrierPreviouslySeen:1;
		unsigned char	CodeViolation:1;
		unsigned char	CRCError:1;
		unsigned char	LengthCheckError:1;
		unsigned char	LengthOutOfRange:1;
		unsigned char	ReceiveOk:1;
		unsigned char	Multicast:1;
		unsigned char	Broadcast:1;
		unsigned char	DribbleNibble:1;
		unsigned char	ControlFrame:1;
		unsigned char	PauseControlFrame:1;
		unsigned char	UnsupportedOpcode:1;
		unsigned char	VLANType:1;
		unsigned char	Zero:1;
	} bits;
} RXSTATUS;
#endif
/******************************************************************************
* Register locations
******************************************************************************/
// Bank 0 registers --------
#define ERDPTL		0x00
#define ERDPTH		0x01
#define EWRPTL		0x02
#define EWRPTH		0x03
#define ETXSTL		0x04
#define ETXSTH		0x05
#define ETXNDL		0x06
#define ETXNDH		0x07
#define ERXSTL		0x08
#define ERXSTH		0x09
#define ERXNDL		0x0A
#define ERXNDH		0x0B
#define ERXRDPTL	0x0C
#define ERXRDPTH	0x0D
#define ERXWRPTL	0x0E
#define ERXWRPTH	0x0F
#define EDMASTL		0x10
#define EDMASTH		0x11
#define EDMANDL		0x12
#define EDMANDH		0x13
#define EDMADSTL	0x14
#define EDMADSTH	0x15
#define EDMACSL		0x16
#define EDMACSH		0x17
//#define			0x18
//#define			0x19
//#define r			0x1A
#define EIE			0x1B
#define EIR			0x1C
#define ESTAT		0x1D
#define ECON2		0x1E
#define ECON1		0x1F

// Bank 1 registers -----
#define EHT0		0x100
#define EHT1		0x101
#define EHT2		0x102
#define EHT3		0x103
#define EHT4		0x104
#define EHT5		0x105
#define EHT6		0x106
#define EHT7		0x107
#define EPMM0		0x108
#define EPMM1		0x109
#define EPMM2		0x10A
#define EPMM3		0x10B
#define EPMM4		0x10C
#define EPMM5		0x10D
#define EPMM6		0x10E
#define EPMM7		0x10F
#define EPMCSL		0x110
#define EPMCSH		0x111
//#define			0x112
//#define			0x113
#define EPMOL		0x114
#define EPMOH		0x115
//#define r			0x116
//#define r			0x117
#define ERXFCON		0x118
#define EPKTCNT		0x119
//#define r			0x11A
//#define EIE		0x11B
//#define EIR		0x11C
//#define ESTAT		0x11D
//#define ECON2		0x11E
//#define ECON1		0x11F

// Bank 2 registers -----
#define MACON1		0x200
//#define r			0x201
#define MACON3		0x202
#define MACON4		0x203
#define MABBIPG		0x204
//#define			0x205
#define MAIPGL		0x206
#define MAIPGH		0x207
#define MACLCON1	0x208
#define MACLCON2	0x209
#define MAMXFLL		0x20A
#define MAMXFLH		0x20B
//#define r			0x20C
//#define r			0x20D
//#define r			0x20E
//#define			0x20F
//#define r			0x210
//#define r			0x211
#define MICMD		0x212
//#define r			0x213
#define MIREGADR	0x214
//#define r			0x215
#define MIWRL		0x216
#define MIWRH		0x217
#define MIRDL		0x218
#define MIRDH		0x219
//#define r			0x21A
//#define EIE		0x21B
//#define EIR		0x21C
//#define ESTAT		0x21D
//#define ECON2		0x21E
//#define ECON1		0x21F

// Bank 3 registers -----
#define MAADR5		0x300
#define MAADR6		0x301
#define MAADR3		0x302
#define MAADR4		0x303
#define MAADR1		0x304
#define MAADR2		0x305
#define EBSTSD		0x306
#define EBSTCON		0x307
#define EBSTCSL		0x308
#define EBSTCSH		0x309
#define MISTAT		0x30A
//#define			0x30B
//#define			0x30C
//#define			0x30D
//#define			0x30E
//#define			0x30F
//#define			0x310
//#define			0x311
#define EREVID		0x312
//#define			0x313
//#define			0x314
#define ECOCON		0x315
//#define 			0x316
#define EFLOCON		0x317
#define EPAUSL		0x318
#define EPAUSH		0x319
//#define r			0x31A
//#define EIE		0x31B
//#define EIR		0x31C
//#define ESTAT		0x31D
//#define ECON2		0x31E
//#define ECON1		0x31F



// Structures
typedef union _REG 
{
	BYTE Val;

	// EIE bits ----------
	struct {
		unsigned char RXERIE:1;
		unsigned char TXERIE:1;
		unsigned char :1;
		unsigned char TXIE:1;
		unsigned char LINKIE:1;
		unsigned char DMAIE:1;
		unsigned char PKTIE:1;
		unsigned char INTIE:1;
	} EIEbits;

	// EIR bits ----------
	struct {
		unsigned char RXERIF:1;
		unsigned char TXERIF:1;
		unsigned char :1;
		unsigned char TXIF:1;
		unsigned char LINKIF:1;
		unsigned char DMAIF:1;
		unsigned char PKTIF:1;
		unsigned char :1;
	} EIRbits;

	// ESTAT bits ---------
	struct {
		unsigned char CLKRDY:1;
		unsigned char TXABRT:1;
		unsigned char RXBUSY:1;
		unsigned char :1;
		unsigned char LATECOL:1;
		unsigned char :1;
		unsigned char BUFER:1;
		unsigned char INT:1;
	} ESTATbits;

	// ECON2 bits --------
	struct {
		unsigned char :3;
		unsigned char VRPS:1;
		unsigned char :1;
		unsigned char PWRSV:1;
		unsigned char PKTDEC:1;
		unsigned char AUTOINC:1;
	} ECON2bits;
		
	// ECON1 bits --------
	struct {
		unsigned char BSEL0:1;
		unsigned char BSEL1:1;
		unsigned char RXEN:1;
		unsigned char TXRTS:1;
		unsigned char CSUMEN:1;
		unsigned char DMAST:1;
		unsigned char RXRST:1;
		unsigned char TXRST:1;
	} ECON1bits;
		
	// ERXFCON bits ------
	struct {
		unsigned char BCEN:1;
		unsigned char MCEN:1;
		unsigned char HTEN:1;
		unsigned char MPEN:1;
		unsigned char PMEN:1;
		unsigned char CRCEN:1;
		unsigned char ANDOR:1;
		unsigned char UCEN:1;
	} ERXFCONbits;
		
	// MACON1 bits --------
	struct {
		unsigned char MARXEN:1;
		unsigned char PASSALL:1;
		unsigned char RXPAUS:1;
		unsigned char TXPAUS:1;
		unsigned char :4;
	} MACON1bits;
		

	// MACON3 bits --------
	struct {
		unsigned char FULDPX:1;
		unsigned char FRMLNEN:1;
		unsigned char HFRMEN:1;
		unsigned char PHDREN:1;
		unsigned char TXCRCEN:1;
		unsigned char PADCFG0:1;
		unsigned char PADCFG1:1;
		unsigned char PADCFG2:1;
	} MACON3bits;
	struct {
		unsigned char FULDPX:1;
		unsigned char FRMLNEN:1;
		unsigned char HFRMEN:1;
		unsigned char PHDREN:1;
		unsigned char TXCRCEN:1;
		unsigned char PADCFG:3;
	} MACON3bits2;
		
	// MACON4 bits --------
	struct {
		unsigned char :4;
		unsigned char NOBKOFF:1;
		unsigned char BPEN:1;
		unsigned char DEFER:1;
		unsigned char :1;
	} MACON4bits;
		
	// MICMD bits ---------
	struct {
		unsigned char MIIRD:1;
		unsigned char MIISCAN:1;
		unsigned char :6;
	} MICMDbits;

	// EBSTCON bits -----
	struct {
		unsigned char BISTST:1;
		unsigned char TME:1;
		unsigned char TMSEL0:1;
		unsigned char TMSEL1:1;
		unsigned char PSEL:1;
		unsigned char PSV0:1;
		unsigned char PSV1:1;
		unsigned char PSV2:1;
	} EBSTCONbits;
	struct {
		unsigned char BISTST:1;
		unsigned char TME:1;
		unsigned char TMSEL:2;
		unsigned char PSEL:1;
		unsigned char PSV:3;
	} EBSTCONbits2;
		
	// MISTAT bits --------
	struct {
		unsigned char BUSY:1;
		unsigned char SCAN:1;
		unsigned char NVALID:1;
		unsigned char :5;
	} MISTATbits;
		
	// ECOCON bits -------
	struct {
		unsigned char COCON0:1;
		unsigned char COCON1:1;
		unsigned char COCON2:1;
		unsigned char :5;
	} ECOCONbits;
	struct {
		unsigned char COCON:3;
		unsigned char :5;
	} ECOCONbits2;
		
	// EFLOCON bits -----
	struct {
		unsigned char FCEN0:1;
		unsigned char FCEN1:1;
		unsigned char FULDPXS:1;
		unsigned char :5;
	} EFLOCONbits;
	struct {
		unsigned char FCEN:2;
		unsigned char FULDPXS:1;
		unsigned char :5;
	} EFLOCONbits2;
} REG;


/******************************************************************************
* PH Register Locations
******************************************************************************/
#define PHCON1	0x00
#define PHSTAT1	0x01
#define PHID1	0x02
#define PHID2	0x03
#define PHCON2	0x10
#define PHSTAT2	0x11
#define PHIE	0x12
#define PHIR	0x13
#define PHLCON	0x14


typedef union {
	WORD Val;
	WORD_VAL VAL;

	// PHCON1 bits ----------
	struct {
		unsigned :8;
		unsigned PDPXMD:1;
		unsigned :2;
		unsigned PPWRSV:1;
		unsigned :2;
		unsigned PLOOPBK:1;
		unsigned PRST:1;
	} PHCON1bits;

	// PHSTAT1 bits --------
	struct {
		unsigned :1;
		unsigned JBSTAT:1;
		unsigned LLSTAT:1;
		unsigned :5;
		unsigned :3;
		unsigned PHDPX:1;
		unsigned PFDPX:1;
		unsigned :3;
	} PHSTAT1bits;

	// PHID2 bits ----------
	struct {
		unsigned PREV0:1;
		unsigned PREV1:1;
		unsigned PREV2:1;
		unsigned PREV3:1;
		unsigned PPN0:1;
		unsigned PPN1:1;
		unsigned PPN2:1;
		unsigned PPN3:1;
		unsigned PPN4:1;
		unsigned PPN5:1;
		unsigned PID19:1;
		unsigned PID20:1;
		unsigned PID21:1;
		unsigned PID22:1;
		unsigned PID23:1;
		unsigned PID24:1;
	} PHID2bits;
	struct {
		unsigned PREV:4;
		unsigned PPNL:4;
		unsigned PPNH:2;
		unsigned PID:6;
	} PHID2bits2;

	// PHCON2 bits ----------
	struct {
		unsigned :8;
		unsigned HDLDIS:1;
		unsigned :1;
		unsigned JABBER:1;
		unsigned :2;
		unsigned TXDIS:1;
		unsigned FRCLNK:1;
		unsigned :1;
	} PHCON2bits;

	// PHSTAT2 bits --------
	struct {
		unsigned :5;
		unsigned PLRITY:1;
		unsigned :2;
		unsigned :1;
		unsigned DPXSTAT:1;
		unsigned LSTAT:1;
		unsigned COLSTAT:1;
		unsigned RXSTAT:1;
		unsigned TXSTAT:1;
		unsigned :2;
	} PHSTAT2bits;

	// PHIE bits -----------
	struct {
		unsigned :1;
		unsigned PGEIE:1;
		unsigned :2;
		unsigned PLNKIE:1;
		unsigned :3;
		unsigned :8;
	} PHIEbits;

	// PHIR bits -----------
	struct {
		unsigned :2;
		unsigned PGIF:1;
		unsigned :1;
		unsigned PLNKIF:1;
		unsigned :3;
		unsigned :8;
	} PHIRbits;

	// PHLCON bits -------
	struct {
		unsigned :1;
		unsigned STRCH:1;
		unsigned LFRQ0:1;
		unsigned LFRQ1:1;
		unsigned LBCFG0:1;
		unsigned LBCFG1:1;
		unsigned LBCFG2:1;
		unsigned LBCFG3:1;
		unsigned LACFG0:1;
		unsigned LACFG1:1;
		unsigned LACFG2:1;
		unsigned LACFG3:1;
		unsigned :4;
	} PHLCONbits;
	struct {
		unsigned :1;
		unsigned STRCH:1;
		unsigned LFRQ:2;
		unsigned LBCFG:4;
		unsigned LACFG:4;
		unsigned :4;
	} PHLCONbits2;
} PHYREG;


/******************************************************************************
* Individual Register Bits
******************************************************************************/
// ETH/MAC/MII bits

// EIE bits ----------
#define	EIE_INTIE		(1<<7)
#define	EIE_PKTIE		(1<<6)
#define	EIE_DMAIE		(1<<5)
#define	EIE_LINKIE		(1<<4)
#define	EIE_TXIE		(1<<3)
#define	EIE_TXERIE		(1<<1)
#define	EIE_RXERIE		(1)

// EIR bits ----------
#define	EIR_PKTIF		(1<<6)
#define	EIR_DMAIF		(1<<5)
#define	EIR_LINKIF		(1<<4)
#define	EIR_TXIF		(1<<3)
#define	EIR_TXERIF		(1<<1)
#define	EIR_RXERIF		(1)
	
// ESTAT bits ---------
#define	ESTAT_INT		(1<<7)
#define ESTAT_BUFER		(1<<6)
#define	ESTAT_LATECOL	(1<<4)
#define	ESTAT_RXBUSY	(1<<2)
#define	ESTAT_TXABRT	(1<<1)
#define	ESTAT_CLKRDY	(1)
	
// ECON2 bits --------
#define	ECON2_AUTOINC	(1<<7)
#define	ECON2_PKTDEC	(1<<6)
#define	ECON2_PWRSV		(1<<5)
#define	ECON2_VRPS		(1<<3)
	
// ECON1 bits --------
#define	ECON1_TXRST		(1<<7)
#define	ECON1_RXRST		(1<<6)
#define	ECON1_DMAST		(1<<5)
#define	ECON1_CSUMEN	(1<<4)
#define	ECON1_TXRTS		(1<<3)
#define	ECON1_RXEN		(1<<2)
#define	ECON1_BSEL1		(1<<1)
#define	ECON1_BSEL0		(1)
	
// ERXFCON bits ------
#define	ERXFCON_UCEN	(1<<7)
#define	ERXFCON_ANDOR	(1<<6)
#define	ERXFCON_CRCEN	(1<<5)
#define	ERXFCON_PMEN	(1<<4)
#define	ERXFCON_MPEN	(1<<3)
#define	ERXFCON_HTEN	(1<<2)
#define	ERXFCON_MCEN	(1<<1)
#define	ERXFCON_BCEN	(1)
	
// MACON1 bits --------
#define	MACON1_TXPAUS	(1<<3)
#define	MACON1_RXPAUS	(1<<2)
#define	MACON1_PASSALL	(1<<1)
#define	MACON1_MARXEN	(1)
	
// MACON3 bits --------
#define	MACON3_PADCFG2	(1<<7)
#define	MACON3_PADCFG1	(1<<6)
#define	MACON3_PADCFG0	(1<<5)
#define	MACON3_TXCRCEN	(1<<4)
#define	MACON3_PHDREN	(1<<3)
#define	MACON3_HFRMEN	(1<<2)
#define	MACON3_FRMLNEN	(1<<1)
#define	MACON3_FULDPX	(1)
	
// MACON4 bits --------
#define	MACON4_DEFER	(1<<6)
#define	MACON4_BPEN		(1<<5)
#define	MACON4_NOBKOFF	(1<<4)
	
// MICMD bits ---------
#define	MICMD_MIISCAN	(1<<1)
#define	MICMD_MIIRD		(1)

// EBSTCON bits -----
#define	EBSTCON_PSV2	(1<<7)
#define	EBSTCON_PSV1	(1<<6)
#define	EBSTCON_PSV0	(1<<5)
#define	EBSTCON_PSEL	(1<<4)
#define	EBSTCON_TMSEL1	(1<<3)
#define	EBSTCON_TMSEL0	(1<<2)
#define	EBSTCON_TME		(1<<1)
#define	EBSTCON_BISTST	(1)

// MISTAT bits --------
#define	MISTAT_NVALID	(1<<2)
#define	MISTAT_SCAN		(1<<1)
#define	MISTAT_BUSY		(1)
	
// ECOCON bits -------
#define	ECOCON_COCON2	(1<<2)
#define	ECOCON_COCON1	(1<<1)
#define	ECOCON_COCON0	(1)
	
// EFLOCON bits -----
#define	EFLOCON_FULDPXS	(1<<2)
#define	EFLOCON_FCEN1	(1<<1)
#define	EFLOCON_FCEN0	(1)



// PHY bits

// PHCON1 bits ----------
#define	PHCON1_PRST		(1ul<<15)
#define	PHCON1_PLOOPBK	(1ul<<14)
#define	PHCON1_PPWRSV	(1ul<<11)
#define	PHCON1_PDPXMD	(1ul<<8)

// PHSTAT1 bits --------
#define	PHSTAT1_PFDPX	(1ul<<12)
#define	PHSTAT1_PHDPX	(1ul<<11)
#define	PHSTAT1_LLSTAT	(1ul<<2)
#define	PHSTAT1_JBSTAT	(1ul<<1)

// PHID2 bits --------
#define	PHID2_PID24		(1ul<<15)
#define	PHID2_PID23		(1ul<<14)
#define	PHID2_PID22		(1ul<<13)
#define	PHID2_PID21		(1ul<<12)
#define	PHID2_PID20		(1ul<<11)
#define	PHID2_PID19		(1ul<<10)
#define	PHID2_PPN5		(1ul<<9)
#define	PHID2_PPN4		(1ul<<8)
#define	PHID2_PPN3		(1ul<<7)
#define	PHID2_PPN2		(1ul<<6)
#define	PHID2_PPN1		(1ul<<5)
#define	PHID2_PPN0		(1ul<<4)
#define	PHID2_PREV3		(1ul<<3)
#define	PHID2_PREV2		(1ul<<2)
#define	PHID2_PREV1		(1ul<<1)
#define	PHID2_PREV0		(1ul)

// PHCON2 bits ----------
#define	PHCON2_FRCLNK	(1ul<<14)
#define	PHCON2_TXDIS	(1ul<<13)
#define	PHCON2_JABBER	(1ul<<10)
#define	PHCON2_HDLDIS	(1ul<<8)

// PHSTAT2 bits --------
#define	PHSTAT2_TXSTAT	(1ul<<13)
#define	PHSTAT2_RXSTAT	(1ul<<12)
#define	PHSTAT2_COLSTAT	(1ul<<11)
#define	PHSTAT2_LSTAT	(1ul<<10)
#define	PHSTAT2_DPXSTAT	(1ul<<9)
#define	PHSTAT2_PLRITY	(1ul<<5)

// PHIE bits -----------
#define	PHIE_PLNKIE		(1ul<<4)
#define	PHIE_PGEIE		(1ul<<1)

// PHIR bits -----------
#define	PHIR_PLNKIF		(1ul<<4)
#define	PHIR_PGIF		(1ul<<2)

// PHLCON bits -------
#define	PHLCON_LACFG3	(1ul<<11)
#define	PHLCON_LACFG2	(1ul<<10)
#define	PHLCON_LACFG1	(1ul<<9)
#define	PHLCON_LACFG0	(1ul<<8)
#define	PHLCON_LBCFG3	(1ul<<7)
#define	PHLCON_LBCFG2	(1ul<<6)
#define	PHLCON_LBCFG1	(1ul<<5)
#define	PHLCON_LBCFG0	(1ul<<4)
#define	PHLCON_LFRQ1	(1ul<<3)
#define	PHLCON_LFRQ0	(1ul<<2)
#define	PHLCON_STRCH	(1ul<<1)

#endif
