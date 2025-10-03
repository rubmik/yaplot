// gcode.c

#include "plotter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] ) {
	printf( "<html><body><svg width='%d' height='%d' style='background-color:#ff9'>\n", (int)WIDTH, (int)HEIGHT ); // tmp

	// init
	FILE *fp;
	if( argc == 2 ) fp = fopen( argv[1], "r" );
	else fp = fopen( "GCODE", "r" );
	if( fp == NULL ) return 1;
	char line[128];

	state machine;
	init_machine( &machine );
	point home; home.x = 10; home.y = 10;

	char cmd = -1;
	int lineno = 0;
	// read line per line
	while( fgets( line, sizeof line, fp ) != NULL ) {
		lineno++; printf( "%d\n", lineno ); // tmp

		if( line[0] != 'M' && line[0] != 'G' ) continue;
		else cmd = atoi( line+1 );

		// handle the M codes
		if( line[0] == 'M' ) {
			switch( cmd ) {
				// M2 -- end of program
				case 2:
				break;

				// M3 -- spindle on, CW
				case 3:
				break;

				// M5 -- spindle stop
				case 5:
				break;
			} continue;
		}

		// handle the G codes
		if( cmd == -1 ) { // not M nor G code -> error
			printf( "ERROR: trying to exec command, but none specified\n" );
			break;
		}

		// variables needed for handling
		point p, center;
		char *ptr;

		switch( cmd ) {
			// G00 -- rapid positioning
			// G01 -- linear interpolation
			case 0:
			case 1:
				ptr = strchr( line, 'Z' );
				if( ptr != NULL ) machine.z = atof( ptr+1 );
				if( ptr && !( strchr( line, 'X' ) && strchr( line, 'Y' ) ) ) break;
			
				ptr = strchr( line, 'X' );
				if( ptr != NULL ) p.x = atof( ptr+1 ); else p.x = 0;

				ptr = strchr( line, 'Y' );
				if( ptr != NULL ) p.y = atof( ptr+1 ); else p.y = 0;

				apply_mode( &machine, &p );

				if( cmd == 0 ) {	// G00
					if( go_to_pos( &machine, p ) ) return 1;
				} else {		// G01
					if( mv_lin( &machine, p ) ) return 1;
				}
				break;

			// G02 -- circular interpolation CW
			// G03 -- circular interpolation CCW
			case 2:
			case 3:
				ptr = strchr( line, 'X' );
				if( ptr != NULL ) p.x = atof( ptr+1 ); else p.x = 0;

				ptr = strchr( line, 'Y' );
				if( ptr != NULL ) p.y = atof( ptr+1 ); else p.y = 0;

				ptr = strchr( line, 'Z' );
				if( ptr != NULL ) machine.z = atof( ptr+1 );

				ptr = strchr( line, 'I' );
				if( ptr != NULL ) center.x = atof( ptr+1 ); else center.x = 0;
				center.x += machine.pos.x; // note, I and J are always relative

				ptr = strchr( line, 'J' );
				if( ptr != NULL ) center.y = atof( ptr+1 ); else center.y = 0;
				center.y += machine.pos.y;

				apply_mode( &machine, &p );

				printf( "\n\nSTARTING:\nX = %f, %Y = %f\nI = %f, J = %f\n\n", p.x, p.y, center.x, center.y );
				if( cmd == 2 ) {	// G02
					if( mv_circ( &machine, p, center, CW ) ) return 1;
				} else {		// G03
					if( mv_circ( &machine, p, center, CCW ) ) return 1;
				}
			break;

			// G21 -- unit = mm
			case 21:
			printf( "units = mm\n" );
			break;

			// G28 -- return home
			case 28:
				go_to_pos( &machine, home );
			break;

			// G90 -- absolute mode
			case 90:
			printf( "mode = abs\n" );
				machine.mode |= M_ABS;
			break;
			// G91 -- relative mode
			case 91:
				machine.mode &= ~M_ABS;
			break;
		}
	}

	// close
	fclose( fp );

	printf( "</svg></body></html>\n" ); // tmp

	return 0;
}
