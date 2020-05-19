#include "mvcode.h"

unsigned mv_code_table [33][2] =
/*
 *  MPEG's huffman code for vector components. Format: code_value, length
 */
{
   {0x19, 11}, {0x1b, 11}, {0x1d, 11}, {0x1f, 11}, {0x21, 11}, {0x23, 11},
   {0x13, 10}, {0x15, 10}, {0x17, 10}, {0x7, 8}, {0x9, 8}, {0xb, 8}, {0x7, 7},
   {0x3, 5}, {0x3, 4}, {0x3, 3}, {0x1, 1}, {0x2, 3}, {0x2, 4}, {0x2, 5},
   {0x6, 7}, {0xa, 8}, {0x8, 8}, {0x6, 8}, {0x16, 10}, {0x14, 10}, {0x12, 10}, 
   {0x22, 11}, {0x20, 11}, {0x1e, 11}, {0x1c, 11}, {0x1a, 11}, {0x18, 11}
};

