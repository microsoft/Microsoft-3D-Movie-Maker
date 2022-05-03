/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Contains AudioMan Pubic Interfaces and Types
//
//	Copyright (c) Microsoft Corporation	1995
//    
//	4/27/95 *****
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _AUDIOMANPUBLICINTEFACES_
#define _AUDIOMANPUBLICINTEFACES_

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Class ID's
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _CLSID_AMMixer 94C7E510-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMMixer, 0x94C7E510L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMWavFileSrc 94C7E514-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMWavFileSrc, 0x94C7E514L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMSilentSound 94C7E516-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMSilentSound, 0x94C7E516L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMAppendFilter 94C7E517-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMAppendFilter, 0x94C7E517L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMClipFilter 94C7E518-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMClipFilter, 0x94C7E518L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMConvertFilter 94C7E522-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMConvertFilter, 0x94C7E522L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMDelayFilter 94C7E519-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMDelayFilter, 0x94C7E519L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMGainFilter 94C7E51A-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMGainFilter, 0x94C7E51AL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMGateFilter 94C7E51B-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMGateFilter, 0x94C7E51BL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMLoopFilter 94C7E51C-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMLoopFilter, 0x94C7E51CL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMMixFilter 94C7E51D-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMMixFilter, 0x94C7E51DL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMFaderFilter 94C7E51E-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMFaderFilter, 0x94C7E51EL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMScheduleFilter 94C7E51F-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMScheduleFilter, 0x94C7E51FL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMRandomizeFilter 94C7E520-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMRandomizeFilter, 0x94C7E520L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMDistortFilter 94C7E521-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMDistortFilter, 0x94C7E521L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMCacheFilter 94C7E523-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMCacheFilter, 0x94C7E523L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMTrimFilter 94C7E524-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMTrimFilter, 0x94C7E524L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMStereoFilter 94C7E525-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMStereoFilter, 0x94C7E525L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _CLSID_AMBiasFilter 94C7E526-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(CLSID_AMBiasFilter, 0x94C7E526L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Interface Definitions 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _IID_IAMMixer A0434E40-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMMixer, 0xA0434E40L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMChannel A0434E42-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMChannel, 0xA0434E42L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMWaveOut A0434E43-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMWaveOut, 0xA0434E43L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMSound A0434E44-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMSound, 0xA0434E44L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMNotifySink A0434E45-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMNotifySink, 0xA0434E45L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMWavFileSrc A0434E46-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMWavFileSrc, 0xA0434E46L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMSilentSound A0434E48-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMSilentSound, 0xA0434E48L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMAppendFilter A0434E49-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMAppendFilter, 0xA0434E49L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMClipFilter A0434E4A-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMClipFilter, 0xA0434E4AL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMConvertFilter A0434E4B-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMConvertFilter, 0xA0434E4BL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMDelayFilter A0434E4C-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMDelayFilter, 0xA0434E4CL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMGainFilter A0434E4D-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMGainFilter, 0xA0434E4DL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMGateFilter A0434E4E-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMGateFilter, 0xA0434E4EL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMLoopFilter A0434E4F-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMLoopFilter, 0xA0434E4FL, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMMixFilter A0434E50-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMMixFilter, 0xA0434E50L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMFaderFilter A0434E51-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMFaderFilter, 0xA0434E51L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMScheduleFilter A0434E52-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMScheduleFilter, 0xA0434E52L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMRandomizeFilter A0434E53-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMRandomizeFilter, 0xA0434E53L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMDistortFilter A0434E54-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMDistortFilter, 0xA0434E54L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMCacheFilter A0434E55-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMCacheFilter, 0xA0434E55L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMTrimFilter A0434E56-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMTrimFilter, 0xA0434E56L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMStereoFilter A0434E57-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMStereoFilter, 0xA0434E57L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

#define _IID_IAMBiasFilter A0434E58-9573-11CE-B61B-00AA006EBBE5
DEFINE_GUID(IID_IAMBiasFilter, 0xA0434E58L, 0x9573, 0x11CE, 0xB6, 0x1B, 0x00, 0xAA, 0x00, 0x6E, 0xBB, 0xE5);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Interface Typedefs
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef interface IAMMixer				IAMMixer;
typedef IAMMixer FAR *					LPMIXER;

typedef interface IAMChannel			IAMChannel;
typedef IAMChannel FAR *				LPCHANNEL;

typedef interface IAMWaveOut			IAMWaveOut;
typedef IAMWaveOut FAR *				LPWAVEOUT;

typedef interface IAMSound				IAMSound;
typedef IAMSound FAR *					LPSOUND;

typedef interface IAMNotifySink			IAMNotifySink;
typedef IAMNotifySink FAR *				LPNOTIFYSINK;

typedef interface IAMWavFileSrc			IAMWavFileSrc;
typedef IAMWavFileSrc FAR *				LPWAVFILESRC;

typedef interface IAMSilentSound		IAMSilentSound;
typedef IAMSilentSound FAR *			LPSILENTSOUND;

typedef interface IAMAppendFilter		IAMAppendFilter;
typedef IAMAppendFilter FAR *			LPAPPENDFILTER;

typedef interface IAMClipFilter			IAMClipFilter;
typedef IAMClipFilter FAR *				LPCLIPFILTER;

typedef interface IAMConvertFilter		IAMConvertFilter;
typedef IAMConvertFilter FAR *			LPCONVERTFILTER;

typedef interface IAMDelayFilter		IAMDelayFilter;
typedef IAMDelayFilter FAR *			LPDELAYFILTER;

typedef interface IAMGainFilter			IAMGainFilter;
typedef IAMGainFilter FAR *				LPGAINFILTER;

typedef interface IAMGateFilter			IAMGateFilter;
typedef IAMGateFilter FAR *				LPGATEFILTER;

typedef interface IAMLoopFilter			IAMLoopFilter;
typedef IAMLoopFilter FAR *				LPLOOPFILTER;

typedef interface IAMMixFilter			IAMMixFilter;
typedef IAMMixFilter FAR *				LPMIXFILTER;

typedef interface IAMFaderFilter		IAMFaderFilter;
typedef IAMFaderFilter FAR *			LPFADERFILTER;

typedef interface IAMScheduleFilter		IAMScheduleFilter;
typedef IAMScheduleFilter FAR *			LPSCHEDULEFILTER;

typedef interface IAMRandomizeFilter	IAMRandomizeFilter;
typedef IAMRandomizeFilter FAR *		LPRANDOMIZEFILTER;

typedef interface IAMDistortFilter		IAMDistortFilter;
typedef IAMDistortFilter FAR *			LPDISTORTFILTER;

typedef interface IAMCacheFilter		IAMCacheFilter;
typedef IAMCacheFilter FAR *			LPCACHEFILTER;

typedef interface IAMTrimFilter			IAMTrimFilter;
typedef IAMTrimFilter FAR *				LPTRIMFILTER;

typedef interface IAMStereoFilter		IAMStereoFilter;
typedef IAMStereoFilter FAR *			LPSTEREOFILTER;

typedef interface IAMBiasFilter			IAMBiasFilter;
typedef IAMBiasFilter FAR *				LPBIASFILTER;

#ifndef LPSTREAM
typedef IStream FAR *					LPSTREAM;
#endif

#ifndef LPUNKNOWN
typedef IUnknown FAR *					LPUNKNOWN;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Audio HRESULT Return Codes
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Success Codes

#define AM_S_FIRST				(OLE_S_FIRST + 5000)

#define	S_ENDOFSOUND			(AM_S_FIRST + 1)
#define S_SOUNDIDLE				(AM_S_FIRST + 2)

// Error Codes

#define AM_E_FIRST				(OLE_E_FIRST + 5000)

#define E_BADPCMFORMAT			(AM_E_FIRST + 1)
#define E_NOSOUNDCARD			(AM_E_FIRST + 2)
#define E_INVALIDCARD			(AM_E_FIRST + 3)
#define E_AUDIODEVICEBUSY		(AM_E_FIRST + 4)
#define E_NOTACTIVE				(AM_E_FIRST + 5)
#define E_NEEDSETUP				(AM_E_FIRST + 6)
#define	E_OPENDEVICEFAILED		(AM_E_FIRST + 7)
#define E_INITFAILED			(AM_E_FIRST + 8)
#define E_NOTINITED				(AM_E_FIRST + 9)
#define E_MMSYSERROR			(AM_E_FIRST + 10)
#define E_MMSYSBADDEVICEID		(AM_E_FIRST + 11)
#define E_MMSYSNOTENABLED		(AM_E_FIRST + 12)
#define E_MMSYSALLOCATED		(AM_E_FIRST + 13)
#define E_MMSYSINVALHANDLE		(AM_E_FIRST + 14)
#define E_MMSYSNODRIVER			(AM_E_FIRST + 15)
#define E_MMSYSNOMEM			(AM_E_FIRST + 16)
#define E_MMSYSNOTSUPPORTED		(AM_E_FIRST + 17)
#define E_MMSYSBADERRNUM		(AM_E_FIRST + 18)
#define E_MMSYSINVALFLAG		(AM_E_FIRST + 19)
#define E_MMSYSINVALPARAM		(AM_E_FIRST + 20)
#define E_MMSYSHANDLEBUSY		(AM_E_FIRST + 21)
#define E_MMSYSINVALIDALIAS		(AM_E_FIRST + 22)
#define E_MMSYSBADDB			(AM_E_FIRST + 23)
#define E_MMSYSKEYNOTFOUND		(AM_E_FIRST + 24)
#define E_MMSYSREADERROR		(AM_E_FIRST + 25)
#define E_MMSYSWRITEERROR		(AM_E_FIRST + 26)
#define E_MMSYSDELETEERROR		(AM_E_FIRST + 27)
#define E_MMSYSVALNOTFOUND		(AM_E_FIRST + 28)
#define E_MMSYSNODRIVERCB		(AM_E_FIRST + 29)
#define E_WAVEERRBADFORMAT		(AM_E_FIRST + 30)
#define E_WAVEERRSTILLPLAYING	(AM_E_FIRST + 31)
#define E_WAVERRUNPREPARED		(AM_E_FIRST + 32)
#define E_WAVERRSYNC			(AM_E_FIRST + 33)
#define	E_TIMERRNOCANDO			(AM_E_FIRST	+ 34)
#define	E_TIMERRSTRUCT			(AM_E_FIRST	+ 35)
#define E_ALREADYREGISTERED		(AM_E_FIRST + 36)
#define E_CHANNELNOTREGISTERED	(AM_E_FIRST + 37)
#define E_ALLGROUPSALLOCATED	(AM_E_FIRST + 38)
#define E_GROUPNOTALLOCATED		(AM_E_FIRST + 39)
#define E_BADTIMERPERIOD		(AM_E_FIRST + 40)
#define E_NOTIMER				(AM_E_FIRST + 41)
		
// Mixer Error Codes

#define E_ALREADYINITED		(OLE_E_FIRST + 8)


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// AudioMan common defines
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

enum RAND_FREQUENCY 
{
	NORMAL_FREQ			= 	0,		
	LO_OCCURRING		= 	1,		
	HI_OCCURRING		= 	2	
};


enum SMPTE_FRAMERATES
{
	SMPTE_30_FPS		= 	3000,	
	SMPTE_29DF_FPS		= 	2997,	
	SMPTE_29_FPS		= 	2900,			
	SMPTE_25_FPS		= 	2500	
}; 

enum NOTIFYSINKFLAGS
{
	NOTIFYSINK_ONSTART		= 1,	
	NOTIFYSINK_ONCOMPLETION	= 2,
	NOTIFYSINK_ONERROR		= 4,	
	NOTIFYSINK_ONSYNCOBJECT	= 8	
};

enum DELAY_FLAGS
{
	DELAY_FADE			= 	1,		
	DELAY_IN			= 	2,				
	DELAY_OUT			= 	4,		
	DELAY_FLIP			= 	8		
};


enum WODM_CUSTOM
{
	WODM_SETGAIN	= 	100,		
	WODM_GETGAIN	= 	101,			
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// AudioMan Type definitions
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
				
typedef struct SMPTE							
{
	BYTE					hour;				
	BYTE					min;
	BYTE					sec;
	BYTE					frame;
	DWORD					fps;	
}SMPTE, FAR * LPSMPTE;

typedef struct CacheConfig
{
	DWORD					dwSize;
	BOOL					fSrcFormat;
	LPWAVEFORMATEX			lpFormat;
	DWORD					dwFormat;
	DWORD					dwCacheTime;
}CACHECONFIG, FAR * LPCACHECONFIG;


typedef struct AdvMixerConfig
{					   	
	DWORD					dwSize;
	UINT					uVoices;
	BOOL					fRemixEnabled;
	UINT					uBufferTime;
}ADVMIXCONFIG, FAR * LPADVMIXCONFIG;
								
	
typedef struct MixerConfig
{
	DWORD					dwSize;
	LPWAVEFORMATEX			lpFormat;
	DWORD					dwFormat;
}MIXERCONFIG, FAR * LPMIXERCONFIG;


typedef struct SchedulerConfig
{
	DWORD					dwSize;
	DWORD					dwUser;
	DWORD					dwDuration;
	DWORD					dwLoops;
	BOOL					fExclusiveBlocks;
}SCHEDULERCONFIG, FAR * LPSCHEDULERCONFIG;


typedef struct RandomizeConfig
{
	DWORD					dwSize;				
	DWORD					dwUser;			
	DWORD					dwNumSubChannels;
	DWORD					dwMinTime;		
	DWORD					dwMaxTime;		
	BOOL					fRandomVol;		
	BOOL					fRandomPan;		
	DWORD					dwMinVol;		
	DWORD					dwMaxVol;		
	DWORD					dwMaxPan;
}RANDOMIZECONFIG, FAR * LPRANDOMIZECONFIG;
 


typedef struct RequestParam
{
	DWORD					dwSize;
	DWORD					dwFinishPos;
	LPNOTIFYSINK 			lpNotifySink;
	DWORD 					fdwNotifyMask;
//	REFIID 					riid;
}REQUESTPARAM, FAR * LPREQUESTPARAM;



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// AudioMan Interface Definitions
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////


DECLARE_INTERFACE_( IAMMixer, IUnknown)
{
    //---  IUnknown methods--- 

    STDMETHOD (QueryInterface) 			(THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) 			(THIS) PURE;
    STDMETHOD_(ULONG,Release) 			(THIS) PURE;
 

	//--- IAMMixer methods---

	STDMETHOD (TestConfig)							(THIS_	LPWAVEOUT pWaveOut,
															LPMIXERCONFIG pMixerConfig,
															LPADVMIXCONFIG pAdvMixConfig,
															BOOL fRecommend) PURE;

	STDMETHOD (Init)								(THIS_	HINSTANCE hInst,                  
															LPWAVEOUT pWaveOut,
															LPMIXERCONFIG pMixerConfig,
															LPADVMIXCONFIG pAdvMixConfig) PURE;
		
	STDMETHOD (Uninit)								(THIS)	PURE;

	STDMETHOD (Activate)							(THIS_	BOOL fActive) PURE;

	STDMETHOD (Suspend)								(THIS_	BOOL fSuspend) PURE;

 	STDMETHOD (SetConfig)							(THIS_	LPMIXERCONFIG pMixerConfig,
															LPADVMIXCONFIG pAdvMixConfig) PURE;

	STDMETHOD (GetConfig)							(THIS_	LPMIXERCONFIG pMixerConfig,
															LPADVMIXCONFIG pAdvMixConfig) PURE;
	
	STDMETHOD (SetMixerVolume)						(THIS_	DWORD dwVolume) PURE;

	STDMETHOD (GetMixerVolume)						(THIS_	LPDWORD lpdwVolume) PURE;

	STDMETHOD (PlaySound)							(THIS_	LPSOUND pSound) PURE;

	STDMETHOD_(BOOL, RemixMode)						(THIS_	BOOL fActive) PURE;

	STDMETHOD_(DWORD, GetAvgSample)					(THIS)	PURE;

	STDMETHOD (AllocGroup)							(THIS_	LPDWORD lpdwGroup) PURE;

	STDMETHOD (FreeGroup)							(THIS_	DWORD dwGroup) PURE;
  	
  	STDMETHOD (EnlistGroup)							(THIS_	IUnknown FAR * pChannel,
															DWORD dwGroup) PURE;

	STDMETHOD (DefectGroup)							(THIS_	LPUNKNOWN pUnknown,
															DWORD dwGroup) PURE;

	STDMETHOD (StartGroup)							(THIS_	DWORD dwGroup,
															BOOL fStart) PURE;

	STDMETHOD (ResetGroup)							(THIS_	DWORD dwGroup) PURE;

	STDMETHOD (SetGroupVolume)						(THIS_	DWORD dwGroup, 
															DWORD dwVolume) PURE;

	STDMETHOD (SetGroupGain)						(THIS_	DWORD dwGroup, 
															float flDBLeft, 
															float flDBRight,
															BOOL fAbsolute) PURE;

	STDMETHOD (SetGroupPosition)					(THIS_	DWORD dwGroup, 
															DWORD dwPosition) PURE;

	STDMETHOD (AllocChannel)						(THIS_	LPCHANNEL FAR *ppChannel) PURE;
	
	STDMETHOD (RegisterChannel)						(THIS_	LPUNKNOWN pUnknown) PURE;

	STDMETHOD (UnregisterChannel)					(THIS_	LPUNKNOWN pUnknown) PURE;

	STDMETHOD (SetPriority)							(THIS_	LPUNKNOWN pUnknown,
															DWORD dwPriority) PURE;

	STDMETHOD (GetPriority)							(THIS_	LPUNKNOWN pUnknown,
															LPDWORD lpdwPriority) PURE;

	STDMETHOD_(void,Refresh)						(THIS)	PURE;

};


DECLARE_INTERFACE_(IAMChannel, IUnknown)
{
    //---  IUnknown methods--- 

    STDMETHOD (QueryInterface) 			(THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) 			(THIS) PURE;
    STDMETHOD_(ULONG,Release) 			(THIS) PURE;

	//---  IAMChannel methods---
    
	STDMETHOD (RegisterNotify)			(THIS_	LPNOTIFYSINK pNotifySink,
												DWORD fdwNotifyFlags) PURE;

	STDMETHOD (SetSoundSrc)				(THIS_	LPSOUND pSound) PURE;

	STDMETHOD (SetCachedSrc)			(THIS_	LPSOUND pSound,
												LPCACHECONFIG pCacheConfig) PURE;

	STDMETHOD (GetSoundSrc)				(THIS_	LPSOUND FAR *ppSound) PURE;

   	STDMETHOD (Play)					(THIS)	PURE;

	STDMETHOD (Stop)					(THIS)	PURE;

	STDMETHOD (Finish)					(THIS)	PURE;

	STDMETHOD_(BOOL,IsPlaying)			(THIS)	PURE;

	STDMETHOD_(DWORD,Samples)			(THIS)	PURE;

	STDMETHOD (SetPosition)				(THIS_	DWORD dwSample) PURE;

	STDMETHOD (GetPosition)				(THIS_	LPDWORD lpdwSample) PURE;

	STDMETHOD (Mute)					(THIS_	BOOL fMute) PURE;

	STDMETHOD (SetVolume)				(THIS_	DWORD dwVolume) PURE;

	STDMETHOD (GetVolume)				(THIS_ 	LPDWORD lpdwVolume) PURE;

	STDMETHOD (SetGain)					(THIS_	float flLeft,
  												float flRight) PURE;

	STDMETHOD (GetGain)					(THIS_	float FAR * lpflLeft,
												float FAR * lpflRight) PURE;

	STDMETHOD (GetSMPTEPos)				(THIS_	LPSMPTE lpSMPTE) PURE;

	STDMETHOD (SetSMPTEPos)				(THIS_	LPSMPTE lpSMPTE) PURE;

	STDMETHOD (GetTimePos)				(THIS_	LPDWORD lpdwTime) PURE;

	STDMETHOD (SetTimePos)				(THIS_	DWORD dwTime) PURE;

};



DECLARE_INTERFACE_(IAMWaveOut, IUnknown)
{
    //---  IUnknown methods--- 

    STDMETHOD (QueryInterface) 			(THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) 			(THIS) PURE;
    STDMETHOD_(ULONG,Release) 			(THIS) PURE;
 

    //---  IAMWaveOut methods--- 

	STDMETHOD_(UINT,GetNumDevs)						(THIS)	PURE;


	STDMETHOD_(MMRESULT, Open)						(THIS_	UINT uDeviceID,
															LPWAVEFORMATEX lpwfx,
   															DWORD dwCallback,
   															DWORD dwCallbackInstance,
   															DWORD fdwOpen) PURE;

	STDMETHOD_(MMRESULT, Close)						(THIS)	PURE;

    STDMETHOD_(MMRESULT, GetDevCaps)				(THIS_	UINT uDeviceID,
    														LPWAVEOUTCAPS lpCaps,
    														UINT cbCaps) PURE;

    STDMETHOD_(MMRESULT, GetVolume)					(THIS_	LPDWORD lpdwVolume) PURE;

    STDMETHOD_(MMRESULT, SetVolume)					(THIS_	DWORD dwVolume) PURE;

    STDMETHOD_(MMRESULT, PrepareHeader)				(THIS_	LPWAVEHDR pwh,
    														UINT cbwh) PURE;

    STDMETHOD_(MMRESULT, UnprepareHeader)			(THIS_	LPWAVEHDR pwh,
    														UINT cbwh) PURE;

    STDMETHOD_(MMRESULT, Write)						(THIS_	LPWAVEHDR pwh,
    														UINT cbwh) PURE;

    STDMETHOD_(MMRESULT, Pause)						(THIS)	PURE;

    STDMETHOD_(MMRESULT, Restart)					(THIS)	PURE;

    STDMETHOD_(MMRESULT, Reset)						(THIS)	PURE;

    STDMETHOD_(MMRESULT, BreakLoop)					(THIS)	PURE;

    STDMETHOD_(MMRESULT, GetPosition)				(THIS_ 	LPMMTIME lpmmt,
    														UINT cbmmt) PURE;

    STDMETHOD_(MMRESULT, GetPitch)					(THIS_	LPDWORD lpdwPitch) PURE;

    STDMETHOD_(MMRESULT, SetPitch)					(THIS_	DWORD dwPitch) PURE;

    STDMETHOD_(MMRESULT, GetPlaybackRate)			(THIS_	LPDWORD lpdwRate) PURE;

    STDMETHOD_(MMRESULT, SetPlaybackRate)			(THIS_	DWORD dwRate) PURE;

    STDMETHOD_(MMRESULT, GetID)						(THIS_	UINT FAR *lpuDeviceID) PURE;

    STDMETHOD_(MMRESULT, Message)					(THIS_	UINT uMsg,
    														DWORD dw1,
    														DWORD dw2) PURE;

	STDMETHOD_(MMRESULT, GetErrorText)				(THIS_	MMRESULT err,
															LPSTR lpText,
															UINT cchText) PURE;	
};


DECLARE_INTERFACE_( IAMSound, IUnknown)
{
    //---  IUnknown methods--- 
    STDMETHOD (QueryInterface) 			(THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) 			(THIS) PURE;
    STDMETHOD_(ULONG,Release) 			(THIS) PURE;

    //---  IAMSound methods--- 

	STDMETHOD (GetFormat)				(THIS_	LPWAVEFORMATEX pFormat, DWORD cbSize) PURE;

	STDMETHOD_(DWORD,GetSamples)		(THIS)	PURE;

	STDMETHOD (GetAlignment)			(THIS_	LPDWORD	lpdwLeftAlign,
												LPDWORD lpdwRightAlign)	PURE;

	STDMETHOD (GetSampleData)			(THIS_	LPBYTE lpBuffer,
												DWORD dwPosition, 
											    LPDWORD lpdwSamples,
											    LPREQUESTPARAM lpRequestParams) PURE;

	STDMETHOD (SetCacheSize)			(THIS_	DWORD dwCacheSize) PURE;

	STDMETHOD (SetMode)					(THIS_	BOOL fActive,
												BOOL fRecurse) PURE;

};


DECLARE_INTERFACE_( IAMNotifySink, IUnknown)
{
    //---  IUnknown methods--- 
    STDMETHOD (QueryInterface) 			(THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) 			(THIS) PURE;
    STDMETHOD_(ULONG,Release) 			(THIS) PURE;

    //---  IAMNotifySink methods--- 

	STDMETHOD_(void,OnStart)			(THIS_	LPSOUND	pSound,
												DWORD dwPosition)	PURE;

	STDMETHOD_(void,OnCompletion)		(THIS_	LPSOUND	pSound,
												DWORD dwPosition)	PURE;
	
	STDMETHOD_(void,OnError)			(THIS_	LPSOUND	pSound,
												DWORD dwPosition,
												HRESULT	hrError)	PURE;

	STDMETHOD_(void,OnSyncObject)		(THIS_	LPSOUND	pSound,
												DWORD	dwPosition,
												void *	pvObject)	PURE;
};


DECLARE_INTERFACE_(IAMWavFileSrc, IUnknown)
{
    //---  IAMWavFileSrc methods--- 

	STDMETHOD (InitFromStream)			(THIS_	IStream *pStream,
												BOOL fSpooled) PURE;

	STDMETHOD (InitFromMemory)			(THIS_	char *lpMemory,
												DWORD dwLength) PURE;

	STDMETHOD (InitFromFile)			(THIS_	char *pAbsFilePath,
												DWORD dwOffset,
												BOOL fSpooled) PURE;
};



DECLARE_INTERFACE_(IAMScheduleFilter, IUnknown)
{
    //---  IAMScheduleFilter methods--- 

   	STDMETHOD (Init)					(THIS_	LPSCHEDULERCONFIG pSchedulerConfig) PURE;

	STDMETHOD (Schedule)				(THIS_	LPSOUND	pSound,
												DWORD	dwBeginTime,
												DWORD	dwEndTime,
										 		DWORD	dwLoops,
										 		BOOL	fGrowSchedule) PURE;
	
	STDMETHOD (ScheduleLast)			(THIS_	LPSOUND	pSound,
												DWORD	dwLoops,
										 		BOOL	fGrowSchedule) PURE;

	STDMETHOD (ClearTime)				(THIS_	DWORD	dwBeginTime,
												DWORD	dwEndTime) PURE;


	STDMETHOD (ClearAll)				(THIS) PURE;


	STDMETHOD_(DWORD, GetNumScheduled)	(THIS) PURE;

	STDMETHOD (GetSound)				(THIS_	LPSOUND  FAR *ppSound,
												DWORD dwIndex) PURE;

	STDMETHOD (GetTimeBlock)			(THIS_	LPSOUND pSound,
												LPDWORD	lpdwBeginTime,
												LPDWORD lpdwEndTime) PURE;

	STDMETHOD_(BOOL, IsScheduled)		(THIS_	LPSOUND	pSound) PURE;

	STDMETHOD_(BOOL, IsPlaying)			(THIS_	LPSOUND	pSound) PURE;
};



DECLARE_INTERFACE_(IAMRandomizeFilter, IUnknown)
{
    //---  IAMRandomizeFilter methods--- 

	STDMETHOD (Init)					(THIS_	LPRANDOMIZECONFIG pRandomizeConfig) PURE;

	STDMETHOD (Add)						(THIS_	LPSOUND	pSound,
												DWORD	dwFrequency) PURE;

	STDMETHOD (Remove)					(THIS_	LPSOUND	pSound) PURE;

	STDMETHOD (Clear)					(THIS)	PURE;
};


DECLARE_INTERFACE_( IAMSilentSound, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPWAVEFORMATEX lpwfx, DWORD dwSamples) PURE;
};

DECLARE_INTERFACE_(IAMCacheFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSound, LPCACHECONFIG lpCacheConfig) PURE;
};

DECLARE_INTERFACE_(IAMTrimFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSound) PURE;
};

DECLARE_INTERFACE_(IAMStereoFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSound) PURE;
};

DECLARE_INTERFACE_(IAMBiasFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSound) PURE;
};

DECLARE_INTERFACE_(IAMClipFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrc,DWORD dwStartPos, DWORD dwEndPos) PURE; 
};

DECLARE_INTERFACE_(IAMLoopFilter, IUnknown)
{
   	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrc, DWORD dwLoops) PURE;
};

DECLARE_INTERFACE_(IAMConvertFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrc, LPWAVEFORMATEX lpwfxDest) PURE; 
};


DECLARE_INTERFACE_(IAMGainFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrc, float flLeft, float flRight) PURE;  
	STDMETHOD (SetGain)					(THIS_	float flLeft, float flRight) PURE;
};


DECLARE_INTERFACE_(IAMDelayFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrc, DWORD dwDelay, DWORD dwReflectLeft, DWORD dwReflectRight,DWORD dwDuration,DWORD dwFlags) PURE; 
};


DECLARE_INTERFACE_(IAMGateFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrc, float flDBGate) PURE; 
};


DECLARE_INTERFACE_(IAMMixFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrcA, LPSOUND pSoundSrcB) PURE; 
};


DECLARE_INTERFACE_(IAMAppendFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrcA, LPSOUND pSoundSrcB, BOOL fAlign) PURE;
};


DECLARE_INTERFACE_(IAMFaderFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrc, DWORD dwFadeTime, DWORD dwPosition) PURE; 
};

DECLARE_INTERFACE_(IAMDistortFilter, IUnknown)
{
	STDMETHOD (Init)					(THIS_	LPSOUND pSoundSrc, float flDBThreshold) PURE; 
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT	AM_CreateInstance(REFCLSID rclsid, IUnknown *pUnkOuter, REFIID riid, void ** ppvObject);


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	AudioMan Helper Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


STDAPI AllocSoundFromStream (LPSOUND FAR *ppSound, LPSTREAM pStream, BOOL fSpooled, LPCACHECONFIG lpCacheConfig);
STDAPI AllocSoundFromFile (LPSOUND FAR *ppSound, char  FAR *szFileName,  DWORD dwOffset, BOOL fSpooled, LPCACHECONFIG lpCacheConfig);
STDAPI AllocSoundFromMemory (LPSOUND FAR *ppSound, LPBYTE lpFileData, DWORD dwSize);
STDAPI AllocSilentSound (LPSOUND FAR *ppSound, LPWAVEFORMATEX lpwfx, DWORD dwSamples);
STDAPI_(LPMIXER) GetAudioManMixer		(void);
STDAPI AllocClipFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrc,DWORD dwStartPos, DWORD dwEndPos);	
STDAPI AllocCacheFilter	(LPSOUND FAR *ppSound,LPSOUND pSoundSrc,LPCACHECONFIG lpCacheConfig);
STDAPI AllocTrimFilter	(LPSOUND FAR *ppSound,LPSOUND pSoundSrc);
STDAPI AllocStereoFilter (LPSOUND FAR *ppSound,LPSOUND pSoundSrc);
STDAPI AllocBiasFilter	(LPSOUND FAR *ppSound,LPSOUND pSoundSrc);
STDAPI AllocLoopFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrc, DWORD dwLoops);
STDAPI AllocConvertFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrc, LPWAVEFORMATEX lpwfx);
STDAPI AllocGainFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrc, float flLeft, float flRight);
STDAPI AllocDelayFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrc, DWORD dwDelay, DWORD dwReflectLeft, DWORD dwReflectRight,DWORD dwDuration,DWORD dwFlags);
STDAPI AllocGateFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrc, float flDBGate);
STDAPI AllocDistortFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrc, float flDBThreshold);
STDAPI AllocMixFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrcA, LPSOUND pSoundSrcB);
STDAPI AllocAppendFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrcA, LPSOUND pSoundSrcB, BOOL fAlign);
STDAPI AllocFaderFilter (LPSOUND FAR *ppSound, LPSOUND pSoundSrc, DWORD dwFadeTime, DWORD dwPosition);
STDAPI AllocScheduleFilter (LPSOUND FAR *ppSound, LPSCHEDULERCONFIG pSchedulerConfig);
STDAPI AllocRandomizeFilter (LPSOUND FAR *ppSound, LPRANDOMIZECONFIG pRandomizeConfig);
STDAPI AllocWaveOut (LPWAVEOUT FAR *ppWaveOut, LPMIXER pMixer);

STDAPI_(DWORD) TimeToSamples (LPSOUND pSound, DWORD dwTime);
STDAPI_(DWORD) SamplesToTime (LPSOUND pSound, DWORD dwSamples);
STDAPI_(DWORD) SizeToSamples (LPSOUND pSound, DWORD dwSize);
STDAPI_(DWORD) SamplesToSize (LPSOUND pSound, DWORD dwSamples);
STDAPI SoundToStreamAsWave(LPSOUND pSound, LPSTREAM pStream, LPDWORD pcbFileSize);
STDAPI SoundToFileAsWave(LPSOUND pSound, char FAR *pAbsFilePath);

STDAPI_(int) DetectLeaks(BOOL fDebugOut, BOOL fMessageBox);

#endif  //_AUDIOMANPUBLICINTEFACES_
