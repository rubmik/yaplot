// main.c

#include "plotter.h"
#include <stdio.h>

int main( int argc, char *argv[] ) {
	printf( "<html><body><svg width='100' height='100' style='background-color:#ff9'>\n" ); // tmp

	// define the start and end
	point p1; p1.x = 10; p1.y = 10;
	point p2; p2.x = 70; p2.y = 10;

	// NEXT:
	// - selvitä, miksi joskus hyvin vähän pisteitä
	// 	(esim kun dy = 0)
	// - merkitse, missä ja miten moottorien askellukset
	// - ympyräliikkeet
	// - g-koodi

	// define the machine
	state machine;
	init_machine( &machine );
	go_to_pos( &machine, p1 );
	mv_lin( &machine, p2 );

	printf( "</svg></body></html>\n" ); // tmp
	return 0;
}
