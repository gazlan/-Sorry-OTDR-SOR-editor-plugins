/* ******************************************************************** **
** @@ NULL
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

/* ******************************************************************** **
** uses precompiled headers
** ******************************************************************** */

#include "stdafx.h"

#include "sorry_sdk.h"

#include "null.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// Note!
//////////////////////////////////////////////////////////////////////////
//
//    If this DLL is dynamically linked against the MFC
//    DLLs, any functions exported from this DLL which
//    call into MFC must have the AFX_MANAGE_STATE macro
//    added at the very beginning of the function.
//
//    For example:
//
//    extern "C" BOOL PASCAL EXPORT ExportedFunction()
//    {
//       AFX_MANAGE_STATE(AfxGetStaticModuleState());
//       // normal function body here
//    }
//
//    It is very important that this macro appear in each
//    function, prior to any calls into MFC.  This means that
//    it must appear as the first statement within the 
//    function, even before any object variable declarations
//    as their constructors may generate calls into the MFC
//    DLL.
//
//    Please see MFC Technical Notes 33 and 58 for additional
//    details.
//////////////////////////////////////////////////////////////////////////

/* ******************************************************************** **
** @@ internal defines
** ******************************************************************** */

//////////////////////////////////////////////////////////////////////////
// Attention: Each plugin should have very own GUID string !
//////////////////////////////////////////////////////////////////////////

static char*   pszOWNER_GUID_STR = "{D93B82C9-6BFC-4956-AE1E-DAD461D41C54}";

/* ******************************************************************** **
** @@ internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@ external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@ static global variables
** ******************************************************************** */
                  
CNullApp       theApp;  // The one and only CNullApp object

/* ******************************************************************** **
** @@ real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ CNullApp::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CNullApp,CWinApp)
   //{{AFX_MSG_MAP(CNullApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CNullApp::CNullApp()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

CNullApp::CNullApp()
{
   // TODO: add construction code here,
   // Place all significant initialization in InitInstance
}

/* ******************************************************************** **
** @@ SRPL_Initialize()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

extern "C" __declspec(dllexport) BOOL SRPL_Initialize()
{
   DWORD    dwInfoSize = 0;

   void*    pInfo = DB_LoadSession(pszOWNER_GUID_STR,0,&dwInfoSize,NULL);

   return TRUE;
}

/* ******************************************************************** **
** @@ SRPL_Terminate()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

extern "C" __declspec(dllexport) void SRPL_Terminate()
{
   DB_SaveSession(pszOWNER_GUID_STR,0,NULL,0,NULL);
}

/* ******************************************************************** **
** @@ SRPL_Run()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

extern "C" __declspec(dllexport) BOOL SRPL_Run()
{
   MessageBox(::GetActiveWindow(),"This is a NULL Sorry! plugin.\n\nIt do nothing - just Demo.","Hi!",MB_OK | MB_ICONINFORMATION);
   return TRUE;
}

/* ******************************************************************** **
** @@ SRPL_Info()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

extern "C" __declspec(dllexport) void* SRPL_Info()
{
   const char*    pszGreetings = "Enjoy with Sorry! plugin's system!";
   
   static SRPL_INFO     Info;

   Info._wVersionMajor    = 1;
   Info._wVersionMinor    = 0;
   Info._bStandalone      = TRUE;
   Info._pszDescription   = (char*)pszGreetings;
   Info._pszBlockNameList = NULL;

   return &Info;
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
