/****************************************************************************/
/*																			*/
/*	Module:			jamcomp.h												*/
/*																			*/
/*					Copyright (C) Altera Corporation 1997					*/
/*																			*/
/*	Description:	Contains the function prototypes for compressing		*/
/*					and uncompressing Boolean array data.					*/
/*																			*/
/****************************************************************************/

#ifndef INC_JAMCOMP_H
#define INC_JAMCOMP_H

long jam_uncompress
(
	char *in, 
	long in_length, 
	char *out, 
	long out_length,
	int version
);

#endif /* INC_JAMCOMP_H */

