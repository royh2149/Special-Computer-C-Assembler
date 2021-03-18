.extern TOJUMP

; relative addressing on external variable
jmp %TOJUMP

; a symbol that is both extern and entry
.extern K
.entry K

; jumping to a .data symbol
JMPDATA:	.data 6, 7, 8
	jmp %JMPDATA

; extra text after .entry
.entry JMPDATA more_text
