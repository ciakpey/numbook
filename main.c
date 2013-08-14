#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#define Mat(img,i,j) (img->imageData +(i)*img->widthStep +(j)* img->nChannels)
#define poly(x,tab) (tab[0] + tab[1]*(x)+tab[2]*(x)*(x) +tab[3]*(x)*(x)*(x))


int cmpcolor(uchar* pixel,uchar ref1,uchar ref2,uchar ref3,float tolerance){
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
	 char *ligne;
	 for(x=0;x< image->width;x++){
		 h=0;
		 b=image->height-1;
		 ligne = x*image->nChannels +image->imageData;
		 while (cmpcolor((uchar *)ligne+h*image->widthStep,couleur.val[0],couleur.val[1],couleur.val[2],tolerance) && h<image->height){
			 h++;
		}
		while(cmpcolor((uchar *)ligne+b*image->widthStep,couleur.val[0],couleur.val[1],couleur.val[2],tolerance) && b>0	){
			b--;
		}
		*(bordurebas+x)=b;
		*(bordurehaut+x)=h;
		
	}
}

void cubicregression( int *tab, float resul[4],int size){
		int i;
		FILE *fp ;
		fp=fopen("/home/man/opencv/numbook/sortie.csv","w");
		if (fp==NULL)
			printf("Erreur à l'écriture du fichier sortie.csv");
		else{
			fprintf(fp,"x , y1\n");
			for(i=0;i<size;i++){
				fprintf(fp,"%d , %d\n",i,tab[i]);
			}
		
			char resultat[200];
			FILE *fp2;
			fp2=popen("~/opencv/numbook/script.sh","r");
			fgets(resultat,200,fp2);fgets(resultat, 200,fp2);
				char res[20];
			int j=0;int k=0;int i=0;
			while(i<4){
				if (resultat[j]!=' ') {
					res[k]=resultat[j];
					k++;j++;
				}
				else if ((resultat[j]=' ' )&& (resultat[j+1]!=' ')&& (k>0)){
					res[k]='\0';
					resul[i]=atof(res);
					k=0;j++;i++;
				}
				else {
					j++;
					res[k]='\0';
				}
				}
				printf("\n%f+x*%f+x^2*%f+x^3*%f\n",resul[0],resul[1],resul[2],resul[3]); 
				fclose(fp2);
				fclose(fp);
			}
			
		}


void dewarp(IplImage * src,IplImage * dst,int *bordurehaut, int *bordurebas,int pos){
	
	IplImage *matx,*maty;
	matx=cvCreateImage(cvSize(src->height,src->width),IPL_DEPTH_32F,1);
	maty=cvCreateImage(cvSize(src->height,src->width),IPL_DEPTH_32F,1);
	dst->origin = src->origin;
	int inf=0;
	int sup=src->width;
		int x,y;
	for(x=0;x<src->width;x++){
			if (bordurehaut[x]<bordurehaut[inf]){
				inf=x;
			}
			if(bordurebas[x]>bordurebas[sup]){
				sup=x;
			}
		}
		int supp,inff;
		supp=bordurebas[sup];
		inff=bordurehaut[inf];
		float polynomhaut[4],polynombas[4];
		cubicregression (bordurehaut,polynomhaut,src->width);
		cubicregression(bordurebas,polynombas,src->width);


	/*for(x=0;x<src->width;x++){
		for(y=inff;y<supp;y++){
			*(Mat(matx,y,x))=x;
			if (y<pos){
					*(Mat(matx,y,x))=*(Mat(src,(int) pos +(pos-inff)*(y-pos)/(pos-bordurehaut[x]),x));
				}
				else{
					*(Mat(maty,y,x))=*(Mat(src,(int) pos +(pos-y)/(bordurebas[x]-pos)*(pos-supp) ,x));
				}
		}
	}
	cvRemap(src,dst,matx,maty,CV_INTER_LINEAR,cvScalarAll(255));*/
	int i;
	for(x=0;x<src->width;x++){
		for(y=inff;y<supp;y++){
			for(i=0;i<3;i++){
				if (y<pos){
					*(Mat(dst,y,x)+i)=*(i+Mat(src,(int) (pos - (pos-poly(x,polynomhaut)/*bordurehaut[x]*/)*(pos-y)/(pos-inff)) ,x));
				}
				else{
					*(Mat(dst,y,x)+i)=*(i+Mat(src,(int)( pos +(pos-y)*(poly(x,polynombas)/*bordurebas[x]*/ -pos)/(pos-supp)) ,x));
				}
			}
		}
	}

	
	cvNamedWindow ("essai", CV_WINDOW_NORMAL);
	cvShowImage ("essai", dst);
	cvWaitKey(0);
	
	cvReleaseImage(&matx);
	cvReleaseImage(&maty);
}
	void tracerbordure(IplImage *img,int *tab1,int *tab2){
		int i;
		for(i=0;i<img->width;i++){
			*(Mat(img,tab1[i],i)+2)=255;
			*(Mat(img,tab2[i],i)+2)=255;
			}
	}
	
	
					
		


int main (int argc, char* argv[])
{
	IplImage* img = NULL;
  int error = 0;
 
  /* Vérification des arguments */
  if (argc < 2)
  {
    fprintf (stderr, "usage: %s SRC_IMAGE \n", argv[0]);
    return EXIT_FAILURE;
  }
 
  /* Chargement du fichier image à convertir */
  if (!(img = cvLoadImage (argv[1], CV_LOAD_IMAGE_UNCHANGED)))
  {
    fprintf (stderr, "couldn't open image file: %s\n", argv[1]);
    return EXIT_FAILURE;
  }
 
	cvNamedWindow ("test", CV_WINDOW_NORMAL);
	cvShowImage ("test", img);
	cvWaitKey(0);
	
	IplImage *dst;
	dst=cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U,3);
	
	int *tabx=malloc((img->width)*sizeof(int));
	int *taby = malloc((img->width)*sizeof(int));
	CvScalar pixel;
	pixel.val[0]=0;
	pixel.val[1]=0;
	pixel.val[2]=0;
	bordures(img,tabx,taby,pixel,100);
	tracerbordure(img,tabx,taby);
	cvNamedWindow ("test1", CV_WINDOW_NORMAL);
	cvShowImage ("test1", img);
	cvWaitKey(0);
	dewarp(img,dst,tabx,taby,400);
	
  /* Libération de la mémoire */
  cvReleaseImage (&img);
  cvReleaseImage(&dst);
	free(tabx);
	free(taby);
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
	
}
