#include <mmreg.h>      // has WAVE_FORMAT_* defines for wHaveACMCodec()


// return codes for wHaveWaveDevice()

#define HWD_SUCCESS     0
#define HWD_NODEVICE    1
#define HWD_NODRIVER    2
#define HWD_ERROR       3
#define HWD_NOFORMAT    4



// return codes for wHaveACMCodec()

#define HAC_SUCCESS     0   // ACM is installed, we've got a codec for the desired format
#define HAC_NOACM       1   // Audio compression manager is not installed or is an old version
#define HAC_NOCODEC     2   // ACM is installed, The requested codec is not active or installed
#define HAC_NOCONVERT   3   // ACM is installed, Codec is installed but won't convert wav data.



// video codec types

#define MS_VIDEO1       mmioFOURCC('m','s','v','c')
#define INTEL_INDEO32   mmioFOURCC('i','v','3','2')

// return codes for wHaveICMCodec()

#define HIC_SUCCESS     0   // ICM is installed, we've got a codec for the desired format
#define HIC_NOICM       1   // Installable compression manager is not installed or is an old version
#define HIC_NOCODEC     2   // The requested codec is not active or installed
#define HIC_NOCONVERT   3   // Codec is installed but won't convert wav data.



// Installable components

#define IC_MCI_SOUND    0   // MCI wave audio driver
#define IC_MCI_VFW      1   // MCI video driver
#define IC_ACM          2   // audio compression manager (wave mapper) driver
#define IC_ACM_ADPCM    3   // ACM Microsoft ADPCM codec driver
#define IC_ICM_VIDEO1   4   // Microsoft Video 1 codec driver
#define IC_ICM_INDEO32  5   // Intel Indeo 3.2 codec driver


#ifdef __cplusplus
extern "C" {
#endif

WORD wInstallComp(WORD wComp);
WORD wHaveWaveDevice(DWORD dwReqFormats);
WORD wHaveACM();
WORD wHaveACMCodec(DWORD dwReqCodec);
WORD wHaveICMCodec(DWORD dwReqCodec);
WORD wHaveMCI(LPSTR dwDeviceType);

#ifdef __cplusplus
}
#endif
