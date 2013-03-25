//
//  The tool removes C/C++ comment for standard input and output to standard output.
//  You use this module for Vim editor.
//
//	put this to your gvimrc file
//  "map f !delcmt.exe"
//
//  add "delcmt.exe" located path to 'Path' environment 
//  in windows control panel
//
//  example.
//      [before]
//
//		int main( int argc, char* argv[] ){ // I can't understand this comment
//          /* func1 */
//          func1(); // good comment
//
//			/* func2 */
//      	func2(); // bad comment
//
//      	/*
//      	 commented code
//      	 */
//      	
//      }
//
//      [after]
//
//		int main( int argc, char* argv[] ){
//          func1();
//
//      	func2();
//
//      	
//      }
//
// compiler : Microsoft Visual C++ 6.0 Japanese
//
// copyright 2009-2012 pebble8888@gmail.com
//
// known bug : sometimes Mojibake
//
// revision history :
//
// 		Ver1.0.0 2009-09 initial
// 		Ver1.1.0 2010-11 add function that delete more two blank line  
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char* read_stdin( size_t& out_len );
void delete_comment( char* p_in, size_t len, char* p_out, size_t& out_len );
void delete_blankline( char* p_in, size_t len, char* p_out, size_t& out_len );

/*
 * @brief main
 */
int main( int, char* [] ){
	size_t  stdin_len = 0;
	char*   p_stdin  = read_stdin( stdin_len );
	if( p_stdin == NULL ) {
		printf( "Out of memory\n" );
		return 1;
	}
	if( stdin_len > 0 ) {
		char* p_out = (char*)malloc( stdin_len );
		char* p_out2 = (char*)malloc( stdin_len );
		size_t out_len = 0;
		size_t out2_len = 0;

		delete_comment( p_stdin, stdin_len, p_out, out_len );

		delete_blankline( p_out, out_len, p_out2, out2_len );	

		// out to stdout 
		size_t i;
		for( i = 0; i < out2_len; ++i ){
#if 1
			putchar( p_out2[i] );
#else
			printf( "%02x ", p_out2[i] );
#endif
		}

		free( p_out );
		p_out = NULL;

		free( p_out2 );
		p_out2 = NULL;
	}
	free( p_stdin );
	p_stdin = NULL;
	stdin_len = 0;
	return 0;
}

/*
 * @brief
 * @note  you must free for retval
 */
char *read_stdin( size_t& out_len )
{
	char*  p_data;
	char*  new_data;
	int    data_size;
	int    data_len;
	int    len;

	/* Start with 64k */
	data_size = 64 * 1024;
	p_data    = (char *)malloc( data_size );
	data_len  = 0;

	if ( p_data == NULL ) {
		return( NULL );
	}
	while (( len = fread( &p_data[data_len], 1, data_size - data_len, stdin )) > 0 )
	{
		data_len += len;
		if ( data_len == data_size )
		{
			/* Double the buffer size */
			data_size *= 2;
			if (( new_data = (char *)realloc( p_data, data_size )) == NULL )
			{
				free( p_data );
				return( NULL );
			}
			p_data = new_data;
		}
	}

	assert( data_len < data_size );
	/* Make sure the buffer is terminated */
	p_data[data_len] = 0;

	out_len = data_len;

	return( p_data );
}

/*
 * @brief delete_comment
 * @param char*   p_in
 * @param size_t  in_len
 * @param char*   p_out
 * @param size_t& out_len
 */
void delete_comment( char* p_in, size_t in_len, char* p_out, size_t& out_len )
{
	char*   inbuf = p_in;
	int    	literal = 0;
	size_t  i; 		/* in counter */
	size_t	j = 0; 	
	char*   outbuf = p_out;

#if 0 // for debug
	printf( "in\n" );
	for( i = 0; i < in_len; ++i ) {
		printf( "%02x ", inbuf[i] );
	}
	printf( "\n" );
	printf( "out\n" );
#endif

	for( i = 0; i < in_len; ++i ) {
		if( !literal && inbuf[i] == '/' && ( i+1 ) < in_len && inbuf[i+1] == '/' ) {
			// C++ comment
			i += 2;
			while( i < in_len && inbuf[i] != '\n' ) {
				i++;
			}
		} else if( !literal && inbuf[i] == '/' && ( i+1 ) < in_len && inbuf[i+1] == '*' ) {
			// C comment
			i += 2;
			while( i < in_len && !( inbuf[i] == '*' && ( i+1 ) < in_len && inbuf[i+1] == '/' )) {
				i++;
			}

			i += 2;
		} else if(( inbuf[i] == '\"' || inbuf[i] == '\'' ) && ( inbuf[i-1] != '\\' )) {
			// reverse literal
			literal = !literal;
		}
		*outbuf = inbuf[i];
		++j;
		outbuf++;
	}
	out_len = j;
}

/*
 * @brief	omit more two blank line
 * @note 	don't cover zenkaku space.
 */
void delete_blankline( char* p_in, size_t in_len, char* p_out, size_t& out_len )
{
	char	linebuf[1024];
	char*	inbuf = p_in;
	size_t	i;		/* in counter */

	out_len = 0;

	size_t  k = 0;
	char* 	outbuf = p_out;
	int		n_blank_line = 0;
	bool	thisline_is_blank = true;
	const char SPACE = 0x20;
	const char TAB   = 0x09;

// for debug
#if 0
	printf( "in\n" );
	for( i = 0; i < in_len; ++i ) {
		printf( "%02x ", inbuf[i] );
	}
	printf( "\n" );
	printf( "out\n" );
#endif

	for( i = 0; i < in_len; ++i ){
		if( inbuf[i] == '\n' ){
			if( thisline_is_blank ){
				n_blank_line++;
				k = 0;
			} else {
				if( n_blank_line > 0 ){
					*outbuf = '\n';
					out_len += 1;
					outbuf += 1;
					n_blank_line = 0;
				}

				linebuf[k++] = '\n';

				/* put */
				memcpy( outbuf, linebuf, k );
				out_len += k;
				outbuf += k;
				k = 0;

				thisline_is_blank = true;
			}
		} else if( inbuf[i] == SPACE || inbuf[i] == TAB ){
			linebuf[k++] = inbuf[i];
		} else {
			linebuf[k++] = inbuf[i];
			thisline_is_blank = false;
		}
	}

	/* put */
	memcpy( outbuf, linebuf, k );
	out_len += k;
	outbuf += k;
	k = 0;
}

