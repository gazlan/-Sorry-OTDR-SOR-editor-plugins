#ifndef _DATE_UPDATER_HPP_
#define _DATE_UPDATER_HPP_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_FILE_MATCH      (WM_USER + 10)

/* ******************************************************************** **
** @@ struct MATCHED_FILE
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

struct MATCHED_FILE
{
   FILETIME       _FT_CreationTime;  
   FILETIME       _FT_LastAccessTime;
   FILETIME       _FT_LastWriteTime;  
   SYSTEMTIME     _UTC_Time;
   SYSTEMTIME     _LocalTime;
   char           _pszFile[MAX_PATH + 1];
};

/* ******************************************************************** **
** @@ class CDateUpdater : public CDialog
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

class CDateUpdater : public CDialog
{
   private:

      SortedVector      _FileList;
      FILE*             _pReport;
      char              _pszFolder[MAX_PATH + 1];
      char              _pszReport[MAX_PATH + 1];
      bool              _bUseBefore;
      bool              _bUseAfter;

   // Construction
   public:

      CDateUpdater(CWnd* pParent = NULL);   // standard constructor

      void  AppendList();

   // Dialog Data
   //{{AFX_DATA(CDateUpdater)
   enum { IDD = IDD_DATE_UPDATER };
   CSpinButtonCtrl   m_SPN_Local;
   BOOL  m_CHK_Day;
   BOOL  m_CHK_Month;
   BOOL  m_CHK_Recursively;
   BOOL  m_CHK_Year;
   CString  m_Day;
   CString  m_Month;
   CString  m_Year;
   int      m_CMB_Year;
   int      m_CMB_Month;
   int      m_CMB_Day;
   CString  m_Path;
   CString  m_Status;
   CString  m_TimeShift;
   CString  m_Report;
   BOOL  m_CHK_Report;
   CString  m_Year2;
   CString  m_Day2;
   CString  m_Month2;
   BOOL  m_CHK_Only;
   BOOL  m_CHK_Reset;
   CTime m_Date1;
   CTime m_Date2;
   int      m_RB_IN;
   //}}AFX_DATA

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CDateUpdater)
   protected:

      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Implementation
   protected:

   // Generated message map functions
   //{{AFX_MSG(CDateUpdater)
   virtual BOOL OnInitDialog();
   afx_msg void OnBtnRun();
   afx_msg void OnBtnFile();
   afx_msg void OnChkYear();
   afx_msg void OnChkMonth();
   afx_msg void OnChkDay();
   afx_msg void OnChkReport();
   afx_msg void OnBtnReport();
   afx_msg void OnCloseupCmbYear();
   afx_msg void OnCloseupCmbMonth();
   afx_msg void OnCloseupCmbDay();
   afx_msg void OnDatetimechangeDtDate1(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDatetimechangeDtDate2(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDestroy();
   //}}AFX_MSG

   afx_msg LRESULT   OnFileMatch(WPARAM wParam,LPARAM lParam);

   DECLARE_MESSAGE_MAP()

   private:

      void  Enabler();
      void  Enabler2();

      void  Cleanup();
      void  PopulateFileList();
      bool  Patch (MATCHED_FILE& rMatchedFile);
      void  Report(const MATCHED_FILE& rMatchedFile);
                  
   public:

      static bool Matcher(int iCondition,int iUser,int iUser2,int iFile);
      static bool IsMatchYear (int iFileYear);
      static bool IsMatchMonth(int iFileMonth);
      static bool IsMatchDay  (int iFileDay);

   private:

      void  PrintHeader(const char* const pszLine);
      void  PrintFooter(const char* const pszLine);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
