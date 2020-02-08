//========================================================================
// ZipFile.cpp : API to use Zip files
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

#define _CRT_SECURE_NO_DEPRECATE

#include <cctype>            // for std::tolower
#include <memory>

#include "ZipFile.h"

#include "Miniz.h"
#include <string.h>

// Windows...
#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

// --------------------------------------------------------------------------
// ZIP file structures. Note these have to be packed.
// --------------------------------------------------------------------------

#pragma pack(1)
// --------------------------------------------------------------------------
// struct ZipFile::TZipLocalHeader                    - Chapter 8, page 215
// --------------------------------------------------------------------------
struct ZipFile::TZipLocalHeader
{
    enum
    {
        SIGNATURE = 0x04034b50,
    };
    uint32   sig;
    uint16   version;
    uint16   flag;
    uint16   compression;      // Z_NO_COMPRESSION or Z_DEFLATED
    uint16   modTime;
    uint16   modDate;
    uint32   crc32;
    uint32   cSize;
    uint32   ucSize;
    uint16   fnameLen;         // Filename string follows header.
    uint16   xtraLen;          // Extra field follows filename.
};

// --------------------------------------------------------------------------
// struct ZipFile::TZipDirHeader                    - Chapter 8, page 215
// --------------------------------------------------------------------------
struct ZipFile::TZipDirHeader
{
    enum
    {
        SIGNATURE = 0x06054b50
    };
    uint32   sig;
    uint16   nDisk;
    uint16   nStartDisk;
    uint16   nDirEntries;
    uint16   totalDirEntries;
    uint32   dirSize;
    uint32   dirOffset;
    uint16   cmntLen;
};

// --------------------------------------------------------------------------
// struct ZipFile::TZipDirFileHeader                    - Chapter 8, page 215
// --------------------------------------------------------------------------
struct ZipFile::TZipDirFileHeader
{
    enum
    {
        SIGNATURE = 0x02014b50
    };
    uint32   sig;
    uint16   verMade;
    uint16   verNeeded;
    uint16   flag;
    uint16   compression;      // COMP_xxxx
    uint16   modTime;
    uint16   modDate;
    uint32   crc32;
    uint32   cSize;            // Compressed size
    uint32   ucSize;           // Uncompressed size
    uint16   fnameLen;         // Filename string follows header.
    uint16   xtraLen;          // Extra field follows filename.
    uint16   cmntLen;          // Comment field follows extra field.
    uint16   diskStart;
    uint16   intAttr;
    uint32   extAttr;
    uint32   hdrOffset;

    char *GetName() const { return (char *)(this + 1); }
    char *GetExtra() const { return GetName() + fnameLen; }
    char *GetComment() const { return GetExtra() + xtraLen; }
};

#pragma pack()

static bool IsZipDir(const std::string& node)
{
    return node.back() == '/';
}

static char* string_to_lowercase(char* s)
{
    char* tmp = s;

    for (; *tmp; ++tmp) {
        *tmp = tolower((unsigned char)*tmp);
    }

    return s;
}

// --------------------------------------------------------------------------
// Function:      Init
// Purpose:       Initialize the object and read the zip file directory.
// Parameters:    A stdio FILE* used for reading.
// --------------------------------------------------------------------------
bool ZipFile::Init(const std::string &resFileName)
{
    End();

    m_pFile = fopen(resFileName.c_str(), "rb");
    //_wfopen_s(&m_pFile, resFileName.c_str(), "rb"));
    if (!m_pFile)
        return false;

    // Assuming no extra comment at the end, read the whole end record.
    TZipDirHeader dh;

    fseek(m_pFile, -(int)sizeof(dh), SEEK_END);
    long dhOffset = ftell(m_pFile);
    memset(&dh, 0, sizeof(dh));
    if (fread(&dh, sizeof(dh), 1, m_pFile) != 1)
        return false;

    // Check
    if (dh.sig != TZipDirHeader::SIGNATURE)
        return false;

    // Go to the beginning of the directory.
    fseek(m_pFile, dhOffset - dh.dirSize, SEEK_SET);

    // Allocate the data buffer, and read the whole thing.
    m_pDirData = new /*(std::nothrow)*/ char[dh.dirSize + dh.nDirEntries*sizeof(*m_papDir)];
    if (!m_pDirData)
        return false;
    memset(m_pDirData, 0, dh.dirSize + dh.nDirEntries*sizeof(*m_papDir));
    if (fread(m_pDirData, dh.dirSize, 1, m_pFile) != 1)
        return false;

    // Now process each entry.
    char *pfh = m_pDirData;
    m_papDir = (const TZipDirFileHeader **)(m_pDirData + dh.dirSize);

    bool success = true;

    //std::string lastDirPath = "/";
    std::string dirPath;

    for (int i = 0; i < dh.nDirEntries && success; i++)
    {
        TZipDirFileHeader &fh = *(TZipDirFileHeader*)pfh;

        // Store the address of nth file for quicker access.
        m_papDir[i] = &fh;

        // Check the directory entry integrity.
        if (fh.sig != TZipDirFileHeader::SIGNATURE)
            success = false;
        else
        {
            pfh += sizeof(fh);

            char fileName[_MAX_PATH];
            memcpy(fileName, pfh, fh.fnameLen);
            fileName[fh.fnameLen] = 0;
            string_to_lowercase(fileName);
            std::string spath = fileName;
            spath.insert(0, "/");
            m_ZipContentsMap[spath] = i;

            //LOG("spath: " + spath);

            // Skip name, extra and comment fields.
            pfh += fh.fnameLen + fh.xtraLen + fh.cmntLen;

            std::string dirName = spath;
            auto pos = dirName.rfind("/");
            if (pos != std::string::npos)
            {
                if (pos == 0) {
                    dirName = "/";
                } else {
                    dirName.erase(pos);
                }
            }
            if (dirName.back() != '/')
            {
                dirName += '/';
            }
            //LOG("DirName: " + dirName);

            if (!IsZipDir(spath))
            {
                m_DirToFileListMap[dirName].push_back(spath);
            }
        }
    }
    if (!success)
    {
        SAFE_DELETE_ARRAY(m_pDirData);
    }
    else
    {
        m_nEntries = dh.nDirEntries;
    }

    /*for (auto it : m_DirToFileListMap)
    {
        LOG("DIR: " + it.first);
        LOG("--------------");
        for (auto it2 : it.second)
        {
            LOG("FILE: " + it2);
        }
        LOG("==============");
    }*/

    return success;
}

int ZipFile::Find(const std::string &path) const
{
    std::string lowerCase = path;
    std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), (int(*)(int)) std::tolower);
    // In case the name doesnt start with forward flash, e.g. path == "folder1/folder2/file1", convert it
    // to "/folder1/folder2/file1 format
    if (lowerCase.size() > 0 && lowerCase[0] != '/')
    {
        lowerCase.insert(0, "/");
    }
    ZipContentsMap::const_iterator i = m_ZipContentsMap.find(lowerCase);
    if (i == m_ZipContentsMap.end())
        return -1;

    return i->second;
}

std::vector<std::string>  ZipFile::GetAllFilesInDirectory(const std::string& dirPath)
{
    return {};
}

// --------------------------------------------------------------------------
// Function:      End
// Purpose:       Finish the object
// Parameters:    
// --------------------------------------------------------------------------
void ZipFile::End()
{
    m_ZipContentsMap.clear();
    SAFE_DELETE_ARRAY(m_pDirData);
    m_nEntries = 0;
}

// --------------------------------------------------------------------------
// Function:      GetFilename
// Purpose:       Return the name of a file
// Parameters:    The file index and the buffer where to store the filename
// --------------------------------------------------------------------------
std::string ZipFile::GetFilename(int i)  const
{
    std::string fileName = "";
    if (i >= 0 && i < m_nEntries)
    {
        char pszDest[_MAX_PATH];
        memcpy(pszDest, m_papDir[i]->GetName(), m_papDir[i]->fnameLen);
        pszDest[m_papDir[i]->fnameLen] = '\0';
        fileName = pszDest;

        if (fileName.size() > 0 && fileName[0] != '/')
        {
            fileName.insert(0, "/");
        }
    }
    return fileName;
}


// --------------------------------------------------------------------------
// Function:      GetFileLen
// Purpose:       Return the length of a file so a buffer can be allocated
// Parameters:    The file index.
// --------------------------------------------------------------------------
int ZipFile::GetFileLen(int i) const
{
    if (i < 0 || i >= m_nEntries)
        return -1;
    else
        return m_papDir[i]->ucSize;
}

// --------------------------------------------------------------------------
// Function:      ReadFile
// Purpose:       Uncompress a complete file
// Parameters:    The file index and the pre-allocated buffer
// --------------------------------------------------------------------------
bool ZipFile::ReadFile(int i, void *pBuf)
{
    if (pBuf == NULL || i < 0 || i >= m_nEntries)
        return false;

    // Quick'n dirty read, the whole file at once.
    // Ungood if the ZIP has huge files inside

    // Go to the actual file and read the local header.
    fseek(m_pFile, m_papDir[i]->hdrOffset, SEEK_SET);
    TZipLocalHeader h;

    memset(&h, 0, sizeof(h));
    if (fread(&h, sizeof(h), 1, m_pFile) != 1)
        return false;
    if (h.sig != TZipLocalHeader::SIGNATURE)
        return false;

    // Skip extra fields
    fseek(m_pFile, h.fnameLen + h.xtraLen, SEEK_CUR);

    if (h.compression == Z_NO_COMPRESSION)
    {
        // Simply read in raw stored data.
        if (fread(pBuf, h.cSize, 1, m_pFile) != 1)
            return false;
        return true;
    }
    else if (h.compression != Z_DEFLATED)
        return false;

    // Alloc compressed data buffer and read the whole stream
    std::unique_ptr<char[]> pcData = std::unique_ptr<char[]>{ new /*(std::nothrow)*/ char[h.cSize] };
    if (!pcData)
        return false;

    memset(pcData.get(), 0, h.cSize);
    if (fread(pcData.get(), h.cSize, 1, m_pFile) != 1)
        return false;

    bool ret = true;

    // Setup the inflate stream.
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)pcData.get();
    stream.avail_in = (uInt)h.cSize;
    stream.next_out = (Bytef*)pBuf;
    stream.avail_out = h.ucSize;
    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    // Perform inflation. wbits < 0 indicates no zlib header inside the data.
    err = inflateInit2(&stream, -MAX_WBITS);
    if (err == Z_OK)
    {
        err = inflate(&stream, Z_FINISH);
        inflateEnd(&stream);
        if (err == Z_STREAM_END)
            err = Z_OK;
        inflateEnd(&stream);
    }
    if (err != Z_OK)
        ret = false;

    return ret;
}



// --------------------------------------------------------------------------
// Function:      ReadLargeFile
// Purpose:       Uncompress a complete file with callbacks.
// Parameters:    The file index and the pre-allocated buffer
// --------------------------------------------------------------------------
bool ZipFile::ReadLargeFile(int i, void *pBuf, void(*progressCallback)(int, bool &))
{
    if (pBuf == NULL || i < 0 || i >= m_nEntries)
        return false;

    // Quick'n dirty read, the whole file at once.
    // Ungood if the ZIP has huge files inside

    // Go to the actual file and read the local header.
    fseek(m_pFile, m_papDir[i]->hdrOffset, SEEK_SET);
    TZipLocalHeader h;

    memset(&h, 0, sizeof(h));
    if (fread(&h, sizeof(h), 1, m_pFile) != 1)
        return false;
    if (h.sig != TZipLocalHeader::SIGNATURE)
        return false;

    // Skip extra fields
    fseek(m_pFile, h.fnameLen + h.xtraLen, SEEK_CUR);

    if (h.compression == Z_NO_COMPRESSION)
    {
        // Simply read in raw stored data.
        if (fread(pBuf, h.cSize, 1, m_pFile) != 1)
            return false;
        return true;
    }
    else if (h.compression != Z_DEFLATED)
        return false;

    // Alloc compressed data buffer and read the whole stream
    std::unique_ptr<char[]> pcData = std::unique_ptr<char[]>{ new /*(std::nothrow)*/ char[h.cSize] };
    if (!pcData)
        return false;

    memset(pcData.get(), 0, h.cSize);
    if (fread(pcData.get(), h.cSize, 1, m_pFile) != 1)
        return false;

    bool ret = true;

    // Setup the inflate stream.
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)pcData.get();
    stream.avail_in = (uInt)h.cSize;
    stream.next_out = (Bytef*)pBuf;
    stream.avail_out = (128 * 1024); //  read 128k at a time h.ucSize;
    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    // Perform inflation. wbits < 0 indicates no zlib header inside the data.
    err = inflateInit2(&stream, -MAX_WBITS);
    if (err == Z_OK)
    {
        uInt count = 0;
        bool cancel = false;
        while (stream.total_in < (uInt)h.cSize && !cancel)
        {
            err = inflate(&stream, Z_SYNC_FLUSH);
            if (err == Z_STREAM_END)
            {
                err = Z_OK;
                break;
            }
            else if (err != Z_OK)
            {
                assert(false && "Something happened.");
                break;
            }

            stream.avail_out = (128 * 1024);
            stream.next_out += stream.total_out;

            progressCallback(count * 100 / h.cSize, cancel);
        }
        inflateEnd(&stream);
    }
    if (err != Z_OK)
        ret = false;

    return ret;
}
