/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// sys_null.h -- null system driver to aid porting efforts

#include "quakedef.h"
#include "errno.h"

qboolean isDedicated;

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES 10
int sys_handles[MAX_HANDLES];

int 
findhandle (void)
{
	int             i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength (int fd)
{
	return fsize(fd);
}

int Sys_FileOpenRead (char *path, int *hndl)
{
	int i = findhandle();

	int fd = open(path, O_RDONLY)

	if (fd < 0) {
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = fd;
	*hndl = i;
	return filelength(fd);
}

int Sys_FileOpenWrite (char *path)
{

	int i = findhandle ();
	int fd = open(path, O_CREATE | O_WRONLY);

	if (fd < 0) return Sys_Error ("Error opening %s\n", path);

	sys_handles[i] = fd;
	return i;
}

void Sys_FileClose (int handle)
{
	close(sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void Sys_FileSeek (int handle, int position)
{
	seek(sys_handles[handle], (off_t) position);
}

int Sys_FileRead (int handle, void *dest, int count)
{
	return read(sys_handles[handle], dest, count);
}

int Sys_FileWrite (int handle, void *data, int count)
{
	return write(sys_handles[handle], data, count);
}

int Sys_FileTime (char *path)
{
	int fd = open(path, O_RDONLY);

	if (fd) {
		close(fd);
		return 1;
	}
	return -1;
}

void Sys_mkdir (char *path)
{
}


/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
}


void Sys_Error (char *error, ...)
{
	printf ("Sys_Error: ");   
	printf(error, ...);
	exit();
}

int Sys_Printf (char *fmt, ...)
{
	printf(fmt, ...);
}

void Sys_Quit (void)
{
	exit();
}

double Sys_FloatTime (void)
{
	static double t;
	t += 0.1;
	return t;
}

char *Sys_ConsoleInput (void)
{
	return NULL;
}

void Sys_Sleep (void)
{
}

void Sys_SendKeyEvents (void)
{
}

void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}

//=============================================================================

/*

void main (int argc, char **argv)
{
	static quakeparms_t    parms;

	parms.memsize = 8*1024*1024;
	parms.membase = malloc (parms.memsize);
	parms.basedir = ".";

	COM_InitArgv (argc, argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	printf ("Host_Init\n");
	Host_Init (&parms);
	while (1)
	{
		Host_Frame (0.1);
	}
}

*/
