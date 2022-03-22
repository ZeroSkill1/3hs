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

#include "settings.hh"
#include "log_view.hh"
#include "panic.hh"
#include "log.hh"

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <3ds.h>

#define F "/3ds/3hs/3hs.log"
#define GEN_MAX_LEN 100

static FILE *log_file = NULL;
static LightLock file_lock = -1;


static void reserve_main_log()
{
	if(access(F, F_OK) != 0)
		return; /* nothing to do ... */

	static char path1[] = F ".XXX";
	static char path2[] = F ".XXX";
	u8 max = get_settings()->maxLogs;
	u8 val;

	if(max == 0) /* always override; nothing to do */
		return;
	--max;

#define SET(p,i) sprintf(p + sizeof("/3ds/3hs/3hs.log.") - 1, "%u", (i)+1);
	for(val = 0; val != max; ++val)
	{
		SET(path1, val);
		if(access(path1, F_OK) != 0)
			break;
	}
	SET(path1, val);
	/* upper log number is now in val; shift all up */
	if(val == max) remove(path1); /* we need to discard one */
	for(; val != 0; --val)
	{
		SET(path1, val - 1);
		SET(path2, val);
		rename(path1, path2);
	}
	rename(F, F ".1");
#undef SET
}

static FILE *open_f()
{
	mkdir("/3ds", 0777);
	mkdir("/3ds/3hs", 0777);
	log_file = fopen(F, "a");
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

static void clear_all_logs()
{
	DIR *d = opendir("/3ds/3hs");
	struct dirent *e;
	while((e = readdir(d)))
	{
#define S(a) a, sizeof(a)
		if(strncmp(e->d_name, S("3hs.log")))
		{
#define L (sizeof("/3ds/3hs/"))
			static char path[256+L] = "/3ds/3hs/";
			strcpy(path + L, e->d_name);
			remove(path);
#undef L
		}
#undef S
	}
	closedir(d);
}

void log_init()
{
#ifndef RELEASE
	consoleDebugInit(debugDevice_SVC);
#endif

	LightLock_Init(&file_lock);
	reserve_main_log();
	open_f();
}

void log_del()
{
	if(log_file) fclose(log_file);
	clear_all_logs();
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

