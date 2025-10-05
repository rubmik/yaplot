#!/usr/bin/gawk

BEGIN {
	path = 0;
	d = 0;
}

/<path/ {
	path = 1;
}
/>/ {
	path = 0;
}
/d="/ {
	if( path == 1 ) d = 1;
	if( match( $0, /d=".*"/ ) ) {
		s = substr( $0, index( $0, "d=\"" )+3 );
		print substr( s, 0, index( s, "\"" )-1 );
	} else {
		print substr( $0, index( $0, "d=\"" )+3 );
	}
	next;
}

{
	if( d == 1 ) {
		i = index( $0, "\"" );
		if( !i ) {
			print substr( $0, match( $0, /\S/ ) );
		} else {
			j = match( $0, /\S/ );
			print substr( $0, j, i-j );
			d = 0;
		}
	}
}
