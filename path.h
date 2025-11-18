// path.h

// math
#define SQ(x) ((x)*(x))
#define CB(x) ((x)*(x)*(x))
int fac( int );
int nck( int, int );

// point
typedef struct {
	double x, y;
	char type;
} point;
point get_point( double, double );

// ellipse
typedef struct {
	double k, h;
	double a, b;
	double th;
} ellipse;
ellipse get_ellipse( double, double, double, double, double );
point rotate_point( point, double );
point scale_point( point, double, double );
point get_midpoint( point, point );
int find_ellipse_center( ellipse *, point, point, char, char );
point get_ellipse_point( ellipse e, double t );
double angle_incr( double, double );

// path
typedef struct {
	point *p;
	int n;
} path;

path init_path( );
int free_path( path * );
int add_point( path *, point );
point cubic_bez( point *, double );
point line_bez( point *, double );

// machine
#define ARM_A 60.0
#define ARM_B 50.0
#define STEPS 180
#define PI2 6.283185
typedef struct {
	double alpha;
	double beta;
	point pos;
	float z;
} state;
int init_machine( state * );
int go_to_pos( state *, point );
int calc_pos( point *, double, double );
int calc_angles( double *, double *, point );
int round_angle( double * );
int turn_motors( state *, int, int );
int draw_pos( state * ); // tmp

// print
#define T_RESO 0.01
#define M_CUBIC 1
#define M_LINE 2
point print_curve( path *, char );

// load file
int load_file( char **, char * );

// find functionality
int isnum( char * );
int find_delim( char *, int );
int find_non_delim( char *, int );
int find_num( char *, int _i );
int find_non_num( char *, int );

// tokens
typedef struct token {
	struct token *next;
	char *data;
	char type;
} token;

token *init_tl( );
int push_tl( token *, char *, char );
int free_tl( token * );

// token types
#define M_ABS 0b00001
#define CMD_M 0b00010
#define CMD_L 0b00100
#define CMD_H 0b00110
#define CMD_V 0b01000
#define CMD_C 0b01010
#define CMD_S 0b01100
#define CMD_Q 0b01110
#define CMD_T 0b10000
#define CMD_A 0b10010
#define CMD_Z 0b10100

char get_cmd_flag( char );
int tokenize( char *, token ** );
char hasflag( char, char );
int print_type( char ); // tmp
int map_path( path *, token * );
int print_point( point );
int lineto( point, point, state * );
int curveto( point, point, point, point, state * );
int q_curveto( point, point, point, state * );
int ell_arc( point, double, double, double, char, char, point, state * );
int print( path * );
