/* ******************************************************************** **
** @@ Sorry! SDK
** @  Copyrt : Gazlan * 2014
** @  Author : gazlan@yandex.ru
** @  Modify :
** @  Update :
** @  Notes  : Sorry! - SOR-files Interactive Editor SDK
** ******************************************************************** */

#ifndef _SORRY_SDK_HPP_
#define _SORRY_SDK_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

/* ******************************************************************** **
** @@ internal defines
** ******************************************************************** */

// FileTime type
#define STR_FILETIME_TYPE_SOR_TIMESTAMP               "as SOR Timestamp"
#define STR_FILETIME_TYPE_FILE_TIMESTAMP              "as File Timestamp"
#define STR_FILETIME_TYPE_TIME_NOW                    "as Time Now"

#pragma pack(push,1)
struct SRPL_INFO
{
   WORD        _wVersionMajor;
   WORD        _wVersionMinor;
   BOOL        _bStandalone;
   char*       _pszDescription;
   char*       _pszBlockNameList;
};
#pragma pack(pop)

enum CKSUM_TYPE
{
   HASH16_NONE        = 0,
   HASH16_CCITT_FALSE = 1,
   HASH16_XMODEM      = 2,
   HASH16_JDSU_BUGGY  = 3
};

/* ******************************************************************** **
** @@ internal prototypes
** ******************************************************************** */

typedef  void (* KRNL_WALKER_HANDLER)(char* pszFile);

/* ******************************************************************** **
** @@ external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@ static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@ Global Function Prototypes
** ******************************************************************** */
      
//////////////////////////////////////////////////////////////////////////
// *** EXPORT *** section
// *Any* plagin SHOULD Export these four Entries 
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL  SRPL_Initialize();
extern "C" __declspec(dllexport) void  SRPL_Terminate();
extern "C" __declspec(dllexport) BOOL  SRPL_Run();
extern "C" __declspec(dllexport) void* SRPL_Info();
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// *** IMPORT *** section - Sorry! API
// Plagin can Import these Entries from Sorry! Lib
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CHK - Checksum
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllimport) WORD CHK_GetCkSumOriginal(); 
extern "C" __declspec(dllimport) BOOL CHK_SetCkSum(WORD wCkSum); 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DIR - DiskFolders
//////////////////////////////////////////////////////////////////////////
//extern "C" __declspec(dllimport) void  DIR_EnsureDirectory(char* pszPath);
extern "C" __declspec(dllimport) void  DIR_BrowseForFolder(char* pszTitle,UINT dwFlags,char* pszDir);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// FILE - DiskFiles
//////////////////////////////////////////////////////////////////////////
//bool     ReadBuffer (HANDLE hFile,void* pBuf,DWORD dwBufSize);
//bool     WriteBuffer(HANDLE hFile,const void* const pBuf,DWORD dwBufSize);
//HANDLE   CreateFile(const char* const pszFileName,DWORD dwCreationDisposition = CREATE_ALWAYS,DWORD dwShareMode = FILE_SHARE_READ);
//HANDLE   OpenFileReadWrite(const char* const pszFileName,DWORD dwShareMode = FILE_SHARE_READ);
extern "C" __declspec(dllimport) HANDLE FILE_OpenFileReadOnly(char* pszFileName);
extern "C" __declspec(dllimport) BOOL   FILE_SetFileTimeStampUTC(char* pszFile,FILETIME* pBaseTime,int iDelayMin,int iDelaySec,BOOL bDelayRandom);
//HANDLE   Open_or_CreateFile(const char* const pszFileName);
//DWORD    GetFilePointer(HANDLE hFile);
//DWORD    GetFileSizeLo(HANDLE hFile);
//DWORD    SetFilePointerBOF(HANDLE hFile);
//DWORD    SetFilePointerEOF(HANDLE hFile);
//DWORD    SetFilePointerCUR(HANDLE hFile,int iOfs,DWORD dwMethod);
extern "C" __declspec(dllimport) BOOL FILE_GetLatestFileTimeByHandle(HANDLE hFile, FILETIME* pFileTime);
extern "C" __declspec(dllimport) BOOL FILE_GetLatestFileTimeByName  (char* pszFile,FILETIME* pFileTime);
extern "C" __declspec(dllimport) BOOL FILE_SetFileTSbyHandle(HANDLE hFile,FILETIME* pFileTime);
extern "C" __declspec(dllimport) BOOL FILE_SetFileTSbyName(char* pszFile,FILETIME* pFileTime);

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// FXD - Fixed Parameters Block
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllimport) DWORD FXD_GetSorTimeOriginal();
extern "C" __declspec(dllimport) DWORD FXD_GetSorTime();
extern "C" __declspec(dllimport) BOOL  FXD_SetSorTime(DWORD dwTimeStamp);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// KRNL - Sorry! kernel
//////////////////////////////////////////////////////////////////////////
// Enable/Disable visible pages update
extern "C" __declspec(dllimport) void  KRNL_SetSilenceMode(BOOL bMode);

// Ask where is it placed
extern "C" __declspec(dllimport) char* KRNL_GetPluginPath();

// Load exsisting SOR-file
extern "C" __declspec(dllimport) BOOL  KRNL_Load(char* pszFilename);

// Save on disk with some name, overwrite existing
extern "C" __declspec(dllimport) BOOL  KRNL_Save();

// Save on disk, ask for SOR-file name
extern "C" __declspec(dllimport) BOOL  KRNL_Write();

// Save on disk wuth specified SOR-file name
extern "C" __declspec(dllimport) BOOL  KRNL_WriteFile(char* pszFile);

// Timing
extern "C" __declspec(dllimport) BOOL KRNL_GetFileTime(FILETIME* pFileTime);
extern "C" __declspec(dllimport) BOOL KRNL_SetFileTime(FILETIME* pFileTime);
extern "C" __declspec(dllimport) BOOL KRNL_Time_Local_2_Local(FILETIME* pLocalFrom,FILETIME* pLocalTo,int iTZShiftFrom,int iTZShiftTo);
extern "C" __declspec(dllimport) BOOL KRNL_Time_Local_2_UTC(FILETIME* pLocal,FILETIME* pUTC);
extern "C" __declspec(dllimport) BOOL KRNL_Time_UTC_2_Local(FILETIME* pUTC,FILETIME* pLocal);

// Navigation
extern "C" __declspec(dllimport) void  KRNL_Walker(char* pszRoot,KRNL_WALKER_HANDLER pHandler,char* pszMask,BOOL bRecursive);

// Block Names List
extern "C" __declspec(dllimport) void KRNL_ClearBlackList();
extern "C" __declspec(dllimport) void KRNL_ClearWhiteList();
extern "C" __declspec(dllimport) BOOL KRNL_AppendBlackList(char* pszName);
extern "C" __declspec(dllimport) BOOL KRNL_AppendWhiteList(char* pszName);
extern "C" __declspec(dllimport) void KRNL_StripBlackList();
extern "C" __declspec(dllimport) void KRNL_EnsureWhiteList();
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OPT - Options
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllimport) int   OPT_GetTimeZoneShift();
extern "C" __declspec(dllimport) BOOL  OPT_SetTimeZoneShift(int iTimeZoneShift);
extern "C" __declspec(dllimport) BOOL  OPT_SetFixedDelayMin(int iMin);
extern "C" __declspec(dllimport) BOOL  OPT_SetFixedDelaySec(int iSec);
extern "C" __declspec(dllimport) BOOL  OPT_SetFixedDelayRandom(bool bSet);
extern "C" __declspec(dllimport) BOOL  OPT_SetFixedTSstrType(char* pszType);

//////////////////////////////////////////////////////////////////////////
// DB - DataBase
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllimport) BOOL  DB_SaveSession(char* pszOwnerGUID,DWORD dwSessionID,void* pInfo,DWORD dwSize,char* pszPassword);
extern "C" __declspec(dllimport) void* DB_LoadSession(char* pszOwnerGUID,DWORD dwSessionID,DWORD* pSize,char* pszPassword);
extern "C" __declspec(dllimport) void  DB_KillSession(char* pszOwnerGUID,DWORD dwSessionID);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CRYPT - Cryptoservice
//////////////////////////////////////////////////////////////////////////
// Encryption
//extern "C" __declspec(dllimport) void* CRYPT_EncryptBlock(void* pPlain, DWORD dwSize,char* pswPassword,BOOL bPack);
//extern "C" __declspec(dllimport) void* CRYPT_DecryptBlock(void* pCipher,DWORD dwSize,char* pswPassword);
// Compression
//extern "C" __declspec(dllimport) DWORD CRYPT_CompressBlock  (void* pOriginal,void* pPacked,DWORD dwSizeORG);
//extern "C" __declspec(dllimport) DWORd CRYPT_DecompressBlock(void* pOriginal,void* pPacked,DWORD dwSizePack);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MAP - HeaderInfoBlock
//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllimport) WORD  MAP_CalcHash16(CKSUM_TYPE Type); 
extern "C" __declspec(dllimport) WORD  MAP_CalcHash16File(char* pszFile,CKSUM_TYPE Type); 
extern "C" __declspec(dllimport) char* MAP_GetFileMD5SignatureOriginal(); 
//////////////////////////////////////////////////////////////////////////

#endif

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
