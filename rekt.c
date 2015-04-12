#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R 0

const char* BF = "><+-.,[] \n\r\0";
char *mem;
FILE *f, *input;
int memsize = 1024;
char *program;
int pos;

void PrintHelp( void )
{
	printf( "Usage:\n\n" );
	printf( "REKT -f file [-s] [-i]\n\n" );
	printf( "-m size   - memory size in bytes (1024 by default)\n" );
	printf( "-i        - set the input file\n\n" );
	printf( "Example:\n" );
	printf( "REKT -f hello.bf -s 256\n" );
}

int FileLength( void )
{
	fseek( f, 0, SEEK_END );
	int ret = ftell( f );
	fseek( f, 0, SEEK_SET );

	return ret;
}

int Open( const char *file )
{
	f = fopen( file, "rt" );
	if ( !f )
	{
		printf( "Can't open '%s'!\n", file );
		return 0;
	}
	int psize = FileLength( );
	program = malloc( psize + 1 );
	fread( program, 1, psize, f );
	program[psize] = 0;

	return 1;
}

void CloseFile( void )
{
	if ( program )
		free( program );

	if ( f )
		fclose( f );
}

void FallBack( )
{
	int closes = 0;
	while ( 1 )
	{
		--pos;
		if ( program[pos] == BF[R + 6] )
			if ( !closes )
			{
				pos--;
				return;
			}
			else
				closes--;
		if ( program[pos] == BF[R + 7] )
			closes++;
	}
}

void FallUp( )
{
	int opens = 0;
	while ( 1 )
	{
		pos++;
		if ( program[pos] == BF[R + 7] )
			if ( !opens )
				return;
			else
				opens--;

		if ( program[pos] == BF[R + 6] )
			opens++;
	}
}

int main( int argc, char *argv[] )
{
	if ( argc < 2 )
	{
		PrintHelp( );
		return -1;
	}
	for ( int i = 0; i<argc; i++ )
	{
		if ( !strcmp( "-f", argv[i] ) )
		{
			if ( !Open( argv[++i] ) )
				goto error;

			continue;
		}

		if ( !strcmp( "-m", argv[i] ) )
		{
			if ( !( memsize = atoi( argv[++i] ) ) )
			{
				printf( "Memsize cannot be zero!\n" );
				goto cleanup;
			}

			continue;
		}

		if ( !strcmp( "-i", argv[i] ) )
		{
			if ( !( input = fopen( argv[++i], "rt" ) ) )
			{
				printf( "Could not open input file!\n" );
				goto cleanup;
			}
		}
	}
	if ( !f )
	{
		printf( "No sorce file specified!\n" );
		goto error;
	}

	mem = malloc( memsize );
	if ( !mem )
	{
		printf( "Memory could not be allocated\n" );
		goto error;
	}
	memset( mem, 0, memsize );
	char *beg = mem;

	while ( 1 )
	{
		char *buffer;

		switch ( ( buffer = strchr( BF, program[pos] ) ) - BF )
		{
		case R: {
			if ( ( ( ++mem ) - beg ) > memsize )
			{
				printf( "Out of bounds!\n" );
				goto error;
			}
		} break;
		case R + 1: {
			if ( ( ( --mem ) - beg ) < 0 )
			{
				printf( "Out of bounds!\n" );
				goto error;
			}

		}break;
		case R + 2: ( *mem )++; break;
		case R + 3: ( *mem )--; break;
		case R + 4: putc( *mem, stdout ); break;
		case R + 5: *mem = fgetc( input ); break;
		case R + 6:
		{
			if ( !( *mem ) )
				FallUp( );
		}; break;
		case R + 7: FallBack( ); break;
		case R+8:
		case R+9:
		case R + 10: break;
		default: printf( "Unrecognized symbol!\n" ); goto error;
		}

		pos++;
		if ( !program[pos] )
			goto cleanup;
	}

error:
	printf( "An error occoured\n" );

cleanup:
	mem = beg;

	if ( mem )
		free( mem );

	CloseFile( );
	if ( input )
		fclose( input );
}
