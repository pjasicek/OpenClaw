//========================================================================
// PrimeSearch.cpp -  traverse a known set of items randomly only once
//
//
// (c) Copyright 2012 Michael L. McShaffry and David Graham
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser GPL v3
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
// http://www.gnu.org/licenses/lgpl-3.0.txt for more details.
//
// You should have received a copy of the GNU Lesser GPL v3
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//========================================================================

#include <assert.h>
#include "PrimeSearch.h"
#include <stdlib.h>


/////////////////////////////////////////////////////////////////////////////
// DEBUG info
/////////////////////////////////////////////////////////////////////////////

#define max(a,b)            (((a) > (b)) ? (a) : (b))

int PrimeSearch::prime_array[] =
{
    2, 3, 5, 7,
    11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
    53, 59, 61, 67, 71, 73, 79, 83, 89, 97,
    101, 103, 107, 109, 113, 127, 131, 137, 139, 149,
    151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199,
    211, 223, 227, 229, 233, 239, 241,
    251, 257, 263, 269, 271, 277, 281, 283, 293,
    307, 311, 313, 317, 331, 337, 347, 349,
    353, 359, 367, 373, 379, 383, 389, 397,
    401, 409, 419, 421, 431, 433, 439, 443, 449,
    457, 461, 463, 467, 479, 487, 491, 499,
    503, 509, 521, 523, 541, 547,
    557, 563, 569, 571, 577, 587, 593, 599,
    601, 607, 613, 617, 619, 631, 641, 643, 647,
    653, 659, 661, 673, 677, 683, 691,
    701, 709, 719, 727, 733, 739, 743,
    751, 757, 761, 769, 773, 787, 797,
    809, 811, 821, 823, 827, 829, 839,
    853, 857, 859, 863, 877, 881, 883, 887,
    907, 911, 919, 929, 937, 941, 947,
    953, 967, 971, 977, 983, 991, 997,

    // after 1000, for space efficiency reasons, 
    // we choose to include fewer prime numbers the bigger the number get.

    1009, 1051, 1103, 1151, 1201, 1259, 1301, 1361, 1409, 1451,
    1511, 1553, 1601, 1657, 1709, 1753, 1801, 1861, 1901, 1951,
    2003, 2053, 2111, 2113, 2153, 2203, 2251,
    2309, 2351, 2411, 2459, 2503, 2551, 2557,
    2609, 2657, 2707, 2753, 2767, 2801, 2851, 2903, 2953,
    3001, 3061, 3109, 3163, 3203, 3251, 3301, 3359, 3407, 3457,
    3511, 3557, 3607, 3659, 3701, 3761, 3803, 3851, 3907, 3967,
    4001, 4051, 4111, 4153, 4201, 4253, 4327, 4357, 4409, 4451,
    4507, 4561, 4603, 4651, 4703, 4751, 4801, 4861, 4903, 4951,

    // begin to skip even more primes

    5003, 5101, 5209, 5303, 5407, 5501, 5623, 5701, 5801, 5903,
    6007, 6101, 6211, 6301, 6421, 6521, 6607, 6701, 6803, 6907,
    7001, 7103, 7207, 7307, 7411, 7507, 7603, 7703, 7817, 7901,
    8009, 8101, 8209, 8311, 8419, 8501, 8609, 8707, 8803, 8923,
    9001, 9103, 9203, 9311, 9403, 9511, 9601, 9719, 9803, 9901,

    // and even more
    10007, 10501, 11003, 11503, 12007, 12503, 13001, 13513, 14009, 14503,
    15013, 15511, 16033, 16519, 17011, 17509, 18013, 18503, 19001, 19501,
    20011, 20507, 21001, 21503, 22003, 22501, 23003, 23509, 24001, 24509

    // if you need more - go get them yourself!!!!

    // Create a bigger array of prime numbers by using this web site: 
    // http://www.rsok.com/~jrm/printprimes.html

};


PrimeSearch::PrimeSearch(int elements)
{
    assert(elements > 0 && "Can't do a PrimeSearch if you have 0 elements to search through");

    maxElements = elements;

    int a = (rand() % 13) + 1;
    int b = (rand() % 7) + 1;
    int c = (rand() % 5) + 1;

    skip = (a * maxElements * maxElements) + (b * maxElements) + c;
    skip &= ~0xc0000000;        // this keeps skip from becoming too large....

    Restart();

    currentPrime = prime_array;
    int s = sizeof(prime_array) / sizeof(prime_array[0]);

    // if this GCC_ASSERT gets hit you didn't have enough prime numbers to deal with this number of 
    // elements. Go back to the web site.
    assert(prime_array[s - 1]>maxElements);

    while (*currentPrime < maxElements)
    {
        currentPrime++;
    }

    int test = skip % *currentPrime;
    if (!test)
        skip++;
}

int PrimeSearch::GetNext(bool restart)
{
    if (restart)
        Restart();

    if (Done())
        return -1;

    bool done = false;

    int nextMember = currentPosition;

    while (!done)
    {
        nextMember = nextMember + skip;
        nextMember %= *currentPrime;
        searches++;

        if (nextMember < maxElements)
        {
            currentPosition = nextMember;
            done = true;
        }
    }

    return currentPosition;
}





