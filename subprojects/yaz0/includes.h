/**
* @copyright 2018 - Max Bebök
* @author Max Bebök
* @license GNU-GPLv3 - see the "LICENSE" file in the root directory
*/

/**** Libs ****/
#include <cstdint>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <exception>
#include <stdexcept>
#include <thread>

/**** Data Types ****/
typedef std::uint8_t u8;
typedef std::int8_t s8;

typedef std::uint16_t u16;
typedef std::int16_t s16;

typedef std::uint32_t u32;
typedef std::int32_t s32;

/**** Header ****/

#include "chunk.h"
#include "base.h"
#include "parser.h"
#include "creator.h"