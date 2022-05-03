/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/******************************************************************************

@doc    MMINSTAL

@mod    MMINSTAL.CPP |

        <cp> Copyright 1995 Microsoft Corporation - All rights reserved

*******************************************************************************

        Description: Ensures appropriate drivers and codecs are installed for
                      audio and video playback.

        Current Owner: *****

*******************************************************************************

        Known bugs: None

*******************************************************************************

        How it could be Improved:
        
*******************************************************************************

        Revision History:

        07/31/95    *****    Created
        
*******************************************************************************

@comm   Under Windows95 we can automatically install the appropriate sections.
        Under NT 3.51 we can not.

        Link this module with MSACM32.LIB, VFW32.LIB and WINMM.LIB (from the
        Win95 SDK).

******************************************************************************/

#include "studio.h"
#include <mmsystem.h>   // winmm.lib
#include <mmreg.h>      // required by msacm.h
#include <msacm.h>      // msacm32.lib
#include <vfw.h>        // vfw32.lib

#include "mminstal.h"
#include "utestres.h"	

/******************************************************************************

@func   WORD | wHaveWaveDevice |

        Do we have a wave device capable of playing the passed PCM format(s).

@parm  DWORD | dwFormats | WAVE formats needed to be supported.  These can be
                            a bitwise combination of WAVE_FORMAT_???? flags
                            which are defined in mmsystem.h.  If you don't
                            care what formats are supported you can pass zero.

@rdesc  HWD_SUCCESS   we have a functioning wave device that supports the format
        HWD_NODEVICE  there are no wave devices
        HWD_NODRIVER  the wave device doesn't have a driver installed
        HWD_ERROR     some undefined error occured
        HWD_NOFORMAT  we have a device but the requested format(s) is not supported

******************************************************************************/

WORD wHaveWaveDevice(DWORD dwReqFormats)
{
  WORD        wNumWavDev;
  WAVEOUTCAPS WOC;
  WORD        wDevID;
  WORD        wErr;

          // Determine how many WAVE devices are in the user's system
  wNumWavDev = waveOutGetNumDevs();

          // If there are none, return indicating that
  if(0 == wNumWavDev)
  {
    #ifdef _DEBUG
    MessageBox(NULL, "No WAVE devices on your machine.", NULL, MB_ICONINFORMATION);
    #endif
    return(HWD_NODEVICE);
  }

          // Cycle through the WAVE devices to determine if any support
          // the desired format.
  for(wDevID = 0 ; wDevID < wNumWavDev ; wDevID++)
  {
    wErr = waveOutGetDevCaps(wDevID, &WOC, sizeof(WAVEOUTCAPS));

          // If we obtain a WAVE device's capabilities OK
    if( (0 == wErr) &&
          // and it supports the desired format
        ( (WOC.dwFormats & dwReqFormats) == dwReqFormats) )
          // then return success - we have a device that supports what we want
      return(HWD_SUCCESS);

  } // for(wDevID

          // If we didn't find a device that supported what we wanted, why didn't we...
  switch(wErr)
  {
          // We got the capabilities OK but the formats didn't match
    case 0:
      #ifdef _DEBUG
      MessageBox(NULL, "Found 1 or more WAVE devices, but none support the desired PCM format.", NULL, MB_ICONSTOP);
      #endif
      return(HWD_NOFORMAT);

          // There wasn't an installed driver for the WAVE device
    case MMSYSERR_NODRIVER:
      #ifdef _DEBUG
      MessageBox(NULL, "Found 1 or more WAVE devices, but there was no driver installed for it.", NULL, MB_ICONSTOP);
      #endif
      return(HWD_NODRIVER);
  }

          // Some other error occured.
  #ifdef _DEBUG
  MessageBox(NULL, "Found 1 or more WAVE devices, but an error occured getting the device capabilities.", NULL, MB_ICONSTOP);
  #endif
  return(HWD_ERROR);
}



/******************************************************************************

@func   WORD | wHaveACM |

        Do we have the audio compression manager installed?

        Sound Mapper is another term for the "audio compression manager".

        Under NT the drivers control pannel applet lists it as "Microsoft
        Audio Compression Manager".

        Under Win95 there does not appear to be a way to remove just the manager.

@rdesc  0 ACM is installed
        1 ACM is not installed or is an old version

******************************************************************************/

WORD wHaveACM()
{
/* The following taken from MSDN ACMAPP sample exe.
   Select options - wave device - output menu item.
   Select the Microsoft Sound Mapper device.

Microsoft Audio Compression Manager driver installed:

[Wave Output Device Capabilities]

        Device Id: -1
  Manufacturer Id: 1
       Product Id: 2
   Driver Version: 3.51
      Device Name: 'Microsoft Sound Mapper'
         Channels: 2
 Standard Formats: 00000FFFh
                   8M11, 8S11, 16M11, 16S11
                   8M22, 8S22, 16M22, 16S22
                   8M44, 8S44, 16M44, 16S44
 Standard Support: 0000000Ch
                   Volume, L/R Volume

Microsoft Audio Compression Manager driver NOT installed:

[Wave Output Device Capabilities]

        Device Id: -1
  Manufacturer Id: 0
       Product Id: 0
   Driver Version: 0.00
      Device Name: 'Default Wave Output Mapper'
         Channels: 0
 Standard Formats: 00000000h

 Standard Support: 00000000h

*/

  WAVEOUTCAPS WOC;
  WORD        wErr;

  wErr = waveOutGetDevCaps(WAVE_MAPPER, &WOC, sizeof(WAVEOUTCAPS));

  if( (MMSYSERR_NOERROR != wErr) ||     // If the driver wasn't installed or

      (WOC.vDriverVersion < 0x0332) )   // ACM ver is less than 3.50 (for Win32) HIBYTE=Major LOBYTE=Minor ver num
  {  
    #ifdef _DEBUG
    MessageBox(NULL, "ACM not installed or it is an old version.", NULL, MB_ICONSTOP);
    #endif
    return(1);
  }

  #ifdef _DEBUG
  MessageBox(NULL, "Microsoft Audio Compression Manager (Sound Mapper) is properly installed on your system",
    "ACM Installed", MB_ICONINFORMATION);
  #endif

  return(0);
}



/******************************************************************************

@func   WORD | wHaveACMCodec |

        Do we have the audio compression manager codecs we need installed?

@parm   DWORD | dwReqCodec | Audio codec we want to check for.  This value is
                              one of the WAVE_FORMAT_* defines in mmreg.h.

@rdesc  HAC_SUCCESS   ACM is installed, we've got a codec for the desired format
        HAC_NOACM     Audio compression manager is not installed or is an old version
        HAC_NOCODEC   The requested codec is not active or installed
        HAC_NOCONVERT Codec is installed but won't convert wav data.

******************************************************************************/

WORD wHaveACMCodec(DWORD dwReqCodec)
{
  MMRESULT  mmRet;
  ACMFORMATTAGDETAILS acmFTD;

  acmFTD.cbStruct = sizeof(ACMFORMATTAGDETAILS);
  acmFTD.dwFormatTagIndex = 0;
  acmFTD.dwFormatTag = dwReqCodec;
  acmFTD.cbFormatSize = 0;  // These must be initialized or the retail build breaks under NT
  acmFTD.fdwSupport = 0;

  mmRet = acmFormatTagDetails(NULL, &acmFTD, ACM_FORMATTAGDETAILSF_FORMATTAG);

  if(MMSYSERR_NOERROR != mmRet)
  {
    #ifdef _DEBUG
    MessageBox(NULL, "Requested audio codec is not installed or is not active.", NULL, MB_ICONSTOP);
    #endif
    return(HAC_NOCODEC);
  }

  if(acmFTD.fdwSupport & ACMDRIVERDETAILS_SUPPORTF_CODEC)
  {
    #ifdef _DEBUG
    MessageBox(NULL, "... is properly installed on your machine.", acmFTD.szFormatTag, MB_ICONINFORMATION);
    #endif
    return(HAC_SUCCESS);
  }

  #ifdef _DEBUG
  MessageBox(NULL, "codec is installed but won't convert to PCM.", acmFTD.szFormatTag, MB_ICONSTOP);
  #endif
  return(HAC_NOCONVERT);
}



/******************************************************************************

@func   WORD | wHaveICMCodec |

        Do we have the installable (video) compression manager codecs we need
        installed?

@parm   DWORD | dwReqCodec | Video codec we want to check for.  This value is
                              one of the WAVE_FORMAT_* defines in mmreg.h.

@rdesc  HIC_SUCCESS   ICM is installed, we've got a codec for the desired format
        HIC_NOICM     Installable compression manager is not installed or is an old version
        HIC_NOCODEC   The requested codec is not active or installed
        HIC_NOCONVERT Codec is installed but won't convert wav data.

******************************************************************************/

WORD wHaveICMCodec(DWORD dwReqCodec)
{
  ICINFO icInfo;

  if(FALSE == ICInfo(ICTYPE_VIDEO, dwReqCodec, &icInfo))
  {
    #ifdef _DEBUG
    MessageBox(NULL, "No ICM or no codec installed.", NULL, MB_ICONSTOP);
    #endif
    return(HIC_NOCODEC);
  }

  #ifdef _DEBUG
  char szDesc[256];
  WideCharToMultiByte(CP_ACP, 0, icInfo.szDriver, -1, szDesc, 256, NULL, NULL);
  MessageBox(NULL, "... is properly installed on your machine.", szDesc, MB_ICONINFORMATION);
  #endif

  return(HIC_SUCCESS);
}




/******************************************************************************

@func   WORD | wHaveMCI |

        Do we have the media control interface (MCI) for the desired device type?

@parm   DWORD | dwDeviceType | MCI device type that we want supported.  It is
                                one of the MCI_DEVTYPE_* constants defined in
                                mmsystem.h.

@rdesc  MMSYSERR_NOERROR  MCI for device type is installed
        MCIERR_CANNOT_LOAD_DRIVER

******************************************************************************/

WORD wHaveMCI(LPSTR dwDeviceType)
{
  MCI_OPEN_PARMS  mciOpen;
  MCIERROR        mciErr;

  mciOpen.dwCallback = NULL;
  mciOpen.lpstrDeviceType = (LPSTR)dwDeviceType;
  mciOpen.lpstrElementName = NULL;
  mciOpen.lpstrAlias = NULL;

  //mciErr =  mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD)(LPMCI_OPEN_PARMS)&mciOpen);
  mciErr =  mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)(LPMCI_OPEN_PARMS)&mciOpen);

  if(MMSYSERR_NOERROR == mciErr)
  {
    mciSendCommand(mciOpen.wDeviceID, MCI_CLOSE, 0, (DWORD)(LPMCI_OPEN_PARMS)&mciOpen);
  }

  #ifdef _DEBUG
  else
  {
    char szErrBuf[128]="Memory Error";
    mciGetErrorString(mciErr, szErrBuf, 128);
    MessageBox(NULL, szErrBuf, "MCI not installed", MB_ICONSTOP);
  }
  #endif

  return((WORD)mciErr);
}
  


/******************************************************************************

@func   BOOL | FRunningChicagoUI |

        Are we running on Win95?

@rdesc  TRUE if running Win95

******************************************************************************/

BOOL fRunningWin95(void)
{
	DWORD dwVersion = GetVersion();

  if( (LOBYTE(LOWORD(dwVersion)) >= 4) &&   // If windows version is >= 4.0 we're on Win95 or WinNT w/ new shell
      (dwVersion & 0x80000000L) )           // If high bit is set we're on Win95 or Win32s (not WinNT)
    return(TRUE);

  else
    return(FALSE);
}



/******************************************************************************

@func   WORD | wInstallComp |

        Install the section under Win95 or ask user to do it under WinNT

@parm   WORD | wComp | Component ID to install

@rdesc  0 - success
        1 - couldn't launch setup app (under Win95 only)

******************************************************************************/

#define HOW_NEVER_REBOOT  		"0"	// Whatever happens the machine will not be rebooted.
#define MMINF                 "MOTOWN.INF"

  // The following 2 arrays must parallel each other between Win95 setup sections & NT driver names

WORD wInstallComp(WORD wComp)
{
  	TCHAR szText[512];
  	TCHAR szDriverName[128];
	bool fWin95 = fRunningWin95();

	if (!LoadString(vwig.hinst, (fWin95) ? stidInstallDriverWin95 : stidInstallDriverNT, 
					szText, sizeof(szText)))
		return (1); //fail
	if (!LoadString(vwig.hinst, stidDriverName + wComp, szDriverName, sizeof(szDriverName)))
		return (1); //fail
    MessageBox(NULL, szText, szDriverName, MB_ICONSTOP);

    if (fWin95)  // Running Win95 NOT NT 3.51 with or without new shell
		{
		STARTUPINFO		    si;
		PROCESS_INFORMATION	pi;
		TCHAR              	szCommandLine[512];
		TCHAR				szSectionName[255];

		if (!LoadString(vwig.hinst, stidInstallCmdLine, szCommandLine, sizeof(szCommandLine)))
			return (1);	// fail

		if (!LoadString(vwig.hinst, stidSectionName + wComp, szSectionName, sizeof(szSectionName)))
			return (1);	// fail

		// Command line should look like this:
		//   RunDll.exe setupx.dll,InstallHinfSection <section> <reboot mode> <inf name>

		  // Add <section>
		lstrcat(szCommandLine, szSectionName);

			// Add <reboot mode> <inf name>
		lstrcat(szCommandLine, " " HOW_NEVER_REBOOT " " MMINF);


		#ifdef _DEBUG
		MessageBox(NULL, szCommandLine, "Win95 setup command", MB_ICONINFORMATION);
		#endif

			// Now execute the command line to cause the actual install
		si.cb			= sizeof(STARTUPINFO);
		si.lpReserved		= NULL;
		si.lpDesktop		= NULL;
		si.lpTitle		= NULL;
		si.dwFlags		= 0;
		si.cbReserved2		= 0;
		si.lpReserved2		= NULL;

		if(CreateProcess(
    			NULL,     		      //address of module name
    			szCommandLine,		  //address of command line
    			NULL,     		      //process security
    			NULL,     		      //thread security
    			FALSE,    		      //inherit handles?
    			DETACHED_PROCESS,	  //creation flags
    			NULL,			          //address of new environment
    			NULL,			          //address of current directory
    			&si,			          //STARTUPINFO
    			&pi))			          //PROCESS_INFORMATION
			{
    			// Wait until process is finished
			WaitForSingleObject(pi.hProcess, INFINITE);  

    			// Close the process down
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			return(0);
			}

		#ifdef _DEBUG
		MessageBox(NULL, szCommandLine, "Error launching command line", MB_ICONINFORMATION);
		#endif

		return(1);
		}
	return (1);
}



/******************************************************************************

@func   int WINAPI | WinMain |

        Entry point for a MS Windows app

@rdesc  exit code

******************************************************************************/

/* Uncomment this WinMain function to make this module into a stand alone
   Win32 app that will verify audio support with the MS-ADPCM codec and
   video support with MS Video1 and Intel Indeo 3.2 codecs.

int WINAPI
  WinMain(
    HINSTANCE   hInstance,	    // handle of current instance
    HINSTANCE   hPrevInstance,	// handle of previous instance
    LPSTR       lpszCmdLine,	  // address of command line
    int         nCmdShow 	      // show state of window
  )
{
  if(HWD_SUCCESS == wHaveWaveDevice(WAVE_FORMAT_2M08)) // 22kHz, Mono, 8bit
  {
    if(wHaveMCI("WAVEAUDIO"))
        // MCI for audio is not installed
      wInstallComp(IC_MCI_SOUND);

    if(wHaveACM())
        // audio compression manager (sound mapper) not installed
      wInstallComp(IC_ACM);

    if(wHaveACMCodec(WAVE_FORMAT_ADPCM))
        // audio codecs not installed
      wInstallComp(IC_ACM_ADPCM);

  } // have wave device


  if(wHaveMCI("AVIVIDEO"))
      // MCI for video is not installed
    wInstallComp(IC_MCI_VFW);

  if(HIC_SUCCESS != wHaveICMCodec(MS_VIDEO1))
      //  video 1 codec not installed
    wInstallComp(IC_ICM_VIDEO1);

  if(HIC_SUCCESS != wHaveICMCodec(INTEL_INDEO32))
      // indeo 3.2 codec not installed
    wInstallComp(IC_ICM_INDEO32);

  return(0);
}
*/
