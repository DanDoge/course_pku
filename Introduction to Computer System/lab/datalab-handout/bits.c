/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 * Name: 
 * Nick Name:
 * Student ID:
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function.
     The max operator count is checked by dlc. Note that '=' is not
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 8.0.0.  Version 8.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, plus Amendment 1 (published
   2015-05-15).  */
/* We do not support C11 <threads.h>.  */
/*
 * testAndSet: if low word of x == y, then let high word of x = z and return x,
 *   otherwise return x directly.
 *   x is a 32-bit integer, both y and z are 16-bit integers.
 *   Example: testAndSet(0xFF00, 0x01, 0x03) = 0xFF00,
 *            testAndSet(0xFF00, 0x00, 0x03) = 0xF300,
 *   Legal ops: ~ | ^ & << >> + !
 *   Max ops: 20
 *   Rating: 2
 */
int testAndSet(int x, int y, int z) {
    // ops: 18 -> 17 date: 09/20
    int low_word_filled_with_1s = (1 << 16) + ~0;// - 1 == ~0
    int low_word_x = x & low_word_filled_with_1s;
    int low_word_equal = !((x ^ y) << 16);//low_word_x == y
    //int low_word_equal = low_word_x == y;//for debugging
    int high_word_x = x & (~low_word_filled_with_1s);
    int high_word_filled_with_low_word_equal = (low_word_equal << 31) >> 15;//arithmetically shift
    return (high_word_filled_with_low_word_equal & (z << 16))
         + ((~high_word_filled_with_low_word_equal) & high_word_x)
         + low_word_x;
}
/*
 * oneMoreThan - return 1 if y is one more than x, and 0 otherwise
 *   Examples oneMoreThan(0, 1) = 1, oneMoreThan(-1, 1) = 0
 *   Legal ops: ~ & ! ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int oneMoreThan(int x, int y) {
    //ops: 12 -> 9 date: 09/20
    int y_equals_tmin = !(y ^ (1 << 31));
    int ans = x + ~y + 2;
    return (!y_equals_tmin) & !ans;
}
/*
 * isTmin - returns 1 if x is the minimum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmin(int x) {
    //ops: 5 -> still 5 date: 09/20
    return !(x + x) & (!!x);
}
/*
 * halfAdd - single-bit add using bit-wise operations only.
 *   Both x and y belong to {0, 1}.
 *   Example: halfAdd(1, 0) = 1,
 *            halfAdd(1, 1) = 2,
 *   Legal ops: ~ | ^ & << >>
 *   Max ops: 7
 *   Rating: 1
 */
int halfAdd(int x, int y) {
    // lower bit ==> x xor y;
    //highbit == 1 only if x, y == 1;
    return (x ^ y) | ((x & y) << 1);
}
/*
 * sameSign - return 1 if x and y have same sign, and 0 otherwise
 *   Examples sameSign(0x12345678, 0) = 1, sameSign(0xFFFFFFFF,0x1) = 0
 *   Legal ops: ! ~ & ! ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int sameSign(int x, int y) {
    //ops: 4 -> 3 date: 09/20
    //check if signbit is same
    return !((x ^ y) >> 31);
}
/*
 * fullAdd - 4-bits add using bit-wise operations only.
 *   (0 <= x, y < 16)
 *   Example: fullAdd(12, 7) = 3,
 *            fullAdd(7, 8) = 15,
 *   Legal ops: ~ | ^ & << >>
 *   Max ops: 30
 *   Rating: 2
 */
int fullAdd(int x, int y) {
    //ops: 27 -> 22 date: 09/20
    //     22 -> 20 date: 09/20
    //     20 -> 14 date: 09/21
/*
    int xor = x ^ y;
    int and = x & y;
    int bit_zero = xor & 1;
    int bit_one = (xor & 2) ^ (and & 1);
    int bit_two = (xor & 4) ^ 0;
    return ?;*/ //old and falied try....

/*

    int x_xor_y = x ^ y;
    int x_and_y = x & y;
    int bit_zero = x_xor_y & 1;
    int jinwei_from_zero = (x_and_y & 1) << 1;

    int bit_one_without_jinwei = x_xor_y & 2;
    int jinwei_from_one_without_jinwei = x_and_y & 2;
    int bit_one = bit_one_without_jinwei ^ jinwei_from_zero;
    int jinwei_from_one = (jinwei_from_one_without_jinwei | (bit_one_without_jinwei & jinwei_from_zero)) << 1;

    int bit_two_without_jinwei = x_xor_y & 4;
    int jinwei_from_two_without_jinwei = x_and_y & 4;
    int bit_two = bit_two_without_jinwei ^ jinwei_from_one;
    int jinwei_from_two = (jinwei_from_two_without_jinwei | (bit_two_without_jinwei & jinwei_from_one)) << 1;

    int bit_three_without_jinwei = x_xor_y & 8;
    //int jinwei_from_three_without_jinwei = (x & y) & 8;
    int bit_three = bit_three_without_jinwei ^ jinwei_from_two;
    //int jinwei_from_three = jinwei_from_three_without_jinwei | (bit_three_without_jinwei & jinwei_from_two);
    return bit_three | bit_two | bit_one | bit_zero;*/ //old but right try

    //like halfAdd, x + y ==> (x ^ y) + ((x & y) << 1), so we get the lowest bit, and repeat that
    //int bit_zero = 0;
    //int bit_one = 0;
    //int bit_two = 0;
    //int bit_three = 0;
    //no need to save these, 'cause they will be saved in x or tmpx
    //(for after shifting y / tmpy, the low bit is 0 and 0 ^ 1 = 1)
    int tmpx, tmpy;

    tmpx = x ^ y;
    tmpy = x & y;
    //bit_zero = tmpx & 1;// bit_zero done here
    tmpy <<= 1;
    x = tmpx ^ tmpy;
    y = tmpx & tmpy;
    //bit_one = x & 2;
    y <<= 1;
    tmpx = x ^ y;
    tmpy = x & y;
    //bit_two = tmpx & 4;
    tmpy <<= 1;
    x = tmpx ^ tmpy;
    //bit_three = x & 8;
    //return bit_zero | bit_one | bit_two | bit_three;
    return x & 0xF;
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    return ~x + 1;
}
/*
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
    //op: 15 -> 8 date: 09/20
    int sub = x + ~y + 1;//x - y;
    // overflow only occurs when x * y < 0
    // and if the ans overflows, z's sign won't be equal to x's sign bit
    int sign_x_equals_y = x ^ y;
    int sign_x_equals_sub = x ^ sub;
    return !((sign_x_equals_y & sign_x_equals_sub) >> 31);
}
/*
 * negPerByte: negate each byte of x, then return x.
 *   Example: negPerByte(0xF8384CA9) = 0x08C8B457,
 *   Legal ops: ~ | ^ & << >> + !
 *   Max ops: 30
 *   Rating: 3
 */
int negPerByte(int x) {
    //op: 25 -> 18 date: 09/20
    int byte_zero;
    int byte_one;
    int byte_two;
    int byte_three;
    x = ~x;
    byte_zero = x;
    byte_one = (x >> 8);
    byte_two = (x >> 16);
    byte_three = (x >> 24);
    byte_zero += 1;
    byte_one += 1;
    byte_two += 1;
    byte_three += 1;
    byte_zero &= 0xFF;
    byte_one &= 0xFF;
    byte_two &= 0xFF;
    byte_three &= 0xFF;
    return byte_zero | (byte_one << 8) | (byte_two << 16) | (byte_three << 24);
}
/*
 * isGreater - if x > y  then return 1, else return 0
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
    //ops: 20 -> 18 date: 09/20
    //     18 -> 14 date: 09/21
    //when x * y < 0, it may overflow when doing x - y
    int tmin = 1 << 31;
    int signbit_x = x & tmin;
    int sign_x_not_equals_y = (x ^ y) >> 31;
    int x_no_less_than_zero = !signbit_x;
    //x_subs_y = x - y;//for debugging
    //int x_subs_y_more_than_zero = (!(x_subs_y & (1 << 31))) & x_subs_y;//old and falied try
    int x_subs_y_no_more_than_zero = ((y + ~x + 1) & tmin) >> 31;//y - x < 0
    //x_subs_y_more_than_zero = x > y;//for debugging
    return ((sign_x_not_equals_y) & (x_no_less_than_zero)) | ((!sign_x_not_equals_y) & (x_subs_y_no_more_than_zero));
}
/*
 * zeroByte - return 1 if one of four bytes of x is zero, and 0 otherwise
 *   Example: zeroByte(0xFF00EEDD) = 1, zeroByte(-0x12345678) = 0
 *   Legal ops: ~ & ! | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int zeroByte(int x) {
    int byte_zero = x & 0xFF;
    //x = x >> 8;//also besauce dlc compiler...
    int byte_one = (x >> 8) & 0xFF;
    //x = x >> 8;
    int byte_two = (x >> 16) & 0xFF;
    //x = x >> 8;
    int byte_three = (x >> 24) & 0xFF;
    return (!byte_zero) | (!byte_one) | (!byte_two) | (!byte_three);
}
/*
 * modThree - calculate x mod 3 without using %.
 *   Example: modThree(12) = 0,
 *            modThree(2147483647) = 1,
 *            modThree(-8) = -2,
 *   Legal ops: ~ ! | ^ & << >> +
 *   Max ops: 60
 *   Rating: 4
 */
int modThree(int x) {
    // n            0 1 2 3 4 5 6 7
    // 2 ^ n mod 3  1 2 1 2 1 2 1 2  // actually all 2 ^ (2k) mod 3 == 1
    //so first divide 4-byte num into 4 bytes, add together
    //we get a maxinum 10-bit num, divide into 4-bit parts, add together
    //then get a maxinum 6-bit num...until we get a maxinum 2-bit num
    int sign_x = x >> 31;
    int eight_bit_0, eight_bit_1, eight_bit_2, eight_bit_3;
    int ten_bit_tmp;
    int four_bit_0, four_bit_1, four_bit_2;
    int six_bit_tmp;
    int three_bit_0, three_bit_1;
    int four_bit_tmp;
    //purely to save operators
    int sign_x_to_bool = !!sign_x;
    int minus_three = ~3 + 1;

    x = (x ^ sign_x) + sign_x_to_bool;// absolute of x
    eight_bit_0 = x & 0xFF;
    eight_bit_1 = (x & (0xFF << 8)) >> 8;
    eight_bit_2 = (x & (0xFF << 16)) >> 16;
    eight_bit_3 = (x & (0xFF << 24)) >> 24;
    ten_bit_tmp = eight_bit_0 + eight_bit_1 + eight_bit_2 + eight_bit_3;

    four_bit_0 = ten_bit_tmp & 0xF;
    four_bit_1 = (ten_bit_tmp & 0xF0) >> 4;
    four_bit_2 = (ten_bit_tmp & (0xF << 8)) >> 8;
    six_bit_tmp = four_bit_0 + four_bit_1 + four_bit_2;

    three_bit_0 = six_bit_tmp & 7;
    three_bit_1 = six_bit_tmp & 56;//00111000
    four_bit_tmp = three_bit_0 + (three_bit_1 >> 2);

    //after this step, maxinum = 6
    four_bit_tmp = (four_bit_tmp & 1) + (four_bit_tmp & 2) + (((four_bit_tmp & 4)) >> 2) + (((four_bit_tmp & 8)) >> 2);
    // maxinum = 4
    four_bit_tmp = (four_bit_tmp & 1) + (four_bit_tmp & 2) + (((four_bit_tmp & 4)) >> 2);
    // maxinum = 3 ==> actually useless
    //four_bit_tmp = (four_bit_tmp & 1) + (four_bit_tmp & 2) + (((four_bit_tmp & 4)) >> 2);
    // change 3 to 0
    four_bit_tmp += ~(!(four_bit_tmp + minus_three) + ((!(four_bit_tmp + minus_three)) << 1)) + 1;
    //if(four_bit_tmp == 3 || four_bit_tmp == -3){
    //    four_bit_tmp = 0;//for debugging
    //}

    four_bit_tmp = (four_bit_tmp ^ sign_x) + sign_x_to_bool;//for x < 0 ans has to be negative
    return four_bit_tmp;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    //find where the first 1 is(if positive), if negative, try ~x,  plus one for signbit
    //declare first....'cause dlc compiler...
    int pos_first_1or0 = 0, bit_zero = 0, bit_one = 0, bit_two = 0, bit_three = 0, bit_four = 0;

    //try if absolute value works
    int sign_x = x >> 31;// x >= 0:all 0s x < 0 all 1s
    x = x ^ sign_x;// absolute of x

    //pos_first_1or0 = 16 * bit_four + 8 * bit_three + ... + 1 * bit_zero
    bit_four = !!((x >> 16));
    pos_first_1or0 += bit_four << 4;
    //like binary search
    bit_three = !!((x >> (pos_first_1or0 + 8)));
    pos_first_1or0 += bit_three << 3;

    bit_two = !!((x >> (pos_first_1or0 + 4)));
    pos_first_1or0 += bit_two << 2;

    bit_one = !!((x >> (pos_first_1or0 + 2)));
    pos_first_1or0 += bit_one << 1;

    bit_zero = !!((x >> (pos_first_1or0 + 1)));
    pos_first_1or0 += bit_zero;

    //plus one for signbit,plus one for we start at zero
    pos_first_1or0 += 1;
    pos_first_1or0 += !!x;//don't need to puls one(which stands for signbit) when x == 0
    return pos_first_1or0;
}
/*
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
    //ops: 27 -> 24 date: 09/20
    int signbit = uf & (1 << 31);
    int exp = (uf & (0xFF << 23));
    int f = uf - signbit - exp;
    int last_two_bits = (f & 3) - 3;
    exp >>= 23;
    if(exp == (1 << 8) - 1){//NaN or inf
        return uf;
    }
    if(exp == 0){//denormalized
        if(last_two_bits){//rounding

        }else{
            f += 1;
        }
        f >>= 1;
    }else if(exp == 1){
        if(last_two_bits){//rounding

        }else{
            f += 1;
        }
        f += 1 << 23;// m = 1 + f ==> m = f
        f >>= 1;
        exp = 0;
    }else{
        exp -= 1;
    }
    return signbit + (exp << 23) + f;
}
/*
 * float_negpwr2 - Return bit-level equivalent of the expression 2.0^-x
 *   (2.0 raised to the power -x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^-x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_negpwr2(int x) {
    int ans = 0;
    int exp = 0;
    if(x > 149){
        ans = 0;
    }else if(x > 126){
        ans = 1 << (149 - x);
    }else{
        exp = 127 - x;
        if(x < -128){
            exp = 255;
        }
        ans = exp << 23;
    }
    return ans;
}
