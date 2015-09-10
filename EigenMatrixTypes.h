//#include <Eigen/Dense>
#include </usr/include/eigen3/Eigen/Dense>

#include "PulseParameters.h"

typedef Eigen::Matrix<double,NSAMPLES,1> SampleVector;
typedef Eigen::Matrix<double,2*NSAMPLES,1> FullSampleVector;
typedef Eigen::Matrix<double,Eigen::Dynamic,1,0,NSAMPLES,1> PulseVector;
typedef Eigen::Matrix<char,Eigen::Dynamic,1,0,NSAMPLES,1> BXVector;
typedef Eigen::Matrix<double,NSAMPLES,NSAMPLES> SampleMatrix;
typedef Eigen::Matrix<double,2*NSAMPLES,2*NSAMPLES> FullSampleMatrix;
typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,0,NSAMPLES,NSAMPLES> PulseMatrix;
typedef Eigen::Matrix<double,NSAMPLES,Eigen::Dynamic,0,NSAMPLES,NSAMPLES> SamplePulseMatrix;
typedef Eigen::LLT<SampleMatrix> SampleDecompLLT;
