
#ifndef CORREL_H
#define CORREL_H

#include <math.h>

static void
copyseries(int frame, char *data, const int *strides, const float *tempX, const float *tempY, const float *tempZ, const char* datacode, int numdata, const int* atomlist, const int* atomcounts, int lowerb, double* aux)
{
	char code;
	int index1 = 0, index2 = 0, index3 = 0, index4 = 0;
	double x1, x2, y1, y2, z1, z2, x3, y3, z3, aux1, aux2;
	int i = 0, j = 0, atomno = 0;
	int dataset = 0;
	int stride0, stride1;
	
	stride0 = strides[0];
	stride1 = strides[1];

	/* If I eventually switch to using frame,property ordering for timeseries data
	stride0 = strides[1];
	stride1 = strides[0];
	*/

	for (i=0;i<numdata;i++) {
		code = datacode[i];
		switch (code) {
			case 'm':
        x1 = y1 = z1 = 0.0;
				aux2 = 0;
        for (j=0;j<atomcounts[i];j++) {
          index1 = atomlist[atomno]-lowerb;
					aux1 = aux[atomno++];
					aux2 += aux1;
          x1 += tempX[index1]*aux1;
          y1 += tempY[index1]*aux1;
          z1 += tempZ[index1]*aux1;
        }
        *(double*)(data + dataset++*stride0 + frame*stride1) = x1/aux2;
        *(double*)(data + dataset++*stride0 + frame*stride1) = y1/aux2;
        *(double*)(data + dataset++*stride0 + frame*stride1) = z1/aux2;
        break;
			case 'x':
				index1 = atomlist[atomno++]-lowerb;
				*(double*)(data+dataset++*stride0+frame*stride1) = tempX[index1];
				break;
			case 'y':
				index1 = atomlist[atomno++]-lowerb;
				*(double*)(data+dataset++*stride0+frame*stride1) = tempY[index1];
				break;
			case 'z':
				index1 = atomlist[atomno++]-lowerb;
				*(double*)(data+dataset++*stride0+frame*stride1) = tempZ[index1];
				break;
			case 'v':
				index1 = atomlist[atomno++]-lowerb;
				*(double*)(data + dataset++*stride0 + frame*stride1) = tempX[index1];
				*(double*)(data + dataset++*stride0 + frame*stride1) = tempY[index1];
				*(double*)(data + dataset++*stride0 + frame*stride1) = tempZ[index1];
				break;
			case 'a':
				index1 = atomlist[atomno++]-lowerb;
				index2 = atomlist[atomno++]-lowerb;
				index3 = atomlist[atomno++]-lowerb;
				x1 = tempX[index1]-tempX[index2]; y1 = tempY[index1]-tempY[index2]; z1 = tempZ[index1]-tempZ[index2];
				x2 = tempX[index3]-tempX[index2]; y2 = tempY[index3]-tempY[index2]; z2 = tempZ[index3]-tempZ[index2];
				aux1 = sqrt(x1*x1+y1*y1+z1*z1); aux2 = sqrt(x2*x2+y2*y2+z2*z2);
				*(double*)(data + dataset++*stride0 + frame*stride1) = acos((x1*x2+y1*y2+z1*z2)/(aux1*aux2));
				break;
			case 'd':
				index1 = atomlist[atomno++]-lowerb;
				index2 = atomlist[atomno++]-lowerb;
				*(double*)(data + dataset++*stride0+frame*stride1) = tempX[index2]-tempX[index1];
				*(double*)(data + dataset++*stride0+frame*stride1) = tempY[index2]-tempY[index1];
				*(double*)(data + dataset++*stride0+frame*stride1) = tempZ[index2]-tempZ[index1];
				break;
			case 'r':
				index1 = atomlist[atomno++]-lowerb;
				index2 = atomlist[atomno++]-lowerb;
				x1 = tempX[index2]-tempX[index1]; y1 = tempY[index2]-tempY[index1]; z1 = tempZ[index2]-tempZ[index1];
				*(double*)(data + dataset++*stride0+frame*stride1) = sqrt(x1*x1+y1*y1+z1*z1);
				break;
			case 'h':
				index1 = atomlist[atomno++]-lowerb;
				index2 = atomlist[atomno++]-lowerb;
				index3 = atomlist[atomno++]-lowerb;
				index4 = atomlist[atomno++]-lowerb;
				x1 = tempX[index2]-tempX[index1]; y1 = tempY[index2]-tempY[index1]; z1 = tempZ[index2]-tempZ[index1];
				x2 = tempX[index3]-tempX[index2]; y2 = tempY[index3]-tempY[index2]; z2 = tempZ[index3]-tempZ[index2];
				x3 = tempX[index3]-tempX[index4]; y3 = tempY[index3]-tempY[index4]; z3 = tempZ[index3]-tempZ[index4];
				double nx1, ny1, nz1, nx2, ny2, nz2;
				// v1 x v2
				nx1 = (y1*z2) - (y2*z1); ny1 = (z1*x2)-(z2*x1); nz1 = (x1*y2)-(x2*y1);
				// v3 x v2
				nx2 = (y3*z2) - (y2*z3); ny2 = (z3*x2)-(z2*x3); nz2 = (x3*y2)-(x2*y3);
				double a, b;
				a = sqrt(nx1*nx1+ny1*ny1+nz1*nz1); b = sqrt(nx2*nx2+ny2*ny2+nz2*nz2);
				// normalized the cross products
				nx1 /= a; ny1 /= a; nz1 /= a; nx2 /= b; ny2 /= b; nz2 /= b;
				// find the angle
				aux1 = acos(nx1*nx2+ny1*ny2+nz1*nz2);
				// and the sign of the angle
				aux2 = (nx2*x1+ny2*y1+nz2*z1);
				if ((aux2 < 0 && aux1 > 0) || (aux2 > 0 && aux1 < 0)) {
					aux1 *= -1;
				}
				// Check if the dihedral has wrapped around 2 pi
				aux2 = *(double*)(data + dataset*stride0 + (frame-1)*stride1);
				if (fabs(aux1-aux2) > M_PI) {
					if (aux1 > 0) { aux1 -= 2*M_PI; }
					else { aux1 += 2*M_PI; }
				}
				*(double*)(data + dataset++*stride0 + frame*stride1) = aux1;
				break;
		}
	}
}

// This accounts for periodic boundary conditions
// taken from MMTK
#define distance_vector_2(d, r1, r2, data) \
  { \
    double xh = 0.5*(data)[0]; \
    double yh = 0.5*(data)[1]; \
    double zh = 0.5*(data)[2]; \
    d[0] = r2[0]-r1[0]; \
    if (d[0] > xh) d[0] -= (data)[0]; \
    if (d[0] <= -xh) d[0] += (data)[0]; \
    d[1] = r2[1]-r1[1]; \
    if (d[1] > yh) d[1] -= (data)[1]; \
    if (d[1] <= -yh) d[1] += (data)[1]; \
    d[2] = r2[2]-r1[2]; \
    if (d[2] > zh) d[2] -= (data)[2]; \
    if (d[2] <= -zh) d[2] += (data)[2]; \
  }                                                                                                                    

#endif
