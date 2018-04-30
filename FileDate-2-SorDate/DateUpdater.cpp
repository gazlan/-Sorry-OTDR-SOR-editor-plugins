/* ******************************************************************** **
** @@ FileDate2SorDate
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

#include "..\shared\vector.h"
#include "..\shared\vector_sorted.h"

#include "sorry_sdk.h"

#include "resource.h"

#include "FileDate2SorDate.h"
#include "DateUpdater.h"

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
//   int             _iIdx;
   bool           _bError;
   char           _pszFile[MAX_PATH + 1];
   FILETIME       _LastWriteTime;
   SYSTEMTIME     _UTC_Time;
   SYSTEMTIME     _LocalTime;
};
#pragma pack(pop)

// Conditions
#define _ANY_    (0)
#define _GT_     (1)
#define _GE_     (2)
#define _EQ_     (3)
#define _NE_     (4)
#define _LE_     (5)
#define _LT_     (6)
#define _IN_     (7)
#define _OUT_    (8)

/* ******************************************************************** **
** @@ internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@ external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@ static global variables
** ******************************************************************** */
                  
static CDateUpdater*    _pThis = NULL;

static MATCHED_FILE     _MatchedFile;

static int              _iTimeShift = 0;

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
      ASSERT(0);
      CloseHandle(hFile);
      hFile = INVALID_HANDLE_VALUE;
      return;
   }

   SYSTEMTIME     UTC_Time;
   SYSTEMTIME     LocalTime;
    
   TIME_ZONE_INFORMATION      TZInfo;

   GetTimeZoneInformation(&TZInfo);

   TZInfo.Bias = -_iTimeShift * 60; // Inverted Sign !

   // Convert the last-write time to local time.
   FileTimeToSystemTime(&FT_LastWriteTime,&UTC_Time);
   SystemTimeToTzSpecificLocalTime(&TZInfo,&UTC_Time,&LocalTime);

   memset(&_MatchedFile,0,sizeof(MATCHED_FILE));

   if (CDateUpdater::IsMatchYear(LocalTime.wYear) && CDateUpdater::IsMatchMonth(LocalTime.wMonth) && CDateUpdater::IsMatchDay(LocalTime.wDay))
   {
      memcpy(&_MatchedFile._FT_CreationTime,  &FT_CreationTime,  sizeof(FILETIME));
      memcpy(&_MatchedFile._FT_LastAccessTime,&FT_LastAccessTime,sizeof(FILETIME));
      memcpy(&_MatchedFile._FT_LastWriteTime, &FT_LastWriteTime, sizeof(FILETIME));

      memcpy(&_MatchedFile._UTC_Time, &UTC_Time, sizeof(SYSTEMTIME));
      memcpy(&_MatchedFile._LocalTime,&LocalTime,sizeof(SYSTEMTIME));

      strncpy(_MatchedFile._pszFile,pszFile,MAX_PATH);
      _MatchedFile._pszFile[MAX_PATH] = 0; // Ensure ASCIIZ

      ::SendMessage(_pThis->m_hWnd,WM_FILE_MATCH,0,0);
   }

   CloseHandle(hFile);
   hFile = INVALID_HANDLE_VALUE;
}

/* ******************************************************************** **
** @@ CDateUpdater::CDateUpdater()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

CDateUpdater::CDateUpdater(CWnd* pParent /*=NULL*/)
:  CDialog(CDateUpdater::IDD, pParent)
{
   //{{AFX_DATA_INIT(CDateUpdater)
   m_CHK_Day = FALSE;
   m_CHK_Month = FALSE;
   m_CHK_Recursively = FALSE;
   m_CHK_Year = FALSE;
   m_Day = _T("");
   m_Month = _T("");
   m_Year = _T("");
   m_CMB_Year = -1;
   m_CMB_Month = -1;
   m_CMB_Day = -1;
   m_Path = _T("");
   m_Status = _T("");
   m_TimeShift = _T("");
   m_Report = _T("");
   m_CHK_Report = FALSE;
   m_Year2 = _T("");
   m_Day2 = _T("");
   m_Month2 = _T("");
   m_CHK_Only = FALSE;
   m_CHK_Reset = FALSE;
   m_RB_IN = -1;
   //}}AFX_DATA_INIT

   _pReport = NULL;

   memset(_pszFolder,0,MAX_PATH + 1);
   memset(_pszReport,0,MAX_PATH + 1);
}

/* ******************************************************************** **
** @@ CDateUpdater::DoDataExchange()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDateUpdater)
   DDX_Control(pDX, IDC_SPN_LOCAL, m_SPN_Local);
   DDX_Check(pDX, IDC_CHK_DAY, m_CHK_Day);
   DDX_Check(pDX, IDC_CHK_MONTH, m_CHK_Month);
   DDX_Check(pDX, IDC_CHK_RECURSIVELY, m_CHK_Recursively);
   DDX_Check(pDX, IDC_CHK_YEAR, m_CHK_Year);
   DDX_Text(pDX, IDC_EDT_DAY, m_Day);
   DDV_MaxChars(pDX, m_Day, 2);
   DDX_Text(pDX, IDC_EDT_MONTH, m_Month);
   DDV_MaxChars(pDX, m_Month, 2);
   DDX_Text(pDX, IDC_EDT_YEAR, m_Year);
   DDV_MaxChars(pDX, m_Year, 4);
   DDX_CBIndex(pDX, IDC_CMB_YEAR, m_CMB_Year);
   DDX_CBIndex(pDX, IDC_CMB_MONTH, m_CMB_Month);
   DDX_CBIndex(pDX, IDC_CMB_DAY, m_CMB_Day);
   DDX_Text(pDX, IDC_EDT_PATH, m_Path);
   DDV_MaxChars(pDX, m_Path, 255);
   DDX_Text(pDX, IDC_STT_STATUS, m_Status);
   DDV_MaxChars(pDX, m_Status, 255);
   DDX_Text(pDX, IDC_EDT_LOCAL, m_TimeShift);
   DDV_MaxChars(pDX, m_TimeShift, 3);
   DDX_Text(pDX, IDC_EDT_REPORT, m_Report);
   DDV_MaxChars(pDX, m_Report, 255);
   DDX_Check(pDX, IDC_CHK_REPORT, m_CHK_Report);
   DDX_Text(pDX, IDC_EDT_YEAR2, m_Year2);
   DDV_MaxChars(pDX, m_Year2, 4);
   DDX_Text(pDX, IDC_EDT_DAY2, m_Day2);
   DDV_MaxChars(pDX, m_Day2, 2);
   DDX_Text(pDX, IDC_EDT_MONTH2, m_Month2);
   DDV_MaxChars(pDX, m_Month2, 2);
   DDX_Check(pDX, IDC_CHK_ONLY, m_CHK_Only);
   DDX_Check(pDX, IDC_CHK_RESET_TIME, m_CHK_Reset);
   DDX_Radio(pDX, IDC_RB_IN, m_RB_IN);
   //}}AFX_DATA_MAP
}

/* ******************************************************************** **
** @@ CDateUpdater::MESSAGE_MAP()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BEGIN_MESSAGE_MAP(CDateUpdater, CDialog)
   //{{AFX_MSG_MAP(CDateUpdater)
   ON_BN_CLICKED(IDC_BTN_RUN, OnBtnRun)
   ON_BN_CLICKED(IDC_BTN_FILE, OnBtnFile)
   ON_BN_CLICKED(IDC_CHK_YEAR, OnChkYear)
   ON_BN_CLICKED(IDC_CHK_MONTH, OnChkMonth)
   ON_BN_CLICKED(IDC_CHK_DAY, OnChkDay)
   ON_BN_CLICKED(IDC_CHK_REPORT, OnChkReport)
   ON_BN_CLICKED(IDC_BTN_REPORT, OnBtnReport)
   ON_CBN_CLOSEUP(IDC_CMB_YEAR, OnCloseupCmbYear)
   ON_CBN_CLOSEUP(IDC_CMB_MONTH, OnCloseupCmbMonth)
   ON_CBN_CLOSEUP(IDC_CMB_DAY, OnCloseupCmbDay)
   ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DT_DATE1, OnDatetimechangeDtDate1)
   ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DT_DATE2, OnDatetimechangeDtDate2)
   ON_WM_DESTROY()
   ON_MESSAGE(WM_FILE_MATCH,OnFileMatch)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ******************************************************************** **
** @@ CDateUpdater::OnInitDialog()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

BOOL CDateUpdater::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   CenterWindow();

   m_SPN_Local.SetRange(-12,+13);

   TIME_ZONE_INFORMATION      TZInfo;

   GetTimeZoneInformation(&TZInfo);

   _iTimeShift = -TZInfo.Bias / 60; // Inverted Sign !

   m_SPN_Local.SetPos(_iTimeShift);

   // Width is 3 due Sign character
   m_TimeShift.Format("%3d",_iTimeShift);

   // Now   
   CTime    TimeNow; 

   // Set Time to Now !
   TimeNow = CTime::GetCurrentTime();

   m_Year. Format("%04d",TimeNow.GetYear());
   m_Month.Format("%02d",TimeNow.GetMonth());
   m_Day.  Format("%02d",TimeNow.GetDay());

   m_Year2. Format("%04d",TimeNow.GetYear());
   m_Month2.Format("%02d",TimeNow.GetMonth());
   m_Day2.  Format("%02d",TimeNow.GetDay());

   // Set to ANY !
   m_CMB_Year  = 0;
   m_CMB_Month = 0;
   m_CMB_Day   = 0;

   // Disable ALL
   GetDlgItem(IDC_EDT_REPORT)->EnableWindow(FALSE);
   GetDlgItem(IDC_BTN_REPORT)->EnableWindow(FALSE);
   GetDlgItem(IDC_CHK_ONLY)->  EnableWindow(FALSE);

   GetDlgItem(IDC_EDT_YEAR)-> EnableWindow(FALSE);
   GetDlgItem(IDC_EDT_YEAR2)->EnableWindow(FALSE);
   GetDlgItem(IDC_CMB_YEAR)-> EnableWindow(FALSE);

   GetDlgItem(IDC_EDT_MONTH)-> EnableWindow(FALSE);
   GetDlgItem(IDC_EDT_MONTH2)->EnableWindow(FALSE);
   GetDlgItem(IDC_CMB_MONTH)-> EnableWindow(FALSE);

   GetDlgItem(IDC_EDT_DAY)-> EnableWindow(FALSE);
   GetDlgItem(IDC_EDT_DAY2)->EnableWindow(FALSE);
   GetDlgItem(IDC_CMB_DAY)-> EnableWindow(FALSE);

   // Date Pickers
   m_RB_IN = 0;   // Search IN

   GetDlgItem(IDC_RB_IN)-> EnableWindow(FALSE);
   GetDlgItem(IDC_RB_OUT)->EnableWindow(FALSE);

   m_Status = "* READY *";

   // Reset Pickers to DTS_SHOWNONE style. 
   // http://msdn.microsoft.com/en-us/library/windows/desktop/bb761813%28v=vs.85%29.aspx
   DateTime_SetSystemtime(GetDlgItem(IDC_DT_DATE1)->m_hWnd,GDT_NONE,NULL);
   DateTime_SetSystemtime(GetDlgItem(IDC_DT_DATE2)->m_hWnd,GDT_NONE,NULL);

   _FileList.Resize(FILE_LIST_DEFAULT_SIZE);
   _FileList.Delta (FILE_LIST_DEFAULT_DELTA);
   _FileList.SetSorter(FileListSorter);

   UpdateData(FALSE);

   _pThis = this;

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/* ******************************************************************** **
** @@ CDateUpdater::OnBtnRun()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnBtnRun() 
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

   // Update TimeShift value to current setting
   _iTimeShift = atoi(m_TimeShift);

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

   UpdateData(TRUE);

   CWaitCursor    Waiter;

   CString     sMessage = _T("");
   
   m_Status = _MatchedFile._pszFile;

   UpdateData(FALSE);

   if (!m_CHK_Only)
   {
      Patch(_MatchedFile);
   }

   if (m_CHK_Report)
   {
      Report(_MatchedFile);
   }

   m_Status = "* READY *";

   if (_pReport)
   {
      PrintFooter(pszLine);

      fclose(_pReport);
      _pReport = NULL;
   }

   // Enable visible pages update
   KRNL_SetSilenceMode(FALSE);

   MessageBeep(MB_OK);

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CDateUpdater::OnBtnFile()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnBtnFile() 
{
   UpdateData(TRUE);

   DWORD    dwFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE;

   DIR_BrowseForFolder("Select folder for browsing",dwFlags,_pszFolder);

   m_Path = _pszFolder;

   CWaitCursor    Waiter;

   PopulateFileList();

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CDateUpdater::OnChkYear()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnChkYear() 
{
   Enabler();
}

/* ******************************************************************** **
** @@ CDateUpdater::OnChkMonth()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnChkMonth() 
{
   Enabler();
}

/* ******************************************************************** **
** @@ CDateUpdater::OnChkDay()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnChkDay() 
{
   Enabler();
}

/* ******************************************************************** **
** @@ CDateUpdater::Enabler()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::Enabler()
{
   UpdateData(TRUE);

   if (!m_CHK_Year)
   {
      GetDlgItem(IDC_EDT_YEAR2)->EnableWindow(FALSE);
   }

   if (!m_CHK_Month)
   {
      GetDlgItem(IDC_EDT_MONTH2)->EnableWindow(FALSE);
   }

   if (!m_CHK_Day)
   {
      GetDlgItem(IDC_EDT_DAY2)->EnableWindow(FALSE);
   }

   if (m_CHK_Year || m_CHK_Month || m_CHK_Day)
   {
      Enabler2();
   }

   GetDlgItem(IDC_EDT_YEAR)->EnableWindow(m_CHK_Year);
   GetDlgItem(IDC_CMB_YEAR)->EnableWindow(m_CHK_Year);

   GetDlgItem(IDC_EDT_MONTH)->EnableWindow(m_CHK_Month);
   GetDlgItem(IDC_CMB_MONTH)->EnableWindow(m_CHK_Month);

   GetDlgItem(IDC_EDT_DAY)->EnableWindow(m_CHK_Day);
   GetDlgItem(IDC_CMB_DAY)->EnableWindow(m_CHK_Day);

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CDateUpdater::IsMatchYear()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool CDateUpdater::IsMatchYear(int iFileYear)
{
   if (!_pThis)
   {
      // Error !
      return false;
   }

   return CDateUpdater::Matcher(_pThis->m_CMB_Year,atoi(_pThis->m_Year),atoi(_pThis->m_Year2),iFileYear);
}

/* ******************************************************************** **
** @@ CDateUpdater::IsMatchMonth()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool CDateUpdater::IsMatchMonth(int iFileMonth)
{
   if (!_pThis)
   {
      // Error !
      return false;
   }

   return Matcher(_pThis->m_CMB_Month,atoi(_pThis->m_Month),atoi(_pThis->m_Month2),iFileMonth);
}

/* ******************************************************************** **
** @@ CDateUpdater::IsMatchDay()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool CDateUpdater::IsMatchDay(int iFileDay)
{
   if (!_pThis)
   {
      // Error !
      return false;
   }

   return Matcher(_pThis->m_CMB_Day,atoi(_pThis->m_Day),atoi(_pThis->m_Day2),iFileDay);
}

/* ******************************************************************** **
** @@ CDateUpdater::Matcher()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool CDateUpdater::Matcher(int iCondition,int iUser,int iUser2,int iFile)
{
   if (!iCondition)
   {
      // ANY
      return true;
   }

   switch (iCondition)
   {
      case _ANY_:
      {
         return true;
      }
      case _GT_:
      {
         return (iFile > iUser);
      }
      case _GE_:
      {
         return (iFile >= iUser);
      }
      case _EQ_:
      {
         return (iFile == iUser);
      }
      case _NE_:
      {
         return (iFile != iUser);
      }
      case _LE_:
      {
         return (iFile <= iUser);
      }
      case _LT_:
      {
         return (iFile < iUser);
      }
      case _IN_:
      {
         return (iFile >= iUser) && (iFile <= iUser2);
      }
      case _OUT_:
      {
         return (iFile < iUser) || (iFile > iUser2);
      }
   }

   return false;
}

/* ******************************************************************** **
** @@ CDateUpdater::OnFileMatch()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

LRESULT CDateUpdater::OnFileMatch(WPARAM wParam,LPARAM lParam)
{
   if (_pThis)
   {
      _pThis->AppendList();
   }

   return 0;
}

/* ******************************************************************** **
** @@ CDateUpdater::OnChkReport()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnChkReport() 
{
   UpdateData(TRUE);

   GetDlgItem(IDC_EDT_REPORT)->EnableWindow(m_CHK_Report);
   GetDlgItem(IDC_BTN_REPORT)->EnableWindow(m_CHK_Report);
   GetDlgItem(IDC_CHK_ONLY)->  EnableWindow(m_CHK_Report);
}

/* ******************************************************************** **
** @@ CDateUpdater::OnBtnReport()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnBtnReport() 
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
** @@ CDateUpdater::OnCloseupCmbYear()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnCloseupCmbYear() 
{
   Enabler2();
}

/* ******************************************************************** **
** @@ CDateUpdater::OnCloseupCmbMonth()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnCloseupCmbMonth() 
{
   Enabler2();
}

/* ******************************************************************** **
** @@ CDateUpdater::OnCloseupCmbDay()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnCloseupCmbDay() 
{
   Enabler2();
}

/* ******************************************************************** **
** @@ CDateUpdater::Enabler2()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::Enabler2()
{
   UpdateData(TRUE);

   if (m_CHK_Year)
   {
      switch (m_CMB_Year)
      {
         case _IN_:
         case _OUT_:
         {
            GetDlgItem(IDC_EDT_YEAR2)->EnableWindow(TRUE);
            break;
         }
         default:
         {
            GetDlgItem(IDC_EDT_YEAR2)->EnableWindow(FALSE);
            break;
         }
      }
   }

   if (m_CHK_Month)
   {
      switch (m_CMB_Month)
      {
         case _IN_:
         case _OUT_:
         {
            GetDlgItem(IDC_EDT_MONTH2)->EnableWindow(TRUE);
            break;
         }
         default:
         {
            GetDlgItem(IDC_EDT_MONTH2)->EnableWindow(FALSE);
            break;
         }
      }
   }

   if (m_CHK_Day)
   {
      switch (m_CMB_Day)
      {
         case _IN_:
         case _OUT_:
         {
            GetDlgItem(IDC_EDT_DAY2)->EnableWindow(TRUE);
            break;
         }
         default:
         {
            GetDlgItem(IDC_EDT_DAY2)->EnableWindow(FALSE);
            break;
         }
      }
   }

   UpdateData(FALSE);
}

/* ******************************************************************** **
** @@ CDateUpdater::Patch()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool CDateUpdater::Patch(MATCHED_FILE& rMatchedFile)
{
   DWORD    dwCnt = _FileList.Count();

   if (!dwCnt)
   {
      // Error !
      return false;
   }

   for (DWORD ii = 0; ii < dwCnt; ++ii)
   {
      FILE_LIST_ENTRY*     pEntry = (FILE_LIST_ENTRY*)_FileList.At(ii);

      if (pEntry)
      {
         KRNL_Load(pEntry->_pszFile);

         CTime    SorTS(pEntry->_LastWriteTime);

         DWORD    dwTimeStamp = SorTS.GetTime();

         FXD_SetSorTime(dwTimeStamp);

         OPT_SetFixedDelayMin(0);
         OPT_SetFixedDelaySec(0);

         OPT_SetFixedDelayRandom(false);

      //   STR_FILETIME_TYPE_SOR_TIMESTAMP);
         OPT_SetFixedTSstrType(STR_FILETIME_TYPE_FILE_TIMESTAMP);
   
         OPT_SetTimeZoneShift(0);

         KRNL_Save()  ?  true  :  false;
      }
   }

   return true;
}

/* ******************************************************************** **
** @@ CDateUpdater::Report()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::Report(const MATCHED_FILE& rMatchedFile)
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

   DWORD    dwCnt = _FileList.Count();

   if (!dwCnt)
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
   
   for (DWORD ii = 0; ii < dwCnt; ++ii)
   {
      FILE_LIST_ENTRY*     pEntry = (FILE_LIST_ENTRY*)_FileList.At(ii);

      if (pEntry)
      {
         KRNL_Load(pEntry->_pszFile);

         SYSTEMTIME     UTC_Time;
         SYSTEMTIME     LocalTime;
          
         TIME_ZONE_INFORMATION      TZInfo;
         
         GetTimeZoneInformation(&TZInfo);
         TZInfo.Bias = -_iTimeShift * 60; // Inverted Sign !
         
         // Convert the last-write time to local time.
         FileTimeToSystemTime(&pEntry->_LastWriteTime,&UTC_Time);
         SystemTimeToTzSpecificLocalTime(&TZInfo,&UTC_Time,&LocalTime);

         sFileTimeUTC.Format
         (
            "%04d-%02d-%02d | %02d:%02d:%02d,%03d",
            UTC_Time.wYear,
            UTC_Time.wMonth,
            UTC_Time.wDay,
            UTC_Time.wHour,
            UTC_Time.wMinute,
            UTC_Time.wSecond, 
            UTC_Time.wMilliseconds
         );

         fprintf(_pReport,"%s # ",(LPCTSTR)sFileTimeUTC);

         sFileTimeLocal.Format
         (
            "%04d-%02d-%02d | %02d:%02d:%02d,%03d",
            LocalTime.wYear,
            LocalTime.wMonth,
            LocalTime.wDay,
            LocalTime.wHour,
            LocalTime.wMinute,
            UTC_Time.wSecond, 
            LocalTime.wMilliseconds
         );

         fprintf(_pReport,"%s # ",(LPCTSTR)sFileTimeLocal);

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

         fprintf(_pReport,"%s\n",pEntry->_pszFile);
      }
   }
}

/* ******************************************************************** **
** @@ CDateUpdater::OnDatetimechangeDtDate1()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnDatetimechangeDtDate1(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // Show None style
   // Displays a checkbox in the left-hand side of the control which, 
   // when cleared, provides a way to have no date currently selected in the control. 
   // You can also set this state with DTM_SETSYSTEMTIME wParam = GDT_NONE. 
   // Verify the state with DTM_GETSYSTEMTIME. 
   // Type: Bool. Default: False.

   *pResult = 0;

   SYSTEMTIME     SysTime;

   // http://msdn.microsoft.com/en-us/library/windows/desktop/bb761801%28v=vs.85%29.aspx   
   DWORD    dwFlag1 = DateTime_GetSystemtime(GetDlgItem(IDC_DT_DATE1)->m_hWnd,&SysTime);
   DWORD    dwFlag2 = DateTime_GetSystemtime(GetDlgItem(IDC_DT_DATE2)->m_hWnd,&SysTime);

   BOOL     bEnable = ((dwFlag1 == GDT_NONE) && (dwFlag2 == GDT_NONE))  ?  TRUE  :  FALSE;

   BOOL     bBoth = ((dwFlag1 == GDT_VALID) && (dwFlag2 == GDT_VALID))  ?  TRUE  :  FALSE;

   GetDlgItem(IDC_RB_IN)-> EnableWindow(bBoth);
   GetDlgItem(IDC_RB_OUT)->EnableWindow(bBoth);

   GetDlgItem(IDC_CHK_YEAR)-> EnableWindow(bEnable);
   GetDlgItem(IDC_CHK_MONTH)->EnableWindow(bEnable);
   GetDlgItem(IDC_CHK_DAY)->  EnableWindow(bEnable);
}

/* ******************************************************************** **
** @@ CDateUpdater::OnDatetimechangeDtDate2()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnDatetimechangeDtDate2(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // Show None style
   // Displays a checkbox in the left-hand side of the control which, 
   // when cleared, provides a way to have no date currently selected in the control. 
   // You can also set this state with DTM_SETSYSTEMTIME wParam = GDT_NONE. 
   // Verify the state with DTM_GETSYSTEMTIME. 
   // Type: Bool. Default: False.

   *pResult = 0;

   SYSTEMTIME     SysTime;

   // http://msdn.microsoft.com/en-us/library/windows/desktop/bb761801%28v=vs.85%29.aspx   
   DWORD    dwFlag1 = DateTime_GetSystemtime(GetDlgItem(IDC_DT_DATE1)->m_hWnd,&SysTime);
   DWORD    dwFlag2 = DateTime_GetSystemtime(GetDlgItem(IDC_DT_DATE2)->m_hWnd,&SysTime);

   _bUseBefore = (dwFlag1 == GDT_VALID);
   _bUseAfter  = (dwFlag2 == GDT_VALID);

   BOOL     bEnable = (_bUseBefore || _bUseAfter)  ?  FALSE  :  TRUE;

   BOOL     bBoth = ((dwFlag1 == GDT_VALID) && (dwFlag2 == GDT_VALID))  ?  TRUE  :  FALSE;

   GetDlgItem(IDC_RB_IN)-> EnableWindow(bBoth);
   GetDlgItem(IDC_RB_OUT)->EnableWindow(bBoth);
   
   GetDlgItem(IDC_CHK_YEAR)-> EnableWindow(bEnable);
   GetDlgItem(IDC_CHK_MONTH)->EnableWindow(bEnable);
   GetDlgItem(IDC_CHK_DAY)->  EnableWindow(bEnable);
}

/* ******************************************************************** **
** @@ CDateUpdater::PrintHeader()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::PrintHeader(const char* const pszLine)
{
   fprintf(_pReport,"%s\n",pszLine);
   fprintf(_pReport,"File TimeStamp UTC        # File TimeStamp Local      # SOR ORG TimeStamp UTC # SOR ORG TimeStamp Loc # SOR TimeStamp UTC     # SOR TimeStamp Local   # File\n");
   fprintf(_pReport,"---------- + ------------ # ---------- + ------------ # --------- + --------- # ---------- + -------- # ---------- + -------- # ---------- + -------- # ---------\n");
}

/* ******************************************************************** **
** @@ CDateUpdater::PrintFooter()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::PrintFooter(const char* const pszLine)
{
   fprintf(_pReport,"---------- + ------------ # ---------- + ------------ # --------- + --------- # ---------- + -------- # ---------- + -------- # ---------- + -------- # ---------\n");
   fprintf(_pReport,"File TimeStamp UTC        # File TimeStamp Local      # SOR ORG TimeStamp UTC # SOR ORG TimeStamp Loc # SOR TimeStamp UTC     # SOR TimeStamp Local   # File\n");
   fprintf(_pReport,"%s\n",pszLine);
}

/* ******************************************************************** **
** @@ CDateUpdater::Cleanup()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::Cleanup()
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
** @@ CDateUpdater::PopulateFileList()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::PopulateFileList()
{
   Cleanup();

   KRNL_Walker(_pszFolder,WalkerHandler,"*.sor",m_CHK_Recursively);
}

/* ******************************************************************** **
** @@ CDateUpdater::AppendList()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::AppendList()
{
   FILE_LIST_ENTRY*     pEntry = new FILE_LIST_ENTRY;

   if (pEntry)
   {
      memset(pEntry,0,sizeof(FILE_LIST_ENTRY));

//      pEntry->_iNum = ++_iTotal;

      strcpy(pEntry->_pszFile,_MatchedFile._pszFile);

      memcpy(&pEntry->_LastWriteTime,&_MatchedFile._FT_LastWriteTime,sizeof(FILETIME));

      memcpy(&pEntry->_UTC_Time, &_MatchedFile._UTC_Time, sizeof(SYSTEMTIME));
      memcpy(&pEntry->_LocalTime,&_MatchedFile._LocalTime,sizeof(SYSTEMTIME));

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
** @@ CDateUpdater::OnDestroy()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update : 
** @  Notes  : 
** ******************************************************************** */

void CDateUpdater::OnDestroy() 
{
   CDialog::OnDestroy();
   
      Cleanup();
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
