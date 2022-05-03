#include "version.h"


#ifdef RC_INVOKED

#include "verdef.h"
#define VER_FILEVERSION_STR      "1.2\0"

#define VER_PRODUCTVERSION_STR   "1.2\0"
#define VER_FILEVERSION          rmj,rmm,0,rup
#define VER_PRODUCTVERSION      rmj,rmm,0,rup

#define VER_PRODUCTNAME_STR      "Microsoft App-wide Setup for Windows\0"
#define VER_LEGALCOPYRIGHT_STR   "Copyright \251 Microsoft Corp. 1991-1995\0"
#define VER_COMMENT_STR          "Windows App-wide Setup (Acme)\0"
#define VER_FILETYPE             VFT_APP
#define VER_FILESUBTYPE          0

#endif  /* RC_INVOKED */
