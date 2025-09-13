// plotter.h

#define WIDTH 100
#define HEIGHT 100
#define ARM_A 60.0
#define ARM_B 50.0
#define STEPS 180
#define RESO 1.0

#define M_ABS 0b1
#define CW 0
#define CCW 1

#define PI2 6.283185
#define RTD(x) (x)*57.29578
#define SQ(x) (x)*(x)

// structs

// point (many purposes)
typedef struct point {
	double x, y;
} point;

// line (for linear movements)
typedef struct {
	point start, end;
	double k, h;
} line;

// circle (for circular movements)
typedef struct {
	point start, end, center;
	double r;
} circle;

// machine state
typedef struct {
	double alpha;
	double beta;
	point pos;
	char mode; // initalized to absolute
	float z;
} state;

// funcs

// linear
int set_line( line *, point, point );
int next_point_lin( line, point * );

// circle
int set_circle( circle *, point, point, point );
int next_point_circ( circle, point *, char );

// machine
int init_machine( state * );
int go_to_pos( state *, point );
int mv_lin( state *, point );
int mv_circ( state *, point, point, char );
int calc_pos( point *, double, double );
int calc_angles( double *, double *, point );
int round_angle( double * );
int apply_mode( state *, point * );

// motor control
int turn_motors( state *, int, int );

// tmp
int draw_pos( state * ); // tmp

