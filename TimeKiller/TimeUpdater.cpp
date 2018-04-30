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

#include "TimeKiller.h"
#include "TimeUpdater.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@ internal defines
** ******************************************************************** */

#define  SORRY                         "Sorry!"

#define FILE_LIST_DEFAULT_SIZE         (128)
#define FILE_LIST_DEFAULT_DELTA        (32)

/* ******************************************************************** **
** @@ struct FILE_LIST_ENTRY
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

#pragma pack(push,1)
struct FILE_LIST_ENTRY
{
//   int         _iIdx;
   bool        _bError;
   char        _pszFile[MAX_PATH + 1];
};
#pragma pack(pop)

/* ******************************************************************** **
** @@ internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@ external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@ static global variables
** ******************************************************************** */
                  
static CTimeUpdater*    _pThis = NULL;

static MATCHED_FILE     _MatchedFile;

static int              _iTZChange = 0;
static int              _iTZShift  = 0;

/* ******************************************************************** **
** @@ real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ FileListSorter()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static int __cdecl FileListSorter(const void** const pKey1,const void** const pKey2)
{
   FILE_LIST_ENTRY*     p1 = (FILE_LIST_ENTRY*)*pKey1;  
   FILE_LIST_ENTRY*     p2 = (FILE_LIST_ENTRY*)*pKey2;  

   return strcmp(p1->_pszFile,p2->_pszFile);
}

/* ******************************************************************** **
** @@ ForEach()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void ForEach(char* pszFilename)
{
   char     pszDrive   [_MAX_DRIVE];
   char     pszDir     [_MAX_DIR];
   char     pszFName   [_MAX_FNAME];
   char     pszExt     [_MAX_EXT];

   _splitpath(pszFilename,pszDrive,pszDir,pszFName,pszExt);

   if (_pThis)
   {
      char     pszTemp[MAX_PATH + 1];

      strcpy(pszTemp,pszFName);
      strcat(pszTemp,pszExt);

      _pThis->AppendList(pszTemp);
   }
}

/* ******************************************************************** **
** @@ WalkerHandler()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

static void WalkerHandler(char* pszFile)
{
   if (!pszFile || !*pszFile)
   {
      // Error !
      return;
   }

   HANDLE   hFile = FILE_OpenFileReadOnly(pszFile);

   if (hFile == INVALID_HANDLE_VALUE)
   {
      // Error !
      return;
   }

   #ifdef _DEBUG
   SYSTEMTIME    SysTimeDebug;
   #endif

   unsigned __int64     iFleTime64 = 0i64;
   
   FILETIME       CreationTime;  
   FILETIME       LastAccessTime;

   if (!::GetFileTime(hFile,&CreationTime,&LastAccessTime,(FILETIME*)&iFleTime64))
   {
      // Error !
      CloseHandle(hFile);
      hFile = INVALID_HANDLE_VALUE;
      return;
   }

   CloseHandle(hFile);
   hFile = INVALID_HANDLE_VALUE;

   #ifdef _DEBUG
   ::FileTimeToSystemTime((FILETIME*)&iFleTime64,&SysTimeDebug);
   #endif

   // Update
   iFleTime64 -= 10000000i64 * 60i64 * 60i64 * _iTZChange;

   #ifdef _DEBUG
   ::FileTimeToSystemTime((FILETIME*)&iFleTime64,&SysTimeDebug);
   #endif

   if (!FILE_SetFileTSbyName(pszFile,(FILETIME*)&iFleTime64))
   {
      // Error !
      ASSERT(0);
      return;
   }
}

/* ******************************************************************** **
** @@ ReportHandler()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

static void ReportHandler(char* pszFile)
{
   if (!pszFile || !*pszFile)
   {
      // Error !
      return;
   }

   CString     sMessage = _T("");

   HANDLE   hFile = FILE_OpenFileReadOnly(pszFile);

   if (hFile == INVALID_HANDLE_VALUE)
   {
      // Error !
      sMessage.Format("Err: Can't open file [%s] for read.",pszFile);
      MessageBox(::GetActiveWindow(),(LPCTSTR)sMessage,SORRY,MB_OK | MB_ICONEXCLAMATION);
      return;
   }

   FILETIME       FT_CreationTime;  
   FILETIME       FT_LastAccessTime;
   FILETIME       FT_LastWriteTime;  

   if (!GetFileTime(hFile,&FT_CreationTime,&FT_LastAccessTime,&FT_LastWriteTime))
   {
      // Error !
      return;
   }

   SYSTEMTIME     UTC_Time;
   SYSTEMTIME     LocalTime;
    
   TIME_ZONE_INFORMATION      TZInfo;

   GetTimeZoneInformation(&TZInfo);

   TZInfo.Bias = -_iTZShift * 60; // Inverted Sign !

   // Convert the last-write time to local time.
   FileTimeToSystemTime(&FT_LastWriteTime,&UTC_Time);
   SystemTimeToTzSpecificLocalTime(&TZInfo,&UTC_Time,&LocalTime);

   memset(&_MatchedFile,0,sizeof(MATCHED_FILE));

   memcpy(&_MatchedFile._FT_CreationTime,  &FT_CreationTime,  sizeof(FILETIME));
   memcpy(&_MatchedFile._FT_LastAccessTime,&FT_LastAccessTime,sizeof(FILETIME));
   memcpy(&_MatchedFile._FT_LastWriteTime, &FT_LastWriteTime, sizeof(FILETIME));

   memcpy(&_MatchedFile._UTC_Time, &UTC_Time, sizeof(SYSTEMTIME));
   memcpy(&_MatchedFile._LocalTime,&LocalTime,sizeof(SYSTEMTIME));

   strncpy(_MatchedFile._pszFile,pszFile,MAX_PATH);
   _MatchedFile._pszFile[MAX_PATH] = 0; // Ensure ASCIIZ

   ::SendMessage(_pThis->m_hWnd,WM_FILE_MATCH,0,0);
}

/* ******************************************************************** **
** @@ CTimeUpdater::CTimeUpdater()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

CTimeUpdater::CTimeUpdater(CWnd* pParent /*=NULL*/)
:  CDialog(CTimeUpdater::IDD, pParent)
{
   //{{AFX_DATA_INIT(CTimeUpdater)
   m_Path = _T("");
   m_Status = _T("");
   m_TZShift = _T("");
   m_Report = _T("");
   m_CHK_Report = FALSE;
   m_CHK_Only = FALSE;
   m_Hours = _T("");
   m_Minutes = _T("");
   m_Seconds = _T("");
   m_TimeStep = _T("");
   m_CHK_Delay = FALSE;
   m_CHK_Change = FALSE;
   m_CHK_Recursively = FALSE;
   m_TZChange = _T("");
   m_RB_Plus = -1;
   m_1st = _T("");
   m_CHK_ALERT = FALSE;
   //}}AFX_DATA_INIT

   _pReport = NULL;

   memset(_pszFolder,0,MAX_PATH + 1);
   memset(_pszReport,0,MAX_PATH + 1);

   memset(&_BaseTime,0,sizeof(FILETIME));
   
   _iHours   = 0;
   _iMinutes = 0;
   _iSeconds = 0;

   _iHoursApx   = 0;
   _iMinutesApx = 0;
   _iSecondsApx = 0;

   _iTimeStep = 0;
}

/* ******************************************************************** **
** @@ CTimeUpdater::DoDataExchange()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTimeUpdater)
   DDX_Control(pDX, IDC_DTP_DATE, m_Date);
   DDX_Control(pDX, IDC_SPN_LOCAL, m_SPN_Local);
   DDX_Text(pDX, IDC_EDT_PATH, m_Path);
   DDV_MaxChars(pDX, m_Path, 255);
   DDX_Text(pDX, IDC_STT_STATUS, m_Status);
   DDV_MaxChars(pDX, m_Status, 255);
   DDX_Text(pDX, IDC_EDT_LOCAL, m_TZShift);
   DDV_MaxChars(pDX, m_TZShift, 3);
   DDX_Text(pDX, IDC_EDT_REPORT, m_Report);
   DDV_MaxChars(pDX, m_Report, 255);
   DDX_Check(pDX, IDC_CHK_REPORT, m_CHK_Report);
   DDX_Check(pDX, IDC_CHK_ONLY, m_CHK_Only);
   DDX_Text(pDX, IDC_EDT_HOURS, m_Hours);
   DDV_MaxChars(pDX, m_Hours, 2);
   DDX_Text(pDX, IDC_EDT_MINUTES, m_Minutes);
   DDV_MaxChars(pDX, m_Minutes, 2);
   DDX_Text(pDX, IDC_EDT_SECONDS, m_Seconds);
   DDV_MaxChars(pDX, m_Seconds, 2);
   DDX_Text(pDX, IDC_EDT_SHIFT, m_TimeStep);
   DDV_MaxChars(pDX, m_TimeStep, 2);
   DDX_Check(pDX, IDC_CHK_DELAY, m_CHK_Delay);
   DDX_Check(pDX, IDC_CHK_CHANGE, m_CHK_Change);
   DDX_Check(pDX, IDC_CHK_RECURSIVELY, m_CHK_Recursively);
   DDX_Text(pDX, IDC_EDT_CHANGE, m_TZChange);
   DDV_MaxChars(pDX, m_TZChange, 2);
   DDX_Radio(pDX, IDC_RB_SHIFT_PLUS, m_RB_Plus);
   DDX_Text(pDX, IDC_EDT_1ST, m_1st);
   DDV_MaxChars(pDX, m_1st, 266);
   DDX_Check(pDX, IDC_CHK_ALERT, m_CHK_ALERT);
   //}}AFX_DATA_MAP
}

/* ******************************************************************** **
** @@ CTimeUpdater::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CTimeUpdater, CDialog)
   //{{AFX_MSG_MAP(CTimeUpdater)
   ON_BN_CLICKED(IDC_BTN_RUN, OnBtnRun)
   ON_BN_CLICKED(IDC_BTN_FILE, OnBtnFile)
   ON_BN_CLICKED(IDC_CHK_REPORT, OnChkReport)
   ON_BN_CLICKED(IDC_BTN_REPORT, OnBtnReport)
   ON_BN_CLICKED(IDC_BTN_NOW, OnBtnNow)
   ON_BN_CLICKED(IDC_BTN_PLUS2, OnBtnPlus2)
   ON_BN_CLICKED(IDC_BTN_PLUS5, OnBtnPlus5)
   ON_BN_CLICKED(IDC_BTN_PLUS10, OnBtnPlus10)
   ON_BN_CLICKED(IDC_BTN_PLUS15, OnBtnPlus15)
   ON_BN_CLICKED(IDC_BTN_PLUS30, OnBtnPlus30)
   ON_EN_CHANGE(IDC_EDT_HOURS, OnChangeEdtHours)
   ON_EN_CHANGE(IDC_EDT_MINUTES, OnChangeEdtMinutes)
   ON_EN_CHANGE(IDC_EDT_SECONDS, OnChangeEdtSeconds)
   ON_EN_KILLFOCUS(IDC_EDT_HOURS, OnKillfocusEdtHours)
   ON_EN_KILLFOCUS(IDC_EDT_SECONDS, OnKillfocusEdtSeconds)
   ON_EN_KILLFOCUS(IDC_EDT_MINUTES, OnKillfocusEdtMinutes)
   ON_BN_CLICKED(IDC_BTN_PLUS1, OnBtnPlus1)
   ON_BN_CLICKED(IDC_BTN_AS_FIRST, OnBtnAsFirst)
   ON_BN_CLICKED(IDC_CHK_CHANGE, OnChkChange)
   ON_WM_DESTROY()
   ON_MESSAGE(WM_FILE_MATCH,OnFileMatch)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CTimeUpdater::OnInitDialog()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BOOL CTimeUpdater::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   CenterWindow();

   EnablerTZ(FALSE);

   m_CHK_ALERT = TRUE;

   m_RB_Plus = 0;

   m_TZChange = _T("0");

   m_SPN_Local.SetRange(-12,+13);

   TIME_ZONE_INFORMATION      TZInfo;

   GetTimeZoneInformation(&TZInfo);

   _iTZShift = -TZInfo.Bias / 60; // Inverted Sign !

   m_SPN_Local.SetPos(_iTZShift);

   // Width is 3 due Sign character
   m_TZShift.Format("%3d",_iTZShift);

   // Now   
   OnBtnNow();

   // Disable
   GetDlgItem(IDC_CHK_ONLY)->EnableWindow(FALSE);

   _iTimeStep = 2;
   m_TimeStep.Format("%2d",_iTimeStep);

   m_CHK_Delay = 1;

   m_Status = "* READY *";

   _FileList.Resize(FILE_LIST_DEFAULT_SIZE);
   _FileList.Delta (FILE_LIST_DEFAULT_DELTA);
   _FileList.SetSorter(FileListSorter);

   UpdateData(FALSE);

   _pThis = this;

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnRun()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnRun() 
{
   UpdateData(TRUE);

   if (!_pszFolder || !*_pszFolder)
   {
      // Error !
      MessageBox("Gimme path to root, plz!",SORRY,MB_OK | MB_ICONEXCLAMATION);
      return;
   }

   if (m_CHK_Report && (!_pszReport || !*_pszReport))
   {
      // Error !
      MessageBox("Gimme report file name, plz!",SORRY,MB_OK | MB_ICONEXCLAMATION);
      return;
   }
   
   // Disable visible pages update
   KRNL_SetSilenceMode(TRUE);

   CWaitCursor    Waiter;

   if (!(m_CHK_Report && m_CHK_Only))
   {
      if (m_CHK_Change)
      {
         UpdateTZ();
      }
      else
      {
         UpdateTS();
      }
   }

   // Report
   const int   REPORT_LINE_SIZE = 161;
   
   char     pszLine[MAX_PATH + 1];

   memset(pszLine,'=',REPORT_LINE_SIZE);
   pszLine[REPORT_LINE_SIZE] = 0; // Ensure ASCIIZ

   if (m_CHK_Report)
   {
      if (_pReport)
      {
         fclose(_pReport);
         _pReport = NULL;
      }

      _pReport = fopen(_pszReport,"wt");

      PrintHeader(pszLine);
   }

   DoReport();

   if (_pReport)
   {
      PrintFooter(pszLine);

      fclose(_pReport);
      _pReport = NULL;
   }

   // Enable visible pages update
   KRNL_SetSilenceMode(FALSE);

   MessageBeep(MB_OK);

   m_Status = "* READY *";

   UpdateData(FALSE);

   if (m_CHK_ALERT)
   {
      MessageBox("Done!",SORRY,MB_OK | MB_ICONINFORMATION);
   }
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnFile()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnFile() 
{
   UpdateData(TRUE);

   DWORD    dwFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE;

   DIR_BrowseForFolder("Select folder for browsing",dwFlags,_pszFolder);

   m_Path = _pszFolder;

   CWaitCursor    Waiter;

   PopulateFileList();

   m_1st = _T("");

   DWORD    dwCnt = _FileList.Count();

   if (dwCnt)
   {
      FILE_LIST_ENTRY*     pEntry = (FILE_LIST_ENTRY*)_FileList.At(0);

      if (pEntry && pEntry->_pszFile && *pEntry->_pszFile)
      {
         m_1st = pEntry->_pszFile;
      }
   }

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnChkReport()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnChkReport() 
{
   UpdateData(TRUE);

   GetDlgItem(IDC_CHK_ONLY)->  EnableWindow(m_CHK_Report);

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnReport()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnReport() 
{
   UpdateData(TRUE);

   DWORD    dwFlags = OFN_EXPLORER | OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
      
   char     pszFilter[] = "*.txt\0*.txt\0*.*\0*.*\0\0";

   char     pszDrive[_MAX_DRIVE];
   char     pszDir  [_MAX_DIR];
   char     pszFName[_MAX_FNAME];
   char     pszExt  [_MAX_EXT];

   _splitpath(_pszFolder,pszDrive,pszDir,pszFName,pszExt);

   strcpy(_pszReport,_pszFolder);
   strcat(_pszReport,"\\report.txt");

   OPENFILENAME         OFN;

   memset(&OFN,0,sizeof(OPENFILENAME));

   OFN.lStructSize     = sizeof(OPENFILENAME); 
   OFN.hwndOwner       = GetSafeHwnd();
   OFN.lpstrFilter     = pszFilter; 
   OFN.nFilterIndex    = 1;
   OFN.lpstrInitialDir = pszDrive;
   OFN.lpstrFile       = _pszReport;
   OFN.nMaxFile        = MAX_PATH;
   OFN.lpstrFileTitle  = NULL;
   OFN.nMaxFileTitle   = MAX_PATH;
   OFN.Flags           = dwFlags;

   m_Report = _T("");

   if (GetSaveFileName(&OFN) == TRUE)
   {
      m_Report = _pszReport;
   }

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::Patch()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool CTimeUpdater::Patch (MATCHED_FILE& rMatchedFile)
{
   return false;
}

/* ******************************************************************** **
** @@ CTimeUpdater::Report()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::Report(const MATCHED_FILE& rMatchedFile)
{
   if (!_pReport)
   {
      // Error !
      return;
   }

   if (!_pszReport || !*_pszReport)
   {
      // Error !
      return;
   }
   
   CString     sFileTimeUTC   = _T("");
   CString     sFileTimeLocal = _T("");
   CString     sUTC_Before    = _T(""); 
   CString     sUTC_After     = _T(""); 
   CString     sLocalBefore   = _T(""); 
   CString     sLocalAfter    = _T(""); 

   sFileTimeUTC.Format
   (
      "%04d-%02d-%02d | %02d:%02d:%02d,%03d",
      rMatchedFile._UTC_Time.wYear,
      rMatchedFile._UTC_Time.wMonth,
      rMatchedFile._UTC_Time.wDay,
      rMatchedFile._UTC_Time.wHour,
      rMatchedFile._UTC_Time.wMinute,
      rMatchedFile._UTC_Time.wSecond, 
      rMatchedFile._UTC_Time.wMilliseconds
   );

   fprintf(_pReport,"%s # ",(LPCTSTR)sFileTimeUTC);

   sFileTimeLocal.Format
   (
      "%04d-%02d-%02d | %02d:%02d:%02d,%03d",
      rMatchedFile._LocalTime.wYear,
      rMatchedFile._LocalTime.wMonth,
      rMatchedFile._LocalTime.wDay,
      rMatchedFile._LocalTime.wHour,
      rMatchedFile._LocalTime.wMinute,
      rMatchedFile._UTC_Time.wSecond, 
      rMatchedFile._LocalTime.wMilliseconds
   );

   fprintf(_pReport,"%s # ",(LPCTSTR)sFileTimeLocal);

   KRNL_Load((char*)rMatchedFile._pszFile);

   int   iTimeStampORG = FXD_GetSorTimeOriginal();
   int   iTimeStamp    = FXD_GetSorTime();

   char     pszDate[MAX_PATH + 1];
   char     pszTime[MAX_PATH + 1];

   // Original TimeStamp
   // UTC !
   struct tm*     pTS_ORG = gmtime((long*)(&iTimeStampORG));

   sprintf(pszDate,"%04d-%02d-%02d",pTS_ORG->tm_year + 1900,pTS_ORG->tm_mon + 1,pTS_ORG->tm_mday);
   sprintf(pszTime,"%02d:%02d:%02d",pTS_ORG->tm_hour,pTS_ORG->tm_min,pTS_ORG->tm_sec);
                                 
   CString     sTS_ORG_UTC = pszDate;
   
   sTS_ORG_UTC += " | ";
   sTS_ORG_UTC += pszTime;

   fprintf(_pReport,"%s # ",(LPCTSTR)sTS_ORG_UTC);

   // Use LocalTime, no UTC ! 
   pTS_ORG = localtime((long*)(&iTimeStamp));

   sprintf(pszDate,"%04d-%02d-%02d",pTS_ORG->tm_year + 1900,pTS_ORG->tm_mon + 1,pTS_ORG->tm_mday);
   sprintf(pszTime,"%02d:%02d:%02d",pTS_ORG->tm_hour,pTS_ORG->tm_min,pTS_ORG->tm_sec);
                                 
   CString     sTS_ORG_Local = pszDate;

   sTS_ORG_Local += " | ";
   sTS_ORG_Local += pszTime;

   fprintf(_pReport,"%s # ",(LPCTSTR)sTS_ORG_Local);

   // Current TimeStamp
   // UTC !
   struct tm*     pTS_SOR = gmtime((long*)(&iTimeStamp));

   sprintf(pszDate,"%04d-%02d-%02d",pTS_SOR->tm_year + 1900,pTS_SOR->tm_mon + 1,pTS_SOR->tm_mday);
   sprintf(pszTime,"%02d:%02d:%02d",pTS_SOR->tm_hour,pTS_SOR->tm_min,pTS_SOR->tm_sec);
                                 
   CString     sTS_SOR_UTC = pszDate;
   
   sTS_SOR_UTC += " | ";
   sTS_SOR_UTC += pszTime;

   fprintf(_pReport,"%s # ",(LPCTSTR)sTS_SOR_UTC);

   // Use LocalTime, no UTC ! 
   pTS_SOR = localtime((long*)(&iTimeStamp));

   sprintf(pszDate,"%04d-%02d-%02d",pTS_SOR->tm_year + 1900,pTS_SOR->tm_mon + 1,pTS_SOR->tm_mday);
   sprintf(pszTime,"%02d:%02d:%02d",pTS_SOR->tm_hour,pTS_SOR->tm_min,pTS_SOR->tm_sec);
                                 
   CString     sTS_SOR_Local = pszDate;

   sTS_SOR_Local += " | ";
   sTS_SOR_Local += pszTime;

   fprintf(_pReport,"%s # ",(LPCTSTR)sTS_SOR_Local);

   fprintf(_pReport,"%s\n",rMatchedFile._pszFile);
}

/* ******************************************************************** **
** @@ CTimeUpdater::PrintHeader()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::PrintHeader(const char* const pszLine)
{
   fprintf(_pReport,"%s\n",pszLine);
   fprintf(_pReport,"File TimeStamp UTC        # File TimeStamp Local      # SOR ORG TimeStamp UTC # SOR ORG TimeStamp Loc # SOR TimeStamp UTC     # SOR TimeStamp Local   # File\n");
   fprintf(_pReport,"---------- + ------------ # ---------- + ------------ # --------- + --------- # ---------- + -------- # ---------- + -------- # ---------- + -------- # ---------\n");
}

/* ******************************************************************** **
** @@ CTimeUpdater::PrintFooter()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::PrintFooter(const char* const pszLine)
{
   fprintf(_pReport,"---------- + ------------ # ---------- + ------------ # --------- + --------- # ---------- + -------- # ---------- + -------- # ---------- + -------- # ---------\n");
   fprintf(_pReport,"File TimeStamp UTC        # File TimeStamp Local      # SOR ORG TimeStamp UTC # SOR ORG TimeStamp Loc # SOR TimeStamp UTC     # SOR TimeStamp Local   # File\n");
   fprintf(_pReport,"%s\n",pszLine);
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnNow()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnNow() 
{
   CTime    TS; 

   // Set Time to Now !
   TS = CTime::GetCurrentTime();

   m_Date.SetTime(&TS); 

   m_Hours.Format  ("%02d",TS.GetHour());
   m_Minutes.Format("%02d",TS.GetMinute());
   m_Seconds.Format("%02d",TS.GetSecond());

   UpdateData(FALSE);
}


/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnPlus1()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnPlus1() 
{
   _iHoursApx   = 0;
   _iMinutesApx = 1;
   _iSecondsApx = 0;

   AppendTime();
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnPlus2()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnPlus2() 
{
   _iHoursApx   = 0;
   _iMinutesApx = 2;
   _iSecondsApx = 0;

   AppendTime();
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnPlus5()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnPlus5() 
{
   _iHoursApx   = 0;
   _iMinutesApx = 5;
   _iSecondsApx = 0;

   AppendTime();
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnPlus10()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnPlus10() 
{
   _iHoursApx   = 0;
   _iMinutesApx = 10;
   _iSecondsApx = 0;

   AppendTime();
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnPlus15()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnPlus15() 
{
   _iHoursApx   = 0;
   _iMinutesApx = 15;
   _iSecondsApx = 0;

   AppendTime();
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnPlus30()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnPlus30() 
{
   _iHoursApx   = 0;
   _iMinutesApx = 30;
   _iSecondsApx = 0;

   AppendTime();
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnChangeEdtHours()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnChangeEdtHours() 
{
   UpdateData(TRUE);

   _iHours = atoi((LPCTSTR)m_Hours);

   if (_iHours < 0)
   {
      _iHours = 0;
      m_Hours.Format("%02d",_iHours);
      UpdateData(FALSE);
   }

   if (_iHours > 23 )
   {
      _iHours = 23;
      m_Hours.Format("%02d",_iHours);
      UpdateData(FALSE);
   }
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnChangeEdtMinutes()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnChangeEdtMinutes() 
{
   UpdateData(TRUE);

   _iMinutes = atoi((LPCTSTR)m_Minutes);

   if (_iMinutes < 0)
   {
      _iMinutes = 0;
      m_Minutes.Format("%02d",_iMinutes);
      UpdateData(FALSE);
   }

   if (_iMinutes > 59 )
   {
      _iMinutes = 59;
      m_Minutes.Format("%02d",_iMinutes);
      UpdateData(FALSE);
   }
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnChangeEdtSeconds()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnChangeEdtSeconds() 
{
   UpdateData(TRUE);

   _iSeconds = atoi((LPCTSTR)m_Seconds);

   if (_iSeconds < 0)
   {
      _iSeconds = 0;
      m_Seconds.Format("%02d",_iSeconds);
      UpdateData(FALSE);
   }

   if (_iSeconds > 59 )
   {
      _iSeconds = 59;
      m_Seconds.Format("%02d",_iSeconds);
      UpdateData(FALSE);
   }
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnKillfocusEdtHours()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnKillfocusEdtHours() 
{
   UpdateData(TRUE);

   _iHours = atoi((LPCTSTR)m_Hours);

   m_Hours.Format("%02d",_iHours);
   
   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnKillfocusEdtMinutes()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnKillfocusEdtMinutes() 
{
   UpdateData(TRUE);

   _iMinutes = atoi((LPCTSTR)m_Minutes);

   m_Minutes.Format("%02d",_iMinutes);
   
   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnKillfocusEdtSeconds()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnKillfocusEdtSeconds() 
{
   UpdateData(TRUE);

   _iSeconds = atoi((LPCTSTR)m_Seconds);

   m_Seconds.Format("%02d",_iSeconds);
   
   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::AppendTime()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::AppendTime()
{
   UpdateData(TRUE);

   CTime    TS; 

   m_Date.GetTime(TS); 

   struct tm     Date;

   TS.GetLocalTm(&Date);

   _iSeconds = atoi((LPCTSTR)m_Seconds) + _iSecondsApx;

   _iMinutesApx += _iSeconds / 60;      
   _iSeconds    %= 60;

   _iMinutes = atoi((LPCTSTR)m_Minutes) + _iMinutesApx;

   _iHoursApx += _iMinutesApx / 60;   
   _iMinutesApx  %= 60;

   _iHoursApx %= 24;

   _iHours = atoi((LPCTSTR)m_Hours)   + _iHoursApx;

   CTime    NewTS(Date.tm_year + 1900,Date.tm_mon + 1,Date.tm_mday,_iHours,_iMinutes,_iSeconds);

   m_Date.SetTime(&NewTS); 

   m_Hours.Format  ("%02d",NewTS.GetHour());
   m_Minutes.Format("%02d",NewTS.GetMinute());
   m_Seconds.Format("%02d",NewTS.GetSecond());

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::Cleanup()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::Cleanup()
{
   DWORD    dwCnt = _FileList.Count();

   if (dwCnt)
   {
      // Should be int !
      for (int ii = (dwCnt - 1); ii >= 0; --ii)
      {
         FILE_LIST_ENTRY*     pEntry = (FILE_LIST_ENTRY*)_FileList.At(ii);

         _FileList.RemoveAt(ii);

         if (pEntry)
         {
            delete pEntry;
            pEntry = NULL;
         }
      }
   }
}

/* ******************************************************************** **
** @@ CTimeUpdater::PopulateFileList()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::PopulateFileList()
{
   Cleanup();

   KRNL_Walker(_pszFolder,ForEach,"*.sor",FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::AppendList()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::AppendList(const char* const pszFName)
{
   FILE_LIST_ENTRY*     pEntry = new FILE_LIST_ENTRY;

   if (pEntry)
   {
      memset(pEntry,0,sizeof(FILE_LIST_ENTRY));

//      pEntry->_iNum = ++_iTotal;

      strcpy(pEntry->_pszFile,pszFName);

      if (_FileList.Insert(pEntry) == -1)
      {
         // Error !
         ASSERT(0);
         delete pEntry;
         pEntry = NULL;
      }
   }
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnDestroy()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnDestroy() 
{
   CDialog::OnDestroy();
   
      Cleanup();
}

/* ******************************************************************** **
** @@ CTimeUpdater::IterateList()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::IterateList() 
{
   DWORD    dwCnt = _FileList.Count();

   if (!dwCnt)
   {
      // Empty !
      return;
   }

   FILETIME    LocalTo;
   FILETIME    UTC_Time;

   TIME_ZONE_INFORMATION      TZInfo;

   GetTimeZoneInformation(&TZInfo);

   int      iBias = -TZInfo.Bias / 60;

   KRNL_Time_Local_2_Local(&_BaseTime,&LocalTo,iBias,_iTZShift);
   KRNL_Time_Local_2_UTC(&LocalTo,&UTC_Time);

   for (DWORD ii = 0; ii < dwCnt; ++ii)
   {
      FILE_LIST_ENTRY*     pEntry = (FILE_LIST_ENTRY*)_FileList.At(ii);

      if (pEntry && pEntry->_pszFile && *pEntry->_pszFile)
      {
         CString     sFile = _pszFolder;

         sFile += "\\";
         sFile += pEntry->_pszFile;

         if (!FILE_SetFileTimeStampUTC((char*)(LPCTSTR)sFile,&UTC_Time,_iTimeStep * ii,0,m_CHK_Delay))
         {
            pEntry->_bError = true; 
         }
      }  
   }
}

/* ******************************************************************** **
** @@ CTimeUpdater::SetBaseTime()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::SetBaseTime()
{
   UpdateData(TRUE);

   CTime    FileDate; 

   m_Date.GetTime(FileDate); 

   struct tm     Date;

   FileDate.GetLocalTm(&Date);

   _iHours   = atoi((LPCTSTR)m_Hours);
   _iMinutes = atoi((LPCTSTR)m_Minutes);
   _iSeconds = atoi((LPCTSTR)m_Seconds);

   CTime    FileTS(Date.tm_year + 1900,Date.tm_mon + 1,Date.tm_mday,_iHours,_iMinutes,_iSeconds);

   // Convert CTime to FILETIME
   SYSTEMTIME     SysTime;

   FileTS.GetAsSystemTime(SysTime);

   ::SystemTimeToFileTime(&SysTime,&_BaseTime);
}

/* ******************************************************************** **
** @@ CTimeUpdater::EnablerTZ()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::EnablerTZ(BOOL bEnable)
{
   // Enable
   GetDlgItem(IDC_EDT_CHANGE)->     EnableWindow(bEnable);
   GetDlgItem(IDC_RB_SHIFT_PLUS)->  EnableWindow(bEnable);
   GetDlgItem(IDC_RB_SHIFT_MINUS)-> EnableWindow(bEnable);
   GetDlgItem(IDC_CHK_RECURSIVELY)->EnableWindow(bEnable);

   // Disable ALL rest
   GetDlgItem(IDC_DTP_DATE)->    EnableWindow(!bEnable);
   GetDlgItem(IDC_BTN_NOW)->     EnableWindow(!bEnable);
   GetDlgItem(IDC_BTN_AS_FIRST)->EnableWindow(!bEnable);
   GetDlgItem(IDC_EDT_HOURS)->   EnableWindow(!bEnable);
   GetDlgItem(IDC_EDT_MINUTES)-> EnableWindow(!bEnable);
   GetDlgItem(IDC_EDT_SECONDS)-> EnableWindow(!bEnable);
   GetDlgItem(IDC_BTN_PLUS1)->   EnableWindow(!bEnable);
   GetDlgItem(IDC_BTN_PLUS2)->   EnableWindow(!bEnable);
   GetDlgItem(IDC_BTN_PLUS5)->   EnableWindow(!bEnable);
   GetDlgItem(IDC_BTN_PLUS10)->  EnableWindow(!bEnable);
   GetDlgItem(IDC_BTN_PLUS15)->  EnableWindow(!bEnable);
   GetDlgItem(IDC_BTN_PLUS30)->  EnableWindow(!bEnable);
   GetDlgItem(IDC_EDT_LOCAL)->   EnableWindow(!bEnable);
   GetDlgItem(IDC_SPN_LOCAL)->   EnableWindow(!bEnable);
   GetDlgItem(IDC_EDT_SHIFT)->   EnableWindow(!bEnable);
   GetDlgItem(IDC_CHK_DELAY)->   EnableWindow(!bEnable);
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnChkChange()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnChkChange() 
{
   UpdateData(TRUE);

   EnablerTZ(m_CHK_Change);

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnBtnAsFirst()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::OnBtnAsFirst() 
{
   if (!_pszFolder || !*_pszFolder)
   {
      // Error !
      MessageBox("Gimme path to root, plz!",SORRY,MB_OK | MB_ICONEXCLAMATION);
      return;
   }

   DWORD    dwCnt = _FileList.Count();

   if (!dwCnt)
   {
      // Empty !
      MessageBox("Err: FileList is empty.",SORRY,MB_OK | MB_ICONSTOP);
      return;
   }

   FILE_LIST_ENTRY*     pEntry = (FILE_LIST_ENTRY*)_FileList.At(0);

   FILETIME    FileTime;

   memset(&FileTime,0,sizeof(FILETIME));

   if (pEntry && pEntry->_pszFile && *pEntry->_pszFile)
   {
      CString     sFile = _pszFolder;

      sFile += "\\";
      sFile += pEntry->_pszFile;
   
      if (FILE_GetLatestFileTimeByName((char*)(LPCTSTR)sFile,&FileTime))
      {
         CTime    TS(FileTime); 
         
         m_Date.SetTime(&TS); 
         
         m_Hours.Format  ("%02d",TS.GetHour());
         m_Minutes.Format("%02d",TS.GetMinute());
         m_Seconds.Format("%02d",TS.GetSecond());
         
         UpdateData(FALSE);
      }
   }      
}

/* ******************************************************************** **
** @@ CTimeUpdater::UpdateTS()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::UpdateTS()
{
   UpdateData(TRUE);

   SetBaseTime();

   // Update TimeShift value to current setting
   _iTZShift = atoi(m_TZShift);

   IterateList(); 
}

/* ******************************************************************** **
** @@ CTimeUpdater::UpdateTZ()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::UpdateTZ()
{
   UpdateData(TRUE);

   _iTZChange = atoi(m_TZChange);

   if (!_iTZChange)
   {
      MessageBox("Nothing to do.",SORRY,MB_OK | MB_ICONEXCLAMATION);
      return;
   }

   if (m_RB_Plus)
   {
      _iTZChange = -_iTZChange;  // Invert !
   }

   KRNL_Walker(_pszFolder,WalkerHandler,"*.sor",m_CHK_Recursively);
}

/* ******************************************************************** **
** @@ CTimeUpdater::DoReport()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CTimeUpdater::DoReport()
{
   UpdateData(TRUE);

   // Update TimeShift value to current setting
   _iTZShift = atoi(m_TZShift);

   KRNL_Walker(_pszFolder,ReportHandler,"*.sor",m_CHK_Recursively);
}

/* ******************************************************************** **
** @@ CTimeUpdater::OnFileMatch()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

LRESULT CTimeUpdater::OnFileMatch(WPARAM wParam,LPARAM lParam)
{
   CString     sMessage = _T("");
   
   m_Status = _MatchedFile._pszFile;

   UpdateData(FALSE);

   Report(_MatchedFile);
   
   return 0;
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
