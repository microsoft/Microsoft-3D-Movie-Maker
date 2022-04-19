/*----------------------------------------------------------------------------*\
|   Routines for dealing with Device independent bitmaps                       |
|									       |
|   History:                                                                   |
|       06/23/89 toddla     Created                                            |
|                                                                              |
\*----------------------------------------------------------------------------*/

typedef     LPBITMAPINFOHEADER PDIB;
typedef     HANDLE             HDIB;

long DibDraw(HDC hdc,  int x,  int y, PDIB pdib, int x0, int y0, LONG rop, UINT wUsage);
PDIB        DibCreate(int bits, int dx, int dy);

/****************************************************************************
 DIB macros.
 ***************************************************************************/

#define _abs(x)    (((x) < 0) ? -(x) : (x))

#ifdef  WIN32
    #define HandleFromDib(lpbi) GlobalHandle(lpbi)
#else
    #define HandleFromDib(lpbi) (HANDLE)GlobalHandle(SELECTOROF(lpbi))
#endif

#define DibFromHandle(h)        (PDIB)GlobalLock(h)

#define DibFree(pdib)           GlobalFreePtr(pdib)

#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */

#define DibWidth(lpbi)          _abs((int)(LONG)(((LPBITMAPINFOHEADER)(lpbi))->biWidth))
#define DibHeight(lpbi)         _abs((int)(LONG)(((LPBITMAPINFOHEADER)(lpbi))->biHeight))
#define DibBitCount(lpbi)       (UINT)(((LPBITMAPINFOHEADER)(lpbi))->biBitCount)
#define DibCompression(lpbi)    (DWORD)(((LPBITMAPINFOHEADER)(lpbi))->biCompression)

#define DibWidthBytesN(lpbi, n) (UINT)WIDTHBYTES((UINT)(lpbi)->biWidth * (UINT)(n))
#define DibWidthBytes(lpbi)     DibWidthBytesN(lpbi, (lpbi)->biBitCount)

#define DibSizeImage(lpbi)      ((lpbi)->biSizeImage == 0 \
                                    ? ((DWORD)(UINT)DibWidthBytes(lpbi) * (DWORD)DibHeight(lpbi)) \
                                    : (lpbi)->biSizeImage)

#define DibSize(lpbi)           ((lpbi)->biSize + (lpbi)->biSizeImage + (int)(lpbi)->biClrUsed * sizeof(RGBQUAD))
#define DibPaletteSize(lpbi)    (DibNumColors(lpbi) * sizeof(RGBQUAD))

#define DibFlipY(lpbi, y)       ((int)_abs((lpbi)->biHeight)-1-(y))

//HACK for NT BI_BITFIELDS DIBs
#if 1 // WIN32
    #define DibPtr(lpbi)            ((lpbi)->biCompression == BI_BITFIELDS \
                                       ? (LPVOID)(DibColors(lpbi) + 3) \
                                       : (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed))
#else
    #define DibPtr(lpbi)            (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed)
#endif

#define DibColors(lpbi)         ((RGBQUAD FAR *)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))

#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)

#define DibNextScan(lpbi)       ((lpbi)->biHeight < 0 ? -(long)DibWidthBytes(lpbi) : (long)DibWidthBytes(lpbi))
#define DibX(lpbi,x)            (x)
#define DibY(lpbi,y)            ((lpbi)->biHeight < 0 ? DibFlipY(lpbi,y) : (y))

#define DibXYN(lpbi,pb,x,y,n)   (LPVOID)(                                     \
                                (BYTE _huge *)(pb) +                          \
                                (UINT)((UINT)(x) * (UINT)(n) / 8u) +          \
                                ((DWORD)DibWidthBytesN(lpbi,n) * (DWORD)DibY(lpbi,y)))

#define DibXY(lpbi,x,y)         DibXYN(lpbi,DibPtr(lpbi),x,y,(lpbi)->biBitCount)

#define FixBitmapInfo(lpbi)     if ((lpbi)->biSizeImage == 0)                 \
                                    (lpbi)->biSizeImage = DibSizeImage(lpbi); \
                                if ((lpbi)->biClrUsed == 0)                   \
                                    (lpbi)->biClrUsed = DibNumColors(lpbi);   \
                                if ((lpbi)->biCompression == BI_BITFIELDS && (lpbi)->biClrUsed == 0) \
                                    ; // (lpbi)->biClrUsed = 3;


/****************************************************************************
 ***************************************************************************/

#ifndef BI_BITFIELDS
	#define BI_BITFIELDS 3
#endif

#ifndef HALFTONE
	#define HALFTONE COLORONCOLOR
#endif

#ifndef DIB_PAL_INDICES
#define DIB_PAL_INDICES 2
#endif

