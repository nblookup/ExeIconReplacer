////////////////////////////////////////////////////////////////////////////
// ExeIconReplacer.h -- replace icon of exe file
// Copyright (C) 2015 Katayama Hirofumi MZ.  All rights reserved.
////////////////////////////////////////////////////////////////////////////

#ifndef EXEICONREPLACER_H_
#define EXEICONREPLACER_H_ 1

#ifndef _INC_WINDOWS
    #include <windows.h>
#endif

#ifdef __cplusplus
    #include <cassert>
#else
    #include <assert.h>
#endif

// NOTE: define NO_ICONREPLACER_PROGRAM if you want to use as a library.
#undef NO_ICONREPLACER_PROGRAM
//#define NO_ICONREPLACER_PROGRAM

////////////////////////////////////////////////////////////////////////////

typedef struct
{
    WORD           idReserved;  // Reserved (must be 0)
    WORD           idType;      // Resource Type (1 for icons)
    WORD           idCount;     // How many images?
} ICONDIR, *LPICONDIR;

typedef struct
{
    BYTE        bWidth;         // Width, in pixels, of the image
    BYTE        bHeight;        // Height, in pixels, of the image
    BYTE        bColorCount;    // Number of colors in image (0 if >=8bpp)
    BYTE        bReserved;      // Reserved ( must be 0)
    WORD        wPlanes;        // Color Planes
    WORD        wBitCount;      // Bits per pixel
    DWORD       dwBytesInRes;   // How many bytes in this resource?
    DWORD       dwImageOffset;  // Where in the file is this image?
} ICONDIRENTRY, *LPICONDIRENTRY;

#include <pshpack2.h>
typedef struct
{
   BYTE   bWidth;               // Width, in pixels, of the image
   BYTE   bHeight;              // Height, in pixels, of the image
   BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
   BYTE   bReserved;            // Reserved
   WORD   wPlanes;              // Color Planes
   WORD   wBitCount;            // Bits per pixel
   DWORD   dwBytesInRes;        // how many bytes in this resource?
   WORD   nID;                  // the ID
} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;
#include <poppack.h>

////////////////////////////////////////////////////////////////////////////

// function ReplaceIconOfExeFile
BOOL ReplaceIconOfExeFile(LPCTSTR pszExeFile, LPCTSTR pszIconFile,
     UINT nIconGroupID, UINT nIconBaseID);

////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
    // class ExeIconReplacer
    class ExeIconReplacer {
    public:
        ExeIconReplacer() {
            memset(&_iconDir, 0, sizeof(_iconDir));
            _iconEntry = NULL;
            _iconImage = NULL;
            _iconGroupData = NULL;
        }

        ~ExeIconReplacer() {
            if (_iconImage) {
                for (int i = 0; i < _iconDir.idCount; i++) {
                    delete _iconImage[i];
                }
            }
            delete _iconImage;
            delete _iconEntry;
            delete _iconGroupData;
        }

        bool Load(LPCTSTR pszFileName);
        LPBYTE CreateIconGroupData(int nBaseID);

        int GetImageCount() const {
            return _iconDir.idCount;
        }

        LPBYTE GetImageData(int index) const {
            assert(0 <= index && index < GetImageCount());
            assert(_iconImage);
            return _iconImage[index];
        }

        DWORD GetImageSize(int index) const {
            assert(0 <= index && index < GetImageCount());
            assert(_iconEntry);
            return _iconEntry[index].dwBytesInRes;
        }

        BOOL IsIconDirOK() const {
            return _iconDir.idReserved == 0 &&
                   _iconDir.idType == 1 && _iconDir.idCount > 0;
        }

        int CountOfIconGroupData() const {
            return sizeof(ICONDIR) +
                   sizeof(GRPICONDIRENTRY) * GetImageCount();
        }

    protected:
        ICONDIR         _iconDir;
        ICONDIRENTRY*   _iconEntry;
        LPBYTE*         _iconImage;
        LPBYTE          _iconGroupData;
    }; // class ExeIconReplacer
#endif  // def __cplusplus

////////////////////////////////////////////////////////////////////////////

#endif  // ndef EXEICONREPLACER_H_
