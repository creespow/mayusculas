#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>    

main (int argc, char* argv[]){
    
    int origen, destino, pipe1[2], pipe2[2], pid;
    int i, j, numlinea, posicion, status, bytes, leido;
    int leidos = 1;
    char cadena1[1000], cadena2[1000];
    char *argvexec[] = {"/home/daniel/c/mayusculas/ponmayusculas", NULL};    

    if (argc != 3){
        printf ("Numero de argumentos de entrada incorrecto. \n");
        exit(1);        
    }

    origen = open(argv[1], O_RDONLY);
    if (origen == -1){
        printf ("Eror al abrir el archivo1\n");
        perror("Open error 1");
        exit(1);
    }
    destino= open(argv[2], O_WRONLY|O_CREAT);
    if (destino == -1){
        printf ("Error al abrir el archivo2\n");
        perror("Open error 2");
        exit(1);
    }

    if (pipe(pipe1) == -1){
        printf ("Error al crear pipe1\n");
        perror ("Pipe error 1");
        exit(1);
    }
    if (pipe(pipe2) == -1){
        printf("Error al crear pipe2\n");
        perror ("Pipe error 2");
        exit(1);
    }    

    pid = fork();
    if (pid == 0){        
        close(pipe1[1]);
        close(pipe2[0]);

        close(0);
        dup(pipe1[0]);
        /*close(pipe1[0]);*/

        close(1);
        dup(pipe2[1]);
        close(pipe2[1]);

        execve(argvexec[0], argvexec, NULL);
        exit(13); 

    }else if (pid < 0){
        printf("Error al crear el hijo\n");
        perror("Fork error");
        exit(1);

    }else{
        close(pipe1[0]);
        close(pipe2[0]); 

        printf("fallo aqui");       

        i = 0;
        numlinea = 1;
        posicion = 0;        
        while (leidos > 0){
            leidos = read(origen, cadena1, 1000);
            if (leidos > 0){
                while (cadena1[i] != '\n' && i<999){
                    cadena2[i] = cadena1[i];
                    i++;
                    if (i == leidos){
                        leidos = read(origen, cadena1, 1000);
                    }
                }
                cadena2[i] = '\n';
                posicion = posicion + i + 1;
                lseek(origen, posicion, SEEK_SET);

                if ((numlinea == 1) || (numlinea%2 == 1)){
                /*/se lo pasamos a ponamyusculas/*/                 
                    j = 0;
                    bytes = i+1;
                    while (bytes > 0){
                        leido = write(pipe1[1], &cadena2[j], bytes);
                        bytes = bytes - leido;
                        j = j + leido;                        
                    }

                    j = 0;
                    bytes = i+1;
                    while (bytes > 0){
                        leido = read(pipe2[0], &cadena2[j], bytes);
                        bytes = bytes -leido;
                        j = j + leido;
                    }
                }

                /*/escribimos la cadena en el fichero salida/*/
                j = 0;
                bytes = i + 1;
                while (bytes > 0){
                    leido = write(destino, &cadena2[j], bytes);
                    bytes = bytes - leido;
                    j = j + leido;
                }
                i = 0;
                numlinea++;

            }else if (leidos < 0){
                    perror("Error al leer del fichero origen: ");
                    exit(-1);
            }
        }    
        close(pipe1[1]);
        close(pipe2[0]);

        pid = wait(&status);
        printf("Se han contado %d líneas y el valor del retorno del hijo es %d.\n", numlinea-1, WEXITSTATUS(status));

        close(origen);
        close(destino);
        exit(0);
    }

    
}