/*------------------------------------------------------------------------------
* rtklib unit test driver : geoid functions
*-----------------------------------------------------------------------------*/
#include "gtest/gtest.h"
#include "rtklib.h"

/* latitude, longitude, geoid height (m) */
/* reference : http://sps.unavco.org/geoid */
static double poss[][3]={
    { 90.001*D2R,  80.000*D2R,  0.000},
    {-90.001*D2R,  80.000*D2R,  0.000},
    { 30.000*D2R, 360.000*D2R,  0.000},
    {-30.000*D2R,-360.001*D2R,  0.000},
    {-90.000*D2R, 359.999*D2R,-29.534},
    { 90.000*D2R,  80.000*D2R, 13.606},
    {-90.000*D2R, -60.000*D2R,-29.534},
    { 30.000*D2R,-360.000*D2R, 35.387},
    {-30.000*D2R, 359.999*D2R, 21.409},
    { 10.000*D2R,  45.000*D2R,-20.486},
    {-60.123*D2R, 135.123*D2R,-33.152},
    { 19.999*D2R, 135.000*D2R, 41.602},
    { 50.001*D2R, 135.000*D2R, 20.555},
    { 35.000*D2R, 119.999*D2R,  4.386},
    { 35.000*D2R, 150.001*D2R, 14.779},
    { 20.000*D2R, 120.000*D2R, 21.269},
    { 50.000*D2R, 150.000*D2R, 20.277},
    { 35.000*D2R, 135.000*D2R, 36.355},
    { 45.402*D2R, 141.750*D2R, 27.229}, /* wakkanai */
    { 24.454*D2R, 122.942*D2R, 21.652}, /* ishigaki */
    { 33.120*D2R, 139.797*D2R, 43.170}, /* hachijo */
    { 30.000*D2R, 135.000*D2R, 36.017}, /* taiheiyo */
    {0,0,0}
};
#ifdef WIN32
#define DATADIR "..\\..\\data\\geoiddata/"
#else
#define DATADIR "../../../../data/geoiddata/"
#endif
static char *file1=DATADIR "WW15MGH.DAC";
static char *file2=DATADIR "Und_min1x1_egm2008_isw=82_WGS84_TideFree_SE";
static char *file3=DATADIR "Und_min2.5x2.5_egm2008_isw=82_WGS84_TideFree_SE";
static char *file4=DATADIR "gsigeome.ver4";

/* opengeoid(), closegeoid() */
TEST(TEST_GEOID,test_open_close)
{
    int ret;
    
    ret=opengeoid(10,file1);
        EXPECT_TRUE(ret==0); /* no model */
    ret=opengeoid(GEOID_EGM96_M150,"../../../geoiddata/WW15MGH.DAA");
        EXPECT_TRUE(ret==0); /* no file */
    ret=opengeoid(GEOID_EMBEDDED,"");
        EXPECT_TRUE(ret==1);
    closegeoid();
    ret=opengeoid(GEOID_EGM96_M150,file1);
        EXPECT_TRUE(ret==1);
    closegeoid();
    ret=opengeoid(GEOID_EGM2008_M10,file2);
        EXPECT_TRUE(ret==1);
    closegeoid();
    ret=opengeoid(GEOID_EGM2008_M25,file3);
        EXPECT_TRUE(ret==1);
    closegeoid();
    ret=opengeoid(GEOID_GSI2000_M15,file4);
        EXPECT_TRUE(ret==1);
    closegeoid();
}

/* geoidh() (1) */
TEST(TEST_GEOID, test_geoidh)
{
    double h[64][6]={{0}};
    int i,j=0;
    
    opengeoid(GEOID_EGM96_M150,file1); /* reference */
    for (i=0;poss[i][0]!=0.0;i++) {
        h[i][j]=geoidh(poss[i]);
    }
    j++;
    closegeoid();
    
    opengeoid(GEOID_EMBEDDED,"");
    for (i=0;poss[i][0]!=0.0;i++) {
        h[i][j]=geoidh(poss[i]);
    }
    j++;
    closegeoid();
    
    opengeoid(GEOID_EGM2008_M10,file2);
    for (i=0;poss[i][0]!=0.0;i++) {
        h[i][j]=geoidh(poss[i]);
    }
    j++;
    closegeoid();
    
    opengeoid(GEOID_EGM2008_M25,file3);
    for (i=0;poss[i][0]!=0.0;i++) {
        h[i][j]=geoidh(poss[i]);
    }
    j++;
    closegeoid();
    
    opengeoid(GEOID_GSI2000_M15,file4);
    for (i=0;poss[i][0]!=0.0;i++) {
        h[i][j]=geoidh(poss[i]);
    }
    j++;
    closegeoid();
    
    for (i=0;poss[i][0]!=0.0;i++) {
        EXPECT_TRUE(fabs(h[i][0]-poss[i][2])<1.0);
    }
}
/* geoidh() (2) */
TEST(TEST_GEOID, test_geoidh2)
{
    double pos[3],h[6],dhmax[6],dh;
    int i,j,k,nlat=113,nlon=237;
    
    for (i=0;i<=nlat;i++) for (j=0;j<=nlon;j++) {
        pos[0]=(90.0-i*180.0/nlat)*D2R;
        pos[1]=j*360.0/nlon*D2R;
        pos[2]=0.0;
        opengeoid(GEOID_EGM96_M150,file1); /* reference */
        h[0]=geoidh(pos);
        closegeoid();
        opengeoid(GEOID_EMBEDDED,"");
        h[1]=geoidh(pos);
        closegeoid();
        opengeoid(GEOID_EGM2008_M10,file2);
        h[2]=geoidh(pos);
        closegeoid();
        opengeoid(GEOID_EGM2008_M25,file3);
        h[3]=geoidh(pos);
        closegeoid();
        opengeoid(GEOID_GSI2000_M15,file4);
        h[4]=geoidh(pos);
        closegeoid();
        for (k=1;k<5;k++) {
            dh=h[k]!=0.0?h[k]-h[0]:0.0;
            if (fabs(dh)>fabs(dhmax[k])) dhmax[k]=dh;
        }
    }
    printf("max difference                                                                 :");
    for (i=1;i<5;i++) {
        printf(" %9.4f",dhmax[i]);
        EXPECT_TRUE(fabs(dhmax[i])<10.0);
    }
    printf("\n");
    printf("%s utset3 : OK\n",__FILE__);
}
