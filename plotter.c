// plotter.c

#include "plotter.h"
#include <stdio.h>
#include <math.h>

// linear

int set_line( line *_l, point _p1, point _p2 ) {
	if( _p2.x == _p1.x ) return 1; // this is an error

	_l->k = (_p2.y-_p1.y)/(_p2.x-_p1.x);
	_l->h = _p1.y-_l->k*_p1.x;
	_l->start = _p1;
	_l->end = _p2;
	return 0;
}

int next_point_lin( line _l, point *_p ) {
	double dx, dy;
	dx = fabs( sqrt( SQ(RESO)/(1+SQ(_l.k)) ) );
	dy = fabs( _l.k*dx );
	if( _l.end.x < _p->x ) dx *= -1;
	if( _l.end.y < _p->y ) dy *= -1;
	_p->x += dx;
	_p->y += dy;

	// handle approaching the end point
	if( SQ(_p->x-_l.end.x) + SQ(_p->y-_l.end.y) < SQ(1.5*RESO) )
		return 1;
	return 0;
}

// circle

int set_circle( circle *_c, point _p1, point _p2, point _p0 ) {
	_c->r = sqrt( SQ(_p1.x-_p0.x) + SQ(_p1.y-_p0.y) );
	_c->start = _p1;
	_c->end = _p2;
	_c->center = _p0;
	return 0;
}

int next_point_circ( circle _c, point *_p, char _dir ) {
	// 1° calculate angle
	double pang = asin( (_p->y-_c.center.y)/_c.r );
	if( _p->x < _c.center.x ) pang = M_PI-pang;
	double dang = RESO/_c.r;
	double next_angle = pang + (_dir == CW ? dang : -dang); // use plus in CW, minus in CCW

	// 2° set next point
	_p->x = _c.r*cos( next_angle ) + _c.center.x;
	_p->y = _c.r*sin( next_angle ) + _c.center.y;

	// handle approaching the end point
	if( SQ(_p->x-_c.end.x) + SQ(_p->y-_c.end.y) < SQ(1.5*RESO) )
		return 1;

	return 0;
}

// machine

int init_machine( state *_m ) {
	_m->alpha = 0;
	_m->beta = 0;
	calc_pos( &_m->pos, _m->alpha, _m->beta );
	_m->mode = M_ABS;
	_m->z = 0;
	return 0;
}

int go_to_pos( state *_m, point _p ) {
	// check if point is within reach
	double d = SQ(_p.x) + SQ(_p.y);
	if( SQ(ARM_A-ARM_B) > d || SQ(ARM_A+ARM_B) < d ) {
		printf( "ERROR: point not within reach\n" );
		return 1;
	}

	double alpha, beta; // used for destination
	calc_angles( &alpha, &beta, _p );
	round_angle( &alpha );
	round_angle( &beta );
	int a_steps, b_steps; // number of steps (incl direction)
	a_steps = round( (alpha - _m->alpha)*STEPS/PI2 );
	b_steps = round( (beta - _m->beta)*STEPS/PI2 );
	turn_motors( _m, a_steps, b_steps ); // turn motors

	return 0;
}

int mv_lin( state *_m, point _p ) {
	if( SQ(_m->pos.x-_p.x) + SQ(_m->pos.y-_p.y) > SQ(1.5*RESO) ) {
		// 1° define the line
		if( _m->pos.x == _p.x ) _p.x += RESO*0.1;
		line a;
		set_line( &a, _m->pos, _p );
		// 2° move by steps
		point p = _m->pos;
		do
			if( go_to_pos( _m, p ) ) break;
		while( !next_point_lin( a, &p ) );
	}
	// 3° last movement
	if( go_to_pos( _m, _p ) ) return 1;

	return 0;
}

int mv_circ( state *_m, point _p, point _center, char _dir ) {
	if( SQ(_m->pos.x-_p.x) + SQ(_m->pos.y-_p.y) > SQ(1.5*RESO) ) {
		// 1° define the circle
		circle c;
		set_circle( &c, _m->pos, _p, _center );
		// 2° move by steps
		point p = _m->pos;
		do
			if( go_to_pos( _m, p ) ) break;
		while( !next_point_circ( c, &p, _dir ) );
	}
	// 3° last movement
	if( go_to_pos( _m, _p ) ) return 1;

	return 0;
}

int calc_pos( point *_p, double _alpha, double _beta ) {
	// for proof, see illus_0.pdf
	double gamma = _alpha+_beta + ( _alpha+_beta < M_PI ? M_PI : -M_PI );
	_p->x = ARM_A*cos( _alpha ) + ARM_B*cos( gamma );
	_p->y = ARM_A*sin( _alpha ) + ARM_B*sin( gamma );
	return 0;
}

int calc_angles( double *_alpha, double *_beta, point _p ) {
	// for proof, see illus_1.pdf
	double alpha_1, alpha_2, c;
	c = sqrt( SQ(_p.x) + SQ(_p.y) );
	*_beta = acos( ( c*c - SQ(ARM_A) - SQ(ARM_B) )/( -2*ARM_A*ARM_B ) );
	alpha_1 = atan( _p.y/_p.x );
	alpha_2 = asin( ARM_B*sin( *_beta )/c );
	if( SQ(ARM_B) < SQ(_p.x) + SQ(_p.y-ARM_A) ) {
		*_beta = PI2-*_beta;
		*_alpha = alpha_1-alpha_2;
	} else  *_alpha = alpha_1+alpha_2;
	return 0;
}

int round_angle( double *_angle ) {
	double step = PI2/STEPS;
	int steps = round( *_angle/step );
	*_angle = step*steps;
	return 0;
}

int apply_mode( state *_m, point *_p ) {
	if( ( _m->mode & M_ABS ) != M_ABS ) {
		_p->x += _m->pos.x;
		_p->y += _m->pos.y;
	}
	return 0;
}

// motor control

int turn_motors( state *_m, int _a, int _b ) {
	_m->alpha += _a*PI2/STEPS;
	round_angle( &_m->alpha );
	_m->beta += _b*PI2/STEPS;
	round_angle( &_m->beta );
	calc_pos( &_m->pos, _m->alpha, _m->beta );
	draw_pos( _m );
	return 0;
}

// tmp

int draw_pos( state *_m ) {
	if( _m->z <= 0 )
		printf( "<circle cx='%f' cy='%f' r='1' fill='red' />\n", _m->pos.x, _m->pos.y );
	return 0;
}

