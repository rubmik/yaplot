// main.c

#include "path.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] ) {
	// load the file to a char
	char *file = NULL; load_file( &file, argc==2?argv[1]:"path" );

	// split into tokens
	token *tl = NULL; tokenize( file, &tl );
	free( file );

	// convert tokens into path
	path the_path = init_path( );
	map_path( &the_path, tl );

	// initiate the machine
	state machine;
	init_machine( &machine );
	point home; home.x = 10; home.y = 10;

	// draw
	printf( "<html><body><svg width='100' height='100' style='background-color:#ff9'>\n" );

	// the magic
	point prev = get_point( 0, 0 );
	point prev_cp;
	point p;
	int i; for( i = 0; i < the_path.n; i++ ) {
		p = the_path.p[i];

		// moveto
		if( hasflag( p.type, CMD_M ) ) {
			prev = p;
			continue;
		}

		// lineto
		if( hasflag( p.type, CMD_L ) ) {
			lineto( prev, p, &machine );
			prev = p;
			continue;
		}

		// horizontal lineto
		if( hasflag( p.type, CMD_H ) ) {
			lineto( prev, p, &machine );
			prev = p;
			continue;
		}

		// vertical lineto
		if( hasflag( p.type, CMD_V ) ) {
			lineto( prev, p, &machine );
			prev = p;
			continue;
		}

		// curveto
		if( hasflag( p.type, CMD_C ) ) {
			point bp1 = p;
			point bp2 = the_path.p[++i];
			point end = the_path.p[++i];
			curveto( prev, bp1, bp2, end, &machine );
			prev = end;
			prev_cp = bp2;
			continue;
		}

		// smooth curveto
		if( hasflag( p.type, CMD_S ) ) {
			point bp1;
			if( !hasflag( prev.type, CMD_C ) && !hasflag( prev.type, CMD_S ) ) {
				bp1 = prev;
				bp1.type = CMD_S + p.type % 2;
			} else {
				bp1.x = 2*prev.x-prev_cp.x;
				bp1.y = 2*prev.y-prev_cp.y;
				bp1.type = CMD_S + p.type % 2;
			}
			point bp2 = p;
			point end = the_path.p[++i];
			curveto( prev, bp1, bp2, end, &machine );
			prev = end;
			prev_cp = bp2;
			continue;
		}

		// quadratic curveto
		if( hasflag( p.type, CMD_Q ) ) {
			point bp = p;
			point end = the_path.p[++i];
			q_curveto( prev, bp, end, &machine );
			prev = end;
			prev_cp = bp;
			continue;
		}

		// smooth quadratic curveto
		if( hasflag( p.type, CMD_T ) ) {
			point bp;
			if( !hasflag( prev.type, CMD_Q ) && !hasflag( prev.type, CMD_T ) ) {
				bp = prev;
				bp.type = CMD_T + p.type % 2;
			} else {
				bp.x = 2*prev.x-prev_cp.x;
				bp.y = 2*prev.y-prev_cp.y;
				bp.type = CMD_T + p.type % 2;
			}
			point end = p;
			q_curveto( prev, bp, end, &machine );
			prev = end;
			prev_cp = bp;
			continue;
		}

		// elliptical arc
		if( hasflag( p.type, CMD_A ) ) {
			double a = p.x;
			double b = p.y;
			double th = the_path.p[++i].x; // NB degrees
			char large_arc = (int)(the_path.p[i].y) % 2;
			char sweep = the_path.p[i].y > 1;
			point end = the_path.p[++i];
			ell_arc( prev, a, b, th, large_arc, sweep, end, &machine );
			prev = end;
			continue;
		}

		// closeto
		if( hasflag( p.type, CMD_Z ) ) {
			lineto( prev, p, &machine );
			prev = p;
			continue;
		}
	}

	printf( "</svg></body></html>\n" );
	// end of draw

	// clean up
	free_path( &the_path );
	free_tl( tl );

	return 0;
}
