;1, Move the 64 bit value 0xb5 into the c register
mov rcx, 0xb5
;Assume that there are three local integer variables x,y and z that have been allo-
;cated on the stack at offsets from the base pointer (rbp) of -4, -8 and -12 bytes re-
;spectively. Write the single x64 instruction to load the lower 8 bits of the variable
;y into lower 8 bits of the a register.
mov al, [rpb-4] ; dont know about this one
;Load the address(64 bits) of the variable z into the b register
lea rbx, [rpb-12]
;Store the 32 bit value in register a into the memory location whose address in register b
mov [rbx], eax