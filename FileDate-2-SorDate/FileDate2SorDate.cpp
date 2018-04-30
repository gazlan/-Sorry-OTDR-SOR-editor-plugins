#include "stdafx.h"

#include "..\shared\vector.h"
#include "..\shared\vector_sorted.h"

#include "sorry_sdk.h"

#include "resource.h"

#include "DateUpdater.h"
#include "FileDate2SorDate.h"

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

static char*   pszOWNER_GUID_STR = "{C3EAD0DE-AE45-423b-B3A5-36118336BAA0}";

// The one and only CFileDate2SorDateApp object
CFileDate2SorDateApp    theApp;

/* ******************************************************************** **
** @@ CFileDate2SorDateApp::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CFileDate2SorDateApp, CWinApp)
   //{{AFX_MSG_MAP(CFileDate2SorDateApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CFileDate2SorDateApp::CFileDate2SorDateApp()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

CFileDate2SorDateApp::CFileDate2SorDateApp()
{
   _hMutext = NULL;
}

/* ******************************************************************** **
** @@ CFileDate2SorDateApp::DoIt()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void CFileDate2SorDateApp::DoIt()
{
   // Avoid the Fucking M$ BUG
   // See Q194300 for details
   // FIX: Asserts When Creating a Dialog Box in an MFC Regular DLL
   AFX_MANAGE_STATE(AfxGetStaticModuleState())
   
   CDateUpdater      Dlg(AfxGetMainWnd());

   Dlg.DoModal();
}

/* ******************************************************************** **
** @@ CFileDate2SorDateApp::InitInstance() 
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

BOOL CFileDate2SorDateApp::InitInstance() 
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
** @@ CFileDate2SorDateApp::ExitInstance()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int CFileDate2SorDateApp::ExitInstance()
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
