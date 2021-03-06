/*****************************************************************************/
/*                                                                           */
/* Module:  TIA Chip Sound Simulator Includes, V1.1                          */
/* Purpose: Define global function prototypes and structures for the TIA     */
/*          Chip Sound Simulator.                                            */
/* Author:  Ron Fries                                                        */
/*                                                                           */
/* Revision History:                                                         */
/*    10-Sep-96 - V1.0 - Initial Release                                     */
/*    14-Jan-97 - V1.1 - Added compiler directives to facilitate compilation */
/*                       on a C++ compiler.                                  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*                 License Information and Copyright Notice                  */
/*                 ========================================                  */
/*                                                                           */
/* TiaSound is Copyright(c) 1997 by Ron Fries                                */
/*                                                                           */
/* This library is free software; you can redistribute it and/or modify it   */
/* under the terms of version 2 of the GNU Library General Public License    */
/* as published by the Free Software Foundation.                             */
/*                                                                           */
/* This library is distributed in the hope that it will be useful, but       */
/* WITHOUT ANY WARRANTY; without even the implied warranty of                */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library */
/* General Public License for more details.                                  */
/* To obtain a copy of the GNU Library General Public License, write to the  */
/* Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   */
/*                                                                           */
/* Any permitted reproduction of these routines, in whole or in part, must   */
/* bear this legend.                                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _TIASOUND_H
#define _TIASOUND_H

#define AUDC0        0x15
#define AUDC1        0x16
#define AUDF0        0x17
#define AUDF1        0x18
#define AUDV0        0x19
#define AUDV1        0x1a

#ifdef __cplusplus
extern "C" {
#endif
    
/* define some data types to keep it platform independent */
#define int8  int8_t
#define int16 int16_t
#define int32 int32_t
    
#define uint8  uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
    
void Tia_sound_init (uint16 sample_freq, uint16 playback_freq);
void Update_tia_sound (uint16 addr, uint8 val);
void Tia_process (unsigned char *buffer, uint16 n);
    
void Tia_get_registers (unsigned char *reg1, unsigned char *reg2, unsigned char *reg3,
                        unsigned char *reg4, unsigned char *reg5, unsigned char *reg6);
void Tia_set_registers (unsigned char reg1, unsigned char reg2, unsigned char reg3,
                        unsigned char reg4, unsigned char reg5, unsigned char reg6);

void Tia_volume (unsigned int percent);
#ifdef __cplusplus
}
#endif

#endif
