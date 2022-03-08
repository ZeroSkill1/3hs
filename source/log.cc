/*
 * Copyright (C) 2021-2022 MyPasswordIsWeak
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "log_view.hh"
#include "panic.hh"
#include "log.hh"

#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <3ds.h>

#define GEN_MAX_LEN 100

static FILE *log_file = NULL;
static LightLock file_lock = -1;


static FILE *open_f()
{
	mkdir("/3ds", 0777);
	mkdir("/3ds/3hs", 0777);
	log_file = fopen("/3ds/3hs/3hs.log", "a");
	if(log_file) fseek(log_file, 0, SEEK_END);
	return log_file;
}

static void write_string(const char *s)
{
	LightLock_Lock(&file_lock);
#ifndef RELEASE
	fputs(s, stderr);
#endif

	if(log_file)
	{
		fputs(s, log_file);
#ifndef RELEASE
		fflush(log_file);
#endif
	}
	LightLock_Unlock(&file_lock);
}

void log_init()
{
#ifndef RELEASE
	consoleDebugInit(debugDevice_SVC);
#endif

	LightLock_Init(&file_lock);
	open_f();
}

void log_del()
{
	if(log_file) fclose(log_file);
	open_f();
}

void log_exit()
{
	if(log_file) fclose(log_file);
	if(file_lock != -1)
		LightLock_Unlock(&file_lock);
}

void _logf(const char *fnname, const char *filen,
	size_t line, LogLevel lvl, const char *fmt, ...)
{
//	if(lvl > MAX_LVL)
//		return; /* check is done in header */

	const char *lvl_s;
	switch(lvl)
	{
	case LogLevel::fatal:
		lvl_s = "FATAL";
		break;
	case LogLevel::error:
		lvl_s = "ERROR";
		break;
	case LogLevel::warning:
		lvl_s = "WARNING";
		break;
	case LogLevel::info:
		lvl_s = "INFO";
		break;
	case LogLevel::debug:
		lvl_s = "DEBUG";
		break;
	case LogLevel::verbose:
		lvl_s = "VERBOSE";
		break;
	default:
		panic("EINVAL");
		return;
	}

	time_t now = time(NULL);
	struct tm *tm = gmtime(&now);

#define TIME_ARGS tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec
#ifdef RELEASE
	#define FMT "%i-%i-%i %02i:%02i:%02i %s [%s@%i] "
	#define ARGS TIME_ARGS, lvl_s, fnname, line
	(void) filen; /* always NULL on release anyways */
#else
	#define FMT "%i-%i-%i %02i:%02i:%02i %s [%s:%s@%i] "
	#define ARGS TIME_ARGS, lvl_s, filen, fnname, line
#endif

	va_list va;
	va_start(va, fmt);
	int ulen = vsnprintf(NULL, 0, fmt, va);
	va_end(va);
	char *s = (char *) malloc(ulen + GEN_MAX_LEN + 2);
	int start = snprintf(s, GEN_MAX_LEN, FMT, ARGS);
	va_start(va, fmt);
	vsprintf(s + start, fmt, va);
	va_end(va);
	s[start + ulen + 0] = '\n';
	s[start + ulen + 1] = '\0';

	write_string(s);
	free(s);

#undef TIME_ARGS
#undef ARGS
#undef FMT
}

