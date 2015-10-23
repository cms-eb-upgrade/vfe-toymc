#ifndef PulseChiSqSNNLS_h
#define PulseChiSqSNNLS_h

// #include "EigenMatrixTypes.h"

#include </usr/include/eigen3/Eigen/Dense>
#include <set>
#include <array>

typedef Eigen::Matrix<double,Eigen::Dynamic,1> SampleVector;
typedef Eigen::Matrix<double,Eigen::Dynamic,1> FullSampleVector;
// typedef Eigen::Matrix<double,Eigen::Dynamic,1> PulseVector;
// typedef Eigen::Matrix<char,  Eigen::Dynamic,1> BXVector;
typedef Eigen::Matrix<double,Eigen::Dynamic,1,0,110,1> PulseVector;
typedef Eigen::Matrix<char,  Eigen::Dynamic,1,0,110,1> BXVector;
typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> SampleMatrix;
typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> FullSampleMatrix;
// typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> PulseMatrix;
// typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> SamplePulseMatrix;
typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,0, 110, 110> PulseMatrix;
typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,0, 110, 110> SamplePulseMatrix;
typedef Eigen::LLT<SampleMatrix> SampleDecompLLT;

typedef BXVector::Index Index;


class PulseChiSqSNNLS {
  public:
       
   PulseChiSqSNNLS();
   ~PulseChiSqSNNLS();
   
   
   bool DoFit(const SampleVector &samples, const SampleMatrix &samplecor, double pederr, const BXVector &bxs, const FullSampleVector &fullpulse, const FullSampleMatrix &fullpulsecov);
   
//    const SamplePulseMatrix &pulsemat() const { return _pulsemat; }
//    const SampleMatrix &invcov() const { return _invcov; }
//    
//    const PulseVector &X() const { return _ampvecmin; }
//    const PulseVector &Errors() const { return _errvec; }
//    const BXVector &BXs() const { return _bxsmin; }
   
   const SamplePulseMatrix* pulsemat() const { return _pulsemat; }
   const SampleMatrix* invcov() const { return _invcov; }
   
   const PulseVector* X() const { return _ampvecmin; }
   const PulseVector* Errors() const { return _errvec; }
   const BXVector* BXs() const { return _bxsmin; }
   
   
   double ChiSq() const { return _chisq; }
   void disableErrorCalculation() { _computeErrors = false; }
   
   void setNSAMPLES( int NSAMPLES ) { _NSAMPLES = NSAMPLES; }
   void setNFREQ   ( float NFREQ )  { _NFREQ = NFREQ; }
   
   
   void Init();
   
   
protected:
 
   bool Minimize(const SampleMatrix &samplecor, double pederr, const FullSampleMatrix &fullpulsecov);
   bool NNLS();
   bool updateCov(const SampleMatrix &samplecor, double pederr, const FullSampleMatrix &fullpulsecov);
   double ComputeChiSq();
   double ComputeApproxUncertainty(unsigned int ipulse);
   
   
   SampleVector* _sampvec;
   SampleMatrix* _invcov;
   SamplePulseMatrix* _pulsemat;
   PulseVector* _ampvec;
   PulseVector* _errvec;
   PulseVector* _ampvecmin;
   
   SampleDecompLLT* _covdecomp;
   SampleMatrix* _covdecomp_support;
   
   BXVector* _bxs;
   BXVector* _bxsmin;
   unsigned int _nP;
 
   int _NSAMPLES;
   float _NFREQ;
    
    
//     typedef Eigen::Matrix<double,NSAMPLES,1> SampleVector;
//     typedef Eigen::Matrix<double,2*NSAMPLES,1> FullSampleVector;
//     --->         _Scalar, _Rows, _Cols, _Options, _MaxRows,  _MaxCols
//     typedef Eigen::Matrix<double,Eigen::Dynamic,1,0,NSAMPLES,1> PulseVector;
//     typedef Eigen::Matrix<char,  Eigen::Dynamic,1,0,NSAMPLES,1> BXVector;
//     typedef Eigen::Matrix<double,NSAMPLES,NSAMPLES> SampleMatrix;
//     typedef Eigen::Matrix<double,2*NSAMPLES,2*NSAMPLES> FullSampleMatrix;
//     typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,0,NSAMPLES,NSAMPLES> PulseMatrix;
//     typedef Eigen::Matrix<double,NSAMPLES,Eigen::Dynamic,0,NSAMPLES,NSAMPLES> SamplePulseMatrix;
//     typedef Eigen::LLT<SampleMatrix> SampleDecompLLT;
    
    
      
    
    
   double _chisq;
   bool _computeErrors;

   
   bool _debug;
 
};

#endif
