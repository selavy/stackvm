/* Generated by re2c 0.16 on Sat Mar 26 19:55:24 2016 */
#line 1 "first.re"
#include <stdio.h>

static const char *lex(const char *YYCURSOR)
{
    const char *YYMARKER;
    
#line 10 "main.cpp"
{
	char yych;
	yych = *YYCURSOR;
	switch (yych) {
	case '0':	goto yy4;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy5;
	default:	goto yy2;
	}
yy2:
	++YYCURSOR;
yy3:
#line 16 "first.re"
	{ return "err"; }
#line 32 "main.cpp"
yy4:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
	case 0x00:	goto yy6;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':	goto yy8;
	case 'B':
	case 'b':	goto yy11;
	case 'X':
	case 'x':	goto yy12;
	default:	goto yy3;
	}
yy5:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
	case 0x00:	goto yy13;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy15;
	default:	goto yy3;
	}
yy6:
	++YYCURSOR;
#line 18 "first.re"
	{ return "oct"; }
#line 71 "main.cpp"
yy8:
	++YYCURSOR;
	yych = *YYCURSOR;
	switch (yych) {
	case 0x00:	goto yy6;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':	goto yy8;
	default:	goto yy10;
	}
yy10:
	YYCURSOR = YYMARKER;
	goto yy3;
yy11:
	yych = *++YYCURSOR;
	if (yych <= 0x00) goto yy10;
	goto yy18;
yy12:
	yych = *++YYCURSOR;
	if (yych <= 0x00) goto yy10;
	goto yy20;
yy13:
	++YYCURSOR;
#line 19 "first.re"
	{ return "dec"; }
#line 102 "main.cpp"
yy15:
	++YYCURSOR;
	yych = *YYCURSOR;
	switch (yych) {
	case 0x00:	goto yy13;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy15;
	default:	goto yy10;
	}
yy17:
	++YYCURSOR;
	yych = *YYCURSOR;
yy18:
	switch (yych) {
	case 0x00:	goto yy21;
	case '0':
	case '1':	goto yy17;
	default:	goto yy10;
	}
yy19:
	++YYCURSOR;
	yych = *YYCURSOR;
yy20:
	switch (yych) {
	case 0x00:	goto yy23;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':	goto yy19;
	default:	goto yy10;
	}
yy21:
	++YYCURSOR;
#line 17 "first.re"
	{ return "bin"; }
#line 164 "main.cpp"
yy23:
	++YYCURSOR;
#line 20 "first.re"
	{ return "hex"; }
#line 169 "main.cpp"
}
#line 21 "first.re"

}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        printf ("%s: %s\n", lex(argv[i]), argv[i]);
    }
    return 0;
}
