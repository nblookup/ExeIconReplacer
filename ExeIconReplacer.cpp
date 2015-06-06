////////////////////////////////////////////////////////////////////////////
// ExeIconReplacer.cpp -- replace icon of exe file
// Copyright (C) 2015 Katayama Hirofumi MZ.  All rights reserved.
////////////////////////////////////////////////////////////////////////////

#include "ExeIconReplacer.h"

////////////////////////////////////////////////////////////////////////////
// ExeIconReplacer

bool ExeIconReplacer::Load(LPCTSTR pszFileName) {
    bool bOK = false;

    HANDLE hFile = ::CreateFile(pszFileName, GENERIC_READ,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    assert(hFile != INVALID_HANDLE_VALUE);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD dwRead;
    if (::ReadFile(hFile, &_iconDir, sizeof(ICONDIR), &dwRead, NULL)) {
        if (IsIconDirOK()) {
            _iconEntry = new ICONDIRENTRY[_iconDir.idCount];
            bOK = true;
            for (int i = 0; i < _iconDir.idCount; i++) {
                if (!::ReadFile(hFile, &_iconEntry[i],
                                sizeof(ICONDIRENTRY), &dwRead, NULL)) {
                    bOK = false;
                    break;
                }
            }
            if (bOK) {
                _iconImage = new LPBYTE[_iconDir.idCount];
                bOK = true;
                for (int i = 0; i < _iconDir.idCount; i++) {
                    ::SetFilePointer(hFile, _iconEntry[i].dwImageOffset,
                                     NULL, FILE_BEGIN);
                    _iconImage[i] = new BYTE[_iconEntry[i].dwBytesInRes];
                    if (!::ReadFile(hFile, _iconImage[i],
                                    _iconEntry[i].dwBytesInRes,
                                    &dwRead, NULL))
                    {
                        for (int j = i; j >= 0; --j) {
                            delete[] _iconImage[i];
                            _iconImage[i] = NULL;
                        }
                        bOK = false;
                        break;
                    }
                }
                if (!bOK) {
                    delete[] _iconImage;
                    _iconImage = NULL;
                }
            }
            if (!bOK) {
                delete _iconEntry;
                _iconEntry = NULL;
            }
        }
    }
    ::CloseHandle(hFile);
    return bOK;
} // ExeIconReplacer::Load

LPBYTE ExeIconReplacer::CreateIconGroupData(int nBaseID) {
    delete _iconGroupData;

    _iconGroupData = new BYTE[CountOfIconGroupData()];
    memcpy(_iconGroupData, &_iconDir, sizeof(ICONDIR));

    int offset = sizeof(ICONDIR);
    for (int i = 0; i < GetImageCount(); i++) {
        BITMAPINFOHEADER bitmapheader;
        CopyMemory(&bitmapheader, GetImageData(i),
                   sizeof(BITMAPINFOHEADER));

        GRPICONDIRENTRY grpEntry;
        grpEntry.bWidth        = _iconEntry[i].bWidth;
        grpEntry.bHeight       = _iconEntry[i].bHeight;
        grpEntry.bColorCount   = _iconEntry[i].bColorCount;
        grpEntry.bReserved     = _iconEntry[i].bReserved;
        grpEntry.wPlanes       = bitmapheader.biPlanes;
        grpEntry.wBitCount     = bitmapheader.biBitCount;
        grpEntry.dwBytesInRes  = _iconEntry[i].dwBytesInRes;
        grpEntry.nID           = nBaseID + i;

        CopyMemory(_iconGroupData + offset, &grpEntry,
                   sizeof(GRPICONDIRENTRY));

        offset += sizeof(GRPICONDIRENTRY);
    }

    return _iconGroupData;
} // ExeIconReplacer::CreateIconGroupData

////////////////////////////////////////////////////////////////////////////

// function ReplaceIconOfExeFile
BOOL ReplaceIconOfExeFile(LPCTSTR pszExeFile, LPCTSTR pszIconFile,
     UINT nIconGroupID, UINT nIconBaseID)
{
    ExeIconReplacer replacer;
    if (replacer.Load(pszIconFile)) {
        HANDLE hUpdate = ::BeginUpdateResource(pszExeFile, FALSE);
        if (hUpdate) {
            // RT_GROUP_ICON
            BOOL bOK = ::UpdateResource(hUpdate, RT_GROUP_ICON,
                MAKEINTRESOURCE(nIconGroupID), 0,
                replacer.CreateIconGroupData(nIconBaseID),
                replacer.CountOfIconGroupData()
            );

            // RT_ICON
            if (bOK) {
                for (int i = 0; i < replacer.GetImageCount(); i++) {
                    bOK = ::UpdateResource(hUpdate, RT_ICON,
                        MAKEINTRESOURCE(nIconBaseID+i), 0,
                        replacer.GetImageData(i),
                        replacer.GetImageSize(i)
                    );
                    if (!bOK) {
                        break;
                    }
                }
            }

            // finish
            if (bOK) {
                return ::EndUpdateResource(hUpdate, FALSE);
            }
            ::EndUpdateResource(hUpdate, TRUE);
        }
    }
    return FALSE;
} // ReplaceIconOfExeFile

////////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <tchar.h>

#ifdef _UNICODE
    typedef std::wstring tstring;
    #define tcout std::wcout
#else
    typedef std::string tstring;
    #define tcout std::cout
#endif

////////////////////////////////////////////////////////////////////////////

#ifndef NO_ICONREPLACER_PROGRAM
extern "C"
int _tmain(int argc, _TCHAR **targv) {
#if 0
    // test only
    if (ReplaceIconOfExeFile(TEXT("hello.exe"), TEXT("excel.ico"), 1, 1)) {
        std::cout << "success" << std::endl;
        return 0;
    }
    std::cout << "failure" << std::endl;
    return 1;
#endif

    if (argc != 3) {
        // show usage
        tstring program = targv[0];
        size_t i = program.find_last_of(TEXT("\\/"));
        if (i != std::string::npos) {
            program = program.substr(i + 1);
        }
        tcout << TEXT("Usage: ") << program <<
                 TEXT(" exefile.exe iconfile.ico") << std::endl;
        return 0;
    }

    // replace icon
    if (ReplaceIconOfExeFile(targv[1], targv[2], 1, 1)) {
        std::cout << "success" << std::endl;
        return 0;
    }
    std::cout << "failure" << std::endl;
    return 1;
} // _tmain
#endif  // ndef NO_ICONREPLACER_PROGRAM

////////////////////////////////////////////////////////////////////////////
