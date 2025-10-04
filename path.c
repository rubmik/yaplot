#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "path.h"

// point
point get_point( double _x, double _y ) {
	point p;
	p.x = _x;
	p.y = _y;
	return p;
}

// ellipse
ellipse get_ellipse( double _k, double _h, double _a, double _b, double _th ) {
	ellipse e;
	e.k = _k; e.h = _h;
	e.a = _a; e.b = _b;
	e.th = _th;
	return e;
}
point rotate_point( point _p, double _th ) {
	point q;
	q.x = _p.x*cos(_th) - _p.y*sin(_th);
	q.y = _p.y*cos(_th) + _p.x*sin(_th);
	return q;
}
point scale_point( point _p, double _a, double _b ) {
	point q;
	q.x = _p.x*_a;
	q.y = _p.y*_b;
	return q;
}
point get_midpoint( point _p1, point _p2 ) {
	point q;
	q.x = (_p1.x+_p2.x)/2;
	q.y = (_p1.y+_p2.y)/2;
	return q;
}
point get_ellipse_point( ellipse _e, double _t ) {
	double r = SQ( cos(_t-_e.th)/_e.a ) + SQ( sin(_t-_e.th)/_e.b );
	r = 1/sqrt(r);
	point p;
	p.x = _e.k + r*cos(_t);
	p.y = _e.h + r*sin(_t);
	return p;
}
double get_point_angle( ellipse _e, point _p ) {
	point p;
	p.x = _p.x - _e.k;
	p.y = _p.y - _e.h;
	p = rotate_point( p, -_e.th );
	return atan2( p.y, p.x );
}
double angle_incr( double _t, double _incr ) {
	_t += _incr;
	if( _t > M_PI ) _t -= 2*M_PI;
	else if( _t < -M_PI ) _t += 2*M_PI;
	return _t;
}

// the magical function to find the ellipse center
// for proof and documentation:
// https://math.stackexchange.com/questions/53093/how-to-find-the-center-of-an-ellipse
//
// sets the (k,h) for the ellipse _e
// returns 1 upon failure,
// otherwise 0
int find_ellipse_center( ellipse *_e, point _p1, point _p2, char _large_arc, char _sweep ) {
	// return if P1 is P2
	if( _p1.x == _p2.x && _p1.y == _p2.y )
		return 0;

	// transform into a unit circle
	point p1 = scale_point( rotate_point( _p1, -_e->th ), 1/_e->a, 1/_e->b );
	point p2 = scale_point( rotate_point( _p2, -_e->th ), 1/_e->a, 1/_e->b );

	point m = get_midpoint( p1, p2 );
	p1.x -= m.x; p1.y -= m.y;
	p2.x -= m.x; p2.y -= m.y;

	// calculate c and solve accordingly
	double c = sqrt( SQ(p1.x) + SQ(p1.y) );

	// handle c > 1 -> expand the radii
	if( c > 1 ) {
		_e->a *= c;
		_e->b *= c;
		find_ellipse_center( _e, _p1, _p2, _large_arc, _sweep );
		return 0;
	}

	// only 1 solution
	if( c == 1 ) {
		point o = rotate_point( scale_point( m, _e->a, _e->b ), _e->th );
		_e->k = o.x; _e->h = o.y;
		return 0;
	}

	// standard 2 solutions
	double s = sqrt( 1 - SQ(c) );

	point o[2];
	o[0] = scale_point( get_point( -p1.y, p1.x ), s/c, s/c );
	o[1] = scale_point( get_point( p1.y, -p1.x ), s/c, s/c );

	o[0].x += m.x; o[0].y += m.y;
	o[1].x += m.x; o[1].y += m.y;

	o[0] = rotate_point( scale_point( o[0], _e->a, _e->b ), _e->th );
	o[1] = rotate_point( scale_point( o[1], _e->a, _e->b ), _e->th );

	// find out which one we want
	double u[] = { _p1.x-o[0].x, _p1.y-o[0].y }; // vector from O[0] to P1
	double v[] = { _p2.x-o[0].x, _p2.y-o[0].y }; // vector from O[0] to P2
	double angles[] = { atan2( u[1], u[0] ), atan2( v[1], v[0] ) };
	angles[0] = ( angles[0] < 0 ? 2*M_PI+angles[0] : angles[0] );
	angles[1] = ( angles[1] < 0 ? 2*M_PI+angles[1] : angles[1] );

	// arc length
	double arc_len = angles[0]-angles[1];
	if( ( _large_arc && arc_len < M_PI ) || ( !_large_arc && arc_len > M_PI ) ) arc_len = 2*M_PI - arc_len;
	if( arc_len < 0 ) arc_len = 2*M_PI + arc_len;

	// test
	_e->k = o[0].x;
	_e->h = o[0].y;
	point test;
	char dir = ( _sweep ? 1 : -1 );
	test = get_ellipse_point( *_e, angles[0] + arc_len*dir );
	if( !( fabs( test.x - _p2.x ) < 0.00001 && fabs( test.y - _p2.y ) < 0.00001 ) ) {
		_e->k = o[1].x;
		_e->h = o[1].y;
	}

	return 0;
}

// path
path init_path( ) {
	path b;
	b.p = NULL;
	b.n = 0;
	return b;
}

int free_path( path *_b ) {
	free( _b->p );
	return 0;
}

int add_point( path *_b, point _p ) {
	_b->n++;
	if( _b->p ) {
		_b->p = realloc( _b->p, _b->n*sizeof(point) );
		_b->p[_b->n-1] = _p;
	} else {
		_b->p = malloc( sizeof(point) );
		_b->p[0] = _p;
	}
	return 0;
}

// cubic bezier
point cubic_bez( point *_p, double _t ) {
	point p;
	p.x = CB(1-_t)*_p[0].x + 3*SQ(1-_t)*_t*_p[1].x + 3*(1-_t)*SQ(_t)*_p[2].x + CB(_t)*_p[3].x;
	p.y = CB(1-_t)*_p[0].y + 3*SQ(1-_t)*_t*_p[1].y + 3*(1-_t)*SQ(_t)*_p[2].y + CB(_t)*_p[3].y;
	return p;
}

// line bezier
point line_bez( point *_p, double _t ) {
	point p;
	p.x = _p[0].x + _t*(_p[1].x-_p[0].x);
	p.y = _p[0].y + _t*(_p[1].y-_p[1].y);
	return p;
}

// print
point print_curve( path *_b, char _mode ) {
	point p; double t;

	// cubic
	if( _mode == M_CUBIC ) {
		int i, n = floor( (_b->n-1) / 3 );
		for( t = 0; t <= 1; t += T_RESO ) {
			for( i = 0; i < n; i++ ) {
				p = cubic_bez( &_b->p[i*3], t );
				printf( "<circle cx='%f' cy='%f' r='1' fill='red' />\n", p.x, p.y );
			}
		}
	}

	// line
	else if( _mode == M_LINE ) {
		for( t = 0; t <= 1; t += T_RESO ) {
			p = line_bez( _b->p, t );
			printf( "<circle cx='%f' cy='%f' r='1' fill='red' />\n", p.x, p.y );
		}
	}

	return p;
}

// load line
int load_line( char **_l, char *_fn ) {
	FILE *fp = fopen( _fn, "r" );
	int c, llen = 128;
	char *line = malloc( llen+1 );
	int i = 0; while( c = fgetc( fp ) ) {
		if( c == EOF ) {
			line[i] = 0;
			break;
		}
		if( i == llen ) {
			llen += 128;
			line = realloc( line, llen+1 );
		} line[i] = c; i++;
	}
	fclose( fp );

	*_l = malloc( strlen( line )+1 );
	strcpy( *_l, line );
	free( line );
	return 0;
}

// find functionality
int isnum( char *_str ) {
	if( isdigit( _str[0] ) || ( ( _str[0] == '-' || _str[0] == '.' ) && isdigit( _str[1] ) ) )
		return 1;
	return 0;
}
int find_delim( char *_str, int _i ) {
	int i, l = strlen( _str );
	for( i = _i; i < l; i++ )
		if( _str[i] == ' ' || _str[i] == ',' || _str[i] == '\t' || _str[i] == '\n' )
			return i;
	return -1;
}
int find_non_delim( char *_str, int _i ) {
	int i, l = strlen( _str );
	for( i = _i; i < l; i++ )
		if( _str[i] != ' ' && _str[i] != ',' && _str[i] != '\t' && _str[i] != '\n' )
			return i;
	return -1;
}
int find_num( char *_str, int _i ) {
	int i, l = strlen( _str );
	for( i = _i; i < l; i++ )
		if( isnum( _str+i ) )
			return i;
	return -1;
}
int find_non_num( char *_str, int _i ) {
	int i, l = strlen( _str );
	for( i = _i; i < l; i++ )
		if( !isnum( _str+i ) )
			return i;
	return -1;
}

// tokens
token *init_tl( ) {
	token *t = malloc( sizeof(token) );
	t->next = NULL;
	t->data = NULL;
	t->type = -1;
	return t;
}

int push_tl( token *_t, char *_data, char _type ) {
	token *t = _t;
	if( t->data ) {
		while( t->next ) t = t->next;
		t->next = malloc( sizeof(token) );
		t = t->next;
	}
	t->data = malloc( strlen( _data )+1 );
	strcpy( t->data, _data );
	t->type = _type;
	t->next = NULL;
	return 0;
}

int free_tl( token *_t ) {
	if( _t->next ) free_tl( _t->next );
	free( _t->data );
	free( _t );
	return 0;
}

// token types
char get_cmd_flag( char _c ) {
	char cmd_char_array[] = "mlhvcsqtaz";
	char cmd_flag_array[] = { CMD_M, CMD_L, CMD_H, CMD_V, CMD_C, CMD_S, CMD_Q, CMD_T, CMD_A, CMD_Z };
	int i; for( i = 0; i < 10; i++ )
		if( _c == cmd_char_array[i] )
			return cmd_flag_array[i];
	return -1;
}

int tokenize( char *_l, token **_tl ) {
	*_tl = init_tl( );
	int i = 0, j;
	char type;
	int len = strlen( _l );
	while( i < len ) {
		i = find_non_delim( _l, i );
		if( i == -1 ) break;
		type = 0;
		if( isalpha( _l[i] ) ) {
			if( isupper( _l[i] ) ) {
				type |= M_ABS;
				_l[i] = tolower( _l[i] );
			}
			type |= get_cmd_flag( _l[i] );
			char cmd[2]; cmd[0] = _l[i]; cmd[1] = 0;
			push_tl( *_tl, cmd, type );
			if( _l[i] == 'z' ) {
				i++;
				continue;
			} else {
				i = find_num( _l, i+1 );
				if( i == -1 ) break;
			}
		}
		if( isnum( _l+i ) ) {
			j = find_non_num( _l, i );
			if( j == -1 ) j = len;
			char num[j-i+1]; strncpy( num, _l+i, j-i ); num[j-i] = 0;
			push_tl( *_tl, num, 0 );
			i = j;
		}
	}
	return 0;
}

char hasflag( char _t, char _f ) {
	if( _f == M_ABS ) {
		if( _t % 2 == 1 )
			return 1;
		return 0;
	}

	_t -= _t % 2;
	if( _t == _f )
		return 1;
	return 0;
}

// tmp
int print_type( char _t ) {
	if( hasflag( _t, CMD_M ) ) printf( "moveto " );
	if( hasflag( _t, CMD_L ) ) printf( "lineto " );
	if( hasflag( _t, CMD_H ) ) printf( "horizontal lineto " );
	if( hasflag( _t, CMD_V ) ) printf( "vertical lineto " );
	if( hasflag( _t, CMD_C ) ) printf( "curveto " );
	if( hasflag( _t, CMD_S ) ) printf( "smooth curveto " );
	if( hasflag( _t, CMD_Q ) ) printf( "quadratic curveto " );
	if( hasflag( _t, CMD_T ) ) printf( "smooth quadratic curveto " );
	if( hasflag( _t, CMD_A ) ) printf( "arc " );
	if( hasflag( _t, CMD_Z ) ) printf( "closeto " );
	if( hasflag( _t, M_ABS ) ) printf( "(absolute)\n" );
	else printf( "(relative)\n" );
	return 0;
}

int map_path( path *_b, token *_tl ) {
	token *t = _tl;
	point initial = get_point( 0, 0 );
	point p0 = initial;
	point p;
	char type;
	while( t ) {
		// set the command to be used on upcoming data
		if( t->type > 1 ) {
			type = t->type;
			if( !hasflag( type, CMD_Z ) ) {
				t = t->next;
				continue;
			}
		}

		// moveto, lineto, smooth quadratic curveto
		if( hasflag( type, CMD_M ) || hasflag( type, CMD_L ) || hasflag( type, CMD_T ) ) {
			if( !t->next ) {
				fprintf( stderr, "warning: incomplete point data\n" );
				break;
			}
			p.x = atof( t->data );
			p.y = atof( t->next->data );
			if( !hasflag( type, M_ABS ) ) {
				p.x += p0.x;
				p.y += p0.y;
			}
			p.type = type;
			add_point( _b, p );

			if( hasflag( type, CMD_M ) )
				initial = p;

			p0 = p;
			t = t->next->next;
			continue;
		}

		// horizontal lineto
		if( hasflag( type, CMD_H ) ) {
			p.x = atof( t->data );
			p.y = p0.y;
			if( !hasflag( type, M_ABS ) )
				p.x += p0.x;
			p.type = type;
			add_point( _b, p );

			p0 = p;
			t = t->next;
			continue;
		}

		// vertical lineto
		if( hasflag( type, CMD_V ) ) {
			p.x = p0.x;
			p.y = atof( t->data );
			if( !hasflag( type, M_ABS ) )
				p.y += p0.y;
			p.type = type;
			add_point( _b, p );

			p0 = p;
			t = t->next;
			continue;
		}

		// curveto
		if( hasflag( type, CMD_C ) ) {
			p.type = type;
			int i; for( i = 0; i < 3; i++ ) {
				if( !t || !t->next ) {
					fprintf( stderr, "warning: incomplete point data\n" );
					break;
				}
				p.x = atof( t->data );
				p.y = atof( t->next->data );
				if( !hasflag( type, M_ABS ) ) {
					p.x += p0.x;
					p.y += p0.y;
				}
				add_point( _b, p );

				t = t->next->next;
			}

			p0 = p;
			continue;
		}

		// smooth curveto, quadratic curveto
		if( hasflag( type, CMD_S ) || hasflag( type, CMD_Q ) ) {
			p.type = type;
			int i; for( i = 0; i < 2; i++ ) {
				if( !t || !t->next ) {
					fprintf( stderr, "warning: incomplete point data\n" );
					break;
				}
				p.x = atof( t->data );
				p.y = atof( t->next->data );
				if( !hasflag( type, M_ABS ) ) {
					p.x += p0.x;
					p.y += p0.y;
				}
				add_point( _b, p );

				t = t->next->next;
			}

			p0 = p;
			continue;
		}

		// elliptical arc
		if( hasflag( type, CMD_A ) ) {
			p.type = type;

			// width & height
			if( !t->next ) {
				fprintf( stderr, "warning: incomplete point data\n" );
				break;
			}
			p.x = atof( t->data );
			p.y = atof( t->next->data );
			if( !hasflag( type, M_ABS ) ) {
				p.x += p0.x;
				p.y += p0.y;
			}
			add_point( _b, p );

			t = t->next->next;

			// degree & flags
			if( !t || !t->next || !t->next->next ) {
				fprintf( stderr, "warning: incomplete point data\n" );
				break;
			}
			p.x = atof( t->data ); // NB: degrees
			p.y = atoi( t->next->data ) + 2*atoi( t->next->next->data );
			add_point( _b, p );

			t = t->next->next->next;

			// end point
			if( !t || !t->next ) {
				fprintf( stderr, "warning: incomplete point data\n" );
				break;
			}
			p.x = atof( t->data );
			p.y = atof( t->next->data );
			if( !hasflag( type, M_ABS ) ) {
				p.x += p0.x;
				p.y += p0.y;
			}
			add_point( _b, p );

			p0 = p;
			t = t->next->next;
			continue;
		}

		// closeto
		if( hasflag( type, CMD_Z ) ) {
			p = initial;
			p.type = type;
			add_point( _b, p );

			p0 = p;
			t = t->next;
			continue;
		}

		// default
		fprintf( stderr, "warning: unrecognized command\n" );
		break;
	}
}

int print_point( point _p ) {
	printf( "<circle cx='%f' cy='%f' r='1' fill='red' />\n", _p.x, _p.y );
	return 0;
}

int lineto( point _p0, point _p1, int (*_func)(point) ) {
	point p;
	double dx = _p1.x-_p0.x;
	double dy = _p1.y-_p0.y;
	double t; for( t = 0; t <= 1; t += T_RESO ) {
		p.x = _p0.x + t*dx;
		p.y = _p0.y + t*dy;
		(*_func)( p );
	}
	return 0;
}

int curveto( point _p0, point _bp1, point _bp2, point _p3, int (*_func)(point) ) {
	point p;
	double t; for( t = 0; t <= 1; t += T_RESO ) {
		p.x = CB(1-t)*_p0.x + 3*SQ(1-t)*t*_bp1.x + 3*(1-t)*SQ(t)*_bp2.x + CB(t)*_p3.x;
		p.y = CB(1-t)*_p0.y + 3*SQ(1-t)*t*_bp1.y + 3*(1-t)*SQ(t)*_bp2.y + CB(t)*_p3.y;
		(*_func)( p );
	}
	return 0;
}

int q_curveto( point _p0, point _bp1, point _p2, int (*_func)(point) ) {
	point p;
	double t; for( t = 0; t <= 1; t += T_RESO ) {
		p.x = SQ(1-t)*_p0.x + 2*(1-t)*t*_bp1.x + SQ(t)*_p2.x;
		p.y = SQ(1-t)*_p0.y + 2*(1-t)*t*_bp1.y + SQ(t)*_p2.y;
		(*_func)( p );
	}
	return 0;
}

int ell_arc( point _start, double _a, double _b, double _th, char _large_arc, char _sweep, point _end, int (*_func)(point) ) {
	ellipse e = get_ellipse( 0, 0, _a, _b, _th*M_PI/180 ); // NB now we convert degrees into radians
	if( find_ellipse_center( &e, _start, _end, _large_arc, _sweep ) ) {
		printf( "improper arc definition, cannot have P1 = P2\n" );
		return 1;
	}
	point p;

	double incr = T_RESO;
	char dir = ( _sweep ? 1 : -1 );

	double t_start = atan2( _start.y-e.h, _start.x-e.k );
	if( t_start < 0 ) t_start += 2*M_PI;
	double t_end = atan2( _end.y-e.h, _end.x-e.k );
	if( t_end < 0 ) t_end += 2*M_PI;
	double t_dist = fabs( t_end-t_start );
	if( ( _large_arc && t_dist < M_PI ) || ( !_large_arc && t_dist > M_PI ) ) t_dist = 2*M_PI - t_dist;

	double t; for( t = 0; t <= t_dist; t += incr ) {
		p = get_ellipse_point( e, t_start + t*dir );
		(*_func)( p );
	}

	return 0;
}

int print( path *_b ) {
	point prev = get_point( 0, 0 );
	point prev_cp;
	point p;
	int i; for( i = 0; i < _b->n; i++ ) {
		p = _b->p[i];

		// moveto
		if( hasflag( p.type, CMD_M ) ) {
			prev = p;
			continue;
		}

		// lineto
		if( hasflag( p.type, CMD_L ) ) {
			lineto( prev, p, print_point );
			prev = p;
			continue;
		}

		// horizontal lineto
		if( hasflag( p.type, CMD_H ) ) {
			lineto( prev, p, print_point );
			prev = p;
			continue;
		}

		// vertical lineto
		if( hasflag( p.type, CMD_V ) ) {
			lineto( prev, p, print_point );
			prev = p;
			continue;
		}

		// curveto
		if( hasflag( p.type, CMD_C ) ) {
			point bp1 = p;
			point bp2 = _b->p[++i];
			point end = _b->p[++i];
			curveto( prev, bp1, bp2, end, print_point );
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
			point end = _b->p[++i];
			curveto( prev, bp1, bp2, end, print_point );
			prev = end;
			prev_cp = bp2;
			continue;
		}

		// quadratic curveto
		if( hasflag( p.type, CMD_Q ) ) {
			point bp = p;
			point end = _b->p[++i];
			q_curveto( prev, bp, end, print_point );
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
			q_curveto( prev, bp, end, print_point );
			prev = end;
			prev_cp = bp;
			continue;
		}

		// elliptical arc
		if( hasflag( p.type, CMD_A ) ) {
			double a = p.x;
			double b = p.y;
			double th = _b->p[++i].x; // NB degrees
			char large_arc = (int)(_b->p[i].y) % 2;
			char sweep = _b->p[i].y > 1;
			point end = _b->p[++i];
			ell_arc( prev, a, b, th, large_arc, sweep, end, print_point );
			prev = end;
			continue;
		}

		// closeto
		if( hasflag( p.type, CMD_Z ) ) {
			lineto( prev, p, print_point );
			prev = p;
			continue;
		}
	}

	return 0;
}
int main( int argc, char *argv[] ) {
	// load the file to a char
	char *line = NULL; load_line( &line, argc==2?argv[1]:"path" );

	// split into tokens
	token *tl = NULL; tokenize( line, &tl );
	free( line );

	// convert tokens into path
	path b = init_path( );
	map_path( &b, tl );

	// draw
	printf( "<html><body><svg width='1000' height='1000' style='background-color:#ff9'>\n" );
	print( &b );
	printf( "</svg></body></html>\n" );

	// clean up
	free_path( &b );
	free_tl( tl );
	return 0;
}
