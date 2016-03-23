#include <stdio.h>
#include <stdarg.h>

#include "log.h"
#include "search.h"
#include "util.h"

static enum log_level log_threshold = LOG_LEVEL_ERR;

void set_log_level(enum log_level threshold) {
    log_threshold = threshold;
}

void log_debug_actual(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vplog(LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);
}

void log_msg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vplog(LOG_LEVEL_MSG, fmt, args);
    va_end(args);
}

void log_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vplog(LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

void log_err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vplog(LOG_LEVEL_ERR, fmt, args);
    va_end(args);
}

void vplog(const unsigned int level, const char *fmt, va_list args) {
    if (level < log_threshold) {
        return;
    }

    FILE *stream = out_fd;

    /*
     * At early program startup, mutiple threads are not running,
     * so this routine is thread safe.  Later...not so much.
     */
    if (print_mtx_initialized) {
        pthread_mutex_lock(&print_mtx);
    }

    switch (level) {
        case LOG_LEVEL_DEBUG:
            fprintf(stream, "DEBUG: ");
            break;
        case LOG_LEVEL_MSG:
            fprintf(stream, "MSG: ");
            break;
        case LOG_LEVEL_WARN:
            fprintf(stream, "WARN: ");
            break;
        case LOG_LEVEL_ERR:
            stream = stderr;
            fprintf(stream, "ERR: ");
            break;
    }

    vfprintf(stream, fmt, args);
    fprintf(stream, "\n");

    if (print_mtx_initialized) {
        pthread_mutex_unlock(&print_mtx);
    }
}

void plog(const unsigned int level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vplog(level, fmt, args);
    va_end(args);
}


void bt(char* p , int l)
{
	void* tracePtrs[100];
	char buf[256];
	int ii;
	FILE* f;
	int count = backtrace( tracePtrs, 100 );

	// prepare command to be executed
	// our program need to be passed after the -e parameter
	if (print_mtx_initialized)
		pthread_mutex_lock(&print_mtx);

	printf("!!! backtrace from --> %s:%d\n", p,l);

	// Print the stack trace
	for(ii = 1; ii < count-2; ii++ )
	{
		sprintf (buf, "/tools/bin/addr2line -C -e /home/danbe/test/git/the_silver_searcher/ag -f -i 0x%lx", (long unsigned int) tracePtrs[ii]);
		f = popen (buf, "r");

		if(f == NULL)
		{
			perror (buf);
		    if (print_mtx_initialized) 
        		pthread_mutex_unlock(&print_mtx);
			return;
		}

		// get function name
		fgets(buf, 256, f);
		printf( "  %s", buf);
		pclose(f);

	}	
	// Free the string pointers
    if (print_mtx_initialized)
        pthread_mutex_unlock(&print_mtx);

}

