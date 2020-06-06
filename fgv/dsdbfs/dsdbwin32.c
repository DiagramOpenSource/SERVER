
#ifdef WIN32

/*#define _WIN32_WINNT 0x0400 */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/locking.h>
#include <share.h>
#include <io.h>
#include <windows.h>
#define sleep(a) Sleep((long)(((long)a)*1000L));

#ifndef LK_UNLCK
#define LK_UNLCK _LK_UNLCK
#endif

#ifndef LK_LOCK
#define LK_LOCK  _LK_LOCK
#endif

#ifndef LK_NBLCK
#define LK_NBLCK _LK_NBLCK
#endif

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include "dsdbfs.h"


#define FOPEN           0x01    /* file handle open */
#define FEOFLAG         0x02    /* end of file has been encountered */
#define FCRLF           0x04    /* CR-LF across read buffer (in text mode) */
#define FPIPE           0x08    /* file handle refers to a pipe */
#define FNOINHERIT      0x10    /* file handle opened _O_NOINHERIT */
#define FAPPEND         0x20    /* file handle opened O_APPEND */
#define FDEV            0x40    /* file handle refers to device */
#define FTEXT           0x80    /* file handle is in text mode */

#define FDSDBCACHED     0x100


#define LF '\n'      /* line feed */
#define CR '\r'      /* carriage return */
#define CTRLZ 26     /* ctrl-z */

#define BUF_SIZE    1025    /* size of LF translation buffer */


static int nwin32handles = 0;
static int win32libre = -1;
static struct
{
	HANDLE handle;
	int win32flags;
	__int64 posicion_local;
	int pwin32libre;	
/*
	int nbloqueos;
	__int64 bloqueo[MAXDSDBBLOQUEOS];
	__int64 lenbloqueo[MAXDSDBBLOQUEOS];
*/
} win32handles[MAX_OPEN_FILES]; /* IXXMAXFILES * 2, por si se no se usa el fichero unico */

static int nhandlesopen = 0;

static int _umaskval = 0;

/* This is the error table that defines the mapping between OS error
   codes and errno values */
static HANDLE evento = NULL;

extern char *dsdb_serverip;

struct errentry {
        unsigned long oscode;           /* OS return value */
        int errnocode;  /* System V error code */
};

__int64 dsdbcache_size(int fh);

static struct errentry errtable[] = {
        {  ERROR_INVALID_FUNCTION,       EINVAL    },  /* 1 */
        {  ERROR_FILE_NOT_FOUND,         ENOENT    },  /* 2 */
        {  ERROR_PATH_NOT_FOUND,         ENOENT    },  /* 3 */
        {  ERROR_TOO_MANY_OPEN_FILES,    EMFILE    },  /* 4 */
        {  ERROR_ACCESS_DENIED,          EACCES    },  /* 5 */
        {  ERROR_INVALID_HANDLE,         EBADF     },  /* 6 */
        {  ERROR_ARENA_TRASHED,          ENOMEM    },  /* 7 */
        {  ERROR_NOT_ENOUGH_MEMORY,      ENOMEM    },  /* 8 */
        {  ERROR_INVALID_BLOCK,          ENOMEM    },  /* 9 */
        {  ERROR_BAD_ENVIRONMENT,        E2BIG     },  /* 10 */
        {  ERROR_BAD_FORMAT,             ENOEXEC   },  /* 11 */
        {  ERROR_INVALID_ACCESS,         EINVAL    },  /* 12 */
        {  ERROR_INVALID_DATA,           EINVAL    },  /* 13 */
        {  ERROR_INVALID_DRIVE,          ENOENT    },  /* 15 */
        {  ERROR_CURRENT_DIRECTORY,      EACCES    },  /* 16 */
        {  ERROR_NOT_SAME_DEVICE,        EXDEV     },  /* 17 */
        {  ERROR_NO_MORE_FILES,          ENOENT    },  /* 18 */
        {  ERROR_LOCK_VIOLATION,         EACCES    },  /* 33 */
        {  ERROR_BAD_NETPATH,            ENOENT    },  /* 53 */
        {  ERROR_NETWORK_ACCESS_DENIED,  EACCES    },  /* 65 */
        {  ERROR_BAD_NET_NAME,           ENOENT    },  /* 67 */
        {  ERROR_FILE_EXISTS,            EEXIST    },  /* 80 */
        {  ERROR_CANNOT_MAKE,            EACCES    },  /* 82 */
        {  ERROR_FAIL_I24,               EACCES    },  /* 83 */
        {  ERROR_INVALID_PARAMETER,      EINVAL    },  /* 87 */
        {  ERROR_NO_PROC_SLOTS,          EAGAIN    },  /* 89 */
        {  ERROR_DRIVE_LOCKED,           EACCES    },  /* 108 */
        {  ERROR_BROKEN_PIPE,            EPIPE     },  /* 109 */
        {  ERROR_DISK_FULL,              ENOSPC    },  /* 112 */
        {  ERROR_INVALID_TARGET_HANDLE,  EBADF     },  /* 114 */
        {  ERROR_INVALID_HANDLE,         EINVAL    },  /* 124 */
        {  ERROR_WAIT_NO_CHILDREN,       ECHILD    },  /* 128 */
        {  ERROR_CHILD_NOT_COMPLETE,     ECHILD    },  /* 129 */
        {  ERROR_DIRECT_ACCESS_HANDLE,   EBADF     },  /* 130 */
        {  ERROR_NEGATIVE_SEEK,          EINVAL    },  /* 131 */
        {  ERROR_SEEK_ON_DEVICE,         EACCES    },  /* 132 */
        {  ERROR_DIR_NOT_EMPTY,          ENOTEMPTY },  /* 145 */
        {  ERROR_NOT_LOCKED,             EACCES    },  /* 158 */
        {  ERROR_BAD_PATHNAME,           ENOENT    },  /* 161 */
        {  ERROR_MAX_THRDS_REACHED,      EAGAIN    },  /* 164 */
        {  ERROR_LOCK_FAILED,            EACCES    },  /* 167 */
        {  ERROR_ALREADY_EXISTS,         EEXIST    },  /* 183 */
        {  ERROR_FILENAME_EXCED_RANGE,   ENOENT    },  /* 206 */
        {  ERROR_NESTING_NOT_ALLOWED,    EAGAIN    },  /* 215 */
        {  ERROR_NOT_ENOUGH_QUOTA,       ENOMEM    }    /* 1816 */
};

/* size of the table */
#define ERRTABLESIZE (sizeof(errtable)/sizeof(errtable[0]))

/* The following two constants must be the minimum and maximum
   values in the (contiguous) range of Exec Failure errors. */
#define MIN_EXEC_ERROR ERROR_INVALID_STARTING_CODESEG
#define MAX_EXEC_ERROR ERROR_INFLOOP_IN_RELOC_CHAIN

/* These are the low and high value in the range of errors that are
   access violations */
#define MIN_EACCES_RANGE ERROR_WRITE_PROTECT
#define MAX_EACCES_RANGE ERROR_SHARING_BUFFER_EXCEEDED


#include "psapi.h"


BOOL GetFileNameFromHandle(int fh,char *pszFilename) 
{
  BOOL bSuccess = FALSE;
  HANDLE hFileMap;


  // Get the file size.
  DWORD dwFileSizeHi = 0;
  DWORD dwFileSizeLo; 
  HANDLE hFile = NULL;


    if ( ((unsigned)fh >= (unsigned)nwin32handles) ||
         !(win32handles[fh].win32flags & FOPEN) )
    {
            /* bad file handle, set errno and abort */
            errno = EBADF;
            _doserrno = 0;
            return FALSE;
	}

   hFile = win32handles[fh].handle;

   dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

  if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
  {
     printf("Cannot map a file with a length of zero.\n");
     return FALSE;
  }

  // Create a file mapping object.
  hFileMap = CreateFileMapping(hFile, 
                    NULL, 
                    PAGE_READONLY,
                    0, 
                    1,
                    NULL);

  if (hFileMap) 
  {
    // Create a file mapping to get the file name.
    void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

    if (pMem) 
    {
      if (GetMappedFileName (GetCurrentProcess(), 
                             pMem, 
                             pszFilename,
                             MAX_PATH)) 
      {
		  bSuccess = TRUE;
      }
      
      UnmapViewOfFile(pMem);
    } 

    CloseHandle(hFileMap);
  }  
  return(bSuccess);
}



static void _dosmaperr (
        unsigned long oserrno
        )
{
        int i;

        _doserrno = oserrno;        /* set _doserrno */

        /* check the table for the OS error code */
        for (i = 0; i < ERRTABLESIZE; ++i) 
		{
                if (oserrno == errtable[i].oscode) 
				{
                        errno = errtable[i].errnocode;
                        return;
                }
        }

        /* The error code wasn't in the table.  We check for a range of */
        /* EACCES errors or exec failure errors (ENOEXEC).  Otherwise   */
        /* EINVAL is returned.                                          */

        if (oserrno >= MIN_EACCES_RANGE && oserrno <= MAX_EACCES_RANGE)
                errno = EACCES;
        else if (oserrno >= MIN_EXEC_ERROR && oserrno <= MAX_EXEC_ERROR)
                errno = ENOEXEC;
        else
                errno = EINVAL;
}

/*
static void _suma_bloqueo(int fd,ds_off pos,ds_off nbytes,int modo)
{
	int i,n;
	if (!modo)
	{
		n = win32handles[fd].nbloqueos;
		if (n > 0)
		{
			for (i = 0;i < n;i++)
			{
				if (win32handles[fd].bloqueo[i] == pos && win32handles[fd].lenbloqueo[i] == nbytes)
				{
					win32handles[fd].nbloqueos--;
					n--;
					for (;i < n;i++)
					{
						win32handles[fd].bloqueo[i] = win32handles[fd].bloqueo[i+1];
						win32handles[fd].lenbloqueo[i] = win32handles[fd].lenbloqueo[i+1];
					}
					return;
				}
			}
		}
	}
	else
	{
		i = win32handles[fd].nbloqueos;
		if (i >= MAXDSDBBLOQUEOS)
		{
			return;
		}
		win32handles[fd].bloqueo[i] = pos;
		win32handles[fd].lenbloqueo[i] = nbytes;
		win32handles[fd].nbloqueos++;
	}
}
*/

__int64 dsdb_lseek(int fh, __int64 distance, DWORD MoveMethod)
{
   LARGE_INTEGER li;
   HANDLE hf;

    if ( ((unsigned)fh >= (unsigned)nwin32handles) ||
         !(win32handles[fh].win32flags & FOPEN) )
    {
            /* bad file handle, set errno and abort */
            errno = EBADF;
            _doserrno = 0;
            return -1;    
	}

	if (MoveMethod == SEEK_CUR && !distance)
		return win32handles[fh].posicion_local;

    hf = win32handles[fh].handle;  
   

   if ((win32handles[fh].win32flags & FDSDBCACHED))
   {	     
		 switch(MoveMethod)
		 {
		 case SEEK_SET:
			win32handles[fh].posicion_local = distance;
			break;
		 case SEEK_END:
			 win32handles[fh].posicion_local = dsdbcache_size((int)hf) + distance;
			 break;
		 case SEEK_CUR:
			 win32handles[fh].posicion_local += distance;	     
			 break;
		 }		
		 return win32handles[fh].posicion_local;   
   }

   li.QuadPart = distance;

   li.LowPart = SetFilePointer (hf, li.LowPart, &li.HighPart, MoveMethod);

   if (li.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
   {
	  _dosmaperr(GetLastError());     /* map error */
      li.QuadPart = -1;
   }
   else {
	   win32handles[fh].posicion_local = li.QuadPart;
	   errno = 0;
   }

   return li.QuadPart;
}


int dsdb_lee(
        int fh,
        void *buf,
        unsigned cnt
        )
{
        HANDLE hf;
		int fileflags;
        int bytes_read;                 /* number of bytes read */
        char *buffer;                   /* buffer to read to */
        int os_read;                    /* bytes read on OS call */
        char *p, *q;                    /* pointers into buffer */
        char peekchr;                   /* peek-ahead character */
        __int64 filepos;                  /* file position after seek */
        ULONG dosretval;                /* o.s. return value */
		//DWORD test;

    if ( ((unsigned)fh >= (unsigned)nwin32handles) ||
         !(win32handles[fh].win32flags & FOPEN) )
    {
            /* bad file handle, set errno and abort */
            errno = EBADF;
            _doserrno = 0;
            return -1;    
	}
        hf = win32handles[fh].handle;
		fileflags = win32handles[fh].win32flags;

        bytes_read = 0;                 /* nothing read yet */
        buffer = buf;

        if (cnt == 0 || (fileflags & FEOFLAG)) 
		{
            /* nothing to read or at EOF, so return 0 read */
            return 0;
        }

		if ((fileflags & FDSDBCACHED))
		{
			return dsdbcache_lee(win32handles[fh].handle,buf,&(win32handles[fh].posicion_local),cnt);
		}

        /* read the data */
		//test = GetTickCount();		
        if ( !ReadFile( hf, buffer, cnt, (LPDWORD)&os_read,
                        NULL ) )
        {
            /* ReadFile has reported an error. recognize two special cases.
             *
             *      1. map ERROR_ACCESS_DENIED to EBADF
             *
             *      2. just return 0 if ERROR_BROKEN_PIPE has occurred. it
             *         means the handle is a read-handle on a pipe for which
             *         all write-handles have been closed and all data has been
             *         read. */

            if ( (dosretval = GetLastError()) == ERROR_ACCESS_DENIED ) {
                /* wrong read/write mode should return EBADF, not EACCES */
                errno = EBADF;
                _doserrno = dosretval;
                return -1;
            }
            else if ( dosretval == ERROR_BROKEN_PIPE ) {
                return 0;
            }
            else {
                _dosmaperr(dosretval);
                return -1;
            }
        }
		/*
		test = GetTickCount() - test;
		if (test > 100)
		{
			test = 0;
		}
		*/

		errno = 0;

		win32handles[fh].posicion_local += os_read;

        bytes_read += os_read;          /* update bytes read */

        if (fileflags & FTEXT) 
		{
            /* now must translate CR-LFs to LFs in the buffer */

            /* set CRLF flag to indicate LF at beginning of buffer */
            if ( (os_read != 0) && (*(char *)buf == LF) )
                fileflags |= FCRLF;
            else
                fileflags &= ~FCRLF;

            /* convert chars in the buffer: p is src, q is dest */
            p = q = buf;
            while (p < (char *)buf + bytes_read) 
			{
                if (*p == CTRLZ) 
				{
                    /* if fh is not a device, set ctrl-z flag */
                    if ( !(fileflags & FDEV) )
                        fileflags |= FEOFLAG;
                    break;              /* stop translating */
                }
                else if (*p != CR)
                    *q++ = *p++;
                else {
                    /* *p is CR, so must check next char for LF */
                    if (p < (char *)buf + bytes_read - 1) {
                        if (*(p+1) == LF) {
                            p += 2;
                            *q++ = LF;  /* convert CR-LF to LF */
                        }
                        else
                            *q++ = *p++;    /* store char normally */
                    }
                    else {
                        /* This is the hard part.  We found a CR at end of
                           buffer.  We must peek ahead to see if next char
                           is an LF. */
                        ++p;

                        dosretval = 0;
                        if ( !ReadFile( hf, &peekchr, 1,
                                        (LPDWORD)&os_read, NULL ) )
                            dosretval = GetLastError();

                        if (dosretval != 0 || os_read == 0) {
                            /* couldn't read ahead, store CR */
                            *q++ = CR;
                        }
                        else 
						{
                            /* peekchr now has the extra character -- we now
                               have several possibilities:
                               1. disk file and char is not LF; just seek back
                                  and copy CR
                               2. disk file and char is LF; seek back and
                                  discard CR
                               3. disk file, char is LF but this is a one-byte
                                  read: store LF, don't seek back
                               4. pipe/device and char is LF; store LF.
                               5. pipe/device and char isn't LF, store CR and
                                  put char in pipe lookahead buffer. */
                            {
                                /* disk file */
                                if (q == buf && peekchr == LF) {
                                    /* nothing read yet; must make some
                                       progress */
                                    *q++ = LF;
                                }
                                else
								{
                                    /* seek back */
                                    filepos = dsdb_lseek(fh, -1, FILE_CURRENT);
                                    if (peekchr != LF)
                                        *q++ = CR;
                                }
                            }
                        }
                    }
                }
            }

            /* we now change bytes_read to reflect the true number of chars
               in the buffer */
            bytes_read = q - (char *)buf;
        }

        return bytes_read;              /* and return */
}

int dsdb_cierra(
        int fh
        )
{
		DWORD dosretval;

        /* validate file handle */
        if ( ((unsigned)fh >= (unsigned)nwin32handles) ||
             !(win32handles[fh].win32flags & FOPEN) )
        {
                /* bad file handle, set errno and abort */
                errno = EBADF;
                _doserrno = 0;
                return -1;    
		}

	   if ((win32handles[fh].win32flags & FDSDBCACHED))
	   {
		   /*
		   if (win32handles[fh].nbloqueos > 0)
		   {
			   dosretval = 0L;
		   }
		   */
		   dsdbcache_cierra(win32handles[fh].handle);
		   dosretval = 0L;
	   }
	   else
	   {
			if ( win32handles[fh].handle == INVALID_HANDLE_VALUE ||
				 CloseHandle( win32handles[fh].handle ))
			{

					dosretval = 0L;
			}
			else
					dosretval = GetLastError();
	   }
        

		win32handles[fh].pwin32libre = win32libre;            						
		win32libre = fh;
		win32handles[fh].handle = INVALID_HANDLE_VALUE;
		win32handles[fh].win32flags = 0;
		win32handles[fh].posicion_local = 0;

		nhandlesopen--;
		if (nhandlesopen <= 0)
		{
			nhandlesopen = 0;
			if (evento)
			{
				CloseHandle(evento);
				evento = NULL;
			}
		}

        if (dosretval) 
		{
                /* OS error */
                _dosmaperr(dosretval);
                return -1;
        }
		else
			errno = 0;
        return 0;                       /* good return */
}

int dsdb_sabre(
        char *path,
        int oflag,
        int shflag,
        ...
        )
{
	    va_list ap;                     /* variable argument (pmode) */
        int fh;                         /* handle of opened file */
        __int64 filepos;                    /* length of file - 1 */
        char ch;                      /* character at end of file */
        int fileflags;                 /* _osfile flags */        
        int pmode;
        HANDLE osfh;                    /* OS handle of opened file */
        DWORD fileaccess;               /* OS file access (requested) */
        DWORD fileshare;                /* OS file sharing mode */
        DWORD filecreate;               /* OS method of opening/creating */
        DWORD fileattrib;               /* OS file attribute flags */
        DWORD isdev;                    /* device indicator in low byte */
        SECURITY_ATTRIBUTES SecurityAttributes;

		memset(&SecurityAttributes,0,sizeof(SecurityAttributes));
        SecurityAttributes.nLength = sizeof( SecurityAttributes );
        SecurityAttributes.lpSecurityDescriptor = NULL;

        if (oflag & _O_NOINHERIT) 
		{
            SecurityAttributes.bInheritHandle = FALSE;
            fileflags = FNOINHERIT;
        }
        else 
		{
            SecurityAttributes.bInheritHandle = TRUE;
            fileflags = 0;
        }

        /* figure out binary/text mode */
        if ((oflag & _O_BINARY) == 0)
            if (oflag & _O_TEXT)
                fileflags |= FTEXT;
            else if (_fmode != _O_BINARY)   /* check default mode */
                fileflags |= FTEXT;

        /*
         * decode the access flags
         */
        switch( oflag & (_O_RDONLY | _O_WRONLY | _O_RDWR) ) 
		{
            case _O_RDONLY:         /* read access */
                    fileaccess = GENERIC_READ;
                    break;
            case _O_WRONLY:         /* write access */
                    fileaccess = GENERIC_WRITE;
                    break;
            case _O_RDWR:           /* read and write access */
                    fileaccess = GENERIC_READ | GENERIC_WRITE;
                    break;
            default:                /* error, bad oflag */
                    errno = EINVAL;
                    _doserrno = 0L; /* not an OS error */
                    return -1;
        }

        /*
         * decode sharing flags
         */
        switch ( shflag ) 
		{

            case _SH_DENYRW:        /* exclusive access */
                fileshare = 0L;
                break;

            case _SH_DENYWR:        /* share read access */
                fileshare = FILE_SHARE_READ;
                break;

            case _SH_DENYRD:        /* share write access */
                fileshare = FILE_SHARE_WRITE;
                break;

            case _SH_DENYNO:        /* share read and write access */
                fileshare = FILE_SHARE_READ | FILE_SHARE_WRITE;
                break;

            default:                /* error, bad shflag */
                errno = EINVAL;
                _doserrno = 0L; /* not an OS error */
                return -1;
        }

        /*
         * decode open/create method flags
         */
        switch ( oflag & (_O_CREAT | _O_EXCL | _O_TRUNC) ) 
		{
            case 0:
            case _O_EXCL:                   // ignore EXCL w/o CREAT
                filecreate = OPEN_EXISTING;
                break;

            case _O_CREAT:
                filecreate = OPEN_ALWAYS;
                break;

            case _O_CREAT | _O_EXCL:
            case _O_CREAT | _O_TRUNC | _O_EXCL:
                filecreate = CREATE_NEW;
                break;

            case _O_TRUNC:
            case _O_TRUNC | _O_EXCL:        // ignore EXCL w/o CREAT
                filecreate = TRUNCATE_EXISTING;
                break;

            case _O_CREAT | _O_TRUNC:
                filecreate = CREATE_ALWAYS;
                break;

            default:
                // this can't happen ... all cases are covered
                errno = EINVAL;
                _doserrno = 0L;
                return -1;
        }

        /*
         * decode file attribute flags if _O_CREAT was specified
         */
        fileattrib = FILE_ATTRIBUTE_NORMAL;     /* default */

        if ( oflag & _O_CREAT ) 
		{
                /*
                 * set up variable argument list stuff
                 */
                va_start(ap, shflag);
                pmode = va_arg(ap, int);
                va_end(ap);

                if ( !((pmode & ~_umaskval) & _S_IWRITE) )
                        fileattrib = FILE_ATTRIBUTE_READONLY;
        }

        /*
         * Set temporary file (delete-on-close) attribute if requested.
         */
        if ( oflag & _O_TEMPORARY ) 
		{
            fileattrib |= FILE_FLAG_DELETE_ON_CLOSE;
            fileaccess |= DELETE;
        }

        /*
         * Set temporary file (delay-flush-to-disk) attribute if requested.
         */
        if ( oflag & _O_SHORT_LIVED )
            fileattrib |= FILE_ATTRIBUTE_TEMPORARY;

        /*
         * Set sequential or random access attribute if requested.
         */
        if ( oflag & _O_SEQUENTIAL )
            fileattrib |= FILE_FLAG_SEQUENTIAL_SCAN;
        else if ( oflag & _O_RANDOM )
            fileattrib |= FILE_FLAG_RANDOM_ACCESS;

		if (win32libre > -1)
		{
			fh = win32libre;
			win32libre = win32handles[fh].pwin32libre;
			win32handles[fh].pwin32libre = -1;
		}	
		else
		{
			if (nwin32handles >= MAX_OPEN_FILES)
			{
				errno = EMFILE;         /* too many open files */
				_doserrno = 0L;         /* not an OS error */
				return -1;              /* return error to caller */
			}
			fh = nwin32handles;
			win32handles[fh].pwin32libre = -1;
			win32handles[fh].handle = INVALID_HANDLE_VALUE;
			win32handles[fh].win32flags = 0;			
			nwin32handles++;
		}	



		if (dsdb_serverip && strstr(path,".dsb"))			
		{
			if ((osfh = dsdbcache_abre(path,oflag,shflag,pmode)) != (HANDLE)0xffffffff)
			{
				fileflags |= FDSDBCACHED;
			}
			else
			{
				errno = ENOENT;
				win32handles[fh].pwin32libre = win32libre;            
				win32libre = fh;
				return -1;                      /* return error to caller */
			}
		}
		else
		{
			/*
			 * try to open/create the file
			 */
			if ( (osfh = CreateFile( (LPTSTR)path,
									 fileaccess,
									 fileshare,
									 &SecurityAttributes,
									 filecreate,
									 fileattrib,
									 NULL ))
				 == (HANDLE)0xffffffff )
			{
				/*
				 * OS call to open/create file failed! map the error, release
				 * the lock, and return -1. note that it's not necessary to
				 * call _free_osfhnd (it hasn't been used yet).
				 */
				_dosmaperr(GetLastError());     /* map error */
				win32handles[fh].pwin32libre = win32libre;            
				win32libre = fh;
				return -1;                      /* return error to caller */
			}

			/* find out what type of file (file/device/pipe) */
			if ( (isdev = GetFileType(osfh)) == FILE_TYPE_UNKNOWN ) 
			{
				CloseHandle(osfh);
				_dosmaperr(GetLastError());     /* map error */
				win32handles[fh].pwin32libre = win32libre;            						
				win32libre = fh;
				return -1;
			}

			/* no lo soportaremos ... es para tratar un fichero unico o ficheros
			   que podrian ir en el fichero unico, nunca pipes o dispositivos */
			/* is isdev value to set flags */
			if (isdev == FILE_TYPE_CHAR || isdev == FILE_TYPE_PIPE)
			{
				errno = EINVAL;         
				_doserrno = 0L;         /* not an OS error */
				win32handles[fh].pwin32libre = win32libre;            						
				win32libre = fh;
				return -1;
			}
			/*
			if (isdev == FILE_TYPE_CHAR)
				fileflags |= FDEV;
			else if (isdev == FILE_TYPE_PIPE)
				fileflags |= FPIPE;
			*/
		}

		errno = 0;

  	    win32handles[fh].handle = osfh;            		

        /*
         * mark the handle as open. store flags gathered so far in _osfile
         * array.
         */
        fileflags |= FOPEN;
		win32handles[fh].win32flags = fileflags;
		win32handles[fh].posicion_local = 0;
		/*win32handles[fh].nbloqueos = 0;*/
		nhandlesopen++;


        if ( !(fileflags & (FDEV|FPIPE|FDSDBCACHED)) && (fileflags & FTEXT) &&
             (oflag & _O_RDWR) )
        {
            /* We have a text mode file.  If it ends in CTRL-Z, we wish to
               remove the CTRL-Z character, so that appending will work.
               We do this by seeking to the end of file, reading the last
               byte, and shortening the file if it is a CTRL-Z. */

            if ((filepos = dsdb_lseek(fh, -1, SEEK_END)) == (__int64)-1) 
			{
                /* OS error -- should ignore negative seek error,
                   since that means we had a zero-length file. */
                if (_doserrno != ERROR_NEGATIVE_SEEK) 
				{
                    dsdb_cierra(fh);                    
                    return -1;
                }
            }
            else 
			{
                /* Seek was OK, read the last char in file. The last
                   char is a CTRL-Z if and only if _read returns 0
                   and ch ends up with a CTRL-Z. */
                ch = 0;
                if (dsdb_lee(fh, &ch, 1) == 0 && ch == 26) 
				{
                    /* read was OK and we got CTRL-Z! Wipe it
                       out! */
					dsdb_lseek(fh, filepos, SEEK_SET);
                    if (!SetEndOfFile(osfh))
                    {
                        dsdb_cierra(fh);
                        return -1;
                    }
                }

                /* now rewind the file to the beginning */
                if ((filepos = dsdb_lseek(fh, 0, SEEK_SET)) == (__int64)-1) 
				{
                    dsdb_cierra(fh);                    
                    return -1;
                }
            }
        }

        /*
         * Set FAPPEND flag if appropriate. Don't do this for devices or pipes.
         */
        if ( !(fileflags & (FDEV|FPIPE)) && (oflag & _O_APPEND) )
            win32handles[fh].win32flags |= FAPPEND;

        
        return fh;                      /* return handle */
}


int dsdb_abre(
        char *path,
        int oflag,
        ...
        )
{
        va_list ap;
        int pmode;

        va_start(ap, oflag);
        pmode = va_arg(ap, int);
        va_end(ap);
        
        return dsdb_sabre(path, oflag, _SH_DENYNO, pmode);
}



int dsdb_locking64(
        int fh,
        int lmode,
        ds_off nbytes
        )
{
        ULONG dosretval;                /* o.s. return code */        
		LARGE_INTEGER li;
		LARGE_INTEGER lo;
        int retry;                      /* retry count */
		HANDLE hf;		
		OVERLAPPED ov;
		DWORD flags;
		DWORD rev = !WAIT_OBJECT_0;				
		//DWORD test;		

		if ( ((unsigned)fh >= (unsigned)nwin32handles) ||
			 !(win32handles[fh].win32flags & FOPEN) )
		{
				/* bad file handle, set errno and abort */
				errno = EBADF;
				_doserrno = 0;
				return -1;    
		}
        hf = win32handles[fh].handle;		

		
		if ((win32handles[fh].win32flags & FDSDBCACHED))
		{			
			int r;
			r = dsdbcache_locking64(hf,lmode,win32handles[fh].posicion_local,nbytes);
			/*
			if (r >= 0)
			{
				_suma_bloqueo(fh,win32handles[fh].posicion_local,nbytes,lmode);
			}
			*/
			return r;
		}

        li.QuadPart = dsdb_lseek(fh, 0L, 1);
        if (li.QuadPart == (__int64)-1) 
		{                
              return -1;
        }


        /* set retry count based on mode */
        if (lmode == _LK_LOCK || lmode == _LK_RLCK)
                retry = 10;              /* retry 10 times */
        else
                retry = 0;              /* don't retry */


		if (!evento)
		{
			evento = CreateEvent(NULL,TRUE,FALSE,"XXXDSDBXXXXDSDBXXXXX");
		}
		for(;;)
		{
			/* ask o.s. to lock the file until success or retry count finished */
			/* note that the only error possible is a locking violation, since */
			/* an invalid handle would have already failed above */
			memset((void *)&ov,0,sizeof(ov));
			dosretval = 0;
			if (lmode == 9999)
			{
				flags = 0;
			}
			else
			{
				flags = LOCKFILE_EXCLUSIVE_LOCK;				
			}
			ov.Offset = li.LowPart;
			ov.OffsetHigh = li.HighPart;
			ov.hEvent = 0;
			flags |= LOCKFILE_FAIL_IMMEDIATELY;

			lo.QuadPart = nbytes;

			if (lmode == _LK_UNLCK) 
			{
				/*
				if ( !(UnlockFileEx(hf,                            
								  0L,
								  lo.LowPart,
								  lo.HighPart,&ov))
				   )
					dosretval = GetLastError();
				*/
				if ( !(UnlockFile(hf,ov.Offset,ov.OffsetHigh,
								  lo.LowPart,
								  lo.HighPart))
				   )
					dosretval = GetLastError();

			} else {
				//test = GetTickCount();
				if (lmode == 9999)
				{
					if ( !(LockFileEx(hf,
									flags, 
									0L,
									lo.LowPart,
									lo.HighPart,&ov))
					   )
						dosretval = GetLastError();
				}
				else
				{
					if ( !(LockFile(hf,									
									ov.Offset,ov.OffsetHigh,
									lo.LowPart,
									lo.HighPart))
					   )
						dosretval = GetLastError();
				}
			
				/*
				test = GetTickCount() - test;
				if (test > 100)
				{
					test = 0;
				}
				*/
			}
			if (lmode == 9999)
			{
				UnlockFileEx(hf,0L,lo.LowPart,lo.HighPart,&ov);				   
			}
			if (!retry || !dosretval)
				break;		
			if (rev == WAIT_OBJECT_0)
			{
				ResetEvent(evento);
			}
			if (!evento || (rev = WaitForSingleObject(evento,10)) == WAIT_FAILED)
			{
				Sleep(10);
			}			
			retry--;
		}	

		if (lmode == _LK_UNLCK && dosretval == 0 && evento)
		{
			SetEvent(evento);
		}


        if (dosretval != 0) 
		{
                /* o.s. error occurred -- file was already locked; if a
                   blocking call, then return EDEADLOCK, otherwise map
                   error normally */
                if (lmode == _LK_LOCK || lmode == _LK_RLCK) 
				{
                        errno = EDEADLOCK;
                        _doserrno = dosretval;
                }
                else {
                        _dosmaperr(dosretval);
                }
                return -1;
        }
        else {
			    errno = 0;
                return 0;
		}
}

int dsdb_locking(
        int fh,
        int lmode,
        long nbytes
        )
{
	return dsdb_locking64(fh,lmode,(ds_off)nbytes);
}


int dsdb_graba(
        int fh,
        void *buf,
        unsigned cnt
        )
{
	    HANDLE hf;
		int fileflags;
        int lfcount;            /* count of line feeds */
        int charcount;          /* count of chars written so far */
        int written;            /* count of chars written on this write */
        ULONG dosretval;        /* o.s. return value */
        char ch;                /* current character */
        char *p, *q;            /* pointers into buf and lfbuf resp. */
        char lfbuf[BUF_SIZE];   /* lf translation buffer */


    if ( ((unsigned)fh >= (unsigned)nwin32handles) ||
         !(win32handles[fh].win32flags & FOPEN) )
    {
            /* bad file handle, set errno and abort */
            errno = EBADF;
            _doserrno = 0;
            return -1;    
	}

        hf = win32handles[fh].handle;
		fileflags = win32handles[fh].win32flags;

		if ((fileflags & FDSDBCACHED))
		{
			return dsdbcache_graba(hf,buf,&(win32handles[fh].posicion_local),cnt);
        }


        lfcount = charcount = 0;        /* nothing written yet */

        if (cnt == 0)
                return 0;               /* nothing to do */


        if (fileflags & FAPPEND)
		{
                /* appending - seek to end of file; ignore error, because maybe
                   file doesn't allow seeking */
                (void)dsdb_lseek(fh, 0, FILE_END);
        }

        /* check for text mode with LF's in the buffer */

        if ( (fileflags & FTEXT) ) 
		{
                /* text mode, translate LF's to CR/LF's on output */

                p = (char *)buf;        /* start at beginning of buffer */
                dosretval = 0;          /* no OS error yet */

                while ( (unsigned)(p - (char *)buf) < cnt ) {
                        q = lfbuf;      /* start at beginning of lfbuf */

                        /* fill the lf buf, except maybe last char */
                        while ( q - lfbuf < BUF_SIZE - 1 &&
                            (unsigned)(p - (char *)buf) < cnt ) {
                                ch = *p++;
                                if ( ch == LF ) {
                                        ++lfcount;
                                        *q++ = CR;
                                }
                                *q++ = ch;
                        }

                        /* write the lf buf and update total */
                        if ( WriteFile( hf,
                                        lfbuf,
                                        q - lfbuf,
                                        (LPDWORD)&written,
                                        NULL) )
                        {
                                charcount += written;
								win32handles[fh].posicion_local += written;
                                if (written < q - lfbuf)
                                        break;
                        }
                        else {
                                dosretval = GetLastError();
                                break;
                        }
                }
        }
        else {
                /* binary mode, no translation */
                if ( WriteFile( hf,
                                (LPVOID)buf,
                                cnt,
                               (LPDWORD)&written,
                                NULL) )
                {
                        dosretval = 0;
                        charcount = written;
						win32handles[fh].posicion_local += written;
						errno = 0;
                }
                else
                        dosretval = GetLastError();
        }

        if (charcount == 0) {
                /* If nothing was written, first check if an o.s. error,
                   otherwise we return -1 and set errno to ENOSPC,
                   unless a device and first char was CTRL-Z */
                if (dosretval != 0) {
                        /* o.s. error happened, map error */
                        if (dosretval == ERROR_ACCESS_DENIED) {
                            /* wrong read/write mode should return EBADF, not
                               EACCES */
                                errno = EBADF;
                                _doserrno = dosretval;
                        }
                        else
                                _dosmaperr(dosretval);
                        return -1;
                }
                else if ((fileflags  & FDEV) && *(char *)buf == CTRLZ) {
					errno = 0;
                    return 0;
				}
                else {
                        errno = ENOSPC;
                        _doserrno = 0;  /* no o.s. error */
                        return -1;
                }
        }
        else {
                /* return adjusted bytes written */
			    errno = 0;
                return charcount - lfcount;
		}
}
#else

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
#include <stdio.h>

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <stdarg.h>

#include <unistd.h>

#include "dsdbfs.h"

#include <rwmalloc.h>

extern char *dsdb_serverip;

#define FOPEN           0x01    
#define FDSDBCACHED     0x100
#define INVALID_HANDLE_VALUE -1
#define HANDLE int

static int nwin32handles = 5;
static int win32libre = -1;
static int nhandlesopen = 0;

static int primerisismo = 1;

struct __UNIXFICHHANDLES__
{
	int handle;
	int win32flags;	
	int pwin32libre;	
	long long posicion_local;
};

static struct __UNIXFICHHANDLES__ *win32handles; /* IXXMAXFILES * 2, por si se no se usa el fichero unico */
/*
static FILE *fflog = NULL;
int laviso(int nada,char *mensa)
{
	if (!fflog)
	{
		fflog = fopen("/tmp/api.log","w");
	}
	fprintf(fflog,"%s %d\r\n",mensa,nada);

	return nada;
}
*/

long long dsdbcache_size(int hf);

int dsdb_sabre(char *path,int oflag,int shflag,...)
{
	va_list ap;
	int fh,i;
	int osfh = -1;
	int pmode = S_IREAD | S_IWRITE;
	va_start(ap, shflag);
	pmode = va_arg(ap, int);
	va_end(ap);

	/*laviso(0,path);*/
	
	if (primerisismo)
	{
		win32handles = (struct __UNIXFICHHANDLES__ *)malloc(sizeof(struct __UNIXFICHHANDLES__)*MAX_OPEN_FILES);
		if (!win32handles)
		{
			aviso(0,"fallo malloc win32handles");
		}
		memset(win32handles,0,sizeof(win32handles));
		primerisismo = 0;
		for (i = 0;i < MAX_OPEN_FILES;i++)
		{
			win32handles[i].pwin32libre = -1;
			win32handles[i].handle = INVALID_HANDLE_VALUE;		
		}
	}

	/*return open(path,oflag,pmode);*/

	/*laviso(0,"open");*/

	if (win32libre > -1)
	{
		fh = win32libre;
		win32libre = win32handles[fh].pwin32libre;		
	}	
	else
	{
		if (nwin32handles >= MAX_OPEN_FILES)
		{
			/*laviso(nwin32handles,"ERROR EMFILE");*/
			errno = EMFILE;         /* too many open files */	
			return -1;              /* return error to caller */
		}
		fh = nwin32handles;		
		nwin32handles++;
		/*
		{
			char tmp[128];
			sprintf(tmp,"open %d",nwin32handles);
			laviso(fh,tmp);
		}
		*/
	}	
	win32handles[fh].handle = INVALID_HANDLE_VALUE;		
	win32handles[fh].win32flags = 0;
	win32handles[fh].pwin32libre = -1;

	if (dsdb_serverip && strstr(path,".dsb"))			
	{
		if ((osfh = dsdbcache_abre(path,oflag,shflag,pmode)) != (HANDLE)0xffffffff)
		{
			win32handles[fh].win32flags |= FDSDBCACHED;
		}
		else
		{
			/*laviso(fh,"ERROR ENOENT");*/
			errno = ENOENT;
			win32handles[fh].pwin32libre = win32libre;            
			win32libre = fh;			
			return -1;                      /* return error to caller */
		}
		win32handles[fh].posicion_local = 0;
	}
	else
	{
#ifdef FGVLINUX
		osfh = open64(path,oflag,pmode);
#else
		osfh = open(path,oflag,pmode);
#endif
		if (osfh < 0)
		{
			/*
			int e = errno;
			laviso(e,"ERROR open");
			errno = e;
			*/
			win32handles[fh].pwin32libre = win32libre;            
			win32libre = fh;			
			return -1;
		}
		win32handles[fh].posicion_local = lseek(osfh,(ds_off)0,SEEK_CUR);
	}	
	win32handles[fh].win32flags |= FOPEN;
	win32handles[fh].handle = osfh;
	nhandlesopen++;

	/*laviso(fh,"finopen");*/

	return fh;
}

int dsdb_abre(char *path,int oflag,...)
{
	va_list ap;
	int pmode = S_IREAD | S_IWRITE;
	va_start(ap, oflag);
	pmode = va_arg(ap, int);
	va_end(ap);
	return dsdb_sabre(path,oflag,0,pmode);
}

long long dsdb_lseek(int fh, long long distance, unsigned long MoveMethod)
{   
   HANDLE hf;

   /*return lseek(fh,distance,MoveMethod);*/

    if ( fh < 0 || fh >= nwin32handles ||
         !(win32handles[fh].win32flags & FOPEN) )
    {
            /* bad file handle, set errno and abort */
		    /*laviso(fh,"lseek EBADF");*/
            errno = EBADF;            
            return -1;    
	}

	/*laviso(fh,"lseek");*/
	
	if (MoveMethod == SEEK_CUR && !distance)
		return win32handles[fh].posicion_local;
	

    hf = win32handles[fh].handle;  
   

   if ((win32handles[fh].win32flags & FDSDBCACHED))
   {	     
	     /*laviso(fh,"lseek_cache");*/

		 switch(MoveMethod)
		 {
		 case SEEK_SET:
			win32handles[fh].posicion_local = distance;
			break;
		 case SEEK_END:
			 win32handles[fh].posicion_local = dsdbcache_size(hf) + distance;
			 break;
		 case SEEK_CUR:
			 win32handles[fh].posicion_local += distance;	     
			 break;
		 }		
		 return win32handles[fh].posicion_local;   
   }

   distance = lseek(hf,distance,MoveMethod);
   if (distance != (long long)-1)
   {
	   win32handles[fh].posicion_local = distance;
	   /*laviso((int)distance,"lseek_pos");*/
   }

   /*laviso(fh,"fin_lseek");*/
   return distance;
}

unsigned int dsdb_lee(int fh,void *buf,unsigned cnt)
{
	int hf;
	unsigned leidos;

	/*return read(fh,buf,cnt);*/

    if (fh < 0 || ((unsigned)fh >= (unsigned)nwin32handles) ||
         !(win32handles[fh].win32flags & FOPEN) )
    {
            /* bad file handle, set errno and abort */
		    /*laviso(fh,"lee EBADF");*/
            errno = EBADF;			
            return -1;    
	}
	
    if (cnt == 0) 
	{        
        return 0;
    }

	/*laviso(fh,"lee");*/
        
	hf = win32handles[fh].handle;


	if ((win32handles[fh].win32flags & FDSDBCACHED))
	{
		return dsdbcache_lee(win32handles[fh].handle,buf,&(win32handles[fh].posicion_local),cnt);
	}

	/*
	if (win32handles[fh].posicion_local != lseek(hf,(ds_off)0,SEEK_CUR))
	{
		laviso((int)win32handles[fh].posicion_local,"lee error posicion");
	}
	*/
	leidos = read(hf,buf,cnt);

	{
		int e;
		e = errno;
		win32handles[fh].posicion_local = lseek(hf,(ds_off)0,SEEK_CUR);
		errno = e;
	}
	/*laviso(fh,"fin_lee");*/
	return leidos;
}

int dsdb_cierra(int fh)
{
	int r = 0;

	/*return close(fh);*/

    /* validate file handle */
    if (fh < 0 || ((unsigned)fh >= (unsigned)nwin32handles) ||
         !(win32handles[fh].win32flags & FOPEN) )
    {
            /* bad file handle, set errno and abort */
		    /*laviso(fh,"cierra EBADF");*/
            errno = EBADF;
            return -1;    
	}

	/*laviso(fh,"cierra");*/
   if ((win32handles[fh].win32flags & FDSDBCACHED))
   {
	   dsdbcache_cierra(win32handles[fh].handle);	   
   }
   else
   {
		if ( win32handles[fh].handle != INVALID_HANDLE_VALUE && close(win32handles[fh].handle) == -1)
		{
			r = -1;
		}
   }    

   win32handles[fh].pwin32libre = win32libre;            						
   win32libre = fh;
   win32handles[fh].handle = INVALID_HANDLE_VALUE;
   win32handles[fh].win32flags = 0;
   win32handles[fh].posicion_local = 0;

   nhandlesopen--;
   if (nhandlesopen < 0)
   {
   	  nhandlesopen = 0;
   }
   /*laviso(fh,"fin_cierra");*/
   return r;
}

int dsdb_locking64(int fh,int lmode,ds_off nbytes)
{
	int hf;	
	int r;
	/*return fgv_lockf64(fh,lmode,nbytes);*/

	if (fh < 0 || ((unsigned)fh >= (unsigned)nwin32handles) ||
		 !(win32handles[fh].win32flags & FOPEN) )
	{
			/* bad file handle, set errno and abort */
		    /*laviso(fh,"locking EBADF");*/
			errno = EBADF;			
			return -1;    
	}
	/*laviso(fh,"locking");*/
    hf = win32handles[fh].handle;
	
	if ((win32handles[fh].win32flags & FDSDBCACHED))
	{
		return dsdbcache_locking64(hf,lmode,win32handles[fh].posicion_local,nbytes);
	}
	/*
	if (win32handles[fh].posicion_local != lseek(hf,(ds_off)0,SEEK_CUR))
	{
		laviso((int)win32handles[fh].posicion_local,"locking error posicion");
	}
	*/
	r = fgv_lockf64(hf,lmode,nbytes);
    /*laviso(fh,"finlocking");*/
	return r;
}

int dsdb_locking(int fh,int lmode,long nbytes)
{
	return dsdb_locking64(fh,lmode,(ds_off)nbytes); 
}

unsigned int dsdb_graba(int fh,void *buf,unsigned cnt)
{
    int hf;
	int fileflags;
	int leidos;

	/*return write(fh,buf,cnt);*/

    if (fh < 0 || ((unsigned)fh >= (unsigned)nwin32handles) ||
         !(win32handles[fh].win32flags & FOPEN) )
    {
            /* bad file handle, set errno and abort */
		    /*laviso(fh,"graba EBADF");*/
            errno = EBADF;
            return -1;    
	}

	/*laviso(fh,"graba");*/

    hf = win32handles[fh].handle;
	fileflags = win32handles[fh].win32flags;

	if ((fileflags & FDSDBCACHED))
	{
		return dsdbcache_graba(hf,buf,&(win32handles[fh].posicion_local),cnt);
    }
	/*
	if (win32handles[fh].posicion_local != lseek(hf,(ds_off)0,SEEK_CUR))
	{
		laviso((int)win32handles[fh].posicion_local,"graba error posicion");
	}
	*/
	leidos = write(hf,buf,cnt);
	{
		int e;
		e = errno;
		win32handles[fh].posicion_local = lseek(hf,(ds_off)0,SEEK_CUR);
		errno = e;
	}
	/*laviso(fh,"fingraba");*/
	return leidos;
}

#endif
