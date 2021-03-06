#include "environmentclass.h"

#include <QTextStream>
#include <QString>
#include <QFile>
#include <QApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "markenvironment.h"

markenvironment::markenvironment()
{
    ReadSettings();

    for (int i=0; i<objectcount; i++)
    {
        objxpos[i]=RandFloat()*MainWin->ui->spinSize->value();
        objypos[i]=RandFloat()*MainWin->ui->spinSize->value();
        objxvel[i]=RandFloat()*maxvel*2-maxvel;
        objyvel[i]=RandFloat()*maxvel*2-maxvel;
        objsize[i]=RandFloat()*(maxsize-minsize)+minsize;
        objsizevel[i]=RandFloat()*maxsizevel*2-maxsizevel;
        for (int o=0; o<3; o++)
            objcolours[i][o]=(float)((int)Rand8()-127);

        for (int o=0; o<3; o++)
            objcolvel[i][o]=RandFloat()*maxcolvel*2-maxcolvel;

        objtightness[i]=RandFloat()*(maxtight-mintight)+mintight;
        objtightvel[i]=RandFloat()*maxtightvel*2-maxtightvel;
    }

    iter_to_accel=iter_reset;

}

void markenvironment::ReadSettings()
{
    objectcount=MainWin->ui->ME_Objects->value();
    maxsize=(float)(MainWin->ui->ME_MaxSize->value());
    minsize=(float)(MainWin->ui->ME_MinSize->value());
    maxvel=MainWin->ui->ME_MaxVel->value();
    maxsizevel=MainWin->ui->ME_MaxSizeVel->value();
    maxcolvel=MainWin->ui->ME_MaxColVel->value();
    maxtightvel=MainWin->ui->ME_MaxTightVel->value();
    maxtight=MainWin->ui->ME_MaxTight->value();
    mintight=MainWin->ui->ME_MinTight->value();
    speedfactor=MainWin->ui->ME_SpeedFactor->value();
    tighttweak=MainWin->ui->ME_MaxTightAccel->value();
    coltweak=MainWin->ui->ME_MaxColAccel->value();
    veltweak=MainWin->ui->ME_MaxAccel->value();
    sizetweak=MainWin->ui->ME_MaxSizeAccel->value();
    iter_reset=MainWin->ui->ME_Iterations->value();

}

void markenvironment::regenerate()
{

    ReadSettings();

    //reset to black
    for (int n=0; n<MainWin->ui->spinSize->value(); n++)
    for (int m=0; m<MainWin->ui->spinSize->value(); m++)
    {

        environment[n][m][2]=127;
        environment[n][m][1]=127;
        environment[n][m][0]=127;
    }

    //iterate and apply lights
    for (int i=0; i<objectcount; i++)
    {
        //add velocities
        objxpos[i]+=(objxvel[i]*speedfactor); objypos[i]+=(objyvel[i]*speedfactor);
        //handle bouncing
        if (objxpos[i]<0){objxpos[i]=0-objxpos[i]; objxvel[i]=0-objxvel[i];}
        if (objypos[i]<0){objypos[i]=0-objypos[i]; objyvel[i]=0-objyvel[i];}
        if (objxpos[i]>(MainWin->ui->spinSize->value()-1)){objxpos[i]=(2*(MainWin->ui->spinSize->value()-1))-objxpos[i]; objxvel[i]=0-objxvel[i];}
        if (objypos[i]>(MainWin->ui->spinSize->value()-1)){objypos[i]=(2*(MainWin->ui->spinSize->value()-1))-objypos[i]; objyvel[i]=0-objyvel[i];}

        //add size change, apply limits
        objsize[i]+=(objsizevel[i]*speedfactor);
        if (objsize[i]>maxsize) {objsize[i]=maxsize; objsizevel[i]=0;}
        if (objsize[i]<minsize) {objsize[i]=minsize; objsizevel[i]=0;}

        //do colour
        for (int o=0; o<3; o++)
        {
            objcolours[i][o]+=(objcolvel[i][o]*speedfactor);
            if (objcolours[i][o]>128) {objcolours[i][o]=128; objcolvel[i][o]=0;}
            if (objcolours[i][o]<-127) {objcolours[i][o]=-127; objcolvel[i][o]=0;}
        }

        //do tightness
        objtightness[i]+=(objtightvel[i]*speedfactor);
        if (objtightness[i]>maxtight) {objtightness[i]=maxtight; objtightvel[i]=0;}
        if (objtightness[i]<mintight) {objtightness[i]=mintight; objtightvel[i]=0;}

        //tweak velocities
        iter_to_accel--;
        if (iter_to_accel<=0)
        {
            objxvel[i]+=(RandFloat()*veltweak*2-veltweak)*speedfactor / (float)iter_reset;
            objyvel[i]+=(RandFloat()*veltweak*2-veltweak)*speedfactor / (float)iter_reset;
            objsizevel[i]+=(RandFloat()*sizetweak*2-sizetweak)*speedfactor / (float)iter_reset;
            objtightvel[i]+=(RandFloat()*tighttweak*2 - tighttweak)*speedfactor / (float)iter_reset;
            for (int o=0; o<3; o++) objcolvel[i][o]+=(RandFloat()*2*coltweak-coltweak)*speedfactor / (float)iter_reset;
            iter_to_accel=iter_reset;
        }


        //limit velocities
        if (objxvel[i]>maxvel) objxvel[i]=maxvel;
        if (objxvel[i]<(0-maxvel)) objxvel[i]=(0-maxvel);
        if (objyvel[i]>maxvel) objyvel[i]=maxvel;
        if (objyvel[i]<(0-maxvel)) objyvel[i]=(0-maxvel);
        if (objsizevel[i]>maxsizevel) objsizevel[i]=maxsizevel;
        if (objsizevel[i]<(0-maxsizevel)) objsizevel[i]=(0-maxsizevel);
        for (int o=0; o<3; o++) if (objcolvel[i][o]<(0-maxcolvel)) objcolvel[i][o]=(0-maxcolvel);
        for (int o=0; o<3; o++) if (objcolvel[i][o]>maxcolvel) objcolvel[i][o]=maxcolvel;


        for (int n=0; n<MainWin->ui->spinSize->value(); n++)
        for (int m=0; m<MainWin->ui->spinSize->value(); m++)
        {
            //work out distance
            float dist=sqrt((n-objxpos[i])*(n-objxpos[i])+(m-objypos[i])*(m-objypos[i]));
            if (dist<objsize[i])
            {
                for (int o=0; o<3; o++)
                    environment[n][m][o]=add_and_limit(environment[n][m][o],(int)(objcolours[i][o]),dist, objsize[i], objtightness[i]);
            }
        }
    }


   saveMe=false;
}

quint8 markenvironment::add_and_limit(int oldvalue, int color, float dist, float maxdist, float tightness)
{
    int retval;

    float angledist= PI * .5 * dist / maxdist; //works out distance as radians - so can use cosine dropoff
    retval=oldvalue+(int)(pow(cos(angledist),tightness)*(float)color);

    if (retval<0) retval=0;
    if (retval>255) return 255; else return (quint8)retval;
}

float markenvironment::RandFloat()
{
    return simulation_randoms->gen_float();
}
