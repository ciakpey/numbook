#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>


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
void dewarp(IplImage * src,IplImage * dst,int *bordurehaut, int *bordurebas){
	
	IplImage *matx,*maty;
	matx=cvCreateImage(cvSize(src->height,src->width),IPL_DEPTH_8U,1);
	maty=cvCreateImage(cvSize(src->height,src->width),IPL_DEPTH_8U,1);
	int inf=0;
	int sup=src->width;
		int x,y;
	for(x=0;x<src->height;x++){
			if (bordurehaut[x]<bordurehaut[inf]){
				inf=x;
			}
			if(bordurebas[x]>bordurebas[inf]){
				sup=x;
			}
		}
	

	for(x=0;x<src->height;x++){
		for(y=0;y<src->width;y++){
			*(matx->imageData+x*src->width+y)=x;
			*(maty->imageData+x*src->width+y)=(bordurebas[x]-bordurehaut[x])*y/(sup-inf);
		}
	}
	cvRemap(src,dst,matx,maty,CV_INTER_LINEAR,cvScalarAll(0));

	
	cvNamedWindow ("essai", CV_WINDOW_NORMAL);
	cvShowImage ("essai", src);
	
	
	cvReleaseImage(&matx);
	cvReleaseImage(&maty);
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
	
	IplImage * dst = NULL;
	dst=cvCloneImage(img);
	dst->origin = img->origin;
	int *tabx=malloc((img->height)*sizeof(int));
	int *taby = malloc((img->height)*sizeof(int));
	CvScalar pixel;
	pixel.val[0]=0;
	pixel.val[1]=0;
	pixel.val[2]=0;
	bordures(img,tabx,taby,pixel,100);
	/*dewarp(img,dst,tabx,taby);
	cvNamedWindow("test2",CV_WINDOW_NORMAL);
	cvShowImage("test2",dst);
	cvWaitKey(0);*/
	
	/*uchar * p;
	int i=0;
	p= (uchar *)img->imageData;
	while(p-(uchar *)img->imageData<img->imageSize-1){
		if (i > img->width-1 ) {
			i=0;
			printf("\n");
		}
		i++;
		printf("%d|", *p);
		p+=4;
	}     
	printf("\n");*/
  /* Libération de la mémoire */
  cvReleaseImage (&img);
  cvReleaseImage(&dst);
	free(tabx);
	free(taby);
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
	
}
