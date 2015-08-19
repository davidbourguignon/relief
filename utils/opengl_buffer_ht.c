/*
** Adapted from:
** BingFeng Zhou and XiFeng Fang, Improving Mid-tone Quality of
** Variable-Coefficient Error Diffusion, Proceedings of ACM SIGGRAPH, 2003.
*/
#include "opengl_buffer.h"

static int lpnDiffMatrix[25];
static int nSum;
static int lpRandomScale[128];
static int lpnCoefficient[512];

/*
 * Local functions declarations
 */
INLINED int  offset           (int x, int y, int w, int h);
INLINED int  offset1          (int x, int y, int w, int h);
static  int  CalcNewThreshold (int nDensity);
static  void InitPara         (void);
static  int  ChangeCoefficient(int nLevel);
static  int  ImageDiffusion   (int *lpImg, int nImgWidth, int nImgHeight);

/*
 * Local functions definitions
 */
INLINED int
offset(int x, int y, int w, int h) {
  return ((x < w && y < h) ? (y*w + x) : 0);
}

INLINED int
offset1(int x, int y, int w, int h) {
  return ((x < w && y < h) ? (y*w + (w - x - 1)) : 0);
}

static int
CalcNewThreshold(int nDensity) {
  int nThresholdScale;
  
  if( nDensity > 127 )
    nDensity = 255 - nDensity;
  
  nThresholdScale = lpRandomScale[nDensity];
  
  if(nThresholdScale == 0)
    return 127;
  
  return 128+(rand()%128)*nThresholdScale/100;
}

static void
InitPara(void) {
  int i;
  
  // interpolated coefficients
  int pnCoefficient[512] =
    {
      0,      13,       0 ,      5, 		//0.722222, 0.000000 ,0.277778 
      1, 1300249,       0 , 499250, 		//0.722562, 0.000000 ,0.277438 
      2,  214114,     287 ,  99357, 		//0.682418, 0.000915 ,0.316668 
      3,  351854,       0 , 199965, 		//0.637626, 0.000000 ,0.362374 
      4,  801100,       0 , 490999, 		//0.619999, 0.000000 ,0.380001 
      5,  606569,   37983 , 355446, 		//0.606570, 0.037983 ,0.355447 
      6,  593140,   75967 , 330891, 		//0.593141, 0.075967 ,0.330892 
      7,  579711,  113951 , 306337, 		//0.579712, 0.113951 ,0.306337 
      8,  283141,   75967 , 140891, 		//0.566283, 0.151934 ,0.281783 
      9,  552853,  189918 , 257228, 		//0.552854, 0.189918 ,0.257228 
      10,  704075,  297466 , 303694, 		//0.539424, 0.227902 ,0.232674 
      11,   76188,   33644 ,  33025, 		//0.533317, 0.235508 ,0.231175 
      12,  527209,  243114 , 229676, 		//0.527210, 0.243114 ,0.229676 
      13,  521101,  250719 , 228178, 		//0.521102, 0.250720 ,0.228178 
      14,  514994,  258325 , 226679, 		//0.514995, 0.258326 ,0.226679 
      15,  508886,  265931 , 225181, 		//0.508887, 0.265932 ,0.225181 
      16,  502779,  273537 , 223682, 		//0.502780, 0.273538 ,0.223682 
      17,  496671,  281143 , 222184, 		//0.496672, 0.281144 ,0.222184 
      18,  490564,  288749 , 220686, 		//0.490564, 0.288749 ,0.220686 
      19,  484456,  296355 , 219187, 		//0.484457, 0.296356 ,0.219187 
      20,  478349,  303961 , 217689, 		//0.478349, 0.303961 ,0.217689 
      21,  472242,  311567 , 216190, 		//0.472242, 0.311567 ,0.216190 
      22,   46613,   31917 ,  21469, 		//0.466135, 0.319173 ,0.214692 
      23,  467003,  317873 , 215123, 		//0.467003, 0.317873 ,0.215123 
      24,  467872,  316573 , 215554, 		//0.467872, 0.316573 ,0.215554 
      25,   22321,   15013 ,  10285, 		//0.468741, 0.315273 ,0.215985 
      26,  469610,  313973 , 216416, 		//0.469610, 0.313973 ,0.216416 
      27,  470479,  312673 , 216847, 		//0.470479, 0.312673 ,0.216847 
      28,   52372,   34597 ,  24142, 		//0.471348, 0.311373 ,0.217278 
      29,  472217,  310073 , 217709, 		//0.472217, 0.310073 ,0.217709 
      30,  473086,  308773 , 218140, 		//0.473086, 0.308773 ,0.218140 
      31,  157985,  102491 ,  72857, 		//0.473955, 0.307473 ,0.218571 
      32,   47482,   30617 ,  21900, 		//0.474825, 0.306173 ,0.219002 
      33,  472921,  298013 , 229065, 		//0.472921, 0.298013 ,0.229065 
      34,  471018,  289853 , 239128, 		//0.471018, 0.289853 ,0.239128 
      35,  469115,  281693 , 249191, 		//0.469115, 0.281693 ,0.249191 
      36,  467211,  273533 , 259254, 		//0.467212, 0.273534 ,0.259255 
      37,  465308,  265374 , 269317, 		//0.465308, 0.265374 ,0.269317 
      38,  463405,  257214 , 279380, 		//0.463405, 0.257214 ,0.279380 
      39,  153834,   83018 ,  96481, 		//0.461502, 0.249054 ,0.289443 
      40,   45959,   24089 ,  29950, 		//0.459599, 0.240895 ,0.299506 
      41,  452279,  286018 , 261701, 		//0.452280, 0.286019 ,0.261702 
      42,  444960,  331142 , 223897, 		//0.444960, 0.331142 ,0.223897 
      43,  437641,  376266 , 186092, 		//0.437641, 0.376266 ,0.186092 
      44,   43024,   42131 ,  14826, 		//0.430322, 0.421390 ,0.148288 
      45,  427011,  421930 , 151058, 		//0.427011, 0.421930 ,0.151058 
      46,  211850,  211235 ,  76914, 		//0.423701, 0.422471 ,0.153828 
      47,  210195,  211505 ,  78299, 		//0.420391, 0.423011 ,0.156598 
      48,  208540,  211775 ,  79684, 		//0.417081, 0.423551 ,0.159368 
      49,  413769,  424091 , 162139, 		//0.413769, 0.424091 ,0.162139 
      50,  410459,  424631 , 164909, 		//0.410459, 0.424631 ,0.164909 
      51,  407148,  425171 , 167679, 		//0.407149, 0.425172 ,0.167679 
      52,  403838,  425711 , 170449, 		//0.403839, 0.425712 ,0.170449 
      53,  400528,  426251 , 173219, 		//0.400529, 0.426252 ,0.173219 
      54,  397217,  426792 , 175990, 		//0.397217, 0.426792 ,0.175990 
      55,  393907,  427332 , 178760, 		//0.393907, 0.427332 ,0.178760 
      56,  195298,  213936 ,  90765, 		//0.390597, 0.427873 ,0.181530 
      57,  193643,  214206 ,  92150, 		//0.387287, 0.428413 ,0.184300 
      58,  383976,  428953 , 187070, 		//0.383976, 0.428953 ,0.187070 
      59,  380665,  429493 , 189841, 		//0.380665, 0.429493 ,0.189841 
      60,  377355,  430033 , 192611, 		//0.377355, 0.430033 ,0.192611 
      61,  374044,  430573 , 195381, 		//0.374045, 0.430574 ,0.195381 
      62,  370734,  431113 , 198151, 		//0.370735, 0.431114 ,0.198151 
      63,  367424,  431654 , 200921, 		//0.367424, 0.431654 ,0.200921 
      64,   36411,   43219 ,  20369, 		//0.364114, 0.432194 ,0.203692 
      65,  366696,  445475 , 187828, 		//0.366696, 0.445475 ,0.187828 
      66,  369279,  458755 , 171964, 		//0.369280, 0.458756 ,0.171964 
      67,  185931,  236018 ,  78050, 		//0.371863, 0.472037 ,0.156100 
      68,  374445,  485317 , 140236, 		//0.374446, 0.485318 ,0.140236 
      69,  188514,  249299 ,  62186, 		//0.377029, 0.498599 ,0.124372 
      70,  379611,  511879 , 108509, 		//0.379611, 0.511880 ,0.108509 
      71,  382194,  525159 ,  92645, 		//0.382195, 0.525160 ,0.092645 
      72,   38477,   53843 ,   7678, 		//0.384778, 0.538441 ,0.076782 
      73,  388829,  533848 ,  77321, 		//0.388830, 0.533849 ,0.077321 
      74,  392881,  529256 ,  77861, 		//0.392882, 0.529257 ,0.077861 
      75,  396933,  524664 ,  78401, 		//0.396934, 0.524665 ,0.078401 
      76,  400986,  520072 ,  78941, 		//0.400986, 0.520073 ,0.078941 
      77,   40503,   51547 ,   7948, 		//0.405038, 0.515480 ,0.079482 
      78,  399240,  493681 , 107078, 		//0.399240, 0.493681 ,0.107078 
      79,  393441,  471883 , 134674, 		//0.393442, 0.471884 ,0.134674 
      80,   38764,   45008 ,  16227, 		//0.387644, 0.450085 ,0.162272 
      81,  381845,  428284 , 189869, 		//0.381846, 0.428285 ,0.189869 
      82,  376047,  406484 , 217468, 		//0.376047, 0.406484 ,0.217468 
      83,  370249,  384683 , 245066, 		//0.370250, 0.384684 ,0.245066 
      84,  364451,  362883 , 272664, 		//0.364452, 0.362884 ,0.272665 
      85,   35865,   34108 ,  30026, 		//0.358654, 0.341083 ,0.300263 
      86,  356905,  343874 , 299219, 		//0.356906, 0.343875 ,0.299220 
      87,  355157,  346665 , 298176, 		//0.355158, 0.346666 ,0.298177 
      88,  353409,  349456 , 297133, 		//0.353410, 0.349457 ,0.297134 
      89,  351661,  352247 , 296090, 		//0.351662, 0.352248 ,0.296091 
      90,  349913,  355038 , 295047, 		//0.349914, 0.355039 ,0.295048 
      91,  348165,  357829 , 294004, 		//0.348166, 0.357830 ,0.294005 
      92,  346417,  360620 , 292961, 		//0.346418, 0.360621 ,0.292962 
      93,  344669,  363411 , 291918, 		//0.344670, 0.363412 ,0.291919 
      94,  342921,  366202 , 290875, 		//0.342922, 0.366203 ,0.290876 
      95,   34117,   36899 ,  28983, 		//0.341173, 0.368994 ,0.289833 
      96,  342623,  367794 , 289581, 		//0.342624, 0.367795 ,0.289582 
      97,  172037,  183297 , 144665, 		//0.344075, 0.366595 ,0.289331 
      98,  345524,  365395 , 289079, 		//0.345525, 0.365396 ,0.289080 
      99,  346975,  364195 , 288828, 		//0.346976, 0.364196 ,0.288829 
      100,  348425,  362996 , 288577, 		//0.348426, 0.362997 ,0.288578 
      101,  174938,  180898 , 144163, 		//0.349877, 0.361797 ,0.288327 
      102,   35132,   36059 ,  28807, 		//0.351327, 0.360597 ,0.288076 
      103,  346970,  363719 , 289309, 		//0.346971, 0.363720 ,0.289310 
      104,  342614,  366841 , 290543, 		//0.342615, 0.366842 ,0.290544 
      105,  338258,  369963 , 291777, 		//0.338259, 0.369964 ,0.291778 
      106,  333902,  373085 , 293011, 		//0.333903, 0.373086 ,0.293012 
      107,   16477,   18810 ,  14712, 		//0.329547, 0.376208 ,0.294246 
      108,  330357,  376874 , 292767, 		//0.330358, 0.376875 ,0.292768 
      109,  331169,  377542 , 291288, 		//0.331169, 0.377542 ,0.291288 
      110,  331980,  378209 , 289810, 		//0.331980, 0.378209 ,0.289810 
      111,  332791,  378876 , 288331, 		//0.332792, 0.378877 ,0.288332 
      112,   33360,   37954 ,  28685, 		//0.333603, 0.379544 ,0.286853 
      113,  334876,  378285 , 286838, 		//0.334876, 0.378285 ,0.286838 
      114,  168074,  188513 , 143412, 		//0.336149, 0.377027 ,0.286825 
      115,  337421,  375767 , 286810, 		//0.337422, 0.375768 ,0.286811 
      116,  338694,  374509 , 286796, 		//0.338694, 0.374509 ,0.286796 
      117,  169983,  186625 , 143391, 		//0.339967, 0.373251 ,0.286783 
      118,  341239,  371991 , 286768, 		//0.341240, 0.371992 ,0.286769 
      119,  342512,  370733 , 286754, 		//0.342512, 0.370733 ,0.286754 
      120,  171892,  184737 , 143370, 		//0.343785, 0.369475 ,0.286741 
      121,  345057,  368215 , 286726, 		//0.345058, 0.368216 ,0.286727 
      122,  346330,  366957 , 286712, 		//0.346330, 0.366957 ,0.286712 
      123,  173801,  182849 , 143349, 		//0.347603, 0.365699 ,0.286699 
      124,  348875,  364439 , 286684, 		//0.348876, 0.364440 ,0.286685 
      125,  175074,  181590 , 143335, 		//0.350149, 0.363181 ,0.286671 
      126,  175710,  180961 , 143328, 		//0.351421, 0.361923 ,0.286657 
      127,   35269,   36066 ,  28664	 	//0.352694, 0.360664 ,0.286643 
    };
  
  //<<modulation strength-interpolated
  lpRandomScale[0]= 0;
  lpRandomScale[1]= 0;
  lpRandomScale[2]= 1;
  lpRandomScale[3]= 2;
  lpRandomScale[4]= 3;
  lpRandomScale[5]= 3;
  lpRandomScale[6]= 4;
  lpRandomScale[7]= 5;
  lpRandomScale[8]= 6;
  lpRandomScale[9]= 6;
  lpRandomScale[10]= 7;
  lpRandomScale[11]= 8;
  lpRandomScale[12]= 9;
  lpRandomScale[13]= 9;
  lpRandomScale[14]= 10;
  lpRandomScale[15]= 11;
  lpRandomScale[16]= 12;
  lpRandomScale[17]= 12;
  lpRandomScale[18]= 13;
  lpRandomScale[19]= 14;
  lpRandomScale[20]= 15;
  lpRandomScale[21]= 15;
  lpRandomScale[22]= 16;
  lpRandomScale[23]= 17;
  lpRandomScale[24]= 18;
  lpRandomScale[25]= 18;
  lpRandomScale[26]= 19;
  lpRandomScale[27]= 20;
  lpRandomScale[28]= 21;
  lpRandomScale[29]= 21;
  lpRandomScale[30]= 22;
  lpRandomScale[31]= 23;
  lpRandomScale[32]= 24;
  lpRandomScale[33]= 24;
  lpRandomScale[34]= 25;
  lpRandomScale[35]= 26;
  lpRandomScale[36]= 27;
  lpRandomScale[37]= 27;
  lpRandomScale[38]= 28;
  lpRandomScale[39]= 29;
  lpRandomScale[40]= 30;
  lpRandomScale[41]= 31;
  lpRandomScale[42]= 32;
  lpRandomScale[43]= 33;
  lpRandomScale[44]= 34;
  lpRandomScale[45]= 34;
  lpRandomScale[46]= 35;
  lpRandomScale[47]= 36;
  lpRandomScale[48]= 37;
  lpRandomScale[49]= 38;
  lpRandomScale[50]= 38;
  lpRandomScale[51]= 39;
  lpRandomScale[52]= 40;
  lpRandomScale[53]= 41;
  lpRandomScale[54]= 42;
  lpRandomScale[55]= 42;
  lpRandomScale[56]= 43;
  lpRandomScale[57]= 44;
  lpRandomScale[58]= 45;
  lpRandomScale[59]= 46;
  lpRandomScale[60]= 46;
  lpRandomScale[61]= 47;
  lpRandomScale[62]= 48;
  lpRandomScale[63]= 49;
  lpRandomScale[64]= 50;
  lpRandomScale[65]= 53;
  lpRandomScale[66]= 56;
  lpRandomScale[67]= 59;
  lpRandomScale[68]= 62;
  lpRandomScale[69]= 65;
  lpRandomScale[70]= 68;
  lpRandomScale[71]= 71;
  lpRandomScale[72]= 75;
  lpRandomScale[73]= 78;
  lpRandomScale[74]= 81;
  lpRandomScale[75]= 84;
  lpRandomScale[76]= 87;
  lpRandomScale[77]= 90;
  lpRandomScale[78]= 93;
  lpRandomScale[79]= 96;
  lpRandomScale[80]= 100;
  lpRandomScale[81]= 100;
  lpRandomScale[82]= 100;
  lpRandomScale[83]= 100;
  lpRandomScale[84]= 100;
  lpRandomScale[85]= 100;
  lpRandomScale[86]= 91;
  lpRandomScale[87]= 83;
  lpRandomScale[88]= 75;
  lpRandomScale[89]= 66;
  lpRandomScale[90]= 58;
  lpRandomScale[91]= 50;
  lpRandomScale[92]= 41;
  lpRandomScale[93]= 33;
  lpRandomScale[94]= 25;
  lpRandomScale[95]= 17;
  lpRandomScale[96]= 21;
  lpRandomScale[97]= 26;
  lpRandomScale[98]= 31;
  lpRandomScale[99]= 35;
  lpRandomScale[100]= 40;
  lpRandomScale[101]= 45;
  lpRandomScale[102]= 50;
  lpRandomScale[103]= 54;
  lpRandomScale[104]= 58;
  lpRandomScale[105]= 62;
  lpRandomScale[106]= 66;
  lpRandomScale[107]= 70;
  lpRandomScale[108]= 71;
  lpRandomScale[109]= 73;
  lpRandomScale[110]= 75;
  lpRandomScale[111]= 77;
  lpRandomScale[112]= 79;
  lpRandomScale[113]= 80;
  lpRandomScale[114]= 81;
  lpRandomScale[115]= 83;
  lpRandomScale[116]= 84;
  lpRandomScale[117]= 86;
  lpRandomScale[118]= 87;
  lpRandomScale[119]= 88;
  lpRandomScale[120]= 90;
  lpRandomScale[121]= 91;
  lpRandomScale[122]= 93;
  lpRandomScale[123]= 94;
  lpRandomScale[124]= 95;
  lpRandomScale[125]= 97;
  lpRandomScale[126]= 98;
  lpRandomScale[127]= 100;
  //>>modulation strength-interpolated
  
  for (i = 0; i < 25; i++)
    lpnDiffMatrix[i] = 0;
  
  for (i = 0; i < 512; i+=4)
    {
      lpnCoefficient[i]   = pnCoefficient[i+1] + pnCoefficient[i+2] + pnCoefficient[i+3];
      lpnCoefficient[i+1] = pnCoefficient[i+1];
      lpnCoefficient[i+2] = pnCoefficient[i+2];
      lpnCoefficient[i+3] = pnCoefficient[i+3];
    }
}

static int
ChangeCoefficient(int nLevel) {
  int nLevel1;
  int i;
  int j;
  
  if (nLevel < 0)
    nLevel = 0;
  else
    if (nLevel > 255)
      nLevel = 255;
  
  if (nLevel > 127)
    nLevel1 = 255 - nLevel;
  else
    nLevel1 = nLevel;
  
  i = nLevel1*4;
  
  for (j = 0; j < 25; j++ )
    lpnDiffMatrix[j] = 0;
  
  lpnDiffMatrix[13] = lpnCoefficient[i+1];//right
  lpnDiffMatrix[17] = lpnCoefficient[i+3];//down 
  lpnDiffMatrix[16] = lpnCoefficient[i+2];//left-down 
  nSum	            = lpnCoefficient[i];
  
  return nSum; 
}

static int
ImageDiffusion(int *lpImg,int nImgWidth,int nImgHeight) {
  int *matrix = lpnDiffMatrix;
  int nImgIndex;
  int nSum = nSum;
  int iError, iPixel, iError1;
  int i, j;
  short int *lpImg0 = (short int *) malloc(nImgWidth*nImgHeight*sizeof(short int));
  
  srand(0);
  for ( j = 0; j < nImgHeight; j++ )
    for ( i = 0; i < nImgWidth; i++ )
      {
	nImgIndex			= offset(i,j,nImgWidth,nImgHeight);
	lpImg0[nImgIndex]	= lpImg[nImgIndex];

      }

  for ( j = 0; j < nImgHeight; j++ )
    for ( i = 0; i < nImgWidth; i++ )
      if ( (j%2) == 0 )
	{
	  nImgIndex	= offset(i,j,nImgWidth,nImgHeight);
	  iPixel		= lpImg[nImgIndex];
	  nSum		= ChangeCoefficient(lpImg0[nImgIndex]);

	  if ( iPixel > CalcNewThreshold(lpImg0[nImgIndex]) )
	    {
	      lpImg[nImgIndex]	= 255;
	      iError				= iPixel - 255;
	      //				iDensity++;
	    }
	  else
	    {
	      lpImg[nImgIndex]	= 0;
	      iError				= iPixel - 0;
	    }

		
	  iPixel	= lpImg[nImgIndex = offset(i+1, j, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(3,2,5,5)] / nSum;
	  iError1	= lpImg[nImgIndex] - iPixel;
			
	  iPixel	= lpImg[nImgIndex = offset(i+2, j, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(4,2,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);

	  iPixel	= lpImg[nImgIndex = offset(i-2, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(0,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset(i-1, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(1,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset(i, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(2,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset(i+1, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(3,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset(i+2, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(4,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);

	  iPixel	= lpImg[nImgIndex = offset(i-2, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(0,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
	
	  iPixel	= lpImg[nImgIndex = offset(i-1, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(1,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset(i, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(2,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset(i+1, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(3,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset(i+2, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(4,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);

	  //
	  iError	= iError - iError1;
	  iPixel	= lpImg[nImgIndex = offset(i, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError;
		
	}
      else
	{//(j%2) = 0
	  nImgIndex	= offset1(i,j,nImgWidth,nImgHeight);
	  iPixel		= lpImg[nImgIndex];
	  nSum		= ChangeCoefficient(lpImg0[nImgIndex]);

	  if ( iPixel > CalcNewThreshold(lpImg0[nImgIndex]) )
	    {
	      lpImg[nImgIndex]	= 255;
	      iError				= iPixel - 255;
	    }
	  else
	    {
	      lpImg[nImgIndex]	= 0;
	      iError				= iPixel - 0;
	    }
		
	  iPixel	= lpImg[nImgIndex = offset1(i+1, j, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(3,2,5,5)] / nSum;
	  iError1	= lpImg[nImgIndex] - iPixel;
				
	  iPixel	= lpImg[nImgIndex = offset1(i+2, j, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(4,2,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
	
	  iPixel	= lpImg[nImgIndex = offset1(i-2, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(0,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset1(i-1, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(1,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset1(i, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(2,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset1(i+1, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(3,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset1(i+2, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(4,3,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);

	  iPixel	= lpImg[nImgIndex = offset1(i-2, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(0,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
	
	  iPixel	= lpImg[nImgIndex = offset1(i-1, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(1,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset1(i, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(2,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset1(i+1, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(3,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);
			
	  iPixel	= lpImg[nImgIndex = offset1(i+2, j+2, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError * matrix[offset(4,4,5,5)] / nSum;
	  iError1	+= (lpImg[nImgIndex] - iPixel);

	  iError	= iError - iError1;
	  iPixel	= lpImg[nImgIndex = offset1(i, j+1, nImgWidth, nImgHeight)];
	  lpImg[nImgIndex]	= iPixel + iError;
	};//(j%2) = 0

  free(lpImg0);
  return 1;
}

/*
 * GLframebuf definitions
 */
GLframebuf *
gl_framebuf_ht(GLframebuf *buf) {
  GLubyte *buf_pixels = NULL;
  GLuint *pixels = NULL;
  int i = 0;
  
  assert(buf != NULL && buf->components == 1 && buf->type == GL_UNSIGNED_BYTE);
  buf_pixels = buf->pixels;
  pixels = (GLuint *) malloc(buf->width*buf->height*sizeof(GLuint));
  for (i = 0; i < buf->width*buf->height; i++) {
    pixels[i] = (GLuint) buf_pixels[i];
  }
  
  { // IMPORTED CODE BEGIN
    static GLboolean is_first_time = GL_TRUE;
    
    if (is_first_time) {
      is_first_time = GL_FALSE;
      InitPara();
    }
    ImageDiffusion((GLint *) pixels, buf->width, buf->height);
  } // IMPORTED CODE END
  
  for (i = 0; i < buf->width*buf->height; i++) {
    buf_pixels[i] = (GLubyte) pixels[i];
  }
  free(pixels);
  return buf;
}
