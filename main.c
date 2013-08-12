#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>


int cmpcolor(char* pixel,char ref1,char ref2,char ref3,float tolerance){
	return (((abs(*pixel -ref1) < tolerance) && 
			 (abs(*(pixel+1)-ref2)<tolerance) && 
			 (abs(*(pixel+2)-ref3)<tolerance)) ? 1 : 0);
}

void bordures(IplImage * image, int *bordurehaut,int *bordurebas, CvScalar couleur, float tolerance){
	/* La fonction va chercher dans l'image les bordures de la page du livres,
	 * bordurehaut/bordurebas sont des tableau de sizex(image)
	 * bordurehaut[i]/bas[i] qui contient l'ordonné de du debut/fin de page à l'abscisse i 
	 * couleur est la couleur de fond de l'image */
	 
	 
	 int x,h,b;
	 for(x=0;x<= image->width;x++){
		 b=(h=0);
		 while (cmpcolor(image->imageData+3*h+3*x*image->width,couleur.val[0],couleur.val[1],couleur.val[2],tolerance) && h<image->width){
			 h++;
		}
		while(cmpcolor(image->imageData+3*(x+1)*image->width -(b+1)*3,couleur.val[0],couleur.val[1],couleur.val[2],tolerance) && b<image->width){
			b++;
		}	
		*(bordurebas+x)=b;
		*(bordurehaut+x)=h;
	}
}
		
	 



int main (int argc, char* argv[])
{
	
	
	
	
	return 0;
}
