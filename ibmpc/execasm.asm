;	Static Name Aliases
;
	TITLE   ../src/execasm.c
	.8087
INCLUDELIB	LLIBCE
EXECASM_TEXT	SEGMENT  WORD PUBLIC 'CODE'
EXECASM_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
$$SYMBOLS	SEGMENT  BYTE PUBLIC 'DEBSYM'
$$SYMBOLS	ENDS
$$TYPES	SEGMENT  BYTE PUBLIC 'DEBTYP'
$$TYPES	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME DS: DGROUP, SS: DGROUP
EXTRN	__aFchkstk:FAR
EXTRN	_cbuf:FAR
EXECASM_TEXT      SEGMENT
	ASSUME	CS: EXECASM_TEXT
; Line 1
; Line 297
; Line 298
; Line 332
; Line 393
; Line 2
; Line 8
	PUBLIC	CBUFNN
CBUFNN	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
; Line 9
$cbuf1274:
	mov	ax,1
; Line 10
	jmp	$fi1275
; Line 11
$cbuf2276:
	mov	ax,2
; Line 12
	jmp	$fi1275
; Line 13
$cbuf3277:
	mov	ax,3
; Line 14
	jmp	$fi1275
; Line 15
$cbuf4278:
	mov	ax,4
; Line 16
	jmp	$fi1275
; Line 17
$cbuf5279:
	mov	ax,5
; Line 18
	jmp	$fi1275
; Line 19
$cbuf6280:
	mov	ax,6
; Line 20
	jmp	$fi1275
; Line 21
$cbuf7281:
	mov	ax,7
; Line 22
	jmp	$fi1275
; Line 23
$cbuf8282:
	mov	ax,8
; Line 24
	jmp	$fi1275
; Line 25
$cbuf9283:
	mov	ax,9
; Line 26
	jmp	$fi1275
; Line 27
$cbuf10284:
	mov	ax,10
; Line 28
	jmp	$fi1275
; Line 29
$cbuf11285:
	mov	ax,11
; Line 30
	jmp	$fi1275
; Line 31
$cbuf12286:
	mov	ax,12
; Line 32
	jmp	$fi1275
; Line 33
$cbuf13287:
	mov	ax,13
; Line 34
	jmp	$fi1275
; Line 35
$cbuf14288:
	mov	ax,14
; Line 36
	jmp	$fi1275
; Line 37
$cbuf15289:
	mov	ax,15
; Line 38
	jmp	$fi1275
; Line 39
$cbuf16290:
	mov	ax,16
; Line 40
	jmp	$fi1275
; Line 41
$cbuf17291:
	mov	ax,17
; Line 42
	jmp	$fi1275
; Line 43
$cbuf18292:
	mov	ax,18
; Line 44
	jmp	$fi1275
; Line 45
$cbuf19293:
	mov	ax,19
; Line 46
	jmp	SHORT $fi1275
; Line 47
	nop	
$cbuf20294:
	mov	ax,20
; Line 48
	jmp	SHORT $fi1275
; Line 49
	nop	
$cbuf21295:
	mov	ax,21
; Line 50
	jmp	SHORT $fi1275
; Line 51
	nop	
$cbuf22296:
	mov	ax,22
; Line 52
	jmp	SHORT $fi1275
; Line 53
	nop	
$cbuf23297:
	mov	ax,23
; Line 54
	jmp	SHORT $fi1275
; Line 55
	nop	
$cbuf24298:
	mov	ax,24
; Line 56
	jmp	SHORT $fi1275
; Line 57
	nop	
$cbuf25299:
	mov	ax,25
; Line 58
	jmp	SHORT $fi1275
; Line 59
	nop	
$cbuf26300:
	mov	ax,26
; Line 60
	jmp	SHORT $fi1275
; Line 61
	nop	
$cbuf27301:
	mov	ax,27
; Line 62
	jmp	SHORT $fi1275
; Line 63
	nop	
$cbuf28302:
	mov	ax,28
; Line 64
	jmp	SHORT $fi1275
; Line 65
	nop	
$cbuf29303:
	mov	ax,29
; Line 66
	jmp	SHORT $fi1275
; Line 67
	nop	
$cbuf30304:
	mov	ax,30
; Line 68
	jmp	SHORT $fi1275
; Line 69
	nop	
$cbuf31305:
	mov	ax,31
; Line 70
	jmp	SHORT $fi1275
; Line 71
	nop	
$cbuf32306:
	mov	ax,32
; Line 72
	jmp	SHORT $fi1275
; Line 73
	nop	
$cbuf33307:
	mov	ax,33
; Line 74
	jmp	SHORT $fi1275
; Line 75
	nop	
$cbuf34308:
	mov	ax,34
; Line 76
	jmp	SHORT $fi1275
; Line 77
	nop	
$cbuf35309:
	mov	ax,35
; Line 78
	jmp	SHORT $fi1275
; Line 79
	nop	
$cbuf36310:
	mov	ax,36
; Line 80
	jmp	SHORT $fi1275
; Line 81
	nop	
$cbuf37311:
	mov	ax,37
; Line 82
	jmp	SHORT $fi1275
; Line 83
	nop	
$cbuf38312:
	mov	ax,38
; Line 84
	jmp	SHORT $fi1275
; Line 85
	nop	
$cbuf39313:
	mov	ax,39
; Line 86
	jmp	SHORT $fi1275
; Line 87
	nop	
$cbuf40314:
	mov	ax,40
; Line 88
	jmp	SHORT $fi1275
; Line 89
	nop	
$fi1275:
; Line 90
	push	ax
; Line 91
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,2
	mov	sp,bp
	pop	bp
	ret	4
; Line 92
	mov	sp,bp
	pop	bp
	ret	4

CBUFNN	ENDP
; Line 95
	PUBLIC	CBUF1
CBUF1	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,1
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF1	ENDP
; Line 96
	PUBLIC	CBUF2
CBUF2	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,2
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF2	ENDP
; Line 97
	PUBLIC	CBUF3
CBUF3	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,3
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF3	ENDP
; Line 98
	PUBLIC	CBUF4
CBUF4	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,4
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF4	ENDP
; Line 99
	PUBLIC	CBUF5
CBUF5	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,5
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF5	ENDP
; Line 100
	PUBLIC	CBUF6
CBUF6	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,6
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF6	ENDP
; Line 101
	PUBLIC	CBUF7
CBUF7	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,7
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF7	ENDP
; Line 102
	PUBLIC	CBUF8
CBUF8	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,8
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF8	ENDP
; Line 103
	PUBLIC	CBUF9
CBUF9	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,9
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF9	ENDP
; Line 104
	PUBLIC	CBUF10
CBUF10	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,10
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF10	ENDP
; Line 105
	PUBLIC	CBUF11
CBUF11	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,11
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF11	ENDP
; Line 106
	PUBLIC	CBUF12
CBUF12	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,12
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF12	ENDP
; Line 107
	PUBLIC	CBUF13
CBUF13	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,13
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF13	ENDP
; Line 108
	PUBLIC	CBUF14
CBUF14	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,14
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF14	ENDP
; Line 109
	PUBLIC	CBUF15
CBUF15	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,15
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF15	ENDP
; Line 110
	PUBLIC	CBUF16
CBUF16	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,16
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF16	ENDP
; Line 111
	PUBLIC	CBUF17
CBUF17	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,17
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF17	ENDP
; Line 112
	PUBLIC	CBUF18
CBUF18	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,18
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF18	ENDP
; Line 113
	PUBLIC	CBUF19
CBUF19	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,19
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF19	ENDP
; Line 114
	PUBLIC	CBUF20
CBUF20	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,20
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF20	ENDP
; Line 115
	PUBLIC	CBUF21
CBUF21	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,21
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF21	ENDP
; Line 116
	PUBLIC	CBUF22
CBUF22	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,22
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF22	ENDP
; Line 117
	PUBLIC	CBUF23
CBUF23	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,23
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF23	ENDP
; Line 118
	PUBLIC	CBUF24
CBUF24	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,24
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF24	ENDP
; Line 119
	PUBLIC	CBUF25
CBUF25	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,25
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF25	ENDP
; Line 120
	PUBLIC	CBUF26
CBUF26	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,26
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF26	ENDP
; Line 121
	PUBLIC	CBUF27
CBUF27	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,27
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF27	ENDP
; Line 122
	PUBLIC	CBUF28
CBUF28	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,28
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF28	ENDP
; Line 123
	PUBLIC	CBUF29
CBUF29	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,29
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF29	ENDP
; Line 124
	PUBLIC	CBUF30
CBUF30	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,30
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF30	ENDP
; Line 125
	PUBLIC	CBUF31
CBUF31	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,31
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF31	ENDP
; Line 126
	PUBLIC	CBUF32
CBUF32	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,32
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF32	ENDP
; Line 127
	PUBLIC	CBUF33
CBUF33	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,33
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF33	ENDP
; Line 128
	PUBLIC	CBUF34
CBUF34	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,34
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF34	ENDP
; Line 129
	PUBLIC	CBUF35
CBUF35	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,35
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF35	ENDP
; Line 130
	PUBLIC	CBUF36
CBUF36	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,36
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF36	ENDP
; Line 131
	PUBLIC	CBUF37
CBUF37	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,37
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF37	ENDP
; Line 132
	PUBLIC	CBUF38
CBUF38	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,38
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF38	ENDP
; Line 133
	PUBLIC	CBUF39
CBUF39	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,39
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF39	ENDP
; Line 134
	PUBLIC	CBUF40
CBUF40	PROC FAR
	push	bp
	mov	bp,sp
	xor	ax,ax
	call	FAR PTR __aFchkstk
;	f = 8
;	n = 6
	mov	ax,40
	push	ax
	push	WORD PTR [bp+6]	;n
	call	FAR PTR _cbuf
	add	sp,4
	mov	sp,bp
	pop	bp
	ret	4
	mov	sp,bp
	pop	bp
	ret	4
	nop	

CBUF40	ENDP
EXECASM_TEXT	ENDS
END
