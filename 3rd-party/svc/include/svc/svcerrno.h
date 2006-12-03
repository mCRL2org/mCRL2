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

   $Id: svcerrno.h,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#ifndef _SVCERRNO_H
#define _SVCERRNO_H

#include <errno.h>
extern int SVCerrno;



#define EACCESS         1
#define ENOINFO        10
#define EFORMATVERSION 11
#define ENOHEADER      20
#define EFILENAME      21
#define EDATE          22
#define EVERSION       23
#define EFILETYPE      24
#define ECREATOR       25
#define ENUMSTATES     26
#define ENUMTRANSITIONS 27
#define ENUMLABELS      28
#define ENUMPARAMETERS 29
#define ECOMMENTS      30
#define EINDEX         40
#define ELABEL         50
#define ESTATE         51
#define EPARAMETERS    52
#define ENOTRAILER     60
#define ECRC           61
#define ENEWSTATE      70
#define EFILEMODE      71

#endif
