* TINY Compilation to TM Code
* File: teste_mdc.tm
* Standard prelude:
  0:     LD  6,0(0) 	load maxaddress from location 0
  1:     ST  0,0(0) 	clear location 0
* End of standard prelude.
  2:     IN  0,0,0 	read integer value
  3:     ST  0,0(5) 	read: store value
  4:     IN  0,0,0 	read integer value
  5:     ST  0,1(5) 	read: store value
* -> if
* -> Op
* -> Id
  6:     LD  0,1(5) 	load id value to ac
* <- Id
* -> Const
  7:    LDC  1,0(0) 	load const ac1
* <- Const
  8:    SUB  0,0,1 	op ==
  9:    JEQ  0,2(7) 	br if true
 10:    LDC  0,0(0) 	false case
 11:    LDA  7,1(7) 	unconditional jmp
 12:    LDC  0,1(0) 	true case
* <- Op
* if: jump to else belongs here
* -> assign
* -> Const
 14:    LDC  0,0(0) 	load const ac
* <- Const
 15:     ST  0,1(5) 	assign: store value
* <- assign
* if: jump to end belongs here
 13:    JEQ  0,3(7) 	if: jmp to else
* -> repeat
* repeat: jump after body comes back here
* -> assign
* -> Id
 17:     LD  0,1(5) 	load id value to ac
* <- Id
 18:     ST  0,2(5) 	assign: store value
* <- assign
* -> assign
* -> Op
* -> Id
 19:     LD  0,0(5) 	load id value to ac
* <- Id
* -> Op
* -> Op
* -> Id
 20:     LD  1,0(5) 	load id value to ac1
* <- Id
* -> Id
 21:     LD  2,1(5) 	load id value to ac1
* <- Id
 22:    DIV  0,0,1 	op /
* <- Op
* -> Id
 23:     LD  2,1(5) 	load id value to ac1
* <- Id
 24:    MUL  0,0,1 	op *
* <- Op
 25:    SUB  0,0,1 	op -
* <- Op
 26:     ST  0,1(5) 	assign: store value
* <- assign
* -> assign
* -> Id
 27:     LD  0,2(5) 	load id value to ac
* <- Id
 28:     ST  0,0(5) 	assign: store value
* <- assign
* -> Op
* -> Id
 29:     LD  0,1(5) 	load id value to ac
* <- Id
* -> Const
 30:    LDC  1,0(0) 	load const ac1
* <- Const
 31:    SUB  0,0,1 	op ==
 32:    JEQ  0,2(7) 	br if true
 33:    LDC  0,0(0) 	false case
 34:    LDA  7,1(7) 	unconditional jmp
 35:    LDC  0,1(0) 	true case
* <- Op
 36:    JEQ  0,-20(7) 	repeat: jmp back to body
* <- repeat
 16:    LDA  7,20(7) 	jmp to end
* <- if
* -> Id
 37:     LD  0,0(5) 	load id value to ac
* <- Id
 38:    OUT  0,0,0 	write ac
* End of execution.
 39:   HALT  0,0,0 	
