#include <stdio.h>
#include <string.h>
#include <conio.h>

#define R 0

const char* REKT = "REKT<>+-\0";
const char* BF   = "><+-.,[]\0";
char *mem;
FILE *f;
int memsize = 1024;
char *program;
int pos;

inline void PrintHelp( void )
{
	printf("Usage:\n\n");
	printf("REKT -f file [-s] [-b]\n\n");
	printf("-s size   - memory size in bytes (1024 by default)\n");
	printf("-b        - enable brainfuck mode\n\n");
	printf("Example:\n");
	printf("REKT -f hello.rekt -s 256");
}

inline int FileLength( void )
{
	fseek( f, 0, SEEK_END );
	int ret = ftell( f );
	fseek( f, 0, SEEK_SET );

	return ret;
}

int Open(const char *file)
{
	f = fopen( file, "rt" );
	if( !f )
	{
		printf("Can't open '%s'!\n", file);
		return 0;
	}
	int psize = FileLength( );
	program = malloc( psize+1 );
	fread( program, 1, psize, f );
	program[psize] = 0;

	return 1;
}

inline void CloseFile( void )
{
	if( program )
		free( program );

	if( f )
		fclose( f );
}

void Fall( int dir )
{
	int opens = 0;
	while( !opens && program[ pos += dir ? 1 : -1 ] != REKT[ R + 6 + dir ] )
		opens += (program[pos] == REKT[R + 6 + dir]) ? 1 : 0;
	if( !dir )
		pos--;
}

int ProcessChar( char c )
{
	int d;
	switch( (d = strchr( REKT, c ) - REKT ) )
	{
		case R: mem++; return 1;
		case R+1: mem--; return 1;
		case R+2: (*mem)++; return 1;
		case R+3: (*mem)--; return 1;
		case R+4: putc( *mem, stdout ); return 1;
		case R+5: *mem = getch( ); return 1;
		case R+6:
		    {
				if(!(*mem))
					Fall( 1 );
				return 1;
			};
		case R+7: Fall( 0 ); return 1;
		default: return 0;
	}
}

int main( int argc, char *argv[] )
{
	if(argc < 2)
	{
		PrintHelp( );
		return -1;
	}
	for(int i=0; i<argc; i++)
	{
		if( !strcmp("-f", argv[i] ) )
		{
			if( !Open(  argv[++i] ) )
				goto cleanup;

			continue;
		}

		if( !strcmp( "-s", argv[i] ) )
		{
			if( ! (memsize = atoi( argv[++i] ) ) )
			{
				printf("Memsize cannot be zero!\n");
				goto cleanup;
			}

			continue;
		}

		if( !strcmp( "-b", argv[i] ) )
		{
			printf("Setting brainfuck mode\n\n");
			REKT = BF;
		}
	}

	mem = malloc( memsize );
	if(!mem)
	{
		printf("Memory could not be allocated\n");
		goto error;
	}
	memset( mem, 0, memsize );

	char *beg = mem;

	while( 1 )
	{
		if( !ProcessChar( program[pos] ) )
			goto error;

		pos++;
		if( !program[pos] )
			goto cleanup;
	}

error:
	printf("An error occoured\n");

cleanup:
	mem = beg;

	if(mem)
		free(mem);

	CloseFile( );
}
