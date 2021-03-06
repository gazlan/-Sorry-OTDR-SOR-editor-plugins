/* ******************************************************************** **
** @@ BrowseForFolder
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

#ifndef _FILE_FOLDER_H_
#define _FILE_FOLDER_H_

#if _MSC_VER > 1000
#pragma once
#endif

/* ******************************************************************** **
** @@ internal defines
** ******************************************************************** */

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
** @@ Prototypes
** ******************************************************************** */

int   CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp,LPARAM pData);
void  BrowseForFolder(HWND hWnd,const char* const pszTitle,UINT dwFlags,char* pszDir);
      
#endif

/* ******************************************************************** **
** End of File
** ******************************************************************** */
