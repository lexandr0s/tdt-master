#ifndef PICS_H
#define PICS_H

#define MAZEW	22
#define	MAZEH	18

static	unsigned char	maze[] =\
"                      "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  #          #        "\
"  ############        "\
"                      ";

#define _	BLACK
#define Y	YELLOW
#define W	WHITE
#define r	RED1
#define R	RED2

static unsigned char	wall[] = {
r,r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,
r,r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,
R,R,R,r,r,r,r,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,
R,R,R,R,r,r,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,
R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,
R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,
r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,
R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,
R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,
R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,
R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,r,
r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,r,r,R,R,r,r,_,_,r,r,r,r,
R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,
R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,
R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,
R,R,R,R,R,R,R,R,R,R,_,_,r,R,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
r,r,_,_,r,r,r,r,r,r,r,r,r,r,r,r,r,r,_,_,r,r,r,r,r,r,r,r,r,r,r,r,
r,r,_,_,r,r,r,r,r,r,r,r,r,r,R,R,r,r,_,_,r,r,r,r,r,r,r,r,r,r,r,r,
R,R,_,_,r,r,R,R,R,R,R,r,r,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,
R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,
R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,r,R,R,R,R,R,R,R,R,R,R,
R,R,_,_,r,R,R,R,R,R,R,R,R,R,R,R,R,R,_,_,r,R,R,R,R,R,R,R,R,R,R,R,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ };

#undef _
#undef Y
#undef W
#undef r
#undef R

static unsigned char	puzdata[] = {
0,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,0,0,1,1,2,
0,3,3,3,0,3,3,0,3,3,3,3,3,3,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,3,1,4,
1,3,5,5,5,5,5,6,5,5,5,5,5,7,7,7,7,7,5,6,6,6,5,5,7,2,2,5,6,5,5,8,
1,3,5,5,5,5,5,5,5,5,5,5,5,7,5,5,7,5,5,5,6,6,5,5,2,7,6,6,5,5,6,8,
1,3,5,6,7,5,7,7,7,7,7,7,5,5,5,5,7,5,5,6,6,6,5,7,2,7,6,5,5,5,6,8,
1,3,2,5,7,7,7,5,7,7,7,5,6,6,5,5,7,7,5,5,5,5,7,7,2,5,6,7,7,5,6,8,
1,0,2,5,5,5,5,5,7,7,7,7,5,5,7,7,5,5,5,6,5,5,7,7,5,6,5,5,7,2,6,9,
1,3,7,5,5,7,7,5,7,7,5,5,5,7,7,7,7,5,6,5,6,5,2,2,5,6,5,7,2,1,6,9,
1,3,5,7,5,7,7,7,7,5,7,7,5,7,2,7,5,6,6,6,5,7,2,2,6,5,5,7,2,2,4,9,
1,3,5,7,7,7,5,5,5,5,5,5,5,5,7,2,6,6,6,5,7,2,2,5,6,5,5,2,2,7,4,9,
1,3,5,5,7,6,5,5,7,7,2,7,7,5,7,7,5,5,6,7,2,2,6,6,2,7,2,2,7,7,4,9,
1,3,5,2,7,6,5,5,7,7,2,2,5,7,7,5,7,5,5,7,2,2,6,7,2,2,2,7,5,6,4,9,
1,3,7,2,7,6,5,5,7,7,7,7,7,7,7,5,5,6,5,7,7,5,5,7,2,5,6,6,6,6,6,8,
1,0,7,2,7,6,6,5,7,7,2,7,5,5,7,5,5,5,5,7,7,6,5,7,5,6,6,6,5,5,6,9,
1,0,2,2,6,6,5,7,2,7,2,7,7,7,5,6,6,6,7,2,7,6,7,2,6,6,6,7,7,5,4,8,
1,3,2,2,6,6,5,5,7,2,2,7,5,6,5,6,6,6,7,7,5,6,7,2,6,6,5,5,5,5,6,9,
1,3,7,5,6,6,7,7,7,2,2,7,6,6,6,5,5,5,2,7,6,7,2,2,6,5,7,7,7,5,4,9,
1,3,7,5,5,5,7,7,7,7,7,5,6,6,6,6,5,5,2,7,5,7,2,5,5,5,5,5,5,7,6,9,
0,3,7,5,5,5,7,2,7,2,7,5,6,6,5,7,2,2,5,6,5,7,2,2,5,5,7,7,5,5,6,9,
0,3,5,5,7,5,7,7,7,7,7,6,6,6,5,7,2,7,6,5,7,2,2,5,5,5,7,5,7,7,4,9,
0,3,5,5,7,5,7,7,2,2,5,6,6,6,2,2,7,5,6,7,5,5,5,6,5,6,5,5,5,7,6,9,
0,3,5,7,7,7,5,7,7,2,6,6,6,5,7,2,7,5,5,5,7,5,5,5,5,5,5,5,7,7,6,8,
0,3,5,5,7,5,5,7,2,2,6,6,5,5,7,2,5,5,5,5,7,5,7,7,5,7,5,7,7,7,6,9,
0,3,5,7,7,7,5,7,7,2,6,6,5,5,7,7,5,7,5,7,5,5,5,5,5,7,7,7,7,5,4,9,
0,3,5,7,5,7,5,7,2,7,5,6,5,5,7,7,5,5,7,7,7,7,5,7,5,5,5,5,6,6,4,9,
0,3,7,5,7,7,7,2,7,7,5,6,5,7,7,7,5,5,7,7,5,5,5,5,5,7,5,5,5,5,4,9,
0,3,5,5,5,7,5,7,7,7,6,5,5,7,7,7,5,5,2,5,6,5,5,5,7,7,7,5,5,5,6,9,
1,3,5,5,5,5,7,2,2,5,5,6,5,5,5,5,5,7,7,7,6,5,5,5,5,5,5,7,5,7,4,8,
0,3,5,5,5,7,2,2,7,5,6,5,5,5,2,5,5,5,5,5,5,6,5,5,5,5,5,5,7,7,6,9,
0,3,5,7,5,7,2,2,7,5,6,5,5,5,7,2,5,7,5,5,5,7,5,7,7,5,7,7,7,5,6,8,
0,3,6,6,7,2,2,7,6,6,6,6,7,5,2,7,5,6,5,6,4,4,4,6,4,4,6,4,6,4,4,9,
1,7,9,9,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9  };

#endif
