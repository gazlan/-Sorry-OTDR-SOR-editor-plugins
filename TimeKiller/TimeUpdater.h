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
   int            _iTimeStampORG;
   int            _iTimeStamp;
   char           _pszFile[MAX_PATH + 1];
};

/* ******************************************************************** **
** @@ class CTimeUpdater : public CDialog
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

class CTimeUpdater : public CDialog
{
   private:

      SortedVector      _FileList;
      FILE*             _pReport;
      FILETIME          _BaseTime;
      char              _pszFolder[MAX_PATH + 1];
      char              _pszReport[MAX_PATH + 1];
      int               _iHours;
      int               _iMinutes;
      int               _iSeconds;
      int               _iHoursApx;
      int               _iMinutesApx;
      int               _iSecondsApx;
      int               _iTimeStep;
         
   // Construction
   public:

      CTimeUpdater(CWnd* pParent = NULL);   // standard constructor

      void  AppendList(const char* const pszFName);

   // Dialog Data
   //{{AFX_DATA(CTimeUpdater)
   enum { IDD = IDD_TIME_UPDATER };
   CDateTimeCtrl  m_Date;
   CSpinButtonCtrl   m_SPN_Local;
   CString  m_Path;
   CString  m_Status;
   CString  m_TZShift;
   CString  m_Report;
   BOOL     m_CHK_Report;
   BOOL     m_CHK_Only;
   CString  m_Hours;
   CString  m_Minutes;
   CString  m_Seconds;
   CString  m_TimeStep;
   BOOL     m_CHK_Delay;
   BOOL     m_CHK_Change;
   BOOL     m_CHK_Recursively;
   CString  m_TZChange;
   int      m_RB_Plus;
   CString  m_1st;
   BOOL  m_CHK_ALERT;
   //}}AFX_DATA

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTimeUpdater)
   protected:

      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Implementation
   protected:

   // Generated message map functions
   //{{AFX_MSG(CTimeUpdater)
   virtual BOOL OnInitDialog();
   afx_msg void OnBtnRun();
   afx_msg void OnBtnFile();
   afx_msg void OnChkReport();
   afx_msg void OnBtnReport();
   afx_msg void OnBtnNow();
   afx_msg void OnBtnPlus2();
   afx_msg void OnBtnPlus5();
   afx_msg void OnBtnPlus10();
   afx_msg void OnBtnPlus15();
   afx_msg void OnBtnPlus30();
   afx_msg void OnChangeEdtHours();
   afx_msg void OnChangeEdtMinutes();
   afx_msg void OnChangeEdtSeconds();
   afx_msg void OnKillfocusEdtHours();
   afx_msg void OnKillfocusEdtSeconds();
   afx_msg void OnKillfocusEdtMinutes();
   afx_msg void OnDestroy();
   afx_msg void OnBtnPlus1();
   afx_msg void OnBtnAsFirst();
   afx_msg void OnChkChange();
   //}}AFX_MSG

   afx_msg LRESULT   OnFileMatch(WPARAM wParam,LPARAM lParam);

   DECLARE_MESSAGE_MAP()

   private:

      void  PopulateFileList();
      bool  Patch (MATCHED_FILE& rMatchedFile);
      void  Report(const MATCHED_FILE& rMatchedFile);
      void  AppendTime();
      void  Cleanup();
      void  PrintHeader(const char* const pszLine);
      void  PrintFooter(const char* const pszLine);
      void  IterateList();
      void  SetBaseTime();
      void  EnablerTZ(BOOL bEnable); 
      void  UpdateTS();
      void  UpdateTZ();
      void  DoReport();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
