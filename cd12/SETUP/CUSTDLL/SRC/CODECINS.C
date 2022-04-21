/***************************************************************************
**
**	File:		CODECINS.C
**	Purpose:	Verifys that audio and video support are properly installed
**            under Windows 95 or NT 3.5x.  Under Win95 we ask Windows to
**            install the appropriate sections automatically for the user.
**            Under NT the user is told what and how to install the missing
**            components.
**
**            We verify audio support with the MS-ADPCM codec and
**            video support with MS Video1 and Intel Indeo 3.2 codecs.
**
**	Dependencies:	CODECHEC.CPP is used to do the actual verification and
**            instalation if necessary.  If components need to be installed
**            CODECHEC.CPP uses RUNDLL.EXE to call SETUPX.DLL on Windows95.
**			      These are part of the Windows95 setup that is installed
**			      on a users machine.  Please see geninst.txt for some
**			      more information on these files.
**
**	STF Syntax:
**		ObjID	Batch	Title		Descr		Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#		"Error Title"	"Error Text"		CustomAction	"EXPSETUP.DLL","InstAudVid",PARAMS
**
**	  where PARAMS blank
**
**	Author: ******
**	Written For:	Explorapedia Win32/Win16 versions of Nature and People
**
****************************************************************************/


#define FILEMOVE_C

#include <windows.h>
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"

#include "mminstal.h"   // audio and video check code


//
// Function prototypes
//

RC SETUPAPI InstAudVid( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))




/*************************************************************************
**  Name:
**      InstAudVid
**
**	Purpose:	Verifys that audio and video support are properly installed
**            under Windows 95 or NT 3.5x.  Under Win95 we ask Windows to
**            install the appropriate sections automatically for the user.
**            Under NT the user is told what and how to install the missing
**            components.
**
**            We verify audio support with the MS-ADPCM codec and
**            video support with MS Video1 and Intel Indeo 3.2 codecs.
**
**  Arguments:
**      pcd   : Ptr to class data.
**      pod   : Ptr to object data.
**      camf  : Custom action member function type.
**      pcamfd: Ptr to custom action member function data
**          (variable size, depends on camf type).
**	szData: Ptr to 3rd param of object data from STF
**
**  Returns:
**      Appropriate RC (return code) value.
**
****************************************************************************/
RC FAR PASCAL InstAudVid ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
  RC rc = rcDoDefault;

  switch (camf)
  {
  	case camfDoVisualMods:
      if(oisToBeInstalled == pod->ois)  // If we're in the tree (oisNotYetInstalled if we're not in tree)
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

        break;
  	  } // if we're in the tree
  } // switch(camf)

return (rc);
}
