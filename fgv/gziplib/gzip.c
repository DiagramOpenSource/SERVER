
#include <fgvsys.h>

#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "tailor.h"
#include "gzip.h"
#include "lzw.h"
#include "revision.h"
#include "getopt.h"


extern char *__Texto[];

/* configuration */

#ifdef NO_TIME_H
#  include <sys/time.h>
#else
#  include <time.h>
#endif

#ifndef NO_FCNTL_H
#  include <fcntl.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#if defined(STDC_HEADERS) || !defined(NO_STDLIB_H)
#  include <stdlib.h>
#else
extern int errno;
#endif

#if defined(DIRENT)
#  include <dirent.h>
typedef struct dirent dir_type;
#  define NLENGTH(dirent) ((int)strlen((dirent)->d_name))
#  define DIR_OPT "DIRENT"
#else
#  define NLENGTH(dirent) ((dirent)->d_namlen)
#  ifdef SYSDIR
#    include <sys/dir.h>
typedef struct direct dir_type;
#    define DIR_OPT "SYSDIR"
#  else
#    ifdef SYSNDIR
#      include <sys/ndir.h>
typedef struct direct dir_type;
#      define DIR_OPT "SYSNDIR"
#    else
#      ifdef NDIR
#        include <ndir.h>
typedef struct direct dir_type;
#        define DIR_OPT "NDIR"
#      else
#        define NO_DIR
#        define DIR_OPT "NO_DIR"
#      endif
#    endif
#  endif
#endif

#ifndef NO_UTIME
#  ifndef NO_UTIME_H
#    include <utime.h>
#    define TIME_OPT "UTIME"
#  else
#    ifdef HAVE_SYS_UTIME_H
#      include <sys/utime.h>
#      define TIME_OPT "SYS_UTIME"
#    else
struct utimbuf {
   time_t actime;
   time_t modtime;
};
#      define TIME_OPT ""
#    endif
#  endif
#else
#  define TIME_OPT "NO_UTIME"
#endif

#if !defined(S_ISDIR) && defined(S_IFDIR)
#  define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG) && defined(S_IFREG)
#  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

typedef RETSIGTYPE (*sig_type) OF((int));

#ifndef	O_BINARY
#  define  O_BINARY  0  /* creation mode for open() */
#endif

#ifndef O_CREAT
/* Pure BSD system? */
#  include <sys/file.h>
#  ifndef O_CREAT
#    define O_CREAT FCREAT
#  endif
#  ifndef O_EXCL
#    define O_EXCL FEXCL
#  endif
#endif

#ifndef S_IRUSR
#  define S_IRUSR 0400
#endif
#ifndef S_IWUSR
#  define S_IWUSR 0200
#endif
#define RW_USER (S_IRUSR | S_IWUSR)  /* creation mode for open() */

#ifndef MAX_PATH_LEN
#  define MAX_PATH_LEN   1024 /* max pathname length */
#endif

#ifndef SEEK_END
#  define SEEK_END 2
#endif

/*
#ifdef NO_OFF_T
typedef long off_t;
off_t lseek OF((int fd, off_t offset, int whence));
#endif
*/

/* Separator for file name parts (see shorten_name()) */
#ifdef NO_MULTIPLE_DOTS
#  define PART_SEP "-"
#else
#  define PART_SEP "."
#endif

/* global buffers */
DECLARE(uch, inbuf,  INBUFSIZ +INBUF_EXTRA);
DECLARE(uch, outbuf, OUTBUFSIZ+OUTBUF_EXTRA);
DECLARE(ush, d_buf,  DIST_BUFSIZE);
DECLARE(uch, window, 2L*WSIZE);
#ifndef MAXSEG_64K
DECLARE(ush, tab_prefix, 1L<<BITS);
#else
DECLARE(ush, tab_prefix0, 1L<<(BITS-1));
DECLARE(ush, tab_prefix1, 1L<<(BITS-1));
#endif

/* local variables */

int abandonalo_ya = 0;

int ascii = 0;        /* convert end-of-lines to local OS conventions */
int to_stdout = 0;    /* output to stdout (-c) */
int decompress = 0;   /* decompress (-d) */
int force = 0;        /* don't ask questions, compress links (-f) */
int no_name = -1;     /* don't save or restore the original file name */
int no_time = -1;     /* don't save or restore the original file time */
int recursive = 0;    /* recurse through directories (-r) */
int list = 0;         /* list the file contents (-l) */
int verbose = 0;      /* be verbose (-v) */
int quiet = 0;        /* be very quiet (-q) */
int do_lzw = 0;       /* generate output compatible with old compress (-Z) */
int test = 0;         /* test .gz file integrity */
int foreground;       /* set if program run in foreground */
char *progname;       /* program name */
int maxbits = BITS;   /* max bits per code for LZW */
int method = DEFLATED;/* compression method */
int level = 6;        /* compression level */
int exit_code = OK;   /* program exit code */
int save_orig_name;   /* set if original name must be saved */
int last_member;      /* set for .zip and .Z files */
int part_nb;          /* number of parts in .gz file */
long time_stamp;      /* original time stamp (modification time) */
long ifile_size;      /* input file size, -1 for devices (debug only) */
char *env;            /* contents of GZIP env variable */
char **args = NULL;   /* argv pointer if GZIP env variable defined */
char z_suffix[MAX_SUFFIX+1]; /* default suffix (can be set with --suffix) */
int  z_len;           /* strlen(z_suffix) */

long bytes_in;             /* number of input bytes */
long bytes_out;            /* number of output bytes */
long total_in = 0;         /* input bytes for all files */
long total_out = 0;        /* output bytes for all files */
char ifname[MAX_PATH_LEN]; /* input file name */
char ofname[MAX_PATH_LEN]; /* output file name */
int  remove_ofname = 0;	   /* remove output file on error */
#ifndef FGVGZIP
struct stat istat;         /* status for input file */
#endif
int  ifd;                  /* input file descriptor */
int  ofd;                  /* output file descriptor */
unsigned insize;           /* valid bytes in inbuf */
unsigned inptr;            /* index of next byte to be processed in inbuf */
unsigned outcnt;           /* bytes in output buffer */

void (*write_interno)() = NULL;
unsigned (*read_interno)() = NULL;
int gzip_asignada_memoria = -1;

#define strequ(s1, s2) (strcmp((s1),(s2)) == 0)
/* local functions */
int (*work) OF((int infile, int outfile)) = ds___zip; /* function to call */

#ifndef FGVGZIP

struct option longopts[] =
{
   /* { name  has_arg  *flag  val } */
   {"ascii",      0, 0, 'a'}, /* ascii text mode */
   {"to-stdout",  0, 0, 'c'}, /* write output on standard output */
   {"stdout",     0, 0, 'c'}, /* write output on standard output */
   {"decompress", 0, 0, 'd'}, /* decompress */
   {"uncompress", 0, 0, 'd'}, /* decompress */
   /* {"encrypt",    0, 0, 'e'},    encrypt */
   {"force",      0, 0, 'f'}, /* force overwrite of output file */
   {"help",       0, 0, 'h'}, /* give help */
   /* {"pkzip",      0, 0, 'k'},    force output in pkzip format */
   {"list",       0, 0, 'l'}, /* list .gz file contents */    
   {"no-name",    0, 0, 'n'}, /* don't save or restore original name & time */
   {"name",       0, 0, 'N'}, /* save or restore original name & time */
   {"quiet",      0, 0, 'q'}, /* quiet mode */
   {"silent",     0, 0, 'q'}, /* quiet mode */
   {"recursive",  0, 0, 'r'}, /* recurse through directories */
   {"suffix",     1, 0, 'S'}, /* use given suffix instead of .gz */
   {"test",       0, 0, 't'}, /* test compressed file integrity */
   {"no-time",    0, 0, 'T'}, /* don't save or restore the time stamp */
   {"verbose",    0, 0, 'v'}, /* verbose mode */
   {"version",    0, 0, 'V'}, /* display version number */
   {"fast",       0, 0, '1'}, /* compress faster */
   {"best",       0, 0, '9'}, /* compress better */
   {"lzw",        0, 0, 'Z'}, /* make output compatible with old compress */
   {"bits",       1, 0, 'b'}, /* max number of bits per code (implies -Z) */
   { 0, 0, 0, 0 }
};


local void usage        OF((void));
local void help         OF((void));
local void version      OF((void));
local void treat_stdin  OF((void));
local void treat_file   OF((char *iname));
local int create_outfile OF((void));
local int  do_stat      OF((char *name, struct stat *sbuf));
local char *get_suffix  OF((char *name));
local int  get_istat    OF((char *iname, struct stat *sbuf));
local int  make_ofname  OF((void));
local int  same_file    OF((struct stat *stat1, struct stat *stat2));
local int name_too_long OF((char *name, struct stat *statb));
local void shorten_name  OF((char *name));
local int  get_method   OF((int in));
local void do_list      OF((int ifd, int method));
local int  check_ofname OF((void));
local void copy_stat    OF((struct stat *ifstat));
local void do_exit      OF((int exitcode));
int main          OF((int argc, char **argv));


#ifndef NO_DIR
local void treat_dir    OF((char *dir));
#endif
#ifndef NO_UTIME
local void reset_times  OF((char *name, struct stat *statb));
#endif


/* ======================================================================== */
local void usage()
{
}

/* ======================================================================== */
local void help()
{
}


/* ======================================================================== */
local void version()
{
}

/* ======================================================================== */
int main (argc, argv)
int argc;
char **argv;
{
   int file_count;     /* number of files to precess */
   int proglen;        /* length of progname */
   int optc;           /* current option */
   
   EXPAND(argc, argv); /* wild card expansion if necessary */
   
   progname = basename(argv[0]);
   proglen = strlen(progname);
   
   /* Suppress .exe for MSDOS, OS/2 and VMS: */
   if (proglen > 4 && strequ(progname+proglen-4, ".exe")) {
      progname[proglen-4] = '\0';
   }
   
   /* Add options in GZIP environment variable if there is one */
   env = add_envopt(&argc, &argv, OPTIONS_VAR);
   if (env != NULL) args = argv;
   
   foreground = signal(SIGINT, SIG_IGN) != SIG_IGN;
   if (foreground) {
      (void) signal (SIGINT, (sig_type)abort_gzip);
   }
#ifdef SIGTERM
   if (signal(SIGTERM, SIG_IGN) != SIG_IGN) {
      (void) signal(SIGTERM, (sig_type)abort_gzip);
   }
#endif
#ifdef SIGHUP
   if (signal(SIGHUP, SIG_IGN) != SIG_IGN) {
      (void) signal(SIGHUP,  (sig_type)abort_gzip);
   }
#endif
   
#ifndef GNU_STANDARD
   /* For compatibility with old compress, use program name as an option.
   * If you compile with -DGNU_STANDARD, this program will behave as
   * gzip even if it is invoked under the name gunzip or zcat.
   *
   * Systems which do not support links can still use -d or -dc.
   * Ignore an .exe extension for MSDOS, OS/2 and VMS.
   */
   if (  strncmp(progname, "un",  2) == 0     /* ungzip, uncompress */
      || strncmp(progname, "gun", 3) == 0) {  /* gunzip */
      decompress = 1;
   } else if (strequ(progname+1, "cat")       /* zcat, pcat, gcat */
      || strequ(progname, "gzcat")) {    /* gzcat */
      decompress = to_stdout = 1;
			}
#endif
   
   strncpy(z_suffix, Z_SUFFIX, sizeof(z_suffix)-1);
   z_len = strlen(z_suffix);
   
   while ((optc = getopt_long (argc, argv, "ab:cdfhH?lLmMnNqrS:tvVZ123456789",
      longopts, (int *)0)) != EOF) {
      switch (optc) {
      case 'a':
         ascii = 1; break;
      case 'b':
         maxbits = atoi(optarg);
         break;
      case 'c':
         to_stdout = 1; break;
      case 'd':
         decompress = 1; break;
      case 'f':
         force++; break;
      case 'h': case 'H': case '?':
         help(); do_exit(OK); break;
      case 'l':
         list = decompress = to_stdout = 1; break;
      case 'm': /* undocumented, may change later */
         no_time = 1; break;
      case 'M': /* undocumented, may change later */
         no_time = 0; break;
      case 'n':
         no_name = no_time = 1; break;
      case 'N':
         no_name = no_time = 0; break;
      case 'q':
         quiet = 1; verbose = 0; break;
      case 'r':
#ifdef NO_DIR
         fprintf(stderr, "%s: -r not supported on this system\n", progname);
         usage();
         do_exit(ERRORGZIP); break;
#else
         recursive = 1; break;
#endif
      case 'S':
#ifdef NO_MULTIPLE_DOTS
         if (*optarg == '.') optarg++;
#endif
         z_len = strlen(optarg);
         strcpy(z_suffix, optarg);
         break;
      case 't':
         test = decompress = to_stdout = 1;
         break;
      case 'v':
         verbose++; quiet = 0; break;
      case 'V':
         version(); do_exit(OK); break;
      case 'Z':
#ifdef LZW
         do_lzw = 1; break;
#else
         fprintf(stderr, "%s: -Z not supported in this version\n",
            progname);
         usage();
         do_exit(ERRORGZIP); break;
#endif
      case '1':  case '2':  case '3':  case '4':
      case '5':  case '6':  case '7':  case '8':  case '9':
         level = optc - '0';
         break;
      default:
         /* Error message already emitted by getopt_long. */
         usage();
         do_exit(ERRORGZIP);
      }
			} /* loop on all arguments */
   
           /* By default, save name and timestamp on compression but do not
           * restore them on decompression.
   */
   if (no_time < 0) no_time = decompress;
   if (no_name < 0) no_name = decompress;
   
   file_count = argc - optind;
   
   if ((z_len == 0 && !decompress) || z_len > MAX_SUFFIX) {
      fprintf(stderr, "%s: incorrect suffix '%s'\n",
         progname, optarg);
      do_exit(ERRORGZIP);
			}
   if (do_lzw && !decompress) work = lzw;
   
   /* Allocate all global buffers (for DYN_ALLOC option) */
   ALLOC(uch, inbuf,  INBUFSIZ +INBUF_EXTRA);
   ALLOC(uch, outbuf, OUTBUFSIZ+OUTBUF_EXTRA);
   ALLOC(ush, d_buf,  DIST_BUFSIZE);
   ALLOC(uch, window, 2L*WSIZE);
#ifndef MAXSEG_64K
   ALLOC(ush, tab_prefix, 1L<<BITS);
#else
   ALLOC(ush, tab_prefix0, 1L<<(BITS-1));
   ALLOC(ush, tab_prefix1, 1L<<(BITS-1));
#endif
   
   /* And get to work */
   if (file_count != 0) {
      if (to_stdout && !test && !list && (!decompress || !ascii)) {
         SET_BINARY_MODE(fileno(stdout));
      }
      while (optind < argc) {
         treat_file(argv[optind++]);
      }
			} else {  /* Standard input */
      treat_stdin();
         }
         if (list && !quiet && file_count > 1) {
            do_list(-1, -1); /* print totals */
         }
         do_exit(exit_code);
         return exit_code; /* just to avoid lint warning */
}

/* ========================================================================
* Compress or decompress stdin
*/
local void treat_stdin()
{
   if (!force && !list &&
      isatty(fileno((FILE *)(decompress ? stdin : stdout)))) {
      /* Do not send compressed data to the terminal or read it from
      * the terminal. We get here when user invoked the program
      * without parameters, so be helpful. According to the GNU standards:
      *
      *   If there is one behavior you think is most useful when the output
      *   is to a terminal, and another that you think is most useful when
      *   the output is a file or a pipe, then it is usually best to make
      *   the default behavior the one that is useful with output to a
      *   terminal, and have an option for the other behavior.
      *
      * Here we use the --force option to get the other behavior.
      */
      fprintf(stderr,
         "%s: compressed data not %s a terminal. Use -f to force %scompression.\n",
         progname, decompress ? "read from" : "written to",
         decompress ? "de" : "");
      fprintf(stderr,"For help, type: %s -h\n", progname);
      do_exit(ERRORGZIP);
   }
   
   if (decompress || !ascii) {
      SET_BINARY_MODE(fileno(stdin));
   }
   if (!test && !list && (!decompress || !ascii)) {
      SET_BINARY_MODE(fileno(stdout));
   }
   strcpy(ifname, "stdin");
   strcpy(ofname, "stdout");
   
   /* Get the time stamp on the input file. */
   time_stamp = 0; /* time unknown by default */
   
#ifndef NO_STDIN_FSTAT
   if (list || !no_time) {
      if (fstat(fileno(stdin), &istat) != 0) {
         error("fstat(stdin)");
      }
# ifdef NO_PIPE_TIMESTAMP
      if (S_ISREG(istat.st_mode))
# endif
         time_stamp = istat.st_mtime;
#endif /* NO_STDIN_FSTAT */
   }
   ifile_size = -1L; /* convention for unknown size */
   
   clear_bufs(); /* clear input and output buffers */
   to_stdout = 1;
   part_nb = 0;
   
   if (decompress) {
      method = get_method(ifd);
      if (method < 0) {
         do_exit(exit_code); /* error message already emitted */
      }
   }
   if (list) {
      do_list(ifd, method);
      return;
   }
   
   /* Actually do the compression/decompression. Loop over zipped members.
   */
   for (;;) {
      if ((*work)(fileno(stdin), fileno(stdout)) != OK) return;
      
      if (!decompress || last_member || inptr == insize) break;
      /* end of file */
      
      method = get_method(ifd);
      if (method < 0) return; /* error message already emitted */
      bytes_out = 0;            /* required for length check */
   }
   
   if (verbose) {
      if (test) {
         fprintf(stderr, " OK\n");
         
      } else if (!decompress) {
         display_ratio(bytes_in-(bytes_out-header_bytes), bytes_in, stderr);
         fprintf(stderr, "\n");
#ifdef DISPLAY_STDIN_RATIO
      } else {
         display_ratio(bytes_out-(bytes_in-header_bytes), bytes_out,stderr);
         fprintf(stderr, "\n");
#endif
      }
   }
}

/* ========================================================================
* Compress or decompress the given file
*/
local void treat_file(iname)
char *iname;
{
   /* Accept "-" as synonym for stdin */
   if (strequ(iname, "-")) {
      int cflag = to_stdout;
      treat_stdin();
      to_stdout = cflag;
      return;
   }
   
   /* Check if the input file is present, set ifname and istat: */
   if (get_istat(iname, &istat) != OK) return;
   
   /* If the input name is that of a directory, recurse or ignore: */
   if (S_ISDIR(istat.st_mode)) {
#ifndef NO_DIR
      if (recursive) {
         struct stat st;
         st = istat;
         treat_dir(iname);
         /* Warning: ifname is now garbage */
#  ifndef NO_UTIME
         reset_times (iname, &st);
#  endif
      } else
#endif
         WARN((stderr,"%s: %s is a directory -- ignored\n", progname, ifname));
      return;
   }
   if (!S_ISREG(istat.st_mode)) {
      WARN((stderr,
         "%s: %s is not a directory or a regular file - ignored\n",
         progname, ifname));
      return;
   }
   if (istat.st_nlink > 1 && !to_stdout && !force) {
      WARN((stderr, "%s: %s has %d other link%c -- unchanged\n",
         progname, ifname,
         (int)istat.st_nlink - 1, istat.st_nlink > 2 ? 's' : ' '));
      return;
   }
   
   ifile_size = istat.st_size;
   time_stamp = no_time && !list ? 0 : istat.st_mtime;
   
   /* Generate output file name. For -r and (-t or -l), skip files
   * without a valid gzip suffix (check done in make_ofname).
   */
   if (to_stdout && !list && !test) {
      strcpy(ofname, "stdout");
      
   } else if (make_ofname() != OK) {
      return;
			}
   
         /* Open the input file and determine compression method. The mode
         * parameter is ignored but required by some systems (VMS) and forbidden
         * on other systems (MacOS).
   */
   ifd = OPEN(ifname, ascii && !decompress ? O_RDONLY : O_RDONLY | O_BINARY,
      RW_USER);
   if (ifd == -1) {
      fprintf(stderr, "%s: ", progname);
      perror(ifname);
      exit_code = ERRORGZIP;
      return;
			}
   clear_bufs(); /* clear input and output buffers */
   part_nb = 0;
   
   if (decompress) {
      method = get_method(ifd); /* updates ofname if original given */
      if (method < 0) {
         close(ifd);
         return;               /* error message already emitted */
      }
			}
   if (list) {
      do_list(ifd, method);
      close(ifd);
      return;
			}
   
         /* If compressing to a file, check if ofname is not ambiguous
         * because the operating system truncates names. Otherwise, generate
         * a new ofname and save the original name in the compressed file.
   */
   if (to_stdout) {
      ofd = fileno(stdout);
      /* keep remove_ofname as zero */
			} else {
      if (create_outfile() != OK) return;
      
      if (!decompress && save_orig_name && !verbose && !quiet) {
         fprintf(stderr, "%s: %s compressed to %s\n",
            progname, ifname, ofname);
      }
         }
         /* Keep the name even if not truncated except with --no-name: */
         if (!save_orig_name) save_orig_name = !no_name;
         
         if (verbose) {
            fprintf(stderr, "%s:\t%s", ifname, (int)strlen(ifname) >= 15 ? 
               "" : ((int)strlen(ifname) >= 7 ? "\t" : "\t\t"));
         }
         
         /* Actually do the compression/decompression. Loop over zipped members.
         */
         for (;;) {
            if ((*work)(ifd, ofd) != OK) {
               method = -1; /* force cleanup */
               break;
            }
            if (!decompress || last_member || inptr == insize) break;
            /* end of file */
            
            method = get_method(ifd);
            if (method < 0) break;    /* error message already emitted */
            bytes_out = 0;            /* required for length check */
         }
         
         close(ifd);
         if (!to_stdout && close(ofd)) {
            write_error();
         }
         if (method == -1) {
            if (!to_stdout) unlink (ofname);
            return;
         }
         /* Display statistics */
         if(verbose) {
            if (test) {
               fprintf(stderr, " OK");
            } else if (decompress) {
               display_ratio(bytes_out-(bytes_in-header_bytes), bytes_out,stderr);
            } else {
               display_ratio(bytes_in-(bytes_out-header_bytes), bytes_in, stderr);
            }
            if (!test && !to_stdout) {
               fprintf(stderr, " -- replaced with %s", ofname);
            }
            fprintf(stderr, "\n");
         }
         /* Copy modes, times, ownership, and remove the input file */
         if (!to_stdout) {
            copy_stat(&istat);
         }
}

/* ========================================================================
* Create the output file. Return OK or ERRORGZIP.
* Try several times if necessary to avoid truncating the z_suffix. For
* example, do not create a compressed file of name "1234567890123."
* Sets save_orig_name to true if the file name has been truncated.
* IN assertions: the input file has already been open (ifd is set) and
*   ofname has already been updated if there was an original name.
* OUT assertions: ifd and ofd are closed in case of error.
*/
local int create_outfile()
{
   struct stat	ostat; /* stat for ofname */
   int flags = O_WRONLY | O_CREAT | O_EXCL | O_BINARY;
   
   if (ascii && decompress) {
      flags &= ~O_BINARY; /* force ascii text mode */
   }
   for (;;) {
      /* Make sure that ofname is not an existing file */
      if (check_ofname() != OK) {
         close(ifd);
         return ERRORGZIP;
      }
      /* Create the output file */
      remove_ofname = 1;
      ofd = OPEN(ofname, flags, RW_USER);
      if (ofd == -1) {
         perror(ofname);
         close(ifd);
         exit_code = ERRORGZIP;
         return ERRORGZIP;
      }
      
      /* Check for name truncation on new file (1234567890123.gz) */
#ifdef NO_FSTAT
      if (stat(ofname, &ostat) != 0) {
#else
         if (fstat(ofd, &ostat) != 0) {
#endif
            fprintf(stderr, "%s: ", progname);
            perror(ofname);
            close(ifd); close(ofd);
            unlink(ofname);
            exit_code = ERRORGZIP;
            return ERRORGZIP;
         }
         if (!name_too_long(ofname, &ostat)) return OK;
         
         if (decompress) {
            /* name might be too long if an original name was saved */
            WARN((stderr, "%s: %s: warning, name truncated\n",
               progname, ofname));
            return OK;
         }
         close(ofd);
         unlink(ofname);
#ifdef NO_MULTIPLE_DOTS
         /* Should never happen, see check_ofname() */
         fprintf(stderr, "%s: %s: name too long\n", progname, ofname);
         do_exit(ERRORGZIP);
#endif
         shorten_name(ofname);
      }
   }
   
   /* ========================================================================
   * Use lstat if available, except for -c or -f. Use stat otherwise.
   * This allows links when not removing the original file.
   */
   local int do_stat(name, sbuf)
      char *name;
   struct stat *sbuf;
   {
      errno = 0;
#if (defined(S_IFLNK) || defined (S_ISLNK)) && !defined(NO_SYMLINK)
      if (!to_stdout && !force) {
         return lstat(name, sbuf);
      }
#endif
      return stat(name, sbuf);
   }
   
   /* ========================================================================
   * Return a pointer to the 'z' suffix of a file name, or NULL. For all
   * systems, ".gz", ".z", ".Z", ".taz", ".tgz", "-gz", "-z" and "_z" are
   * accepted suffixes, in addition to the value of the --suffix option.
   * ".tgz" is a useful convention for tar.z files on systems limited
   * to 3 characters extensions. On such systems, ".?z" and ".??z" are
   * also accepted suffixes. For Unix, we do not want to accept any
   * .??z suffix as indicating a compressed file; some people use .xyz
   * to denote volume data.
   *   On systems allowing multiple versions of the same file (such as VMS),
   * this function removes any version suffix in the given name.
   */
   local char *get_suffix(name)
      char *name;
			{
            int nlen, slen;
            char suffix[MAX_SUFFIX+3]; /* last chars of name, forced to lower case */
            static char *known_suffixes[] =
            {z_suffix, ".gz", ".z", ".taz", ".tgz", "-gz", "-z", "_z",
#ifdef MAX_EXT_CHARS
            "z",
#endif
            NULL};
            char **suf = known_suffixes;
            
            if (strequ(z_suffix, "z")) suf++; /* check long suffixes first */
            
#ifdef SUFFIX_SEP
            /* strip a version number from the file name */
            {
               char *v = strrchr(name, SUFFIX_SEP);
               if (v != NULL) *v = '\0';
            }
#endif
            nlen = strlen(name);
            if (nlen <= MAX_SUFFIX+2) {
               strcpy(suffix, name);
            } else {
               strcpy(suffix, name+nlen-MAX_SUFFIX-2);
            }
            strlwr(suffix);
            slen = strlen(suffix);
            do {
               int s = strlen(*suf);
               if (slen > s && suffix[slen-s-1] != PATH_SEP
                  && strequ(suffix + slen - s, *suf)) {
                  return name+nlen-s;
               }
            } while (*++suf != NULL);
            
            return NULL;
         }
         
         
         /* ========================================================================
         * Set ifname to the input file name (with a suffix appended if necessary)
         * and istat to its stats. For decompression, if no file exists with the
         * original name, try adding successively z_suffix, .gz, .z, -z and .Z.
         * For MSDOS, we try only z_suffix and z.
         * Return OK or ERRORGZIP.
         */
         local int get_istat(iname, sbuf)
            char *iname;
         struct stat *sbuf;
         {
            int ilen;  /* strlen(ifname) */
            static char *suffixes[] = {z_suffix, ".gz", ".z", "-z", ".Z", NULL};
            char **suf = suffixes;
            char *s;
#ifdef NO_MULTIPLE_DOTS
            char *dot; /* pointer to ifname extension, or NULL */
#endif
            
            strcpy(ifname, iname);
            
            /* If input file exists, return OK. */
            if (do_stat(ifname, sbuf) == 0) return OK;
            
            if (!decompress || errno != ENOENT) {
               perror(ifname);
               exit_code = ERRORGZIP;
               return ERRORGZIP;
            }
            /* file.ext doesn't exist, try adding a suffix (after removing any
            * version number for VMS).
            */
            s = get_suffix(ifname);
            if (s != NULL) {
               perror(ifname); /* ifname already has z suffix and does not exist */
               exit_code = ERRORGZIP;
               return ERRORGZIP;
            }
#ifdef NO_MULTIPLE_DOTS
            dot = strrchr(ifname, '.');
            if (dot == NULL) {
               strcat(ifname, ".");
               dot = strrchr(ifname, '.');
            }
#endif
            ilen = strlen(ifname);
            if (strequ(z_suffix, ".gz")) suf++;
            
            /* Search for all suffixes */
            do {
               s = *suf;
#ifdef NO_MULTIPLE_DOTS
               if (*s == '.') s++;
#endif
#ifdef MAX_EXT_CHARS
               strcpy(ifname, iname);
               /* Needed if the suffixes are not sorted by increasing length */
               
               if (*dot == '\0') strcpy(dot, ".");
               dot[MAX_EXT_CHARS+1-strlen(s)] = '\0';
#endif
               strcat(ifname, s);
               if (do_stat(ifname, sbuf) == 0) return OK;
               ifname[ilen] = '\0';
            } while (*++suf != NULL);
            
            /* No suffix found, complain using z_suffix: */
#ifdef MAX_EXT_CHARS
            strcpy(ifname, iname);
            if (*dot == '\0') strcpy(dot, ".");
            dot[MAX_EXT_CHARS+1-z_len] = '\0';
#endif
            strcat(ifname, z_suffix);
            perror(ifname);
            exit_code = ERRORGZIP;
            return ERRORGZIP;
         }
         
         /* ========================================================================
         * Generate ofname given ifname. Return OK, or WARNING if file must be skipped.
         * Sets save_orig_name to true if the file name has been truncated.
         */
         local int make_ofname()
         {
            char *suff;            /* ofname z suffix */
            
            strcpy(ofname, ifname);
            /* strip a version number if any and get the gzip suffix if present: */
            suff = get_suffix(ofname);
            
            if (decompress) {
               if (suff == NULL) {
               /* Whith -t or -l, try all files (even without .gz suffix)
               * except with -r (behave as with just -dr).
                  */
                  if (!recursive && (list || test)) return OK;
                  
                  /* Avoid annoying messages with -r */
                  if (verbose || (!recursive && !quiet)) {
                     WARN((stderr,"%s: %s: unknown suffix -- ignored\n",
                        progname, ifname));
                  }
                  return WARNING;
               }
               /* Make a special case for .tgz and .taz: */
               strlwr(suff);
               if (strequ(suff, ".tgz") || strequ(suff, ".taz")) {
                  strcpy(suff, ".tar");
               } else {
                  *suff = '\0'; /* strip the z suffix */
               }
               /* ofname might be changed later if infile contains an original name */
               
            } else if (suff != NULL) {
               /* Avoid annoying messages with -r (see treat_dir()) */
               if (verbose || (!recursive && !quiet)) {
                  fprintf(stderr, "%s: %s already has %s suffix -- unchanged\n",
                     progname, ifname, suff);
               }
               if (exit_code == OK) exit_code = WARNING;
               return WARNING;
            } else {
               save_orig_name = 0;
               
#ifdef NO_MULTIPLE_DOTS
               suff = strrchr(ofname, '.');
               if (suff == NULL) {
                  strcat(ofname, ".");
#  ifdef MAX_EXT_CHARS
                  if (strequ(z_suffix, "z")) {
                     strcat(ofname, "gz"); /* enough room */
                     return OK;
                  }
                  /* On the Atari and some versions of MSDOS, name_too_long()
                  * does not work correctly because of a bug in stat(). So we
                  * must truncate here.
                  */
               } else if (strlen(suff)-1 + z_len > MAX_SUFFIX) {
                  suff[MAX_SUFFIX+1-z_len] = '\0';
                  save_orig_name = 1;
#  endif
               }
#endif /* NO_MULTIPLE_DOTS */
               strcat(ofname, z_suffix);
               
            } /* decompress ? */
            return OK;
         }
         
         
         /* ========================================================================
         * Check the magic number of the input file and update ofname if an
         * original name was given and to_stdout is not set.
         * Return the compression method, -1 for error, -2 for warning.
         * Set inptr to the offset of the next byte to be processed.
         * Updates time_stamp if there is one and --no-time is not used.
         * This function may be called repeatedly for an input file consisting
         * of several contiguous gzip'ed members.
         * IN assertions: there is at least one remaining compressed member.
         *   If the member is a zip file, it must be the only one.
         */
         local int get_method(in)
            int in;        /* input file descriptor */
         {
            uch flags;     /* compression flags */
            char magic[2]; /* magic header */
            ulg stamp;     /* time stamp */
            
                           /* If --force and --stdout, zcat == cat, so do not complain about
                           * premature end of file: use try_byte instead of get_byte.
            */
            if (force && to_stdout) {
               magic[0] = (char)try_byte();
               magic[1] = (char)try_byte();
               /* If try_byte returned EOF, magic[1] == 0xff */
            } else {
               magic[0] = (char)get_byte();
               magic[1] = (char)get_byte();
            }
            method = -1;                 /* unknown yet */
            part_nb++;                   /* number of parts in gzip file */
            header_bytes = 0;
            last_member = RECORD_IO;
            /* assume multiple members in gzip file except for record oriented I/O */
            
            if (memcmp(magic, GZIP_MAGIC, 2) == 0 || memcmp(magic, DS_GZIP_MAGIC, 2) == 0 
               || memcmp(magic, OLD_GZIP_MAGIC, 2) == 0) {
               
               method = (int)get_byte();
               if (method != DEFLATED) {
                  exit_code = ERRORGZIP;
                  return -1;
               }
               work = unzip;
               flags  = (uch)get_byte();
               
               if ((flags & ENCRYPTED) != 0) {
                  exit_code = ERRORGZIP;
                  return -1;
               }
               if ((flags & CONTINUATION) != 0) {
                  exit_code = ERRORGZIP;
                  if (force <= 1) return -1;
               }
               if ((flags & RESERVED) != 0) {
                  exit_code = ERRORGZIP;
                  if (force <= 1) return -1;
               }
               stamp  = (ulg)get_byte();
               stamp |= ((ulg)get_byte()) << 8;
               stamp |= ((ulg)get_byte()) << 16;
               stamp |= ((ulg)get_byte()) << 24;
               if (stamp != 0 && !no_time) time_stamp = stamp;
               
               (void)get_byte();  /* Ignore extra flags for the moment */
               (void)get_byte();  /* Ignore OS type for the moment */
               
               if ((flags & CONTINUATION) != 0) {
                  unsigned part = (unsigned)get_byte();
                  part |= ((unsigned)get_byte())<<8;
                  if (verbose) {
                  }
               }
               if ((flags & EXTRA_FIELD) != 0) {
                  unsigned len = (unsigned)get_byte();
                  len |= ((unsigned)get_byte())<<8;
                  while (len--) (void)get_byte();
               }
               
               /* Get original file name if it was truncated */
               if ((flags & ORIG_NAME) != 0) {
                  if (no_name || (to_stdout && !list) || part_nb > 1) {
                     /* Discard the old name */
                     char c; /* dummy used for NeXTstep 3.0 cc optimizer bug */
                     do {c=get_byte();} while (c != 0);
                  } else {
                     /* Copy the base name. Keep a directory prefix intact. */
                     char *p = basename(ofname);
                     char *base = p;
                     for (;;) {
                        *p = (char)get_char();
                        if (*p++ == '\0') break;
                        if (p >= ofname+sizeof(ofname)) {
                           error("corrupted input -- file name too large");
                        }
                     }
                     /* If necessary, adapt the name to local OS conventions: */
                     if (!list) {
                        MAKE_LEGAL_NAME(base);
                        if (base) list=0; /* avoid warning about unused variable */
                     }
                  } /* no_name || to_stdout */
               } /* ORIG_NAME */
               
               /* Discard file comment if any */
               if ((flags & COMMENT) != 0) {
                  while (get_char() != 0) /* null */ ;
               }
               if (part_nb == 1) {
                  header_bytes = inptr + 2*sizeof(long); /* include crc and size */
               }
               
            } else if (memcmp(magic, PKZIP_MAGIC, 2) == 0 && inptr == 2
               && memcmp((char*)inbuf, PKZIP_MAGIC, 4) == 0) {
               /* To simplify the code, we support a zip file when alone only.
               * We are thus guaranteed that the entire local header fits in inbuf.
               */
               inptr = 0;
               work = unzip;
               if (check_zipfile(in) != OK) return -1;
               /* check_zipfile may get ofname from the local header */
               last_member = 1;
               
            } else if (memcmp(magic, PACK_MAGIC, 2) == 0) {
               work = unpack;
               method = PACKED;
               
            } else if (memcmp(magic, LZW_MAGIC, 2) == 0) {
               work = unlzw;
               method = COMPRESSED;
               last_member = 1;
               
            } else if (memcmp(magic, LZH_MAGIC, 2) == 0) {
               work = unlzh;
               method = LZHED;
               last_member = 1;
               
            } else if (force && to_stdout && !list) { /* pass input unchanged */
               method = STORED;
               work = copy;
               inptr = 0;
               last_member = 1;
												}
            if (method >= 0) return method;
            
            if (part_nb == 1) {	
               exit_code = ERRORGZIP;
               return -1;
												} else {
               WARN((stderr, "\n%s: %s: decompression OK, trailing garbage ignored\n",
                  progname, ifname));
               return -2;
                                    }
}

/* ========================================================================
* Display the characteristics of the compressed file.
* If the given method is < 0, display the accumulated totals.
* IN assertions: time_stamp, header_bytes and ifile_size are initialized.
*/
local void do_list(ifd, method)
int ifd;     /* input file descriptor */
int method;  /* compression method */
{
   ulg crc;  /* original crc */
   static int first_time = 1;
   static char* methods[MAX_METHODS] = {
      "store",  /* 0 */
         "compr",  /* 1 */
         "pack ",  /* 2 */
         "lzh  ",  /* 3 */
         "", "", "", "", /* 4 to 7 reserved */
         "defla"}; /* 8 */
      char *date;
      
      if (first_time && method >= 0) {
         first_time = 0;
         if (verbose)  {
            printf("method  crc     date  time  ");
         }
         if (!quiet) {
            printf("compressed  uncompr. ratio uncompressed_name\n");
         }
      } else if (method < 0) {
         if (total_in <= 0 || total_out <= 0) return;
         if (verbose) {
            printf("                            %9lu %9lu ",
               total_in, total_out);
         } else if (!quiet) {
            printf("%9ld %9ld ", total_in, total_out);
         }
         display_ratio(total_out-(total_in-header_bytes), total_out, stdout);
         /* header_bytes is not meaningful but used to ensure the same
         * ratio if there is a single file.
         */
         printf(" (totals)\n");
         return;
      }
      crc = (ulg)~0; /* unknown */
      bytes_out = -1L;
      bytes_in = ifile_size;
      
#if RECORD_IO == 0
      if (method == DEFLATED && !last_member) {
      /* Get the crc and uncompressed size for gzip'ed (not zip'ed) files.
      * If the lseek fails, we could use read() to get to the end, but
      * --list is used to get quick results.
      * Use "gunzip < foo.gz | wc -c" to get the uncompressed size if
      * you are not concerned about speed.
         */
         bytes_in = (long)rw_base_lseek_(ifd, (off_t)(-8), SEEK_END);
         if (bytes_in != -1L) {
            uch buf[8];
            bytes_in += 8L;
            if (rw_base_read_(ifd, (char*)buf, sizeof(buf)) != sizeof(buf)) {
               read_error();
            }
            crc       = LG(buf);
            bytes_out = LG(buf+4);
         }
      }
#endif /* RECORD_IO */
      date = ctime((time_t*)&time_stamp) + 4; /* skip the day of the week */
      date[12] = '\0';               /* suppress the 1/100sec and the year */
      if (verbose) {
         printf("%5s %08lx %11s ", methods[method], crc, date);
      }
      printf("%9ld %9ld ", bytes_in, bytes_out);
      if (bytes_in  == -1L) {
         total_in = -1L;
         bytes_in = bytes_out = header_bytes = 0;
      } else if (total_in >= 0) {
         total_in  += bytes_in;
      }
      if (bytes_out == -1L) {
         total_out = -1L;
         bytes_in = bytes_out = header_bytes = 0;
      } else if (total_out >= 0) {
         total_out += bytes_out;
						}
      display_ratio(bytes_out-(bytes_in-header_bytes), bytes_out, stdout);
      printf(" %s\n", ofname);
}

/* ========================================================================
* Return true if the two stat structures correspond to the same file.
*/
local int same_file(stat1, stat2)
struct stat *stat1;
struct stat *stat2;
{
   return stat1->st_ino   == stat2->st_ino
      && stat1->st_dev   == stat2->st_dev
#ifdef NO_ST_INO
      /* Can't rely on st_ino and st_dev, use other fields: */
      && stat1->st_mode  == stat2->st_mode
      && stat1->st_uid   == stat2->st_uid
      && stat1->st_gid   == stat2->st_gid
      && stat1->st_size  == stat2->st_size
      && stat1->st_atime == stat2->st_atime
      && stat1->st_mtime == stat2->st_mtime
      && stat1->st_ctime == stat2->st_ctime
#endif
      ;
}

/* ========================================================================
* Return true if a file name is ambiguous because the operating system
* truncates file names.
*/
local int name_too_long(name, statb)
char *name;           /* file name to check */
struct stat *statb;   /* stat buf for this file name */
{
			int s = strlen(name);
         char c = name[s-1];
         struct stat	tstat; /* stat for truncated name */
         int res;
         
         tstat = *statb;      /* Just in case OS does not fill all fields */
         name[s-1] = '\0';
         res = stat(name, &tstat) == 0 && same_file(statb, &tstat);
         name[s-1] = c;
         Trace((stderr, " too_long(%s) => %d\n", name, res));
         return res;
}

/* ========================================================================
* Shorten the given name by one character, or replace a .tar extension
* with .tgz. Truncate the last part of the name which is longer than
* MIN_PART characters: 1234.678.012.gz -> 123.678.012.gz. If the name
* has only parts shorter than MIN_PART truncate the longest part.
* For decompression, just remove the last character of the name.
*
* IN assertion: for compression, the suffix of the given name is z_suffix.
*/
local void shorten_name(name)
char *name;
{
   int len;                 /* length of name without z_suffix */
   char *trunc = NULL;      /* character to be truncated */
   int plen;                /* current part length */
   int min_part = MIN_PART; /* current minimum part length */
   char *p;
   
   len = strlen(name);
   if (decompress) {
      if (len <= 1) error("name too short");
      name[len-1] = '\0';
      return;
   }
   p = get_suffix(name);
   if (p == NULL) error("can't recover suffix\n");
   *p = '\0';
   save_orig_name = 1;
   
   /* compress 1234567890.tar to 1234567890.tgz */
   if (len > 4 && strequ(p-4, ".tar")) {
      strcpy(p-4, ".tgz");
      return;
   }
   /* Try keeping short extensions intact:
   * 1234.678.012.gz -> 123.678.012.gz
   */
   do {
      p = strrchr(name, PATH_SEP);
      p = p ? p+1 : name;
      while (*p) {
         plen = strcspn(p, PART_SEP);
         p += plen;
         if (plen > min_part) trunc = p-1;
         if (*p) p++;
						}
   } while (trunc == NULL && --min_part != 0);
   
   if (trunc != NULL) {
      do {
         trunc[0] = trunc[1];
						} while (*trunc++);
      trunc--;
   } else {
      trunc = strrchr(name, PART_SEP[0]);
      if (trunc == NULL) error("internal error in shorten_name");
      if (trunc[1] == '\0') trunc--; /* force truncation */
   }
   strcpy(trunc, z_suffix);
}

/* ========================================================================
* If compressing to a file, check if ofname is not ambiguous
* because the operating system truncates names. Otherwise, generate
* a new ofname and save the original name in the compressed file.
* If the compressed file already exists, ask for confirmation.
*    The check for name truncation is made dynamically, because different
* file systems on the same OS might use different truncation rules (on SVR4
* s5 truncates to 14 chars and ufs does not truncate).
*    This function returns -1 if the file must be skipped, and
* updates save_orig_name if necessary.
* IN assertions: save_orig_name is already set if ofname has been
* already truncated because of NO_MULTIPLE_DOTS. The input file has
* already been open and istat is set.
*/
local int check_ofname()
{
   struct stat	ostat; /* stat for ofname */
   
#ifdef ENAMETOOLONG
                      /* Check for strictly conforming Posix systems (which return ENAMETOOLONG
                      * instead of silently truncating filenames).
   */
   errno = 0;
   while (stat(ofname, &ostat) != 0) {
						if (errno != ENAMETOOLONG) return 0; /* ofname does not exist */
                  shorten_name(ofname);
   }
#else
   if (stat(ofname, &ostat) != 0) return 0;
#endif
   /* Check for name truncation on existing file. Do this even on systems
   * defining ENAMETOOLONG, because on most systems the strict Posix
   * behavior is disabled by default (silent name truncation allowed).
   */
   if (!decompress && name_too_long(ofname, &ostat)) {
						shorten_name(ofname);
                  if (stat(ofname, &ostat) != 0) return 0;
   }
   
   /* Check that the input and output files are different (could be
   * the same by name truncation or links).
   */
   if (same_file(&istat, &ostat)) {
						if (strequ(ifname, ofname)) {
                     fprintf(stderr, "%s: %s: cannot %scompress onto itself\n",
                        progname, ifname, decompress ? "de" : "");
                  } else {
                     fprintf(stderr, "%s: %s and %s are the same file\n",
                        progname, ifname, ofname);
                  }
                  exit_code = ERRORGZIP;
                  return ERRORGZIP;
   }
   /* Ask permission to overwrite the existing file */
   if (!force) {
						char response[80];
                  strcpy(response,"n");
                  fprintf(stderr, "%s: %s already exists;", progname, ofname);
                  if (foreground && isatty(fileno(stdin))) {
                     fprintf(stderr, " do you wish to overwrite (y or n)? ");
                     fflush(stderr);
                     (void)fgets(response, sizeof(response)-1, stdin);
                  }
                  if (tolow(*response) != 'y') {
                     fprintf(stderr, "\tnot overwritten\n");
                     if (exit_code == OK) exit_code = WARNING;
                     return ERRORGZIP;
                  }
   }
   (void) chmod(ofname, 0777);
   if (unlink(ofname)) {
						fprintf(stderr, "%s: ", progname);
                  perror(ofname);
                  exit_code = ERRORGZIP;
                  return ERRORGZIP;
   }
   return OK;
}


#ifndef NO_UTIME
/* ========================================================================
* Set the access and modification times from the given stat buffer.
*/
local void reset_times (name, statb)
char *name;
struct stat *statb;
{
   struct utimbuf	timep;
   
   /* Copy the time stamp */
   timep.actime  = statb->st_atime;
   timep.modtime = statb->st_mtime;
   
   /* Some systems (at least OS/2) do not support utime on directories */
   if (utime(name, &timep) && !S_ISDIR(statb->st_mode)) {
						WARN((stderr, "%s: ", progname));
                  if (!quiet) perror(ofname);
   }
}
#endif


/* ========================================================================
* Copy modes, times, ownership from input file to output file.
* IN assertion: to_stdout is false.
*/
local void copy_stat(ifstat)
struct stat *ifstat;
{
#ifndef NO_UTIME
   if (decompress && time_stamp != 0 && ifstat->st_mtime != time_stamp) {
      ifstat->st_mtime = time_stamp;
      if (verbose > 1) {
         fprintf(stderr, "%s: time stamp restored\n", ofname);
      }
   }
   reset_times(ofname, ifstat);
#endif
   /* Copy the protection modes */
   if (chmod(ofname, ifstat->st_mode & 07777)) {
      WARN((stderr, "%s: ", progname));
      if (!quiet) perror(ofname);
   }
#ifndef NO_CHOWN
   chown(ofname, ifstat->st_uid, ifstat->st_gid);  /* Copy ownership */
#endif
   remove_ofname = 0;
   /* It's now safe to remove the input file: */
   (void) chmod(ifname, 0777);
   if (unlink(ifname)) {
      WARN((stderr, "%s: ", progname));
      if (!quiet) perror(ifname);
   }
}

#ifndef NO_DIR

/* ========================================================================
* Recurse through the given directory. This code is taken from ncompress.
*/
local void treat_dir(dir)
char *dir;
{
   dir_type *dp;
   DIR      *dirp;
   char     nbuf[MAX_PATH_LEN];
   int      len;
   
   dirp = opendir(dir);
   
   if (dirp == NULL) {
      fprintf(stderr, "%s: %s unreadable\n", progname, dir);
      exit_code = ERRORGZIP;
      return ;
   }
   /*
   ** WARNING: the following algorithm could occasionally cause
   ** compress to produce error warnings of the form "<filename>.gz
   ** already has .gz suffix - ignored". This occurs when the
   ** .gz output file is inserted into the directory below
   ** readdir's current pointer.
   ** These warnings are harmless but annoying, so they are suppressed
   ** with option -r (except when -v is on). An alternative
   ** to allowing this would be to store the entire directory
   ** list in memory, then compress the entries in the stored
   ** list. Given the depth-first recursive algorithm used here,
   ** this could use up a tremendous amount of memory. I don't
   ** think it's worth it. -- Dave Mack
   ** (An other alternative might be two passes to avoid depth-first.)
   */
   
   while ((dp = readdir(dirp)) != NULL) {
      
      if (strequ(dp->d_name,".") || strequ(dp->d_name,"..")) {
									continue;
      }
      len = strlen(dir);
      if (len + NLENGTH(dp) + 1 < MAX_PATH_LEN - 1) {
									strcpy(nbuf,dir);
                           if (len != 0 /* dir = "" means current dir on Amiga */
#ifdef PATH_SEP2
                              && dir[len-1] != PATH_SEP2
#endif
#ifdef PATH_SEP3
                              && dir[len-1] != PATH_SEP3
#endif
                              ) {
                              nbuf[len++] = PATH_SEP;
                           }
                           strcpy(nbuf+len, dp->d_name);
                           treat_file(nbuf);
      } else {
									fprintf(stderr,"%s: %s/%s: pathname too long\n",
                              progname, dir, dp->d_name);
                           exit_code = ERRORGZIP;
      }
   }
   closedir(dirp);
}
#endif /* ? NO_DIR */

/* ========================================================================
* Free all dynamically allocated variables and exit with the given code.
*/
local void do_exit(exitcode)
int exitcode;
{
   static int in_exit = 0;
   
   if (in_exit) exit(exitcode);
   in_exit = 1;
   if (env != NULL)  free(env),  env  = NULL;
   if (args != NULL) free((char*)args), args = NULL;
   FREE(inbuf);
   FREE(outbuf);
   FREE(d_buf);
   FREE(window);
#ifndef MAXSEG_64K
   FREE(tab_prefix);
#else
   FREE(tab_prefix0);
   FREE(tab_prefix1);
#endif
   exit(exitcode);
}

/* ========================================================================
* Signal and error handler.
*/
RETSIGTYPE abort_gzip()
{
   if (remove_ofname) {
      close(ofd);
      unlink (ofname);
   }
   do_exit(ERRORGZIP);
}
#endif
#ifdef FGVGZIP



local int get_method(in)
int in;        /* input file descriptor */
{
   uch flags;     /* compression flags */
   char magic[2]; /* magic header */
   ulg stamp;     /* time stamp */
   off_t save_pos;
   
   /* If --force and --stdout, zcat == cat, so do not complain about
   * premature end of file: use try_byte instead of get_byte.
   */
   total_out = -1L;
   if (force && to_stdout) {
      magic[0] = (char)try_byte();
      magic[1] = (char)try_byte();
      /* If try_byte returned EOF, magic[1] == 0xff */
   } else {
      magic[0] = (char)get_byte();
      magic[1] = (char)get_byte();
   }
   method = -1;                 /* unknown yet */
   part_nb++;                   /* number of parts in gzip file */
   header_bytes = 0;
   last_member = RECORD_IO;
   /* assume multiple members in gzip file except for record oriented I/O */
   
   if (memcmp(magic, GZIP_MAGIC, 2) == 0 || memcmp(magic, DS_GZIP_MAGIC, 2) == 0
      || memcmp(magic, OLD_GZIP_MAGIC, 2) == 0) {
      
      method = (int)get_byte();
      if (method != DEFLATED) {
         fprintf(stderr,
            "%s: %s: unknown method %d -- get newer version of gzip\n",
            progname, ifname, method);
         exit_code = ERRORGZIP;
         return -1;
      }
      work = ds___unzip;
      if (in > 0 /*&& !isatty(in)*/)
      {
         save_pos = rw_base_lseek_(in, (off_t)(0), SEEK_CUR);
         rw_base_lseek_(in, (off_t)(-4), SEEK_END);
         {
            uch buf[4];
            rw_base_read_(in, (char*)buf, sizeof(buf));
            total_out = LG(buf);
         }
         rw_base_lseek_(in,save_pos, SEEK_SET);
      }
      
      flags  = (uch)get_byte();
      
      if ((flags & ENCRYPTED) != 0) 
	  {
         fprintf(stderr,
            "%s: %s is encrypted -- get newer version of gzip\n",
            progname, ifname);
         exit_code = ERRORGZIP;
         return -1;
      }
      if ((flags & CONTINUATION) != 0) {
         fprintf(stderr,
            "%s: %s is a a multi-part gzip file -- get newer version of gzip\n",
            progname, ifname);
         exit_code = ERRORGZIP;
         if (force <= 1) return -1;
      }
      if ((flags & RESERVED) != 0) {
         fprintf(stderr,
            "%s: %s has flags 0x%x -- get newer version of gzip\n",
            progname, ifname, flags);
         exit_code = ERRORGZIP;
         if (force <= 1) return -1;
      }
      stamp  = (ulg)get_byte();
      stamp |= ((ulg)get_byte()) << 8;
      stamp |= ((ulg)get_byte()) << 16;
      stamp |= ((ulg)get_byte()) << 24;
      if (stamp != 0 && !no_time) time_stamp = stamp;
      
      (void)get_byte();  /* Ignore extra flags for the moment */
      (void)get_byte();  /* Ignore OS type for the moment */
      
      if ((flags & CONTINUATION) != 0) {
         unsigned part = (unsigned)get_byte();
         part |= ((unsigned)get_byte())<<8;
         if (verbose) {
            fprintf(stderr,"%s: %s: part number %u\n",
               progname, ifname, part);
         }
      }
      if ((flags & EXTRA_FIELD) != 0) {
         unsigned len = (unsigned)get_byte();
         len |= ((unsigned)get_byte())<<8;
         if (verbose) {
            fprintf(stderr,"%s: %s: extra field of %u bytes ignored\n",
               progname, ifname, len);
         }
         while (len--) (void)get_byte();
      }
      
      /* Get original file name if it was truncated */
      if ((flags & ORIG_NAME) != 0) {
         if (no_name || (to_stdout && !list) || part_nb > 1) {
            /* Discard the old name */
            char c; /* dummy used for NeXTstep 3.0 cc optimizer bug */
            do {c=get_byte();} while (c != 0);
         } else {
            /* Copy the base name. Keep a directory prefix intact. */
            char *p = basename(ofname);
            char *base = p;
            for (;;) {
               *p = (char)get_char();
               if (*p++ == '\0') break;
               if (p >= ofname+sizeof(ofname)) {
                  error("corrupted input -- file name too large");
															}
            }
            /* If necessary, adapt the name to local OS conventions: */
            if (!list) {
               MAKE_LEGAL_NAME(base);
               if (base) list=0; /* avoid warning about unused variable */
            }
         } /* no_name || to_stdout */
      } /* ORIG_NAME */
      
      /* Discard file comment if any */
      if ((flags & COMMENT) != 0) {
         while (get_char() != 0) /* null */ ;
      }
      if (part_nb == 1) {
         header_bytes = inptr + 2*sizeof(long); /* include crc and size */
      }
      
    } else if (memcmp(magic, PKZIP_MAGIC, 2) == 0 && inptr == 2
       && memcmp((char*)inbuf, PKZIP_MAGIC, 4) == 0) {
       /* To simplify the code, we support a zip file when alone only.
       * We are thus guaranteed that the entire local header fits in inbuf.
       */
       inptr = 0;
       work = ds___unzip;
       if (check_zipfile(in) != OK) return -1;
       /* check_zipfile may get ofname from the local header */
       last_member = 1;
       
    } else if (memcmp(magic, PACK_MAGIC, 2) == 0) {
       work = unpack;
       method = PACKED;
       
			 } else if (memcmp(magic, LZW_MAGIC, 2) == 0) {
       work = unlzw;
       method = COMPRESSED;
       last_member = 1;
       
          } else if (memcmp(magic, LZH_MAGIC, 2) == 0) {
             work = unlzh;
             method = LZHED;
             last_member = 1;
             
          } else if (force && to_stdout && !list) { /* pass input unchanged */
             method = STORED;
             work = copy;
             inptr = 0;
             last_member = 1;
          }
          if (method >= 0) return method;
          
          if (part_nb == 1) {
             fprintf(stderr, "\n%s: %s: not in gzip format\n", progname, ifname);
             exit_code = ERRORGZIP;
             return -1;
          } else {
             return -2;
          }
}

static int hay_errores = 0;

void abort_gzip()
{
	hay_errores++;
   /*aviso(0,__Texto[165]);*/
   return;
}


int ds___gzip(fin,fout,modo,nivel)
int fin,fout,modo,nivel;
{
   int r = 0;

   hay_errores = 0;
   
   if (nivel == -1)
   {
      goto sale_sale;
   }
   
   abandonalo_ya = 0;
   level = nivel;
   if (modo == 10)
   {
	   modo = 1;
	   test = 1;
   }
   else
   {
	   test = 0;
   }
   decompress = modo;
   strcpy(ifname, "agin");
   strcpy(ofname, "agiout");    
   time_stamp = 0;
   ifile_size = -1L; /* convention for unknown size */
   to_stdout = 1;
   part_nb = 0;
   if (no_time < 0) no_time = decompress;
   if (no_name < 0) no_name = decompress;
   
   
   if (gzip_asignada_memoria < 1)
   {
      ALLOC(uch, inbuf,  INBUFSIZ +INBUF_EXTRA);
      ALLOC(uch, outbuf, OUTBUFSIZ+OUTBUF_EXTRA);
      ALLOC(ush, d_buf,  DIST_BUFSIZE);
      ALLOC(uch, window, 2L*WSIZE);
#ifndef MAXSEG_64K
      ALLOC(ush, tab_prefix, 1L<<BITS);
#else
      ALLOC(ush, tab_prefix0, 1L<<(BITS-1));
      ALLOC(ush, tab_prefix1, 1L<<(BITS-1));
#endif
      if (gzip_asignada_memoria > -1)
         gzip_asignada_memoria++;
   }
   
   clear_bufs(); /* clear input and output buffers */
   ifd = fin;
   ofd = fout;
   
   if (decompress) 
   {
      method = get_method(fin);
      if (method < 0) 
      {
         r = -1;
         goto sale_sale;
      }
   }
   else
      work = ds___zip;
   
      /* Actually do the compression/decompression. Loop over zipped members.
   */
   for (;;) 
   {
      if ((*work)(fin,fout) != OK) {r = -1;break;}
      break;
      
      /*
      if (!decompress || last_member || inptr == insize) break;
      
        
          method = get_method(fin);
          if (method < 0) return(-1); 
          bytes_out = 0;            
      */	  
   }
   
sale_sale:
   if (gzip_asignada_memoria < 0)
			{
      FREE(inbuf);
      FREE(outbuf);
      FREE(d_buf);
      FREE(window);
#ifndef MAXSEG_64K
      FREE(tab_prefix);
#else
      FREE(tab_prefix0);
      FREE(tab_prefix1);
#endif
			}
   
   
   if (hay_errores)
   {
	   r -= hay_errores;
	   hay_errores = 0;	   
   }

   return(r);
}

#endif