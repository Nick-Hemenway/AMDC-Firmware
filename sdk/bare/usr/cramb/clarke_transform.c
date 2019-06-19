#include "clarke_transform.h"
#include "../../sys/defines.h"

void forward_clarke(double *abc, double *alpha_beta){

	alpha_beta[0] = abc[0] - 0.5*abc[1] - 0.5*abc[2];
	alpha_beta[1] = (SQRT3/2)*abc[1] - (SQRT3/2)*abc[2];

}

void inverse_clarke(double *alpha_beta, double *abc){

	double a = 1.0/3.0;
	double b = SQRT3/3.0;

	abc[0] = 2.0*a*alpha_beta[0];
	abc[1] = -a*alpha_beta[0] + b*alpha_beta[1];
	abc[2] = -a*alpha_beta[0] - b*alpha_beta[1];

}
