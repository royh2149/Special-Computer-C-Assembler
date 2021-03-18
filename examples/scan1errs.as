; file scan1errs

.entry LIST

; no comma
MAIN:	add r3 LIST

; illegal symbol name
r6:	lea W, r6

; too few arguments
	add W

; invalid addressing method
	add %LIST, #46

; invalid addressing method
	sub #46, %LIST

; too few arguments
	cmp #47,

; too few arguments
	cmp

; comma at the end of .data
.data 5,6,  7,

; too long symbol name
ABCDEEFDHIJKLMNOPQRSTUVWXYZAAAAAAAAAAAAA:	.data 6,7

; no space after label name
LABEL:add r6, #48

; an invalid scalar
	add #48.76, r2

; empty #
	add #, r3

; empty label
A:

; multiple declaration of external symbol
.extern EXTSYMBOL
.extern EXTSYMBOL

; double definition of a symbol
DOUBLEDEF: .data 6
DOUBLEDEF:	add #47, r5

; empty .data and .string
EMPTYDATA:	.data
EMPTYSTRING:	.string

; no comma between .data arguments
NOCOMMADATA: .data 5 6, 7

; unrecognized extra text 
	add #128  , r2 more_text
	.string    "vnjrv" extra
.extern EXTVAR another_text
