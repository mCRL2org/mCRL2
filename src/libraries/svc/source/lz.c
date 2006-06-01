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

   $Id: lz.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include <stdlib.h>
#include <string.h>
#include <lz.h>

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char *strdup(const char *s);
#endif

static int compress(BitStream *bs, LZbuffer *buffer, char *string);
static int decompress(BitStream *bs, LZbuffer *buffer, char **str);
static void LZmakeToken(char c, unsigned int offset, unsigned int length, LZtoken token);
static void LZsplitToken(char *c, unsigned int *offset, unsigned int *length, LZtoken token);
static void LZwriteToken(BitStream *bs, LZtoken token);
static int LZreadToken(BitStream *bs, LZtoken token);

static char scratch[102400];
static int count=0;


void LZinit(LZbuffer *buffer){
   int i;

 
   buffer->last=-1;
   buffer->read=1;
   buffer->written=1;
   for(i=0; i<SEARCHBUF_SIZE;i++){
      buffer->search[i]='\0';
   }
}
 
static void LZmakeToken(char c, unsigned int offset, unsigned int length, LZtoken token){
/*
   if (length<=1){
      token[0]=c;
      token[1]='\0';
   } else {
      token[0]=0x80|(unsigned char)length<<(SEARCH_BITS-8)|(unsigned char)(offset/256);
      token[1]=(offset%256);
   }
*/
   if (length<=1){
      token[0]=c;
      token[1]='\0';
   } else {
      token[0]=0x80|(unsigned char)(length/2);
      token[1]=(unsigned char)length<<7|(unsigned char)(offset/256);
      token[2]=(unsigned char)(offset%256);
   }

}

static void LZsplitToken(char *c, unsigned int *offset, unsigned int *length, LZtoken token){
/*
   if(token[0]&0x80){
     *offset=(token[0]&0x03)*256+token[1];
     *length=(token[0]&0x7c)>>(SEARCH_BITS-8);
   } else {
     *c=token[0];
     *offset=0;
     *length=0;
   }
*/
   if(token[0]&0x80){
     *offset=(token[1]&0x7f)*256+token[2];
     *length=(token[0]&0x7f)<<1|(token[1]&0x80)>>7;
   } else {
     *c=token[0];
     *offset=0;
     *length=0;
   }

}

static void LZwriteToken(BitStream *bs, LZtoken token){
   
   if(token[0]&0x80){
/*
fprintf(stderr, "WRITING TOKEN %02X%02X%02X\n", token[0], token[1], token[2]);
*/

      BSwriteByte(bs, token[0]);
      BSwriteByte(bs, token[1]);
      BSwriteByte(bs, token[2]); /* ADDED */
   } else {
      BSwriteByte(bs, token[0]);
   }

}


static int LZreadToken(BitStream *bs, LZtoken token){

/*
   if(fscanf(fp, "%c", token)==1){
*/
   if(BSreadByte(bs, &token[0])==1){
      if (token[0]&0x80){
         BSreadByte(bs, &token[1]);
         BSreadByte(bs, &token[2]); /* ADDED */
/*
         fscanf(fp, "%c", &token[1]);
*/
      } else {
         token[1]=0;
      }
      return 1;
   } else {
      return 0;
   }
}



int LZwriteString(BitStream *bs, LZbuffer *buffer, char *string){

   compress(bs,buffer,string);
   return 1;
}



int LZreadString(BitStream *bs, LZbuffer *buffer, char **string){

   decompress(bs,buffer,string);
   return 1;
}

int LZreadInt(BitStream *bs, LZbuffer *buffer, long *n){
   return BSreadInt(bs,n);
}

int LZwriteInt(BitStream *bs, LZbuffer *buffer, long n){
   BSwriteInt(bs,n);
   return 1;
}


int LZwriteATerm(BitStream *bs, LZbuffer *buffer, ATerm term){
   char *buf;

   buf=strdup(ATwriteToString(term));
   compress(bs,buffer,buf);
   free(buf);

   return 1;
}


int LZreadATerm(BitStream *bs, LZbuffer *buffer, ATerm *term){
   char *str;

 
   if(decompress(bs,buffer,&str)){
      *term=ATreadFromString(str);
/*    *term=ATmake(str);*/
      return 1;
   } else{
      return 0;
   }

}



static int compress(BitStream *bs, LZbuffer *buffer, char *string){
   int i=0,j=0,k=0;
   int offset=0,length=0;
   LZtoken token;
   char c;

   do{

      length=1;
      offset=0;
      c=string[i];
      for (j=0;j<SEARCHBUF_SIZE-length;j++){
         for(k=0; string[i+k]!='\0' && string[i+k]==buffer->search[(buffer->last-j+k+SEARCHBUF_SIZE)%SEARCHBUF_SIZE]&&k<LOOKAHEADBUF_SIZE;k++) {
            buffer->search[(buffer->last+k+1+SEARCHBUF_SIZE)%SEARCHBUF_SIZE]=string[i+k];
         }
         if(string[i+k]=='\0'&&
            buffer->search[(buffer->last-j+k+SEARCHBUF_SIZE)%SEARCHBUF_SIZE]=='\0' &&
            k<LOOKAHEADBUF_SIZE){
            k++;
         }
         if (k>length){
            offset=j;
            length=k;
         }
         if(c=='\0' || k==LOOKAHEADBUF_SIZE){
            break;
         }
      }

      buffer->search[(buffer->last+length)%SEARCHBUF_SIZE]=string[i+length-1];
/*
if(offset==0L && length>0L){
   fprintf(stderr, "STRING %s %s %s %ld %ld\n", string, tmp, buffer->search,
buffer->last);
}
fprintf(stderr, "OUT(%d, %d, %c)\n", offset, length,c);
*/
      LZmakeToken(c,  offset, length, token);
      LZwriteToken(bs, token);

      buffer->last=(buffer->last+length)%SEARCHBUF_SIZE;

      i+=length;
/*
      buffer->read+=length;
      buffer->written+=(token[0]&0x80?2:1);
*/

   } while (string[i-1]!='\0');

   return 1;
}

static int decompress(BitStream *bs, LZbuffer *buffer, char **str){
   unsigned int offset, length, i, last;
   LZtoken token;
   char c;

   *str=scratch;
   last=0;
count++;
   while(LZreadToken(bs,token)){

      LZsplitToken(&c, &offset, &length, token);
/*
if(offset<=1023){
   fprintf(stderr,"Last %d\n", buffer->last);
   fprintf(stderr,"LENGTH %d\n", length);
   fprintf(stderr,"OFFSET %d\n", offset);
   fprintf(stderr,"CHAR   %c\n", c     );
   fprintf(stderr, "BUFFER=");
      for(i=0;i<1024;i++){
if(i%80==0)fprintf(stderr,"\n");
fprintf(stderr,"%c", buffer->search[i]=='\0'?':':buffer->search[i]);
}
      fprintf(stderr,"\n");
}
*/


      if (length==0){
         buffer->last=(buffer->last+1)%SEARCHBUF_SIZE;
         buffer->search[buffer->last]=c;
         scratch[last++]=c;
      } else {
         for(i=0;i<length;i++){
            buffer->last=(buffer->last+1)%SEARCHBUF_SIZE;
            buffer->search[buffer->last]=buffer->search[(buffer->last-offset-1+SEARCHBUF_SIZE)%SEARCHBUF_SIZE];
            scratch[last++]=buffer->search[buffer->last];
         }
      }

      if (scratch[last-1]=='\0'){
         return 1;
      }
   }
   return 0;

}

