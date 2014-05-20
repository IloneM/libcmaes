/**
 * CMA-ES, Covariance Matrix Adaptation Evolution Strategy
 * Copyright (c) 2014 Inria
 * Author: Emmanuel Benazera <emmanuel.benazera@lri.fr>
 *
 * This file is part of libcmaes.
 *
 * libcmaes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libcmaes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcmaes.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cmaes.h"
#include <cmath>
#include <fstream>
#include <iostream>

using namespace libcmaes;

double background(const double *x, const double *par)
{
  return par[0] + par[1]*x[0] + par[2]*x[0]*x[0];
}

double lorentzianpeak(const double *x, const double *par)
{
  return (0.5*par[0]*par[1]/M_PI) /
    std::max( 1.e-10,(x[0]-par[2])*(x[0]-par[2]) + .25*par[1]*par[1]);
}

double blfunc(const double *x, const double *par)
{
  return background(x,par) + lorentzianpeak(x,&par[3]);
}

//double par[6] = {1,1,1,6,.03,1};
double points[201];
double values[201];

// chi2
FitFunc ff = [](const double *x, const int N)
{
  double sum = 0.0;
  for (int i=0;i<201;i++)
    {
      //std::cout << "x=" << points[i] << " / f=" << values[i] << std::endl;
      if (values[i] != 0.0)
	sum += pow((values[i]-blfunc(&points[i],x))/sqrt(values[i]),2);
    }
  //std::cout << "sum=" << sum << std::endl;
  return sum;
};

void loaddata(const std::string filename)
{
  std::ifstream fin(filename);
  std::string line;
  std::getline(fin,line); // skip header line.
  int a, b, i = 0;
  double c;
  while(fin >> a >> b >> c)
    {
      points[i] = c;
      values[i] = b;
      //std::cout << "x=" << points[i] << " / f=" << values[i] << std::endl;
      i++;
    }
}

int main(int argc, char *argv[])
{
  int dim = 6;
  loaddata("lorentzpeakbench.dat");
  CMAParameters<> cmaparams(dim);
  CMASolutions cmasols = cmaes<>(ff,cmaparams);
  std::cout << "best solution: " << cmasols << std::endl;
  std::cout << "optimization took " << cmasols._elapsed_time / 1000.0 << " seconds\n";
  return cmasols._run_status;
}
