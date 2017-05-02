#ifndef __ZIPFILE_H__
#define __ZIPFILE_H__
//========================================================================
// ZipFile.h : API to use Zip files
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

#include "../SharedDefines.h"

typedef std::map<std::string, int> ZipContentsMap;        // maps path to a zip content id
typedef std::vector<std::string> FileList;
typedef std::map<std::string, FileList> DirToFileListMap;

class ZipFile
{
public:
    ZipFile() { m_nEntries = 0; m_pFile = NULL; m_pDirData = NULL; }
    virtual ~ZipFile() { End(); fclose(m_pFile); }

    bool Init(const std::string &resFileName);
    void End();

    int GetNumFiles()const { return m_nEntries; }
    std::string GetFilename(int i) const;
    int GetFileLen(int i) const;
    bool ReadFile(int i, void *pBuf);
    std::vector<std::string> GetAllFilesInDirectory(const std::string& dirPath);

    // Added to show multi-threaded decompression
    bool ReadLargeFile(int i, void *pBuf, void(*progressCallback)(int, bool &));

    int Find(const std::string &path) const;

    ZipContentsMap m_ZipContentsMap;
    DirToFileListMap m_DirToFileListMap;

private:
    struct TZipDirHeader;
    struct TZipDirFileHeader;
    struct TZipLocalHeader;

    FILE *m_pFile;        // Zip file
    char *m_pDirData;    // Raw data buffer.
    int  m_nEntries;    // Number of entries.

    // Pointers to the dir entries in pDirData.
    const TZipDirFileHeader **m_papDir;
};

#endif