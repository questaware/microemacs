/* File name	- logmsg.c */
#include   <stdio.h>
#include   <ctype.h>
#include   <stdlib.h>
#include   <stdarg.h>
#include   <time.h>
#include   <sys/types.h>
#include   <sys/stat.h>
#include   <errno.h>
#include   <string.h>
#include   <limits.h>

#include   "estruct.h"
/*#define logmsgh				* stop inclusion of prototype */
#include   "logmsg.h"

#define TRUE 1
#define FALSE 0

#ifndef null
#define null 0L
#endif

#define LOG_MAXSZ     1600
#define LOG_LEEWAY_CT 8


#define LOG_MAX_FILE 50000


#if LOGGING_ON

char *log_file_name = NULL;
FILE *log_file = NULL;
int  log_process;

time_t log_last_time = 0;

int  log_max_file = 10000;

int  log_err_thresh = Q_LOG_INFO;

int  log_stderr_clamp = 4;

int  log_leeway_ct = LOG_LEEWAY_CT;

int  log_last_size = 0;



/* Warning, these descriptors must co-relate the definitions in logmsg.h */
static char severity_text [] = { 'D',
				 'L',
				 'I',
				 'W',
				 'E',
				 'S',
				 'F' };


void log_init(char * filename, int filesize, int severity)

{ log_file_name = filename;
  if (log_file != null && log_file != stderr)
  { fclose(log_file);
    log_file = null;
    log_last_time = 0;
  }

  log_max_file = filesize == 0 ? LOG_MAX_FILE : filesize;
  log_err_thresh = severity;
  log_last_size = 0;
}



static void log_restart( logfile, filename, max_size )
  FILE  **logfile;
  char  *filename;
  off_t  max_size;   
{
  if ( *logfile == NULL )
    if ( filename == NULL )
      *logfile = stderr;
    else
    { *logfile = fopen( filename, "a" );
      if( *logfile == NULL )
      { if (--log_stderr_clamp >= 0)
          fprintf( stderr, "Unable to open logfile '%s' (%d)\n", filename,
                                   ferror( *logfile ) );
      }
    }

  if ( *logfile != NULL && *logfile != stderr && max_size != 0 )
  { char cmdbuf[535], fn[500];
  
    if (log_last_size == 0 || --log_leeway_ct < 0)
    { struct stat filstat;
      fstat( fileno( *logfile ), &filstat );
      log_leeway_ct = LOG_LEEWAY_CT;
      log_last_size = filstat.st_size;
    }

    if ( log_last_size >= max_size && strlen(filename) < sizeof(cmdbuf))
    { char *cmd = cmdbuf;
      char *filename_ = fn;
      if (cmd != null && filename_ != null)
      { char *ptr = strrchr( filename_, '.' );
        if ( ptr != NULL )
	  *( ptr ) = 0;

        fclose( *logfile );

        eprintf( cmd, "move %s.bak %s.BAK", filename, filename_ );
        if ( ttsystem( cmd , NULL ) < 0 )
  	      fprintf( stderr, "system(%d): %s\n", errno, cmd );

        eprintf( cmd, "move %s %s.bak", filename, filename_ );
        if ( ttsystem( cmd , NULL ) < 0 )
  	{     fprintf( stderr, "system(%d): %s\n", errno, cmd );
  	      log_max_file = INT_MAX;
  	}

        *logfile = fopen( filename, "a" );
        if( *logfile == NULL )
  	       fprintf( stderr, "Unable (%d) to re-open logfile %s\n",
  				   errno,  filename );
      }
    }
  }
}


/* VARARGS */
void logmsg( int severity, const char * format, ...)
{
  if ( severity < log_err_thresh )
    return;

  log_restart( &log_file, log_file_name, log_max_file);

  if ( log_file == NULL )
    return;

{ time_t time_stamp = time(null);

  if (time_stamp != log_last_time)
  { 
    struct tm *time_brk = localtime( &time_stamp );
    log_last_size += 
      fprintf(log_file, "%.2d:%.2d:%.2d %.2d/%.2d/%0.2d\n", 
	          time_brk->tm_hour,
        	  time_brk->tm_min,
	          time_brk->tm_sec,
        	  time_brk->tm_mday,
	          time_brk->tm_mon,
        	  (time_brk->tm_year > 99) ? time_brk->tm_year - 100
                                           : time_brk->tm_year );
    log_last_time = time_stamp;
  }

  if (log_process == 0)
    log_process = /* getpid(); */ 0;

  log_last_size += 
    log_file == stderr
      ? fprintf(log_file, "%c ", severity_text[severity-1])
      : fprintf(log_file, "%d %c ", log_process, severity_text[severity-1]);

{ int len = strlen( format );

  va_list ap;
  va_start(ap, format);

  log_last_size += 
    vfprintf( log_file, format, ap );
  
  if (format[len-1] != '\n')
    fputc('\n', log_file);

  if (log_file != stderr)  
    fflush(log_file);
  va_end( ap );
}}}


		/* Unfortunately we have to copy the code */
		/* This is the interface for class lout */

void logmsg0( int severity, const char * message )
{
  if ( severity < log_err_thresh )
    return;

  log_restart( &log_file, log_file_name, log_max_file);

  if ( log_file == NULL )
    return;

{ time_t time_stamp = time(null);

  if (time_stamp != log_last_time)
  { 
    struct tm *time_brk = localtime( &time_stamp );
    log_last_size += 
      fprintf(log_file, "%.2d:%.2d:%.2d %.2d/%.2d/%0.2d\n", 
	          time_brk->tm_hour,
        	  time_brk->tm_min,
	          time_brk->tm_sec,
        	  time_brk->tm_mday,
	          time_brk->tm_mon,
        	  (time_brk->tm_year > 99) ? time_brk->tm_year - 100
                                           : time_brk->tm_year );
    log_last_time = time_stamp;
  }

  if (log_process == 0)
    log_process = /* getpid(); */ 0;

  log_last_size += 
    log_file == stderr
      ? fprintf(log_file, "%c ", severity_text[severity-1])
      : fprintf(log_file, "%d %c ", log_process, severity_text[severity-1]);

{ int len = strlen( message );
  log_last_size += 
    fprintf( log_file, message );
  
  if (message[len-1] != '\n')
    fputc('\n', log_file);

  if (log_file != stderr)  
    fflush(log_file);
}}}

#endif
