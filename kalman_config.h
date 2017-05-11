#define N 4
#define T 0.5
#define T2 T*T
#define T3 T*T*T
#define T4 T*T*T*T

typedef struct {

	/* vector */
	double u;
	double x[N];
	double xp[N];
	double z[N / 2];

	/* matrix */
	double P[4 * N]; //4x4
	double Pp[4 * N]; //4x4
	double K[2 * N]; //4x2

	double A[4 * N]; //4x4
	double B[N]; //4x1
	double H[2 * N]; //2x4
	double HT[2 * N]; //4x2

	double Q[4 * N]; //4x4
	double R[N]; //2x2

	/* temporary storage */
	double tmpV1[N];
	double tmpV2[N / 2];
	double tmpV3[N / 2];
	double tmpR1[2 * N];
	double tmpR2[2 * N];
	double tmp1[4 * N];
	double tmp2[4 * N];
	double tmp3[4 * N];
	double tmp4[N];
	double tmp5[N];

} kf_t;