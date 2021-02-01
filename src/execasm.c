#include	<stdio.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"base.h"



/*	cbuf:	Execute the contents of a numbered buffer	*/

static cbuf(int n, int bufnum)
										/* int n;				* default flag and numeric arg */
										/* int bufnum;	* number of buffer to execute */
{
	BUFFER * bp = bmfind(FALSE, bufnum);
	if (bp == NULL)
	{ mlwrite(TEXT130);
					/* "Macro not defined" */
	  return FALSE;
	}

	return dobuf(bp,n);
}

int Pascal cbuf1(int f, int n) { return cbuf(n,1); }
int Pascal cbuf2(int f, int n) { return cbuf(n,2); }
int Pascal cbuf3(int f, int n) { return cbuf(n,3); }
int Pascal cbuf4(int f, int n) { return cbuf(n,4); }
int Pascal cbuf5(int f, int n) { return cbuf(n,5); }
int Pascal cbuf6(int f, int n) { return cbuf(n,6); }
int Pascal cbuf7(int f, int n) { return cbuf(n,7); }
int Pascal cbuf8(int f, int n) { return cbuf(n,8); }
int Pascal cbuf9(int f, int n) { return cbuf(n,9); }
int Pascal cbuf10(int f, int n) { return cbuf(n,10); }
int Pascal cbuf11(int f, int n) { return cbuf(n,11); }
int Pascal cbuf12(int f, int n) { return cbuf(n,12); }
int Pascal cbuf13(int f, int n) { return cbuf(n,13); }
int Pascal cbuf14(int f, int n) { return cbuf(n,14); }
int Pascal cbuf15(int f, int n) { return cbuf(n,15); }
int Pascal cbuf16(int f, int n) { return cbuf(n,16); }
int Pascal cbuf17(int f, int n) { return cbuf(n,17); }
int Pascal cbuf18(int f, int n) { return cbuf(n,18); }
int Pascal cbuf19(int f, int n) { return cbuf(n,19); }
int Pascal cbuf20(int f, int n) { return cbuf(n,20); }
int Pascal cbuf21(int f, int n) { return cbuf(n,21); }
int Pascal cbuf22(int f, int n) { return cbuf(n,22); }
int Pascal cbuf23(int f, int n) { return cbuf(n,23); }
int Pascal cbuf24(int f, int n) { return cbuf(n,24); }
int Pascal cbuf25(int f, int n) { return cbuf(n,25); }
int Pascal cbuf26(int f, int n) { return cbuf(n,26); }
int Pascal cbuf27(int f, int n) { return cbuf(n,27); }
int Pascal cbuf28(int f, int n) { return cbuf(n,28); }
int Pascal cbuf29(int f, int n) { return cbuf(n,29); }
int Pascal cbuf30(int f, int n) { return cbuf(n,30); }
int Pascal cbuf31(int f, int n) { return cbuf(n,31); }
int Pascal cbuf32(int f, int n) { return cbuf(n,32); }
int Pascal cbuf33(int f, int n) { return cbuf(n,33); }
int Pascal cbuf34(int f, int n) { return cbuf(n,34); }
int Pascal cbuf35(int f, int n) { return cbuf(n,35); }
int Pascal cbuf36(int f, int n) { return cbuf(n,36); }
int Pascal cbuf37(int f, int n) { return cbuf(n,37); }
int Pascal cbuf38(int f, int n) { return cbuf(n,38); }
int Pascal cbuf39(int f, int n) { return cbuf(n,39); }
int Pascal cbuf40(int f, int n) { return cbuf(n,40); }



