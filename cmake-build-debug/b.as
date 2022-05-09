; valid file (with a warning in line 6)

.entry ZZZ777

ZZZ777: .string "ab cd  :	; ef "
EEE:   .extern X
mov #-23, X
cmp Yx, r6
Yx: .data 105, -4, -1, 5
    .entry DDD

clr Yx
add DDD, r1
DDD: .data 1108
dec Yx
jmp &Yx
prn #7
sub DDD, Yx
prn ZZZ777

cmp DDD, r2

jmp JJ

END:     stop

bne &END

JJ: .string "  beuf feibf vdwvfy %%% 2@@ "
MMMMMMMMMMM: .data     9 , -77 , 100 , 0 ,  12 , -13 , 15

    .extern JJ
;end of code

