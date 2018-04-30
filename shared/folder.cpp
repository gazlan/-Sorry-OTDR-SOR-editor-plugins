/* ******************************************************************** **
** @@ BrowseForFolder
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Dscr   : 
** ******************************************************************** */

/* ******************************************************************** **
** uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include "folder.h"

/* ******************************************************************** **
** @@ internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@ internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@ external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@ static global variables
** ******************************************************************** */
                  
/* ******************************************************************** **
** @@ real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ BrowseCallbackProc()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

#pragma warning (disable: 4100)     // 'pData' : unreferenced formal parameter
int CALLBACK BrowseCallbackProc
(
   HWND           hwnd,
   UINT           uMsg,
   LPARAM         lp,
   LPARAM         pData
)
{
   TCHAR       pszDir[MAX_PATH + 1];

   switch (uMsg)
   {
      case BFFM_INITIALIZED:
      {
         if (GetCurrentDirectory(sizeof(pszDir) / sizeof(TCHAR),pszDir))
         {
            // WParam is TRUE since you are passing a path.
            // It would be FALSE if you were passing a pidl.
            SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)pszDir);
         }

         break;
      }
      case BFFM_SELCHANGED:
      {
         // Set the status window to the currently selected path.
         if (SHGetPathFromIDList((LPITEMIDLIST)lp,pszDir))
         {
            SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)pszDir);
         }

         break;
      }
      default:
      {
         break;
      }
   }

   return 0;
}

/* ******************************************************************** **
** @@ BrowseForFolder()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

void BrowseForFolder
(
   HWND                    hWnd,
   const char* const       pszTitle,
   UINT                    dwFlags,
   char*                   pszDir
)
{
   if (!pszDir)
   {
      // Error !
      return;
   }
   
   *pszDir = 0;

   LPMALLOC       pMalloc = NULL;

   if (!(SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc))
   {
      // Error !
      return;
   }

   BROWSEINFO     BI;

   memset(&BI,0,sizeof(BI));

   BI.hwndOwner      = hWnd;
   BI.pszDisplayName = pszDir;
   BI.lpszTitle      = pszTitle;
   BI.pidlRoot       = 0;
   BI.ulFlags        = dwFlags; // BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
   BI.lpfn           = BrowseCallbackProc;

   LPITEMIDLIST   pIDL = SHBrowseForFolder(&BI);

   if (pIDL)
   {
      if (!SHGetPathFromIDList((LPITEMIDLIST)pIDL,pszDir))
      {
         *pszDir = 0;
      }

      pMalloc->Free(pIDL);
      pMalloc->Release();
   }
}

/* ******************************************************************** **
** End of File
** ******************************************************************** */
