/* 
   SVC tools -- the SVC (Systems Validation Centre) tool set

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


   $Id: svc2dot.h,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "aterm1.h"
#include "svc/svcerrno.h"
#include "svc/svc.h"

#define EXIT_OK 0
#define EXIT_ERR_ARGS -1
#define EXIT_ERR_FILE -2
#define EXIT_ERR_INPUT -3

#define ERR_ARGS 1
#define ERR_FILE 2
#define CMD_HELP 3
#define CMD_VERSION 4
#define CMD_CONVERT 5

#define INFILE_EXT  ".svc"
#define OUTFILE_EXT ".dot"



extern int errno;
       SVCfile inFile;


int  parseArgs(int, char **, FILE **, int *);
void doHelp(char *);
void doVersion();
int  doConvert(FILE *, int);

#ifdef __cplusplus
}
#endif 

