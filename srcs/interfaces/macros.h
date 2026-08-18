/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file macros.h
 * @brief macros header declarations.
 *
 */

#ifndef INTERFACES_MACROS_H
#define INTERFACES_MACROS_H

#include <stdint.h>

// general system macros
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// #define reg8_setBit(reg, bit) ((reg) |= (uint8_t)(1u << (bit)))
// #define reg8_clearBit(reg, bit) ((reg) &= (uint8_t)~(1u << (bit)))
// #define reg8_getBit(reg, bit) ((reg) & (uint8_t)(1u << (bit)))

/*
 * bit manupulation macros
 */

// masks
#define _8BS(bit) ((uint8_t)(1u << (bit)))
#define _16BS(bit) ((uint16_t)(1u << (bit)))
#define _32BS(bit) ((uint32_t)(1UL << (bit)))
#define _64BS(bit) ((uint64_t)(1ULL << (bit)))

#define _REG_BS(reg, bit)              \
	((sizeof(reg) == 1)	  ? _8BS(bit)  \
	 : (sizeof(reg) == 2) ? _16BS(bit) \
	 : (sizeof(reg) == 4) ? _32BS(bit) \
						  : _64BS(bit))
// n elements macros
#define _SET_BITS_1(reg, a) _REG_BS(reg, a)

#define _SET_BITS_2(reg, a, b) (_REG_BS(reg, a) | _REG_BS(reg, b))

#define _SET_BITS_3(reg, a, b, c) (_REG_BS(reg, a) | _REG_BS(reg, b) | _REG_BS(reg, c))

#define _SET_BITS_4(reg, a, b, c, d) \
	(_REG_BS(reg, a) | _REG_BS(reg, b) | _REG_BS(reg, c) | _REG_BS(reg, d))

// selection macros
#define _REG_SELECT(_1, _2, _3, _4, NAME, ...) NAME

#define _SET_BITS(reg, ...) \
	_REG_SELECT(__VA_ARGS__, _SET_BITS_4, _SET_BITS_3, _SET_BITS_2, _SET_BITS_1)(reg, __VA_ARGS__)

// user api macros
#define TM_WRITEBIT(reg, ...) ((reg) = _SET_BITS((reg), __VA_ARGS__))
#define TM_SETBIT(reg, ...) ((reg) |= _SET_BITS((reg), __VA_ARGS__))
#define TM_CLEARBIT(reg, ...) ((reg) &= (__typeof__(reg))~(_SET_BITS((reg), __VA_ARGS__)))
#define TM_GETBIT(reg, bit) ((reg) & _REG_BS((reg), (bit)))

/*
 * other macros
 */

// string macros
#define CONCAT2(a, b) a##b
#define CONCAT(a, b) CONCAT2(a, b)
#define STRING2(x) #x
#define INT_TO_STRING(x) STRING2(x)

// store source file name in ROM
#define TM_STORE_FILE_NAME(name) TM_STR_ROM_NEW(name, __FILE_NAME__)

// generate unique name
#define UNIQUE_NAME(prefix) CONCAT(prefix, __LINE__)

#endif // INTERFACES_MACROS_H
