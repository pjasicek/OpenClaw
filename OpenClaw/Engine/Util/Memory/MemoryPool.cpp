

//========================================================================
// MemoryPool.cpp : 
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

#include "MemoryPool.h"
#include "../StringUtil.h"
#include <stdlib.h>

const static size_t CHUNK_HEADER_SIZE = (sizeof(unsigned char*));

MemoryPool::MemoryPool(void)
{
    Reset();
}

MemoryPool::~MemoryPool(void)
{
    Destroy();
}

bool MemoryPool::Init(unsigned int chunkSize, unsigned int numChunks)
{
    // it's safe to call Init() without calling Destroy()
    if (m_ppRawMemoryArray)
        Destroy();

    // fill out our size & number members
    m_chunkSize = chunkSize;
    m_numChunks = numChunks;

    // attempt to grow the memory array
    if (GrowMemoryArray())
        return true;
    return false;
}

void MemoryPool::Destroy(void)
{
    // dump the state of the memory pool
#ifdef _DEBUG
    std::string str;
    if (m_numAllocs != 0)
        str = "***(" + ToStr(m_numAllocs) + ") ";
    unsigned long totalNumChunks = m_numChunks * m_memArraySize;
    unsigned long wastedMem = (totalNumChunks - m_allocPeak) * m_chunkSize;
    str += "Destroying memory pool: [" + GetDebugName() + ":" + ToStr((unsigned long)m_chunkSize) + "] = " + ToStr(m_allocPeak) + "/" + ToStr((unsigned long)totalNumChunks) + " (" + ToStr(wastedMem) + " bytes wasted)\n";
    ::OutputDebugStringA(str.c_str());  // the logger is not initialized during many of the initial memory pool growths, so let's just use the OS version
#endif

    // free all memory
    for (unsigned int i = 0; i < m_memArraySize; ++i)
    {
        free(m_ppRawMemoryArray[i]);
    }
    free(m_ppRawMemoryArray);

    // update member variables
    Reset();
}

void* MemoryPool::Alloc(void)
{
    // If we're out of memory chunks, grow the pool.  This is very expensive.
    if (!m_pHead)
    {
        // if we don't allow resizes, return NULL
        if (!m_toAllowResize)
            return NULL;

        // attempt to grow the pool
        if (!GrowMemoryArray())
            return NULL;  // couldn't allocate anymore memory
    }

#ifdef _DEBUG
    // update allocation reports
    ++m_numAllocs;
    if (m_numAllocs > m_allocPeak)
        m_allocPeak = m_numAllocs;
#endif

    // grab the first chunk from the list and move to the next chunks
    unsigned char* pRet = m_pHead;
    m_pHead = GetNext(m_pHead);
    return (pRet + CHUNK_HEADER_SIZE);  // make sure we return a pointer to the data section only
}

void MemoryPool::Free(void* pMem)
{
    if (pMem != NULL)      // calling Free() on a NULL pointer is perfectly valid
    {
        // The pointer we get back is just to the data section of the chunk.  This gets us the full chunk.
        unsigned char* pBlock = ((unsigned char*)pMem) - CHUNK_HEADER_SIZE;

        // push the chunk to the front of the list
        SetNext(pBlock, m_pHead);
        m_pHead = pBlock;

#ifdef _DEBUG
        // update allocation reports
        --m_numAllocs;
        assert(m_numAllocs >= 0);
#endif
    }
}

void MemoryPool::Reset(void)
{
    m_ppRawMemoryArray = NULL;
    m_pHead = NULL;
    m_chunkSize = 0;
    m_numChunks = 0;
    m_memArraySize = 0;
    m_toAllowResize = true;
#ifdef _DEBUG
    m_allocPeak = 0;
    m_numAllocs = 0;
#endif
}

bool MemoryPool::GrowMemoryArray(void)
{
#ifdef _DEBUG
    std::string str("Growing memory pool: [" + GetDebugName() + ":" + ToStr((unsigned long)m_chunkSize) + "] = " + ToStr((unsigned long)m_memArraySize + 1) + "\n");
    LOG(str.c_str());  // the logger is not initialized during many of the initial memory pool growths, so let's just use the OS version
#endif

    // allocate a new array
    size_t allocationSize = sizeof(unsigned char*) * (m_memArraySize + 1);
    unsigned char** ppNewMemArray = (unsigned char**)malloc(allocationSize);

    // make sure the allocation succeeded
    if (!ppNewMemArray)
        return false;

    // copy any existing memory pointers over
    for (unsigned int i = 0; i < m_memArraySize; ++i)
    {
        ppNewMemArray[i] = m_ppRawMemoryArray[i];
    }

    // allocate a new block of memory
    ppNewMemArray[m_memArraySize] = AllocateNewMemoryBlock();  // indexing m_memArraySize here is safe because we haven't incremented it yet to reflect the new size    

    // attach the block to the end of the current memory list
    if (m_pHead)
    {
        unsigned char* pCurr = m_pHead;
        unsigned char* pNext = GetNext(m_pHead);
        while (pNext)
        {
            pCurr = pNext;
            pNext = GetNext(pNext);
        }
        SetNext(pCurr, ppNewMemArray[m_memArraySize]);
    }
    else
    {
        m_pHead = ppNewMemArray[m_memArraySize];
    }

    // destroy the old memory array
    if (m_ppRawMemoryArray)
        free(m_ppRawMemoryArray);

    // assign the new memory array and increment the size count
    m_ppRawMemoryArray = ppNewMemArray;
    ++m_memArraySize;

    return true;
}

unsigned char* MemoryPool::AllocateNewMemoryBlock(void)
{
    // calculate the size of each block and the size of the actual memory allocation
    size_t blockSize = m_chunkSize + CHUNK_HEADER_SIZE;  // chunk + linked list overhead
    size_t trueSize = blockSize * m_numChunks;

    // allocate the memory
    unsigned char* pNewMem = (unsigned char*)malloc(trueSize);
    if (!pNewMem)
        return NULL;

    // turn the memory into a linked list of chunks
    unsigned char* pEnd = pNewMem + trueSize;
    unsigned char* pCurr = pNewMem;
    while (pCurr < pEnd)
    {
        // calculate the next pointer position
        unsigned char* pNext = pCurr + blockSize;

        // set the next & prev pointers
        unsigned char** ppChunkHeader = (unsigned char**)pCurr;
        ppChunkHeader[0] = (pNext < pEnd ? pNext : NULL);

        // move to the next block
        pCurr += blockSize;
    }

    return pNewMem;
}

unsigned char* MemoryPool::GetNext(unsigned char* pBlock)
{
    unsigned char** ppChunkHeader = (unsigned char**)pBlock;
    return ppChunkHeader[0];
}

void MemoryPool::SetNext(unsigned char* pBlockToChange, unsigned char* pNewNext)
{
    unsigned char** ppChunkHeader = (unsigned char**)pBlockToChange;
    ppChunkHeader[0] = pNewNext;
}