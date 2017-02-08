#include <unistd.h>
#include <stdlib.h>

char in;
int leidos=1, escritos=0;

main(){
	while (leidos!=0){ 
		leidos=read(0,&in,1);
		if (leidos==1){
			if ((in>='a')&&(in<='z'))
			in=in-(char)32;
			/* con las ñ nos metemos en caracteres que se representan 
			con más de un byte, asi que lo dejamos
			else if (in=='ñ') in='Ñ';*/
		}
		escritos=write(1,&in,1);
	}
	exit(37);
}
