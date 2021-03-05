/**
@file

@brief Implementation of ITU-T (formerly CCITT) Recomendation %G711

For latest source code see http://www.tixy.clara.net/source/

Copyright (C) 2004 J.D.Medhurst (a.k.a. Tixy)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __G711_H__
#define __G711_H__
#include "common.h"
/**
@defgroup g711 Audio Codec - ITU-T Recomendation G711
@{
*/

/**
@brief A class which implements ITU-T (formerly CCITT) Recomendation G711
"Pulse Code Modulation (PCM) of Voice Frequencies"

This encodes and decodes uniform PCM values to/from 8 bit A-law and u-Law values.

Note, the methods in this class use uniform PCM values which are of 16 bits precision,
these are 'left justified' values corresponding to the 13 and 14 bit values described
in G711.
*/
class G711
{
public:
	/**
	Encode a single 16 bit uniform PCM value into an A-Law value

	@param pcm16 A 16 bit uniform PCM value (sign extented into an int).
	If the value doesn't lie in the range -32768 to 32767 then the
	result is undefined.

	@return 	 The A-Law encoded value corresponding to pcm16
	*/
	IMPORT static uint8 ALawEncode(int16 pcm16);

	/**
	Encode a single A-Law value into a 16 bit uniform PCM value

	@param alaw An A-Law encoded value

	@return 	The 16 bit uniform PCM value corresponding to alaw
	*/
	IMPORT static int ALawDecode(uint8 alaw);

	/**
	Encode single 16 bit uniform PCM value into an u-Law value

	@param pcm16 A 16 bit uniform PCM value (sign extented into an int)
	If the value doesn't lie in the range -32768 to 32767 then the
	result is undefined.

	@return 	 The u-Law encoded value corresponding to pcm16
	*/
	IMPORT static uint8 ULawEncode(int16 pcm16);

	/**
	Encode a single u-Law value into a 16 bit uniform PCM value

	@param ulaw An u-Law encoded value

	@return 	The 16 bit uniform PCM value corresponding to ulaw
	*/
	IMPORT static int ULawDecode(uint8 ulaw);

	/**
	Convert a single A-Law value into a u-law value.

	@param alaw An A-Law encoded value

	@return 	The u-law value.
	*/
	IMPORT static uint8 ALawToULaw(uint8 alaw);

	/**
	Convert a single u-Law value into an A-law value.

	@param ulaw An u-Law encoded value

	@return 	The A-Law value
	*/
	IMPORT static uint8 ULawToALaw(uint8 ulaw);

	/**
	Encode a buffer of 16 bit uniform PCM values into A-Law values

	@param dst	   Pointer to location to store A-Law encoded values
	@param src	   Pointer to the buffer of 16 bit uniform PCM values to be encoded
	@param srcSize The size, in bytes, of the buffer at \a src

	@return 	   The number of bytes which were stored at dst (equal to srcSize>>1)
	*/
	IMPORT static uint ALawEncode(uint8* dst, int16* src, uint srcSize);

	/**
	Decode a buffer of A-Law values into 16 bit uniform PCM values

	@param dst	   Pointer to location to store decoded 16 bit uniform PCM values
	@param src	   Pointer to the buffer of A-Law values to be decoded
	@param srcSize The size, in bytes, of the buffer at \a src

	@return 	   The number of bytes which were stored at \a dst (equal to srcSize<<1)
	*/
	IMPORT static uint ALawDecode(int16* dst, const uint8* src, uint srcSize);

	/**
	Encode a buffer of 16 bit uniform PCM values into u-Law values

	@param dst	   Pointer to location to store u-Law encoded values
	@param src	   Pointer to the buffer of 16 bit uniform PCM values to be encoded
	@param srcSize The size, in bytes, of the buffer at \a src

	@return 	   The number of bytes which were stored at \a dst (equal to srcSize>>1)
	*/
	IMPORT static uint ULawEncode(uint8* dst, int16* src, uint srcSize);

	/**
	Decode a buffer of u-Law values into 16 bit uniform PCM values

	@param dst	   Pointer to location to store decoded 16 bit uniform PCM values
	@param src	   Pointer to the buffer of u-Law values to be decoded
	@param srcSize The size, in bytes, of the buffer at \a src

	@return 	   The number of bytes which were stored at \a dst (equal to srcSize<<1)
	*/
	IMPORT static uint ULawDecode(int16* dst, const uint8* src, uint srcSize);

	/**
	Convert a buffer of A-Law values into u-law values.

	@param dst	   Pointer to location to store u-law values
	@param src	   Pointer to the buffer of A-Law values to be converted
	@param srcSize The size, in bytes, of the buffer at \a src

	@return 	   The number of bytes which were stored at \a dst (equal to srcSize)
	*/
	IMPORT static uint ALawToULaw(uint8* dst, const uint8* src, uint srcSize);

	/**
	Convert a buffer of u-Law values into A-law values.

	@param dst	   Pointer to location to store A-law values
	@param src	   Pointer to the buffer of u-Law values to be converted
	@param srcSize The size, in bytes, of the buffer at \a src

	@return 	   The number of bytes which were stored at \a dst (equal to srcSize)
	*/
	IMPORT static uint ULawToALaw(uint8* dst, const uint8* src, uint srcSize);
};

/** @} */ // End of group

#endif