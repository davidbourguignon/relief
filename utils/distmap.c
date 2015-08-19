/*=======================================================================

  Program:  Distance Transforms
  Module:   $Id: ssedp.c,v 1.1.1.1 2002/03/22 10:40:02 cuisenai Exp $
  Date:     $Date: 2002/03/22 10:40:02 $
  Language: C
  
  Author:   Olivier Cuisenaire, http://ltswww.epfl.ch/~cuisenai
            
  Descr.: distmap_4ssedp(int *map, int n[2]) 

   Takes an image (map) of integers of size n[0]*n[1] as input. It 
   computes for all pixels the square of the Euclidean distance to 
   the nearest zero pixel using the 4SSED+ algorithm. 

  Reference for the algorithm: 
  
   Olivier Cuisenaire and Benoit Macq, "Fast and exact signed Euclidean 
   distance transformation with linear complexity", ICASSP'99 - IEEE Intl 
   Conference on Acoustics, Speech and Signal Processing, Phoenix, USA, 
   March 15-19, 1999, Proceedings - Vol. 6, pp. 3293-3296.	    
   
  Reference for the implementation:

   Chapter 5 of "Distance transformations: fast algorithms and applications 
   to medical image processing", Olivier Cuisenaire's Ph.D. Thesis, October 
   1999, Université catholique de Louvain, Belgium.
    
  Terms of Use: 

   You can use/modify this program for any use you wish, provided you cite 
   the above references in any publication that uses it. 
    
  Disclaimer:

   In no event shall the authors or distributors be liable to any party for 
   direct, indirect, special, incidental, or consequential damages arising out 
   of the use of this software, its documentation, or any derivatives thereof, 
   even if the authors have been advised of the possibility of such damage.
   
   The authors and distributors specifically disclaim any warranties, including,
   but not limited to, the implied warranties of merchantability, fitness for a
   particular purpose, and non-infringement.  this software is provided on an 
   "as is" basis, and the authors and distributors have no obligation to provide
   maintenance, support, updates, enhancements, or modifications.
   
=========================================================================*/

#include "distmap.h"

void  propagate_pp(int,int,int *,int n[],int *);
void  propagate_pm(int,int,int *,int n[],int *);
void  propagate_mp(int,int,int *,int n[],int *);
void  propagate_mm(int,int,int *,int n[],int *);
void error(int,int,int,int,int);
     
int distmap_4ssedp( int *map, int n[2] )
{
  /***********************
    
    IMPORTANT NOTICE: at first we generate a signed EDT, under the form 
    of a array n[0]xn[1] of vectors (dx,dy) coded as two shorts in the
    memory case of an integer in the input image. 

    Later, these two shorts are replaced by the integer value dx*dx+dy*dy

    **********************/

  int *sq,*dummy;

  int i;
  int nmax,maxi;
  int maxx=n[0]-1; int maxy=n[1]-1;

  int x,y,*pt,*tpt;
  short *dx,*dy,*tdx,*tdy;

  short *ts;
  int *ti;

  int difx,dify;//,ok;
  int test_integer[1];
  short *test_short;

 /* initialisation */

  nmax=n[0]; if(nmax<n[1]) nmax=n[1];
  dummy=(int*)calloc(4*nmax+1,sizeof(int)); sq=dummy+2*nmax;
  for(i=2*nmax;i>=0;i--) sq[-i]=sq[i]=i*i;

  ti=&maxi; ts=(short*)ti; *ts=nmax; *(ts+1)=nmax;
  maxi=nmax+256*256*nmax;

  for(y=0,pt=map;y<=maxy;y++)
    for(x=0;x<=maxx;x++,pt++) 
      if(*pt!=0) *pt=maxi;
  
  /* compute simple signed DT */


  /* first raster scan */

  for(y=0;y<=maxy;y++)
    {
      pt=map+y*n[0];
      for(x=0;x<=maxx;x++,pt++) 
	{
	  dx=(short *)pt;
	  dy=dx+1;
	  
	  if(y>0)
	    {
	      tpt=pt-n[0];
	      tdx=(short *)tpt;
	      tdy=tdx+1;
	      if(sq[*dx]+sq[*dy]>sq[*tdx]+sq[*tdy+1]) { *dx=*tdx; *dy=*tdy+1; }
	    }

	  if(x>0)
	    {
	      tpt=pt-1;
	      tdx=(short *)tpt;
	      tdy=tdx+1;
	      if(sq[*dx]+sq[*dy]>sq[*tdx+1]+sq[*tdy]) { *dx=*tdx+1; *dy=*tdy; }
	    }
	}
      pt=map+y*n[0]+maxx-1;
      for(x=maxx-1;x>=0;x--,pt--) 
	{
	  dx=(short *)pt;
	  dy=dx+1;
	  
	  tpt=pt+1;
	  tdx=(short *)tpt;
	  tdy=tdx+1;
	  if(sq[*dx]+sq[*dy]>sq[*tdx-1]+sq[*tdy]) { *dx=*tdx-1; *dy=*tdy; }
	}
    }

  /* second raster scan */

  for(y=maxy,pt=map+n[0]*n[1]-1;y>=0;y--)
    {
      pt=map+n[0]*y+maxx;
      for(x=maxx;x>=0;x--,pt--) 
	{
	  dx=(short *)pt;
	  dy=dx+1;
	  
	  if(y<maxy)
	    {
	      tpt=pt+n[0];
	      tdx=(short *)tpt;
	      tdy=tdx+1;
	      if(sq[*dx]+sq[*dy]>sq[*tdx]+sq[*tdy-1]) { *dx=*tdx; *dy=*tdy-1; }
	    }

	  if(x<maxx)
	    {
	      tpt=pt+1;
	      tdx=(short *)tpt;
	      tdy=tdx+1;
	      if(sq[*dx]+sq[*dy]>sq[*tdx-1]+sq[*tdy]) { *dx=*tdx-1; *dy=*tdy; }
	    }
	}
      pt=map+n[0]*y+1;
      for(x=1;x<=maxx;x++,pt++) 
	{
	  dx=(short *)pt;
	  dy=dx+1;
	  
	  tpt=pt-1;
	  tdx=(short *)tpt;
	  tdy=tdx+1;
	  if(sq[*dx]+sq[*dy]>sq[*tdx+1]+sq[*tdy]) { *dx=*tdx+1; *dy=*tdy; }
	}
    }

  /* improve signed DT */

  test_short=(short*)test_integer;
  test_short[0]=1;
  test_short[1]=0;
  difx=test_integer[0];
  test_short[0]=0;
  test_short[1]=1;
  dify=test_integer[0];

  for(y=1;y<maxy;y++)
    {
      pt=map+y*n[0];
      for(x=0;x<maxx;x++,pt++)
	{
	  if((*(pt+1)-*pt)!=difx) 
	    {	    
	      dx=(short*)pt;
	      if(*dx>0) 
		{
		  dy=dx+1;
		  if(*dy>0) if((*(pt+n[0])-*pt)!=dify)
		    propagate_pp(x,y,pt,n,sq);
		  if(*dy<0) if((*pt-*(pt-n[0]))!=dify)
		    propagate_pm(x,y,pt,n,sq);
		}
	      
	      pt++;
	      dx=(short*)pt;	    
	      if(*dx<0) 
		{
		  dy=dx+1;
		  if(*dy>0) if((*(pt+n[0])-*pt)!=dify)
		    propagate_mp(x+1,y,pt,n,sq);
		  if(*dy<0) if((*pt-*(pt-n[0]))!=dify)
		    propagate_mm(x+1,y,pt,n,sq);
		}
	      pt--;
	    }
	}
    }
  
  for(y=0,dx=(short *)map,pt=(int *)map;y<=maxy;y++)
    for(x=0;x<=maxx;x++,pt++,dx+=2) 
	*pt=sq[*dx]+sq[*(dx+1)];

  return(1);

}

void  propagate_pp(int x,int y,int *pt,int n[3], int *sq)
{
  int *tpt;
  short *dx,*dy,*tdx,*tdy;
  
  short DA,DB,DC,DD,DX,DY;
  float A,B,C,D;
  float step_min,step_max;
  float min,max;
  
  int i,j,i_max;
  int ok;

  /* current point */

  dx=(short *)pt; dy=dx+1;
  DX=*dx; DY=*dy;
  
  /* line of maxima */
  
  DA = *(dx+2*n[0]); DB = *(dy+2*n[0])-1;
  if(DY==DB) error(DA,DB,DX,DY,1) ; else
    step_max = (float)(DA-DX)/(float)(DY-DB);
  A = (DX+DA)*0.5; B= (DY+DB)*0.5;
  max = A * step_max - B ;

  /* line of minima */

  DC = *(dx+2)-1; DD = *(dy+2);
  if(DY!=DD) 
    {
      step_min = (float)(DC-DX)/(float)(DY-DD);
      C = (DX+DC)*0.5; D= (DY+DD)*0.5;
      min = C * step_min - D ;

      /* maximum i in (i,j) masks */

      if(fabs(step_min - step_max) < 0.0000001 )
	/* step_min and step_max are in the same direction. Takes 
	   floating point inaccuracies into account. */
	{
	  if( fabs(step_min-1) < 0.0000001 )
	    /* both borders go in the 45° direction ->
	       check all neighbours in the 45° direction until
	       one is not changed, then end search. */
	    {
	      i=0; ok=1;
	      while(ok)
		{
		  i++;
		  if(((x+i)<n[0])&&((y+i)<n[1]))
		    {
		      tpt=pt+i*(1+n[0]);
		      tdx=(short *)tpt; tdy=tdx+1;
		      if( (sq[DX+i]+sq[DY+i]) < (sq[*tdx]+sq[*tdy]) )
			{ *tdx=DX+i; *tdy=DY+i; }
		      else
			ok=0;
		    }
		  else ok=0;
		}
	    }
	  else 
	    i_max=0;
	}
      else
	i_max = ( max - min ) / ( step_min - step_max );
      
      /* go through all (i,j) between the min and max lines */
      
      min+=step_min; max+=step_max;
      for(i=1;i<=i_max;i++,min+=step_min,max+=step_max)
	if((x+i)<n[0])
	  for(j=min;j<=max;j++)
	    if((y+j)<n[1])
	      {
		tpt=pt+i+j*n[0];
		tdx=(short *)tpt; tdy=tdx+1;
		if( (sq[DX+i]+sq[DY+j]) < (sq[*tdx]+sq[*tdy]) )
		  {
		    *tdx=DX+i; *tdy=DY+j;	    
		  }
	      }
    }
}

void  propagate_pm(int x,int y,int *pt,int n[3],int *sq)
{
  /* based on propagate_pp. modified lines are followed by empty comments */

  int *tpt;
  short *dx,*dy,*tdx,*tdy;
  
  short DA,DB,DC,DD,DX,DY;
  float A,B,C,D;
  float step_min,step_max;
  float min,max;
  
  int i,j,i_max;
  int ok;

  /* current point */

  dx=(short *)pt; dy=dx+1;
  DX=*dx; DY=*dy;
  
  /* line of maxima */
  
  DA = *(dx-2*n[0]); DB = *(dy-2*n[0])+1;      /*  */
  if(DY==DB) error(DA,DB,DX,DY,2) ; else 
    step_max = (float)(DA-DX)/(float)(DY-DB);
  A = (DX+DA)*0.5; B= (DY+DB)*0.5;
  max = A * step_max - B ;
  
  /* line of minima */
  
  DC = *(dx+2)-1; DD = *(dy+2);
  if(DY!=DD) 
    {
      step_min = (float) (float)(DC-DX)/(float)(DY-DD);
      C = (DX+DC)*0.5; D= (DY+DD)*0.5;
      min = C * step_min - D ;
      
      /* maximum i in (i,j) masks */
      
      if(fabs(step_min - step_max) < 0.000001 )
	{
	  if( fabs(step_min+1) < 0.000001 ) 
	    {
	      i=0; ok=1;
	      while(ok)
		{
		  i++;
		  if(((x+i)<n[0])&&((y-i)>=0)) /* */
		    {
		      tpt=pt+i*(1-n[0]); /* */
		      tdx=(short *)tpt; tdy=tdx+1;
		      if( (sq[DX+i]+sq[DY-i]) < (sq[*tdx]+sq[*tdy]) ) /* */
			{ *tdx=DX+i; *tdy=DY-i; } /* */
		      else
			ok=0;
		    }
		  else ok=0;
		}
	    }
	  else i_max=0;
	}
      else
	i_max = ( max - min ) / ( step_min - step_max );

      /* go through all (i,j) between the min and max lines */
      
      min+=step_min; max+=step_max;
      for(i=1;i<=i_max;i++,min+=step_min,max+=step_max)
	if((x+i)<n[0])
	  for(j=min;j>=max;j--) /* */
	    if((y+j)>=0)          /* */
	      {
		tpt=pt+i+j*n[0];  
		tdx=(short *)tpt; tdy=tdx+1;
		if( (sq[DX+i]+sq[DY+j]) < (sq[*tdx]+sq[*tdy]) )
		  {
		    *tdx=DX+i; *tdy=DY+j;	    
		  }
	      }
    }
}

void  propagate_mp(int x,int y,int *pt,int n[3],int *sq)
{
  /* based on propagate_pp. modified lines are followed by empty comments */

  int *tpt;
  short *dx,*dy,*tdx,*tdy;
  
  short DA,DB,DC,DD,DX,DY;
  float A,B,C,D;
  float step_min,step_max;
  float min,max;
  
  int i,j,i_max;
  int ok;

  /* current point */

  dx=(short *)pt; dy=dx+1;
  DX=*dx; DY=*dy;

   /* line of maxima */

  DA = *(dx+2*n[0]); DB = *(dy+2*n[0])-1;
  if(DY==DB) error(DA,DB,DX,DY,3) ; else 
    step_max = (float)(DA-DX)/(float)(DY-DB);
  A = (DX+DA)*0.5; B= (DY+DB)*0.5;
  max = A * step_max - B ;

  /* line of minima */
  
  DC = *(dx-2)+1; DD = *(dy-2);   /*  */
  if(DY!=DD) { 
    step_min = (float)(DC-DX)/(float)(DY-DD);
    C = (DX+DC)*0.5; D= (DY+DD)*0.5;
    min = C * step_min - D ;
    
    /* maximum i in (i,j) masks */
    
    if(fabs(step_min - step_max) < 0.000001 )
      {
	if( fabs(step_min+1) < 0.000001 ) 
	  {
	    i=0; ok=1;
	    while(ok)
	      {
		i++;
		if(((x-i)>=0)&&((y+i)<n[1])) /* */
		  {
		    tpt=pt+i*(n[0]-1); /* */
		    tdx=(short *)tpt; tdy=tdx+1;
		    if( (sq[DX-i]+sq[DY+i]) < (sq[*tdx]+sq[*tdy]) ) /* */
		      { *tdx=DX-i; *tdy=DY+i; } /* */
		    else
		      ok=0;
		  }
		else ok=0;
	      }
	  } 
      else 
        i_max=0; 
      }
    else
      i_max = ( max - min ) / ( step_min - step_max );
    
    /* go through all (i,j) between the min and max lines */
    
    min-=step_min; max-=step_max;  /* */
    for(i=-1;i>=i_max;i--,min-=step_min,max-=step_max) /* */
      if((x+i)>=0) /* */
	for(j=min;j<=max;j++)
	  if((y+j)<n[1])
	    {
	      tpt=pt+i+j*n[0];
	      tdx=(short *)tpt; tdy=tdx+1;
	      if( (sq[DX+i]+sq[DY+j]) < (sq[*tdx]+sq[*tdy]) )
		{
		  *tdx=DX+i; *tdy=DY+j;	    
		}
	    }
  }
}

void  propagate_mm(int x,int y,int *pt,int n[3],int *sq)
{
  /* based on mp */
  
  int *tpt;
  short *dx,*dy,*tdx,*tdy;
  
  short DA,DB,DC,DD,DX,DY;
  float A,B,C,D;
  float step_min,step_max;
  float min,max;
  
  int i,j,i_max;
  int ok;

  /* current point */

  dx=(short *)pt; dy=dx+1;
  DX=*dx; DY=*dy;

   /* line of maxima */

  DA = *(dx-2*n[0]); DB = *(dy-2*n[0])+1; /* this line changed from mp */
  if(DY==DB) error(DA,DB,DX,DY,4); else 
    step_max = (float)(DA-DX)/(float)(DY-DB);
  A = (DX+DA)*0.5; B= (DY+DB)*0.5;
  max = A * step_max - B ;

  /* line of minima */

  DC = *(dx-2)+1; DD = *(dy-2);  
  if(DY!=DD) { 
    step_min = (float) (float)(DC-DX)/(float)(DY-DD);
    C = (DX+DC)*0.5; D= (DY+DD)*0.5;
    min = C * step_min - D ;
    
    /* maximum i in (i,j) masks */
    
    if(fabs(step_min - step_max) < 0.000001 )
      {
	if( fabs(step_min-1) < 0.000001 ) 
	  {
	    i=0; ok=1;
	    while(ok)
	      {
		i++;
		if(((x-i)>=0)&&((y-i)>=0)) /* */
		  {
		    tpt=pt-i*(n[0]+1); /* */
		    tdx=(short *)tpt; tdy=tdx+1;
		    if( (sq[DX-i]+sq[DY-i]) < (sq[*tdx]+sq[*tdy]) ) /* */
		      { *tdx=DX-i; *tdy=DY-i; } /* */
		    else
		      ok=0;
		  }
		else ok=0;
	      } 
	  }
	else 
	  i_max=0; 
      }
    else
      i_max = ( max - min ) / ( step_min - step_max );
    
    /* go through all (i,j) between the min and max lines */
    
    min-=step_min; max-=step_max;  
    for(i=-1;i>=i_max;i--,min-=step_min,max-=step_max)
      if((x+i)>=0)
	for(j=min;j>=max;j--)
	  if((y+j)>=0)   /*  */
	    {
	      tpt=pt+i+j*n[0];
	      tdx=(short *)tpt; tdy=tdx+1;
	      if( (sq[DX+i]+sq[DY+j]) < (sq[*tdx]+sq[*tdy]) )
		{
		  *tdx=DX+i; *tdy=DY+j;	    
		}
	    } 
  }
}

void error(int da,int db,int dx,int dy,int errnum)
{
  fprintf(stderr,"4SSED+ error: impossible pixel configuration reached (%d): %d %d %d %d\n",
	  errnum,da,db,dx,dy);
  exit(-1);
}
