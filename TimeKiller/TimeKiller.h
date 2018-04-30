#ifndef _FILE_DATE_2_SOR_DATE_HPP_
#define _FILE_DATE_2_SOR_DATE_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"      // main symbols

class CTimeKillerApp : public CWinApp
{
   private:

      HANDLE      _hMutext;

   public:
   
      CTimeKillerApp();

      void  DoIt();

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTimeKillerApp)
   public:

      virtual BOOL InitInstance();
      virtual int  ExitInstance();
   //}}AFX_VIRTUAL

   //{{AFX_MSG(CTimeKillerApp)
      // NOTE - the ClassWizard will add and remove member functions here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

#endif
