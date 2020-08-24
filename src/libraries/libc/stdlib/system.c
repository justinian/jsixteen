/* system( const char * )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#include <stdlib.h>

/* This is an example implementation of system() fit for use with POSIX kernels.
*/

extern int fork( void );
extern int execve( const char * filename, char * const argv[], char * const envp[] );
extern int wait( int * status );

int system( const char * string )
{
    const char * argv[] = { "sh", "-c", NULL, NULL };
    argv[2] = string;
    if ( string != NULL )
    {
        int pid = fork();
        if ( pid == 0 )
        {
            execve( "/bin/sh", (char * * const)argv, NULL );
        }
        else if ( pid > 0 )
        {
            while( wait( NULL ) != pid );
        }
    }
    return -1;
}

