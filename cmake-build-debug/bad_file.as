; test file for Maman 14 , Contains a variety of errors

NUM: .entry NUM
add r3,   #4
cmp #5, #-29

; not a good comment!

; too long label name:
    LabelName121232Too21Long666uaahjuu: jsr r3

LAbel##55: .data 789

    .data '5  , -25,  -5, 14, 1
    .data , 5,   12 ,  -110

    .data 51 ,, 2322 , 457
    .data 34  , -100 ,

    MMM:

Main: .string "abc"
Maaa: .string , "abc"

LLLLOOOOOOONNNNNG: .string "A very very very very very very very long line!!!!!!"

Main: .data  5
    .entry Main
lea r2 , r5
string: mov #5 , r8

stop r5
    .string fbidvb  4y783 jbdvijd 543 &*"
    .string "ggregtrhgthc

    .string  sscsucscws "

    .add &LKJ , PPP
; HELLLOOO
clr r11

bne #44

bne &r5

sub r3 , MMM , OOO

rts LL

LLLLLL: .entry 8KJH

    .extern Main

; TOO LONG comment comment comment comment comment comment comment comment!!!!!!!