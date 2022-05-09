;test to Maman 14 - file without errors

    .entry LIST
    .extern XXX
MAIN: add r3, LIST
jsr XXX
LOOP: prn #48
lea STR, r6
inc r6
mov r6, L3
sub r1, r4
cmp r3, #-6
bne END
add r7, r5
cmp    #9 , #-999
; commmmmmmeeennnnnnt
sub XXX   ,   	MAIN
clr K
jmp &END
sub L3, L3
MMMM:    .entry MAIN
    .extern XXX

red r7

jmp LOOP
END: stop
STR: .string "hello:   it's	me"

LIST: .data 6, -9
    .data -100   ,   53

K: .data 31
    .extern L3
