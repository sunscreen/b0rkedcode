#ifndef Parametric_h
#define Parametric_h
#pragma once

//#include "math.h"
#include <cmath>

//typedef enum { LS, HS, PK, NA } ParametricType;



class Parametric
{
public:
typedef enum { LS, HS, PK, NA } ParametricType;

    Parametric(void)
    {
        reset();

        q = 0.0f;
        frequency = 0.0f;
        //this->omega = 2.0f * M_PI * frequency / 192000.0f;
        //this->tsin  = sinf(omega);
        //this->tcos  = cosf(omega);

        this->filterType = NA;
    }

    ~Parametric(void) { }

    void setFilterType(ParametricType type)
    {
        this->filterType = type;
    }

    void reset()
    {
        gain = 0.0f;

        xL1 = xL2 = 0.0f;
        yL1 = yL2 = 0.0f;

        xR1 = xR2 = 0.0f;
        yR1 = yR2 = 0.0f;

        b0 = b1 = b2 = 0.0f;
        a0 = a1 = a2 = 0.0f;
    }

    float FilterLeft(float xL)
    {
        float yL0 = (b0*xL + b1*xL1 + b2*xL2 - a1*yL1 - a2*yL2);

        xL2 = xL1;
        xL1 = xL;

        yL2 = yL1;
        yL1 = yL0;

        return yL0;
    }

    float FilterRight(float xR)
    {
        float yR0 = (b0*xR + b1*xR1 + b2*xR2 - a1*yR1 - a2*yR2);

        xR2 = xR1;
        xR1 = xR;

        yR2 = yR1;
        yR1 = yR0;

        return yR0;
    }

    void CalcCoeffs(float gain, float frequency)
    {
        CalcCoeffs(gain, frequency, 0.0);
    }

    void SetOmega(float frequency_div) {
        omega=pi_2 * frequency_div;
    }

    void CalcCoeffs(float gain, float frequency, float q)
    {
        this->q = q;
        this->gain = gain;
        this->frequency = frequency;

        A     = powf(10.0f, (gain / 40.0f));
        //omega = pi_2 * frequency / 192000.0f;
        //omega = pi_2 * frequency;

        tsin  = sinf(omega);
        tcos  = cosf(omega);
        beta  = sqrtf(A+A);
        alpha = 0.0f;

        switch(this->filterType)
        {
            case LS:

                a0 = (A+1.0)+(A-1.0)*tcos+beta*tsin;
                a1 = (-2.0*((A-1.0)+(A+1.0)*tcos)) / a0;
                a2 = ((A+1.0)+(A-1.0)*tcos-beta*tsin) / a0;

                b0 = (A*((A+1.0)-(A-1.0)*tcos+beta*tsin)) / a0;
                b1 = (2.0*A*((A-1.0)-(A+1.0)*tcos)) / a0;
                b2 = (A*((A+1.0)-(A-1.0)*tcos-beta*tsin)) / a0;

                break;

            case HS:

                a0 = (A+1.0)-(A-1.0)*tcos+beta*tsin;
                a1 = (2.0*((A-1.0)-(A+1.0)*tcos)) / a0;
                a2 = ((A+1.0)-(A-1.0)*tcos-beta*tsin) / a0;

                b0 = (A*((A+1.0)+(A-1.0)*tcos+beta*tsin)) / a0;
                b1 = (-2.0*A*((A-1.0)+(A+1.0)*tcos)) / a0;
                b2 = (A*((A+1.0)+(A-1.0)*tcos-beta*tsin)) / a0;

                break;

            case PK:

                alpha = tsin / (2.0f * q);

                a0 = (1.0f + alpha/A);
                a1 = (-2.0f * tcos) / a0;
                a2 = (1.0f - alpha/A) / a0;

                b0 = (1.0f + alpha*A) / a0;
                b1 = (-2.0f * tcos) / a0;
                b2 = (1.0f - alpha*A) / a0;

                break;

            case NA:
                reset();
                break;

            default:
                reset();
                break;
        }
    }

    float q=0.0;
    float gain=0.0;
    float frequency=0.0;
    float omega=0.0;
    float tsin=0.0;
    float tcos=0.0;
    float beta=0.0;
    float alpha=0.0;
    float A=0.0;
    float pi_2 = 2.0f * M_PI;
    ParametricType filterType;

private:
    float b0, b1, b2=0.0;
    float a0, a1, a2=0.0;

    float xL1, xL2=0.0;
    float yL1, yL2=0.0;

    float xR1, xR2=0.0;
    float yR1, yR2=0.0;
};

class MultibandPEQ {
public:

float m_gains[15];
float m_freqs[15];

Parametric PEQ[15]={};

MultibandPEQ (void) {
for (int x=0;x<15;x++) {
PEQ[x].reset();
PEQ[x].setFilterType(Parametric::PK);
//PEQ[x].q=0.5;
m_gains[x]=0.0;
m_freqs[x]=0.0;
}
qDebug() <<"Init mb eq";
}
~MultibandPEQ (void) {}

void update_freqs(QVector <double> &new_freqs) {

for (int x=0;x<15;x++) {
m_freqs[x]=new_freqs[x]/192000.0;

PEQ[x].SetOmega(m_freqs[x]);
PEQ[x].CalcCoeffs((float)0.0,(float)m_freqs[x],0.2);
qDebug() << m_freqs[x];

}

}

void update_gains(QVector <double>&new_gains) {

for (int x=0;x<15;x++) {

if (new_gains[x] < m_gains[x] || new_gains[x] > m_gains[x]) {
PEQ[x].CalcCoeffs((float)new_gains[x],(float)m_freqs[x],0.2);
}
m_gains[x]=new_gains[x];

//m_gains[x]=0.0;
//qDebug() << m_gains[x];
}

}


float process_bands(float sample) {
float eqsamp=sample;
for (int x=0;x<15;x++) {
eqsamp = PEQ[x].FilterRight(sample);
sample=eqsamp;
}
return eqsamp;


}
private:

};

#endif /* Parametric_h */

