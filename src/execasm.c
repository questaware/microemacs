#include	<stdio.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"base.h"


#if S_MSDOS && S_WIN32 == 0
#define NON_PORTABLE 1
#define STACKDIST 3 /* shorts */
#else
#define NON_PORTABLE 0
#define STACKDIST ?
#endif

#if NON_PORTABLE
#define ccbuf(wh) cbuf(wh)
#else
#define ccbuf(wh) cbuf(n,wh)
#endif

/*	cbuf:	Execute the contents of a numbered buffer	*/

#if NON_PORTABLE
static cbuf(int bufnum)
			/* number of buffer to execute */
#else
static cbuf(int n, int bufnum)
	/* int n;	* default flag and numeric arg */
	/* int bufnum;	* number of buffer to execute */
#endif
{
	register BUFFER * bp = bmfind(FALSE, bufnum);
	if (bp == NULL)
	{ mlwrite(TEXT130);
/*			"Macro not defined" */
	  return FALSE;
	}
{
#if NON_PORTABLE
        register int n = ((Short*)&bufnum)[STACKDIST];
#endif					     /* and now execute it as asked */
	return dobuf(bp,n);
}}




Pascal cbuf1(int f, int n) { return ccbuf(1); }
Pascal cbuf2(int f, int n) { return ccbuf(2); }
Pascal cbuf3(int f, int n) { return ccbuf(3); }
Pascal cbuf4(int f, int n) { return ccbuf(4); }
Pascal cbuf5(int f, int n) { return ccbuf(5); }
Pascal cbuf6(int f, int n) { return ccbuf(6); }
Pascal cbuf7(int f, int n) { return ccbuf(7); }
Pascal cbuf8(int f, int n) { return ccbuf(8); }
Pascal cbuf9(int f, int n) { return ccbuf(9); }
Pascal cbuf10(int f, int n) { return ccbuf(10); }
Pascal cbuf11(int f, int n) { return ccbuf(11); }
Pascal cbuf12(int f, int n) { return ccbuf(12); }
Pascal cbuf13(int f, int n) { return ccbuf(13); }
Pascal cbuf14(int f, int n) { return ccbuf(14); }
Pascal cbuf15(int f, int n) { return ccbuf(15); }
Pascal cbuf16(int f, int n) { return ccbuf(16); }
Pascal cbuf17(int f, int n) { return ccbuf(17); }
Pascal cbuf18(int f, int n) { return ccbuf(18); }
Pascal cbuf19(int f, int n) { return ccbuf(19); }
Pascal cbuf20(int f, int n) { return ccbuf(20); }
Pascal cbuf21(int f, int n) { return ccbuf(21); }
Pascal cbuf22(int f, int n) { return ccbuf(22); }
Pascal cbuf23(int f, int n) { return ccbuf(23); }
Pascal cbuf24(int f, int n) { return ccbuf(24); }
Pascal cbuf25(int f, int n) { return ccbuf(25); }
Pascal cbuf26(int f, int n) { return ccbuf(26); }
Pascal cbuf27(int f, int n) { return ccbuf(27); }
Pascal cbuf28(int f, int n) { return ccbuf(28); }
Pascal cbuf29(int f, int n) { return ccbuf(29); }
Pascal cbuf30(int f, int n) { return ccbuf(30); }
Pascal cbuf31(int f, int n) { return ccbuf(31); }
Pascal cbuf32(int f, int n) { return ccbuf(32); }
Pascal cbuf33(int f, int n) { return ccbuf(33); }
Pascal cbuf34(int f, int n) { return ccbuf(34); }
Pascal cbuf35(int f, int n) { return ccbuf(35); }
Pascal cbuf36(int f, int n) { return ccbuf(36); }
Pascal cbuf37(int f, int n) { return ccbuf(37); }
Pascal cbuf38(int f, int n) { return ccbuf(38); }
Pascal cbuf39(int f, int n) { return ccbuf(39); }
Pascal cbuf40(int f, int n) { return ccbuf(40); }




#if 0

Pascal cbuf1(int f, int n) { _asm mov ax, 1; _asm push ax; 
return cbuf(n); }
Pascal cbuf2(int f, int n) { _asm mov ax, 2; _asm push ax; _asm jmp cbuf1+8;
}
Pascal cbuf3(int f, int n)  { _asm mov ax, 3;  _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf4(int f, int n)  { _asm mov ax, 4;  _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf5(int f, int n)  { _asm mov ax, 5;  _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf6(int f, int n)  { _asm mov ax, 6;  _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf7(int f, int n)  { _asm mov ax, 7;  _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf8(int f, int n)  { _asm mov ax, 8;  _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf9(int f, int n)  { _asm mov ax, 9;  _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf10(int f, int n) { _asm mov ax, 10; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf11(int f, int n) { _asm mov ax, 11; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf12(int f, int n) { _asm mov ax, 12; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf13(int f, int n) { _asm mov ax, 13; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf14(int f, int n) { _asm mov ax, 14; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf15(int f, int n) { _asm mov ax, 15; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf16(int f, int n) { _asm mov ax, 16; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf17(int f, int n) { _asm mov ax, 17; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf18(int f, int n) { _asm mov ax, 18; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf19(int f, int n) { _asm mov ax, 19; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf20(int f, int n) { _asm mov ax, 20; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf21(int f, int n) { _asm mov ax, 21; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf22(int f, int n) { _asm mov ax, 22; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf23(int f, int n) { _asm mov ax, 23; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf24(int f, int n) { _asm mov ax, 24; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf25(int f, int n) { _asm mov ax, 25; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf26(int f, int n) { _asm mov ax, 26; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf27(int f, int n) { _asm mov ax, 27; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf28(int f, int n) { _asm mov ax, 28; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf29(int f, int n) { _asm mov ax, 29; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf30(int f, int n) { _asm mov ax, 30; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf31(int f, int n) { _asm mov ax, 31; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf32(int f, int n) { _asm mov ax, 32; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf33(int f, int n) { _asm mov ax, 33; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf34(int f, int n) { _asm mov ax, 34; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf35(int f, int n) { _asm mov ax, 35; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf36(int f, int n) { _asm mov ax, 36; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf37(int f, int n) { _asm mov ax, 37; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf38(int f, int n) { _asm mov ax, 38; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf39(int f, int n) { _asm mov ax, 39; _asm push ax; _asm jmp cbuf1+8; 
}
Pascal cbuf40(int f, int n) { _asm mov ax, 40; _asm push ax; _asm jmp cbuf1+8; 
}

#endif

