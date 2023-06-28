#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <time.h>
#include <string.h>

int main (){
    int n = 2;
    pid_t pidp = getpid();
    pid_t* pidsh = (pid_t*) malloc(sizeof(pid_t)*n);
    pid_t* pidsn = (pid_t*) malloc(sizeof(pid_t)*n);
    char Buffer_in[20],Buffer_out[20]; 
    //iniciamos las tuberias
    int pps[n*2][2];
    
    for(int i = 0;i<n*2;i++){
        pipe(pps[i]); 
    }

    int ph[n*2][2];
    int id;
    for(id=0; id<n; id++){
        pidsh[id] = fork();
        if(!pidsh[id] ){
            pipe(ph[id +id]);
            pipe(ph[id+id+1]);
            pidsn[id] = fork();
            break;
        }
    }
    //logica de cada proceso
    if(getpid() == pidp){
        //cerrando las tuberias no utilizadas
        close(pps[3][0]);
        close(pps[1][0]);
        close(pps[0][1]);
        close(pps[2][1]);
        //------------
        char cmd[50];
        sprintf(cmd, "pstree -Alp %d", getpid());
        system(cmd);
        //----------
        //logica de padre
        char var[20]="";
        sleep(2);
        while (1){
            printf("por favor ingrese la cadena: ");
            scanf("%s",var);
            if(strcmp(var,"fin")==0){
                printf("cerrando las tuberias hijas por que llego %s \n",var);
                strcpy(Buffer_out,var);
                write(pps[3][1],Buffer_out,sizeof(Buffer_out));
                write(pps[1][1],Buffer_out,sizeof(Buffer_out));
                break;
            }
            strcpy(Buffer_out,var);
            printf("padre[%d] mensaje [%s]\n",getpid(),Buffer_out);
            write(pps[3][1],Buffer_out,sizeof(Buffer_out));
            read(pps[2][0], Buffer_in, sizeof(Buffer_in));
            printf("padre[%d] mensaje [%s]\n",getpid(),Buffer_in);
            write(pps[1][1],Buffer_out,sizeof(Buffer_out));
            read(pps[0][0], Buffer_in, sizeof(Buffer_in));
            printf("padre[%d] mensaje [%s]\n",getpid(),Buffer_in);
        }
        
        for(int i = 0; i<n;i++){
            wait(NULL);
        }
        printf("terminando el padre ...\n");
        //cerrando las tuberias restantes
        close(pps[3][1]);
        close(pps[1][1]);
        close(pps[0][0]);
        close(pps[2][0]);
    }else{
        if(getppid()==pidp){
            //cerrando las tuberias no utilizadas
            //cerrando las tuberias heredadas del hermano
            if(id == 0){
                close(pps[2][1]);
                close(pps[3][1]);
                close(pps[2][0]);
                close(pps[3][0]);
            }else{
                close(pps[0][1]);
                close(pps[1][1]);
                close(pps[0][0]);
                close(pps[1][0]);
            }
            close(pps[id +id +1][1]);
            close(ph[id +id][1]);
            close(ph[id +id +1][0]);
            close(pps[id +id][0]);
            //logica de hijos
            while(1){
                read(pps[id +id +1][0], Buffer_in, sizeof(Buffer_in));
                if(strcmp(Buffer_in,"fin")==0){
                    printf("cerrando la tuberia nieta %d por que llego %s \n",id,Buffer_in);
                    strcpy(Buffer_out,Buffer_in);
                    write(ph[id+id+1][1],Buffer_out,sizeof(Buffer_out));
                    break;
                }
                printf("hijo[%d] mensaje [%s]\n",getpid(),Buffer_in);
                strcpy(Buffer_out,Buffer_in);
                write(ph[id+id+1][1],Buffer_out,sizeof(Buffer_out));
                read(ph[id +id][0], Buffer_in, sizeof(Buffer_in));
                printf("hijo[%d] mensaje [%s]\n",getpid(),Buffer_in);
                strcpy(Buffer_out,Buffer_in);
                write(pps[id+id][1],Buffer_out,sizeof(Buffer_out));
            }
            //cerrando las tuberias restantes
            close(pps[id +id +1][0]);
            close(ph[id +id][0]);
            close(ph[id +id +1][1]);
            close(pps[id +id][1]);
            wait(NULL);
            exit(0);
        }else{
            //cerrando tuberias no utilizadas
            
            close(pps[3][0]);
            close(pps[1][0]);
            close(pps[0][1]);
            close(pps[2][1]);
            close(pps[3][1]);
            close(pps[1][1]);
            close(pps[0][0]);
            close(pps[2][0]);
            close(ph[id+id+1][1]);
            close(ph[id+id][0]);
            //logica de nietos
            while(1){
                read(ph[id +id +1][0], Buffer_in, sizeof(Buffer_in));
                if(strcmp(Buffer_in,"fin")==0){
                    break;
                }
                printf("nieto[%d] mensaje [%s]\n",getpid(),Buffer_in);
                strcpy(Buffer_out,Buffer_in);
                write(ph[id+id][1],Buffer_out,sizeof(Buffer_out));
            }
            //cerrando tuberias restantes
            close(ph[id+id+1][0]);
            close(ph[id+id][1]);
            exit(0);
        }
    }
}