/*
   SVC -- the SVC (Systems Validation Centre) file format library

   Copyright (C) 2000  Stichting Mathematisch Centrum, Amsterdam,
                       The  Netherlands

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


   $Id: code.c,v 1.2 2005/04/05 13:23:01 bertl Exp $ */

#include <stdio.h>
#include <limits.h>
#include <code.h>
#include <stdlib.h>




BitStream *BSinit(FILE *fp){
   BitStream *bs;


   bs=(BitStream *)malloc(sizeof(BitStream));
   bs->numBits=0;
   bs->buffer='\0';
   bs->fp=fp;

   return bs;
}

void BSfree(BitStream *bs){

   fclose(bs->fp);
   free(bs);

}



int BSreadBit(BitStream *bs, Bit *bit){
   int b;

   if(bs->numBits==0){
      if((b=getc(bs->fp))==EOF){
         return 0;
      } else {
         bs->buffer=b;
         bs->numBits=8;
/*
fprintf(stderr, "Reading %02X\n", bs->buffer);
*/

      }
   }

   bs->numBits--;
   *bit=(bs->buffer>>bs->numBits)&0x01;
/*
fprintf(stderr,"BSreadBit %1d (%d)\n", *bit, bs->numBits);
*/
   return 1;

}



void BSwriteBit(BitStream *bs, Bit bit){

/*
fprintf(stderr,"BSwriteBit %1d (%d)\n", bit, bs->numBits);
*/

   bs->buffer<<=1;
   bs->buffer|=((Byte)bit);
   bs->numBits++;
   if (bs->numBits==8){
/*
fprintf(stderr, "Writing %02X\n", bs->buffer);
*/
      putc(bs->buffer,bs->fp);
      bs->numBits=0;
      bs->buffer='\0';
   }

}


void BSwriteChar(BitStream *bs, char c){

   /* Drop the most significant bit */
   BSwriteBit(bs, (c>>6)&0x01);
   BSwriteBit(bs, (c>>5)&0x01);
   BSwriteBit(bs, (c>>4)&0x01);
   BSwriteBit(bs, (c>>3)&0x01);
   BSwriteBit(bs, (c>>2)&0x01);
   BSwriteBit(bs, (c>>1)&0x01);
   BSwriteBit(bs, (c>>0)&0x01);

}

void BSwriteByte(BitStream *bs, Byte byte){
/*
fprintf(stderr, "Writing %02X\n", byte);
*/

   BSwriteBit(bs, (byte>>7)&0x01);
   BSwriteBit(bs, (byte>>6)&0x01);
   BSwriteBit(bs, (byte>>5)&0x01);
   BSwriteBit(bs, (byte>>4)&0x01);
   BSwriteBit(bs, (byte>>3)&0x01);
   BSwriteBit(bs, (byte>>2)&0x01);
   BSwriteBit(bs, (byte>>1)&0x01);
   BSwriteBit(bs, (byte>>0)&0x01);

}



int BSreadChar(BitStream *bs, char *c){
   Bit b6,b5,b4,b3,b2,b1,b0;

   if(BSreadBit(bs, &b6) &&
      BSreadBit(bs, &b5) &&
      BSreadBit(bs, &b4) &&
      BSreadBit(bs, &b3) &&
      BSreadBit(bs, &b2) &&
      BSreadBit(bs, &b1) &&
      BSreadBit(bs, &b0) ){
     *c=b6<<6|b5<<5|b4<<4|b3<<3|b2<<2|b1<<1|b0;
/*
fprintf(stderr,"BSreadChar %c\n", *c);
*/
     return 1;
   } else {
     return 0;
   }

}
int BSreadByte(BitStream *bs, Byte *byte){
   Bit b7,b6,b5,b4,b3,b2,b1,b0;

   if(BSreadBit(bs, &b7) &&
      BSreadBit(bs, &b6) &&
      BSreadBit(bs, &b5) &&
      BSreadBit(bs, &b4) &&
      BSreadBit(bs, &b3) &&
      BSreadBit(bs, &b2) &&
      BSreadBit(bs, &b1) &&
      BSreadBit(bs, &b0) ){
     *byte=b7<<7|b6<<6|b5<<5|b4<<4|b3<<3|b2<<2|b1<<1|b0;
/*
fprintf(stderr, "Reading %02X\n", *byte);
*/
     return 1;
   } else {
     return 0;
   }

}

void BSwriteString(BitStream *bs, char *str){

/*
   fprintf(stderr, "STRING %s\n", str);
*/

   while (*str!='\0'){
      BSwriteChar(bs,*(str++));
   }
   BSwriteChar(bs,'\0');
}

int BSreadString(BitStream *bs, char *str){

   do{
      if(BSreadChar(bs,str)==0){
         return 0;
      }
   } while (*(str++)!='\0');

   return 1;

}

void BSwriteInt(BitStream *bs, long n){
   Byte b0, b1, b2, b3;

/*
fprintf(stderr,"BSwriteInt %ld\n", n);
*/

   if(n==NO_INT){

     /* The invalid integer is represented as -0 */
     BSwriteBit(bs, 1);
     BSwriteBit(bs, 0);
     BSwriteBit(bs, 0);
     BSwriteByte(bs, 0);

   } else { 

     /* Write the sign bit */

     if(n>=0){
       BSwriteBit(bs, 0);
     }else{
       BSwriteBit(bs, 1);
       n=-n;
     }

     /* Write the two length bits and the data bytes */
     b0=n%256;
     n/=256;
     if(n==0L){
       BSwriteBit(bs, 0);
       BSwriteBit(bs, 0);
       BSwriteByte(bs, b0);
     } else {
       b1=n%256;
       n/=256;
       if(n==0L){
         BSwriteBit(bs, 0);
         BSwriteBit(bs, 1);
         BSwriteByte(bs, b1);
         BSwriteByte(bs, b0);
        } else {
          b2=n%256;
          n/=256;
          if(n==0L){
            BSwriteBit(bs, 1);
            BSwriteBit(bs, 0);
            BSwriteByte(bs, b2);
            BSwriteByte(bs, b1);
            BSwriteByte(bs, b0);
          } else {
            b3=n%256;
            n/=256;
            BSwriteBit(bs, 1);
            BSwriteBit(bs, 1);
            BSwriteByte(bs, b3);
            BSwriteByte(bs, b2);
            BSwriteByte(bs, b1);
            BSwriteByte(bs, b0);
          }
       }
     }
  }

}

int  BSreadInt(BitStream *bs, long *n){
    Bit b0, b1, s;
    Byte byte;


    BSreadBit(bs,&s);
    BSreadBit(bs,&b0);
    BSreadBit(bs,&b1);
    BSreadByte(bs, &byte);
    *n=(long)byte;

    if(b0==0){
      if(b1==1){
        BSreadByte(bs, &byte);
        *n=256*(*n)+byte;
      } else {
        if(s==1 && *n==0){
          *n=NO_INT;
        }
      }
    } else {
      BSreadByte(bs, &byte);
      *n=256*(*n)+byte;
      BSreadByte(bs, &byte);
      *n=256*(*n)+byte;
      if(b1==1){
        BSreadByte(bs, &byte);
        *n=256*(*n)+byte;
      } 
    }
    if(s==1 && *n!=NO_INT){
      *n=-*n;
    }
/*
fprintf(stderr,"BSreadInt %ld\n", *n);
*/

    return 1;
}

void BSflush(BitStream *bs){

   if(bs->numBits>0){
      fprintf(bs->fp, "%c", bs->buffer<<(8-bs->numBits));
      bs->numBits=0;
   }

}


int  BSseek(BitStream *bs, long offset, int whence){

   return fseek(bs->fp, offset, whence);
}

long BStell(BitStream *bs){

   return ftell(bs->fp);
}


