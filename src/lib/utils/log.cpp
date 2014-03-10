// Copyright (c) 2004, Antony C. Roberts

// Use of this file is subject to the terms
// described in the LICENSE.TXT file that
// accompanies this file.
//
// Your use of this file indicates your
// acceptance of the terms described in
// LICENSE.TXT.
//
// http://www.freebt.net

/**
 * The original and principal author is Antony C. Roberts http://www.freebt.net.  
 * Other authors are noted in the change history that follows (in reverse chronological order):
 *
 * 2008-11-30 Dmitry Klionsky Added UNICODE support.
 * 2008-11-09 Dmitry Klionsky Added fbtLogDumpBuf function.
 * 2008-11-08 Dmitry Klionsky Changed extension to cpp to compile as a c++ file.
 */

#define  _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <stdio.h>
#include <time.h>
#include <process.h>
#include <windows.h>

#include "fbtlog.h"
#include "fbtxcpt.h"

TCHAR _fbt_log_file[1024];
unsigned int _fbt_log_level;

static BOOL _bInitialised;
static CRITICAL_SECTION	_criticalSection;

#define BPR 8

void fbtLog(unsigned int nLevel, LPCTSTR szText, ...)
{
   __try
   {
      FILE *fd = NULL;
      va_list valist;
      struct tm* newtime;
      time_t long_time;
      TCHAR* szFile = _fbt_log_file;
      
      if ( nLevel != fbtLog_Exception && _fbt_log_level < nLevel )
         return;

      if ( !_bInitialised )
      {
         InitializeCriticalSection( &_criticalSection );
         _bInitialised = TRUE;
      }

      EnterCriticalSection( &_criticalSection );

      fd = _tfopen(szFile, _T("a+"));
      if (fd != NULL)
      {
         time(&long_time);
         newtime = localtime(&long_time);

         // Use a critical section to stop the various threads from trampling
         // over each other. Note this makes the log file a major contention
         // point and thus slows processing considerably
         _ftprintf(fd, _T("%04d-%02d-%02d %02d.%02d:%02d (%d): "),
         newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday,
         newtime->tm_hour, newtime->tm_min, newtime->tm_sec, GetTickCount());

         va_start(valist, szText);
         _vftprintf(fd, szText, valist);
         va_end(valist);

         fprintf(fd, "\n");
         fflush(fd);
         fclose(fd);
      }

      LeaveCriticalSection(&_criticalSection);
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
      // We can't do anything here!
      LeaveCriticalSection(&_criticalSection);
      return;
   }
}

void fbtLogClear()
{
	FBT_TRY

    DeleteFile(_fbt_log_file);

	FBT_CATCH_NORETURN
}

unsigned int fbtLogGetLevel()
{
	FBT_TRY

    return _fbt_log_level;

	FBT_CATCH_RETURN(0)
}

void fbtLogSetLevel(unsigned int nLevel)
{
	FBT_TRY

    fbtLog(fbtLog_Enter, _T("Entered fbtLogSetLevel (%d)"), nLevel);

    _fbt_log_level=nLevel;

    fbtLog(fbtLog_Exit, _T("Leaving fbtLogSetLevel"));

	FBT_CATCH_NORETURN
}

BOOL fbtLogGetFile(LPTSTR szLogFile, unsigned int nSize)
{
	FBT_TRY

	if (nSize < _tcslen(_fbt_log_file))
    {
        szLogFile[0] = _T('\0');
        fbtLog(fbtLog_Exit, _T("Leaving fbtLogGetFile=FALSE (Buffer too small)"));

        return FALSE;

    }

	memcpy( szLogFile, _fbt_log_file, _tcslen(_fbt_log_file) * sizeof(TCHAR) + 1 );

    return TRUE;

	FBT_CATCH_RETURN(FALSE)
}

BOOL fbtLogSetFile(LPCTSTR szDebugFile)
{
	FBT_TRY

	fbtLog(fbtLog_Enter, _T("Entered fbtLogSetFile(%s)"), szDebugFile);

	if (IsBadStringPtr(szDebugFile, 1024))
    {
        fbtLog(fbtLog_Exit, _T("Leaving fbtLogSetFile=FALSE (Log file too long)"));
        return FALSE;

    }

	memcpy(_fbt_log_file, szDebugFile, _tcslen(szDebugFile) * sizeof(TCHAR) + 1);

    fbtLog(fbtLog_Exit, _T("Leaving fbtLogSetFile=TRUE"));

    return TRUE;

	FBT_CATCH_RETURN(FALSE)
}

void fbtLogDumpBuf(unsigned int nLevel, const unsigned char* lpBuffer, unsigned int cBuffer)
{
   __try
   {
      FILE* fd = NULL;
      TCHAR* szFile =_fbt_log_file;

      if (nLevel != fbtLog_Exception && _fbt_log_level<nLevel)
         return;

      if (!_bInitialised)
      {
         InitializeCriticalSection(&_criticalSection);
         _bInitialised = TRUE;
      }

      EnterCriticalSection(&_criticalSection);

      fd = _tfopen(szFile, _T("a+"));
      if (fd != NULL)
      {
         TCHAR szLine[5 + 7 + 2 + 4 * BPR] = {0};

         for ( int i = 0; i < (int)cBuffer; i += BPR ) 
         {
            int bpr = cBuffer - i;
            if ( bpr > BPR )
               bpr = BPR;

            _stprintf( szLine, _T("%04x "), i);
            TCHAR* p = szLine + _tcslen(szLine);

            int j;
            for ( j = 0 ; j < bpr ; ++j ) 
            {
               TCHAR c = (lpBuffer[i + j] >> 4) & 0xf;
               if ( c > 9 ) c += _T('a') - 10; else c += _T('0');
               *p++ = c;
               c = lpBuffer[i + j] & 0xf;
               if ( c > 9 ) c += _T('a') - 10; else c += _T('0');
               *p++ = c;
               *p++ = _T(' ');
            }

            for ( ; j < BPR ; ++j ) 
            {
               *p++ = _T(' ');
               *p++ = _T(' ');
               *p++ = _T(' ');
            }

            *p++ = _T(' ');
            *p++ = _T(' ');
            *p++ = _T(' ');
            *p++ = _T('|');
            *p++ = _T(' ');
            *p++ = _T(' ');
            *p++ = _T(' ');

            for ( j = 0 ; j < bpr ; ++j ) 
            {
               TCHAR c = lpBuffer[i + j];
               if ( ( c < _T(' ') ) || ( c >= 127 ) )
                  c = _T('.');

               *p++ = c;
            }

            for ( ; j < BPR ; ++j ) 
            {
               *p++ = _T(' ');
            }

            *p++ = _T('\n');
            *p++ = _T('\0');

            _ftprintf( fd, szLine );
         }            

         fflush( fd );
         fclose( fd );
      }

      LeaveCriticalSection( &_criticalSection );
   }

   __except(EXCEPTION_EXECUTE_HANDLER)
   {
      // We can't do anything here!
      LeaveCriticalSection(&_criticalSection);      
   }
}

