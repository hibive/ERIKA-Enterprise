/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2006-2010  Simone Mannori, Roberto Bucher
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

/** 
	@brief www.scicos.org, www.scicoslab.org
	@author Roberto Bucher, SUPSI- Lugano
	@author Simone Mannori, ScicosLab developer
	@date 2006-2010
*/
 
 
#include <stdio.h>
#include <string.h>

#include "machine.h"
extern  int C2F(cvstr)  __PARAMS((integer *,integer *,char *,integer *,unsigned long int));
extern void sciprint __PARAMS((char *fmt,...));
extern void mget2 __PARAMS((FILE *fa, integer swap, double *res, integer n, char *type, integer *ierr));


int worldsize(type)
char type[4];
{
  char c;
  int wsz;

  c=type[0];
  if (c=='u') c=type[1];
  switch ( c )
    {
    case 'l' : wsz=sizeof(long);
      break;
    case 's' : wsz=sizeof(short);
      break;
    case 'c' : wsz=sizeof(char);
      break;
    case 'd' : wsz=sizeof(double);
      break;
    case 'f' : wsz=sizeof(float); 
      break;
    }
  return wsz;
}
void 
readc(flag,nevprt,t,xd,x,nx,z,nz,tvec,ntvec,rpar,nrpar,
	       ipar,nipar,inptr,insz,nin,outptr,outsz,nout)
     /*
     ipar[1]   = lfil : file name length
     ipar[2:4] = fmt  : numbers type ascii code
     ipar[5]   = is there a time record
     ipar[6]   = n : buffer length in number of records
     ipar[7]   = maxvoie : record size
     ipar[8]   = swap
     ipar[9]   = first : first record to read
     ipar[10:9+lfil] = character codes for file name
     ipar[10+lfil:9+lfil++ny+ievt] = reading mask
     */
integer *flag,*nevprt,*nx,*nz,*ntvec,*nrpar,ipar[],*nipar;
integer insz[],*nin,outsz[],*nout;
double x[],xd[],z[],tvec[],rpar[];
double *inptr[],*outptr[],*t;

{
  char str[100],type[4];
  int job = 1,three=3;
  FILE *fd;
  int n, k, ievt, kmax,/* no,*//* lfil,*/ m, i, irep,/* nm,*/ ierr;
  double *buffer,*record;
  int *mask;
  long offset;
  

  --ipar;
  --z;
  fd=(FILE *)(long)z[3];
  buffer = (z+4);
  mask = ipar+11+ipar[1]-ipar[5];
    
  /*
    k    : record counter within the buffer
    kmax :  number of records in the buffer
  */

  if (*flag==1) {
    n    = ipar[6];
    ievt = ipar[5];
    k    = (int)z[1];
    /* copy current record to output */
    record=buffer+(k-1)*ipar[7]-1;

    for (i=0;i<outsz[0];i++)
      *(outptr[0]+i)=record[mask[ievt+i]];

    if (*nevprt>0) {
      /*     discrete state */
      kmax = (int)z[2];
      if (k>=kmax&&kmax==n) {
	/*     read a new buffer */
	m=ipar[6]*ipar[7];
	F2C(cvstr)(&three,&(ipar[2]),type,&job, (unsigned long)strlen(type));
	for (i=2;i>=0;i--)
	  if (type[i]!=' ') { type[i+1]='\0';break;}
	ierr=0;
	mget2(fd,ipar[8],buffer,m,type,&ierr);
	if (ierr>0) {
	  sciprint("Read error!\n");
	  fclose(fd);
	  z[3] = 0.0;
	  *flag = -1;
	  return;
	}
	else if (ierr<0) { /* EOF reached */
	  kmax=-(ierr+1)/ipar[7];
	}
	else
	  kmax=ipar[6];

	z[1] = 1.0;
	z[2] = kmax;
      }
      else if (k<kmax) 
	z[1] = z[1]+1.0;
    }
  }
  else if (*flag==3) {
    ievt = ipar[5];
    n    = ipar[6];
    k    = (int)z[1];
    kmax = (int) z[2];
    if (k > kmax && kmax < n) {
      if(ievt) 
	tvec[0] = *t-1.0;
      else
	tvec[0] = *t*(1.0+0.0000000001);
    }
    else {
      record=buffer+(k-1)*ipar[7]-1;
      if(ievt) tvec[0] = record[mask[0]];
    }
  }
  else if (*flag==4) {
    F2C(cvstr)(&(ipar[1]),&(ipar[10]),str,&job,(unsigned long)strlen(str));
    str[ipar[1]] = '\0';
    fd = fopen(str,"rb");
    if (!fd ) {
      sciprint("Could not open the file!\n");
      *flag = -1;
      return;
    }
    z[3]=(long)fd;
    /* skip first records */
    if (ipar[9]>1) {
      F2C(cvstr)(&three,&(ipar[2]),type,&job,(unsigned long)strlen(type));
      for (i=2;i>=0;i--)
	  if (type[i]!=' ') { type[i+1]='\0';break;}
      offset=(ipar[9]-1)*ipar[7]*worldsize(type);
      irep = fseek(fd,offset,0) ;
      if ( irep != 0 ) 
	{
	  sciprint("Read error\r\n");
	  *flag = -1;
	  fclose(fd);
	  z[3] = 0.0;
	  return;
	}
    }
    /* read first buffer */
    m=ipar[6]*ipar[7];
    F2C(cvstr)(&three,&(ipar[2]),type,&job,(unsigned long)strlen(type));
    for (i=2;i>=0;i--)
	  if (type[i]!=' ') { type[i+1]='\0';break;}
    mget2(fd,ipar[8],buffer,m,type,&ierr);
    if (ierr>0) {
      sciprint("Read error!\n");
      *flag = -1;
      fclose(fd);
      z[3] = 0.0;
      return;
    }
    else if (ierr<0) { /* EOF reached */
      kmax=-(ierr+1)/ipar[7];
    }
    else
      kmax=ipar[6];

    z[1] = 1.0;
    z[2] = kmax;
  }
  else if (*flag==5) {
    if(z[3]==0) return;
    fclose(fd);
    z[3] = 0.0;
  }
  return;
}

