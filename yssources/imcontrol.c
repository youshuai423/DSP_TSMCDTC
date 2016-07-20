/******************************************************************************
| includes                          
|----------------------------------------------------------------------------*/
#include "imcontrol.h"

/******************************************************************************
| local variable definitions                          
|----------------------------------------------------------------------------*/

/******************************************************************************
| global variable definitions                          
|----------------------------------------------------------------------------*/
Uint16 switchtable[6][6] =
{
	3, 4, 5, 6, 1, 2,
	0, 7, 0, 7, 0, 7,
	5, 6, 1, 2, 3, 4,
	2, 3, 4, 5, 6, 1,
	7, 0, 7, 0, 7, 0,
	6, 1, 2, 3, 4, 5
};

Uint16 vector[8][3] =
{
	0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,
    0, 1, 1,
    0, 0, 1,
    1, 0, 1,
    1, 1, 1
};
PHASE_ABC uabc = {0, 0, 0};
PHASE_ALBE ualbe = {0, 0};
PHASE_ABC iabc = {0, 0, 0};
PHASE_ALBE ialbe = {0, 0};
PHASE_DQ idq = {0, 0};
PHASE_ALBE ualbe_cmd = {0, 0};
PHASE_DQ udq_cmd = {0, 0};
PHASE_ALBE lamdaralbe = {0, 0};
PHASE_ALBE lamdasalbe = {0, 0};
double Te = 0;
double Tecmd = 0;
double Ud = 0;

double theta = 0;
double lamdar = 0;
double n = 0;
double wr = 0;
double iqset = 0;

double ud_Isum = 0;
double uq_Isum = 0;
double iqset_Isum = 0;
int period_count = 0;

double anglek = 0;
double ualsum = 0;
double ubesum = 0;
double ialsum = 0;
double ibesum = 0;

/******************************************************************************
@brief   Coordinate Transform
******************************************************************************/
void S3toR2(PHASE_ABC *abc, PHASE_DQ *dq, double theta)
{
  //dq->d = sqrt(2.0/3.0) * (cos(theta) * abc->a + cos(theta - 2.0/3.0*pi) * abc->b + cos(theta + 2.0/3.0*pi) * abc->c);
  //dq->q = -sqrt(2.0/3.0) * (sin(theta) * abc->a + sin(theta - 2.0/3.0*pi) * abc->b + sin(theta + 2.0/3.0*pi) * abc->c);
  dq->d = sqrt(2.0) * (cos(theta - 1.0/6.0*pi) * abc->a + sin(theta) * abc->b);
  dq->q = -sqrt(2.0) * (sin(theta - 1.0/6.0*pi) * abc->a - cos(theta) * abc->b);
}

void S3toS2(PHASE_ABC *abc, PHASE_ALBE *albe)
{
  //albe->al = sqrt(2.0/3.0) * (abc->a - 0.5 * abc->b - 0.5 * abc->c);
  //albe->be = sqrt(2.0/3.0) * (sqrt(3)/2.0 * abc->b - sqrt(3)/2.0 * abc->c);
  albe->al = sqrt(3.0/2.0) * abc->a;
  albe->be = 1.0/sqrt(2) * abc->a + sqrt(2) * abc->b;
}

void S2toR2(PHASE_ALBE *albe, PHASE_DQ *dq, double theta)
{
  dq->d = cos(theta) * albe->al + sin(theta) * albe->be;
  dq->q = -sin(theta) * albe->al + cos(theta) * albe->be;
}

void R2toS3(PHASE_DQ *dq, PHASE_ABC *abc, double theta)
{
  abc->a = sqrt(2.0/3.0) * (cos(theta) * dq->d - sin(theta) *dq->q);
  abc->b = sqrt(2.0/3.0) * (cos(theta - 2.0/3.0*pi) * dq->d - sin(theta - 2.0/3.0*pi) *dq->q);
  abc->c = sqrt(2.0/3.0) * (cos(theta + 2.0/3.0*pi) * dq->d - sin(theta + 2.0/3.0*pi) *dq->q);
}

void S2toS3(PHASE_ALBE *albe, PHASE_ABC *abc)
{
  abc->a = sqrt(2.0/3.0) * albe->al;
  abc->b = sqrt(2.0/3.0) * (-0.5 * albe->al + sqrt(3)/2.0 * albe->be);
  abc->c = sqrt(2.0/3.0) * (-0.5 * albe->al - sqrt(3)/2.0 * albe->be);
}

void R2toS2(PHASE_DQ *dq, PHASE_ALBE *albe, double theta)
{
  albe->al = cos(theta) * dq->d - sin(theta) * dq->q;
  albe->be = sin(theta) * dq->d + cos(theta) * dq->q;
}

/******************************************************************************
@brief   Rotor Flux Calculation
******************************************************************************/
double lamdarCal(double lamdar, double ism)
{
  return (1.0 - Ts/Tr) * lamdar + Lm*Ts/Tr * ism;
}

void lamdaralbeCal(PHASE_ALBE ualbe, PHASE_ALBE ialbe, double *ualsum, double *ubesum, double *ialsum, double *ibesum, PHASE_ALBE *lamdaralbe)
{
  double tempal, tempbe;
  tempal = Integrator(ualbe.al, *ualsum, Ts) - Rs * Integrator(ialbe.al, *ialsum, Ts) - (Ls*Lr/Lm - Lm) * ialbe.al;
  tempbe = Integrator(ualbe.be, *ubesum, Ts) - Rs * Integrator(ialbe.be, *ibesum, Ts) - (Ls*Lr/Lm - Lm) * ialbe.be;
  lamdaralbe->al = tempal * Lr/Lm;
  lamdaralbe->be = tempbe * Lr/Lm;
}

/******************************************************************************
@brief   Stator Flux Calculation
******************************************************************************/
void lamdasalbeCal(PHASE_ALBE ualbe, PHASE_ALBE ialbe, PHASE_ALBE *lamdasalbe)
{
	lamdasalbe->al = Integrator(ualbe.al - Rs * ialbe.al, lamdasalbe->al, Ts);
	lamdasalbe->be = Integrator(ualbe.be - Rs * ialbe.be, lamdasalbe->be, Ts);
}

/******************************************************************************
@brief   Torque Calculation
******************************************************************************/
void torqueCal(PHASE_ALBE lamdasalbe, PHASE_ALBE ialbe, double *Te)
{
	*Te = (lamdasalbe.al * ialbe.al - lamdasalbe.be * ialbe.be) * 2 / 3 * np;
}

/******************************************************************************
@brief   Sector Calculation
******************************************************************************/
Uint16 sectorCal(PHASE_ALBE albe)
{
	Uint16 k = albe.al / albe.be;

	if (albe.al > 0 && (k > sqrt(3) || k <= -sqrt(3)))
		return 1;
	else if (albe.al > 0 && albe.be > 0 && k > 0 && k <= sqrt(3))
		return 2;
	else if (albe.al <= 0 && albe.be > 0 && k > -sqrt(3) && k <= 0)
		return 3;
	else if (albe.al < 0 && (k > sqrt(3) || k <= -sqrt(3)))
		return 4;
	else if (albe.al < 0 && albe.be < 0 && k > 0 && k <= sqrt(3))
		return 5;
	else if (albe.al >= 0 && albe.be < 0 && k > -sqrt(3) && k <= 0)
		return 6;
	else
		return 1;
}

/******************************************************************************
@brief   Calculate Position and Speed 
******************************************************************************/
double wrCal_M()
{
  //unsigned int temp;
  //return 60.0 * (cntFTM1 - temp) / (Z * 0.001);
	return 0;
}

double wrCal_T()
{
}

double wrCal_MT()
{
}

double wrCal_lamdar(PHASE_ALBE *lamdaralbe, double *anglek, PHASE_ALBE ualbe, PHASE_ALBE ialbe, double ts) // Î´²âÊÔ
{
  double angle = 0;
  double we =0, wsl = 0;
  
  lamdaralbeCal(ualbe, ialbe, &ualsum, &ubesum, &ialsum, &ibesum, lamdaralbe);
  if (lamdaralbe->al != 0)
  {    
    angle = atan(lamdaralbe->be / lamdaralbe->al);
    if (fabs(angle - *anglek) < 0.5 * pi)
      we = (angle - *anglek) / ts;
    else if (angle <= 0)
      we = (angle - *anglek + pi) / ts;
    else
      we = (angle - *anglek - pi);
    
    *anglek = angle;
    
    wsl = Lm/Tr * (lamdaralbe->be - lamdaralbe->al) / (pow(lamdaralbe->al, 2) + pow(lamdaralbe->be, 2));
  }
  else
  {
    we = 0;
    wsl = 0;
  }
  
  if (fabs(we - wsl) < 90)
    return we - wsl;
  else
    return 0;
}

double positonCal(double wr, double lamdar, double ist, double theta)
{
  double we = 0;  
  
  if (lamdar > 0.01)
    we = Lm/Tr * ist / lamdar + wr;
  else
    we = 0;
  
  return Integrator(we, theta, Ts);
} 

/******************************************************************************
@brief   PI Module 
******************************************************************************/
void PImodule(double Kp, double Ki, double err, double *lasterr, double Uplim, double Downlim, double *inputk)
{
	*inputk += Kp * (err - *lasterr) + Ki * Ts * err;
	*lasterr = err;

	if (*inputk > Uplim)
		*inputk = Uplim;
	else if (*inputk < Downlim)
		*inputk = Downlim;
}

double Integrator(double paramin, double sum, double ts)
{
  return paramin * ts + sum;
}

/******************************************************************************
@brief   Hysteresis
******************************************************************************/
int hysteresis(double paramin, double width)
{
	if (paramin > width)
		return 1;
	else if (paramin < -width)
		return -1;
	else
		return 0;
}

/******************************************************************************
@brief   SVM 
******************************************************************************/ 
void positionSVM(unsigned int *Tinv)
{
  double Angle = 0;
  double theta = 0;
  int sector = 0;
  double Dm = 0, Dn = 0, D0 = 0;  // Dutycycle
  
  Angle = fmod((10 * pi * (period_count / 1000.0)), (2 * pi));
  theta = fmod(Angle,1/3.0 * pi);
  sector = (int)floor( Angle / (1/3.0 * pi)) + 1;
/*  Dm = M * sin(1/3.0 * pi - theta) / 2.0;
  Dn = M * sin(theta) / 2.0;
  D0 = (0.5 - Dm - Dn) / 2.0;*/
  Dm = M * sin(1/3.0 * pi - theta);
  Dn = M * sin(theta);
  D0 = (0.5 - Dm - Dn);
  Dm = roundn(Dm, 8);
  Dn = roundn(Dn, 8);
  D0 = roundn(D0, 8);
  if (D0 < 0) D0 = 0;
  
  switch (sector)
  {
  case 1:
    Tinv[0] = (int)floor(period * (D0));
    Tinv[1] = (int)floor(period * (D0 + Dm));
    Tinv[2] = (int)floor(period * (D0 + Dm + Dn));
    break;
  case 2:
    Tinv[0] = (int)floor(period * (D0 + Dn));
    Tinv[1] = (int)floor(period * (D0));
    Tinv[2] = (int)floor(period * (D0 + Dm + Dn));
    break;
  case 3:
    Tinv[0] = (int)floor(period * (D0 + Dm + Dn));
    Tinv[1] = (int)floor(period * (D0));
    Tinv[2] = (int)floor(period * (D0 + Dm));
    break;
  case 4:
    Tinv[0] = (int)floor(period * (D0 + Dm + Dn));
    Tinv[1] = (int)floor(period * (D0 + Dn));
    Tinv[2] = (int)floor(period * (D0));
    break;
  case 5:
    Tinv[0] = (int)floor(period * (D0 + Dm));
    Tinv[1] = (int)floor(period * (D0 + Dm + Dn));
    Tinv[2] = (int)floor(period * (D0));
    break;  
  case 6:
    Tinv[0] = (int)floor(period * (D0));
    Tinv[1] = (int)floor(period * (D0 + Dm + Dn));
    Tinv[2] = (int)floor(period * (D0 + Dn));
  }   
}

void ualbeSVM(double Ual, double Ube, double Ud, Uint16 invprd1, Uint16 invprd2, Uint16 *Tinv1, Uint16 *Tinv2)
{
  double dm, dn, d0;
  int sector;
  double k = Ube / Ual;
  
  if (Ual > 0 && Ube >= 0 && k >= 0 && k < sqrt(3))
  {
    sector = 1;
    dm = (Ual - Ube/sqrt(3)) / Ud;
    dn = 2/sqrt(3) * Ube / Ud;
  }
  else if (Ube > 0 && (k >= sqrt(3) || k < -sqrt(3)))
  {
    sector = 2;
    dm = (Ual + Ube/sqrt(3)) / Ud;
    dn = (-Ual + Ube/sqrt(3)) / Ud;
  }
  else if (Ual < 0 && Ube > 0 && k >= -sqrt(3) && k < 0)
  {
    sector = 3;
    dm = 2/sqrt(3) * Ube / Ud;
    dn = (-Ual - Ube/sqrt(3)) / Ud;
  }
  else if (Ual < 0 && Ube <= 0 && k >= 0 && k < sqrt(3))
  { 
    sector = 4;
    dm = (-Ual + Ube/sqrt(3)) / Ud;
    dn = -2/sqrt(3) * Ube / Ud;
  }
  else if (Ube < 0 && (k >= sqrt(3) || k < -sqrt(3)))
  {
    sector = 5;
    dm = (-Ual - Ube/sqrt(3)) / Ud;
    dn = (Ual - Ube/sqrt(3)) / Ud;
  }
  else if (Ual > 0 && Ube < 0 && k >= -sqrt(3) && k < 0)
  {
    sector = 6;
    dm = -2/sqrt(3) * Ube / Ud;
    dn = (Ual + Ube/sqrt(3)) / Ud;
  }
  else
  {
    sector = 1;
    dm = 0;
    dn = 0;
  }
  
/*  if (dm + dn >= 1)
  {
    double temp = dm / (dm + dn);
    dn = dn / (dm + dn);
    dm = temp;
    d0 = 0;
  }
  else
    d0 = 0.5 * (1 - dm - dn);*/

/*  dm = 0.4; // test
  dn = 0.4;
  sector = 1;*/

  d0 = (1 - dm - dn) * 0.5;
  
  switch (sector)
  {
  case 1:
    {      
      if (d0 < 0 || (invprd1 * d0 < zerolimit))
      {
    	  double dmt = dm / (dm + dn);
    	  Tinv1[0] = zerolimit;
    	  Tinv1[1] = zerolimit + (int)((invprd1 - 2*zerolimit) * dmt);
    	  Tinv1[2] = invprd1 - zerolimit;
      }
      else
      {
          Tinv1[0] = (int)(invprd1 * (d0));
          Tinv1[1] = (int)(invprd1 * (dm + d0));
          Tinv1[2] = (int)(invprd1 * (dm + dn + d0));
      }

      if (d0 < 0 || (invprd2 * d0 < zerolimit))
      {
    	  double dnt = dn / (dm + dn);
    	  Tinv2[0] = invprd2 - zerolimit;
    	  Tinv2[1] = zerolimit + (int)((invprd2 - 2*zerolimit) * dnt);
    	  Tinv2[0] = zerolimit;
      }
      else
      {
          Tinv2[0] = (int)(invprd2 * (dm + dn + d0));
          Tinv2[1] = (int)(invprd2 * (dn + d0));
          Tinv2[2] = (int)(invprd2 * (d0));
      }

      break;
    }
  case 2:
    {
      if (d0 < 0 || (invprd1 * d0 < zerolimit))
      {
    	  double dnt = dn / (dm + dn);
    	  Tinv1[0] = zerolimit + (int)((invprd1 - 2*zerolimit) * dnt);
      	  Tinv1[1] = zerolimit;
      	  Tinv1[2] = invprd1 - zerolimit;
      }
      else
      {
          Tinv1[0] = (int)(invprd1 * (dn + d0));
          Tinv1[1] = (int)(invprd1 * (d0));
          Tinv1[2] = (int)(invprd1 * (dm + dn + d0));
      }

      if (d0 < 0 || (invprd2 * d0 < zerolimit))
      {
    	  double dmt = dm / (dm + dn);
    	  Tinv2[0] = zerolimit + (int)((invprd2 - 2*zerolimit) * dmt);
    	  Tinv2[1] = invprd2 - zerolimit;
    	  Tinv2[2] = zerolimit;
      }
      else
      {
          Tinv2[0] = (int)(invprd2 * (dm + d0));
          Tinv2[1] = (int)(invprd2 * (dm + dn + d0));
          Tinv2[2] = (int)(invprd2 * (d0));
      }

      break;
    }
  case 3:
    {
      if (d0 < 0 || (invprd1 * d0 < zerolimit))
      {
    	  double dmt = dm / (dm + dn);
      	  Tinv1[0] = invprd1 - zerolimit;
      	  Tinv1[1] = zerolimit;
      	  Tinv1[2] = zerolimit + (int)((invprd1 - 2*zerolimit) * dmt);
      }
      else
      {
          Tinv1[0] = (int)(invprd1 * (dm + dn + d0));
          Tinv1[1] = (int)(invprd1 * (d0));
          Tinv1[2] = (int)(invprd1 * (dm + d0));
      }

      if (d0 < 0 || (invprd2 * d0 < zerolimit))
      {
      	  double dnt = dn / (dm + dn);
      	  Tinv2[0] = zerolimit;
      	  Tinv2[1] = invprd2 - zerolimit;
      	  Tinv2[2] = zerolimit + (int)((invprd2 - 2*zerolimit) * dnt);
      }
      else
      {
          Tinv2[0] = (int)(invprd2 * (d0));
          Tinv2[1] = (int)(invprd2 * (dm + dn + d0));
          Tinv2[2] = (int)(invprd2 * (dn + d0));
      }

      break;
    }
  case 4:
    {
      if (d0 < 0 || (invprd1 * d0 < zerolimit))
      {
      	  double dnt = dn / (dm + dn);
      	  Tinv1[0] = invprd1 - zerolimit;
      	  Tinv1[1] = zerolimit + (int)((invprd1 - 2*zerolimit) * dnt);
      	  Tinv1[2] = zerolimit;
      }
      else
      {
          Tinv1[0] = (int)(invprd1 * (dm + dn + d0));
          Tinv1[1] = (int)(invprd1 * (dn + d0));
          Tinv1[2] = (int)(invprd1 * (d0));
      }

      if (d0 < 0 || (invprd2 * d0 < zerolimit))
      {
    	  double dmt = dm / (dm + dn);
      	  Tinv2[0] = zerolimit;
      	  Tinv2[1] = zerolimit + (int)((invprd2 - 2*zerolimit) * dmt);
      	  Tinv2[2] = invprd2 - zerolimit;
      }
      else
      {
          Tinv2[0] = (int)(invprd2 * (d0));
          Tinv2[1] = (int)(invprd2 * (dm + d0));
          Tinv2[2] = (int)(invprd2 * (dm + dn + d0));
      }

      break;
    }
  case 5:
    {
      if (d0 < 0 || (invprd1 * d0 < zerolimit))
      {
    	  double dmt = dm / (dm + dn);
    	  Tinv1[0] = zerolimit + (int)((invprd1 - 2*zerolimit) * dmt);
    	  Tinv1[1] = invprd1 - zerolimit;
    	  Tinv1[2] = zerolimit;
      }
      else
      {
          Tinv1[0] = (int)(invprd1 * (dm + d0));
          Tinv1[1] = (int)(invprd1 * (dm + dn + d0));
          Tinv1[2] = (int)(invprd1 * (d0));
      }

      if (d0 < 0 || (invprd2 * d0 < zerolimit))
      {
    	  double dnt = dn / (dm + dn);
    	  Tinv2[0] = zerolimit + (int)((invprd2 - 2*zerolimit) * dnt);
    	  Tinv2[1] = zerolimit;
    	  Tinv2[2] = invprd2 - zerolimit;
      }
      else
      {
          Tinv2[0] = (int)(invprd2 * (dn + d0));
          Tinv2[1] = (int)(invprd2 * (d0));
          Tinv2[2] = (int)(invprd2 * (dm + dn + d0));
      }

      break;
    }
  case 6:
    {
      if (d0 < 0 || (invprd1 * d0 < zerolimit))
      {
    	  double dnt = dn / (dm + dn);
    	  Tinv1[0] = zerolimit;
    	  Tinv1[1] = invprd1 - zerolimit;
    	  Tinv1[2] = zerolimit + (int)((invprd1 - 2*zerolimit) * dnt);
      }
      else
      {
          Tinv1[0] = (int)(invprd1 * (d0));
          Tinv1[1] = (int)(invprd1 * (dm + dn + d0));
          Tinv1[2] = (int)(invprd1 * (dn + d0));
      }

      if (d0 < 0 || (invprd2 * d0 < zerolimit))
      {
    	  double dmt = dm / (dm + dn);
    	  Tinv2[0] = invprd2 - zerolimit;
    	  Tinv2[1] = zerolimit;
    	  Tinv2[2] = zerolimit + (int)((invprd2 - 2*zerolimit) * dmt);
      }
      else
      {
          Tinv2[0] = (int)(invprd2 * (dm + dn + d0));
          Tinv2[1] = (int)(invprd2 * (d0));
          Tinv2[2] = (int)(invprd2 * (dm + d0));
      }

      break;
    }
  default:
    {
      Tinv1[0] = invprd1 + 1;
      Tinv1[1] = invprd1 + 1;
      Tinv1[2] = invprd1 + 1;
      Tinv2[0] = invprd2 + 1;
      Tinv2[1] = invprd2 + 1;
      Tinv2[2] = invprd2 + 1;
    }
  }
}

void udqSVM()
{
}

/******************************************************************************
@brief   Auxiliary Function
******************************************************************************/
double roundn(double input, int digit)
{
  double temp;
  temp = input * pow(10, digit);
  temp = floor(temp);
  temp = temp / (pow(10, digit) * 1.0);
  return temp;
}
