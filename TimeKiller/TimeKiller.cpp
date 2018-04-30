/* ******************************************************************** **
** @@ TimeKiller
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

#include <afxdtctl.h>
#include <time.h>

#include "..\shared\vector.h"
#include "..\shared\vector_sorted.h"

#include "sorry_sdk.h"

#include "resource.h"

#include "TimeUpdater.h"
#include "TimeKiller.h"

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

static char*   pszOWNER_GUID_STR = "{6AA6877A-AEF4-4fe5-A611-4E79A984607A}";

/* ******************************************************************** **
** @@ internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@ external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@ static global variables
** ******************************************************************** */
                  
// The one and only CTimeKillerApp object
CTimeKillerApp    theApp;

/* ******************************************************************** **
** @@ real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ CTimeKillerApp::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CTimeKillerApp, CWinApp)
   //{{AFX_MSG_MAP(CTimeKillerApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CTimeKillerApp::CTimeKillerApp()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

CTimeKillerApp::CTimeKillerApp()
{
   _hMutext = NULL;
}

/* ******************************************************************** **
** @@ CTimeKillerApp::DoIt()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CTimeKillerApp::DoIt()
{
   // Avoid the Fucking M$ BUG
   // See Q194300 for details
   // FIX: Asserts When Creating a Dialog Box in an MFC Regular DLL
   AFX_MANAGE_STATE(AfxGetStaticModuleState())
   
   CTimeUpdater      Dlg(AfxGetMainWnd());

   Dlg.DoModal();
}

/* ******************************************************************** **
** @@ CTimeKillerApp::InitInstance() 
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

BOOL CTimeKillerApp::InitInstance() 
{
   // Only One Instance of this App is available !
   _hMutext = CreateMutex(NULL,TRUE,"HashCalc");

   if (!_hMutext)
   {
      // Error !
      return FALSE;
   }

   if (GetLastError() == ERROR_ALREADY_EXISTS)
   {
      // It's OK
      return FALSE;
   }

   return CWinApp::InitInstance();
}

/* ******************************************************************** **
** @@ CTimeKillerApp::ExitInstance()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int CTimeKillerApp::ExitInstance()
{
   CloseHandle(_hMutext);
   _hMutext = NULL;

   return CWinApp::ExitInstance();
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
   theApp.DoIt();
   
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
