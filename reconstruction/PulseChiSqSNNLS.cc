//#define PulseChiSqSNNLS_cxx
#include "PulseChiSqSNNLS.h"
#include <math.h>
#include <iostream>

PulseChiSqSNNLS::PulseChiSqSNNLS() :
  _chisq(0.),
  _computeErrors(true)
{
  
 _debug = false;
//  _debug = true;
 
  Eigen::initParallel();
      
}  

PulseChiSqSNNLS::~PulseChiSqSNNLS() {
  
}

void PulseChiSqSNNLS::Init() {
 
//  _sampvec = new Eigen::Matrix<double,_NSAMPLES,1>;
//  _invcov = new Eigen::Matrix<double,_NSAMPLES,_NSAMPLES>;
//  _pulsemat = new Eigen::Matrix<double,_NSAMPLES,Eigen::Dynamic,0,_NSAMPLES,_NSAMPLES>;
//  _ampvec = new Eigen::Matrix<double,Eigen::Dynamic,1,0,_NSAMPLES,1>;
//  _errvec = new Eigen::Matrix<double,Eigen::Dynamic,1,0,_NSAMPLES,1>;
//  _ampvecmin = new Eigen::Matrix<double,Eigen::Dynamic,1,0,_NSAMPLES,1>;
//  
//  _covdecomp = new Eigen::LLT<Eigen::Matrix<double,_NSAMPLES,_NSAMPLES> >;
//  
//  _bxs = new Eigen::Matrix<char,  Eigen::Dynamic,1,0,_NSAMPLES,1>;
//  _bxsmin = new Eigen::Matrix<char,  Eigen::Dynamic,1,0,_NSAMPLES,1>;

 
 
 _sampvec = new SampleVector; _sampvec->resize(_NSAMPLES,1);
 _invcov = new SampleMatrix; _invcov->resize(_NSAMPLES,_NSAMPLES);
 _pulsemat = new SamplePulseMatrix; _pulsemat->resize(_NSAMPLES,Eigen::NoChange);
 _ampvec = new PulseVector; _ampvec->resize(Eigen::NoChange,1);
 _errvec = new PulseVector; _errvec->resize(Eigen::NoChange,1);
 _ampvecmin = new PulseVector; _ampvecmin->resize(Eigen::NoChange,1);
 
//  _covdecomp = new Eigen::LLT<Eigen::Matrix<double,_NSAMPLES,_NSAMPLES> >; 
 _covdecomp = new SampleDecompLLT; //----> no resize!?!?!?  Eigen::LLT<Eigen::Matrix<double,_NSAMPLES,_NSAMPLES> >;
 _covdecomp_support = new SampleMatrix;  _covdecomp_support->resize(_NSAMPLES,_NSAMPLES);
 
 _bxs = new BXVector; _bxs->resize(Eigen::NoChange,1);
 _bxsmin = new BXVector; _bxsmin->resize(Eigen::NoChange,1);

 
}

bool PulseChiSqSNNLS::DoFit(const SampleVector &samples, const SampleMatrix &samplecor, double pederr, const BXVector &bxs, const FullSampleVector &fullpulse, const FullSampleMatrix &fullpulsecov) {
 
//  const unsigned int nsample = SampleVector::RowsAtCompileTime;
  const unsigned int nsample = _NSAMPLES;
  const unsigned int npulse = bxs.rows();

//   std::cout << " PulseChiSqSNNLS::DoFit::npulse  = " << npulse << std::endl;
//   std::cout << " PulseChiSqSNNLS::DoFit::nsample = " << nsample << std::endl;
  
  (*_sampvec) = samples;
  (*_bxs) = bxs;
  
//   std::cout << " PulseChiSqSNNLS::DoFit::here " << std::endl;
  
  
  _pulsemat->resize(nsample,npulse);  _pulsemat->setZero();
//   std::cout << " PulseChiSqSNNLS::DoFit::here 2" << std::endl;
  _ampvec->resize(npulse,1);          _ampvec->setZero();
//   std::cout << " PulseChiSqSNNLS::DoFit::here 3" << std::endl;
  _pulsemat->resize(nsample,npulse);  _pulsemat->setZero();
//   std::cout << " PulseChiSqSNNLS::DoFit::here 4 " << std::endl;
  
//   (*_pulsemat) = SamplePulseMatrix::Zero(nsample,npulse);
//   (*_ampvec) = PulseVector::Zero(npulse);
  _errvec->resize(npulse,1);          _errvec->setZero();
//   (*_errvec) = PulseVector::Zero(npulse);  
  
  _nP = 0;
  _chisq = 0.;

//   std::cout << " ---- " << std::endl;
  
  //initialize pulse template matrix
  for (unsigned int ipulse=0; ipulse<npulse; ++ipulse) {
    int bx = _bxs->coeff(ipulse);
//     std::cout << " PulseChiSqSNNLS::DoFit::bx = " << bx << std::endl;
    
//     int firstsamplet = std::max(0,bx + 3 * int(25 /_NFREQ));
//         int offset = 3 * int(25 /_NFREQ) - bx;
      
    //--- YES
//     int offset = _NSAMPLES - 5 * int(25. /_NFREQ) - bx;    
//     int firstsamplet = std::max(0,bx + 3 * int(25. /_NFREQ));
    
    int offset = 4 * int(25. /_NFREQ) - bx;    
    int firstsamplet = std::max(0,bx + 3 * int(25. /_NFREQ));
    
//     std::cout << " offset = " << offset << " firstsamplet = " << firstsamplet << " bx = " << bx << std::endl;
    
//     int offset = _NSAMPLES - (7-2) * int(25 /_NFREQ) - bx;    
//     int firstsamplet = std::max(0,bx + 3 * int(25 /_NFREQ));
    
//     int offset = - bx;
//     //     int offset = 3 * int(25 /_NFREQ);
//     //     int offset = (_NSAMPLES/2 - int(1*25 /_NFREQ)) - bx;
    
    
//     int firstsamplet = std::max(0,bx + 3);
//     int offset = 7-3-bx;
//     int offset = (_NSAMPLES/2 + 2) - 3 - bx;
//     int offset =  _NSAMPLES/2 + int(2 * 25 /_NFREQ) - 3 - bx;
//     int offset = (_NSAMPLES-3)-3-bx;
    
//     std::cout << " ipulse = " << ipulse << " nsample = " << nsample << " nsamplepulse = " <<  nsample-firstsamplet << " firstsamplet+offset= " << firstsamplet+offset << " = " << firstsamplet << " + " << offset << " (= " << 3 * int(25 /_NFREQ) << " - " << bx << " )" << std::endl; 
    
    const unsigned int nsamplepulse = nsample-firstsamplet;
    _pulsemat->col(ipulse).segment(firstsamplet,nsamplepulse) = fullpulse.segment(firstsamplet+offset,nsamplepulse);
  }

  //do the actual fit
  if (_debug) std::cout << " PulseChiSqSNNLS::DoFit::Minimize " << std::endl;
  
//   std::cout << " PulseChiSqSNNLS::DoFit::Minimize " << std::endl;
  bool status = Minimize(samplecor,pederr,fullpulsecov);
  (*_ampvecmin) = (*_ampvec);
  (*_bxsmin) = (*_bxs);
  
  for (unsigned int ipulse=0; ipulse<npulse; ++ipulse) {
   int bx = _bxs->coeff(ipulse);
//    std::cout << " PulseChiSqSNNLS::DoFit::bx (after) = " << bx << std::endl;
  }
  
//   std::cout << " PulseChiSqSNNLS::DoFit::status = " << status << std::endl;
  
  
  if (!status) return status;
  
  if(!_computeErrors) return status;

  //compute MINOS-like uncertainties for in-time amplitude
  bool foundintime = false;
  unsigned int ipulseintime = 0;
  for (unsigned int ipulse=0; ipulse<npulse; ++ipulse) {
    if (_bxs->coeff(ipulse)==0) { //---- coeff are -5, -4, ... 0, 1, 2, 3, 4 ---> 0 is by definition the in-time!
      ipulseintime = ipulse;
      foundintime = true;
      break;
    }
  }
  if (!foundintime) return status;
  
  if (_debug) std::cout << " PulseChiSqSNNLS::DoFit::ipulseintime = " << ipulseintime << std::endl;  
  const unsigned int ipulseintimemin = ipulseintime;
  
  double approxerr = ComputeApproxUncertainty(ipulseintime);
  double chisq0 = _chisq;
  double x0 = (*_ampvecmin)[ipulseintime];  
  
  //move in time pulse first to active set if necessary
  if (ipulseintime<_nP) {
    _pulsemat->col(_nP-1).swap(_pulsemat->col(ipulseintime));
    std::swap(_ampvec->coeffRef(_nP-1),_ampvec->coeffRef(ipulseintime));
    std::swap(_bxs->coeffRef(_nP-1),_bxs->coeffRef(ipulseintime));
    ipulseintime = _nP - 1;
    --_nP;    
  }
  
  
  SampleVector pulseintime = _pulsemat->col(ipulseintime);
  _pulsemat->col(ipulseintime).setZero();
  
  //two point interpolation for upper uncertainty when amplitude is away from boundary
  double xplus100 = x0 + approxerr;
  std::cout << " >>>>       xplus100 = " << xplus100 << std::endl;
  _ampvec->coeffRef(ipulseintime) = xplus100;
  if (_debug) std::cout << " samples = " << samples << std::endl;
  if (_debug) std::cout << "  _ampvec->coeff(ipulseintime)*pulseintime = " <<  _ampvec->coeff(ipulseintime)*pulseintime << std::endl;
  if (_debug) std::cout << "  _ampvec->coeff(ipulseintime) = " <<  _ampvec->coeff(ipulseintime) << std::endl;
  if (_debug) std::cout << "  pulseintime = " <<  pulseintime << std::endl;
  
  (*_sampvec) = samples - _ampvec->coeff(ipulseintime)*pulseintime;  
  status &= Minimize(samplecor,pederr,fullpulsecov);
  if (!status) return status;
  double chisqplus100 = ComputeChiSq();
  
  double sigmaplus = std::abs(xplus100-x0)/sqrt(chisqplus100-chisq0);
  
  //if amplitude is sufficiently far from the boundary, compute also the lower uncertainty and average them
  if ( (x0/sigmaplus) > 0.5 ) {
    for (unsigned int ipulse=0; ipulse<npulse; ++ipulse) {
      if (_bxs->coeff(ipulse)==0) {
        ipulseintime = ipulse;
        break;
      }
    }    
    double xminus100 = std::max(0.,x0-approxerr);
    _ampvec->coeffRef(ipulseintime) = xminus100;
   (*_sampvec) = samples - _ampvec->coeff(ipulseintime)*pulseintime;
    status &= Minimize(samplecor,pederr,fullpulsecov);
    if (!status) return status;
    double chisqminus100 = ComputeChiSq();
    
    double sigmaminus = std::abs(xminus100-x0)/sqrt(chisqminus100-chisq0);
    (*_errvec)[ipulseintimemin] = 0.5*(sigmaplus + sigmaminus);
    
  }
  else {
   (*_errvec)[ipulseintimemin] = sigmaplus;
  }
            
  _chisq = chisq0;  

  return status;
  
}

bool PulseChiSqSNNLS::Minimize(const SampleMatrix &samplecor, double pederr, const FullSampleMatrix &fullpulsecov) {

//   std::cout << " :: PulseChiSqSNNLS::Minimize " << std::endl;
  const int maxiter = 50;
  int iter = 0;
  bool status = false;
  while (true) {    
   
//    std::cout << " iter = " << iter << " :: " << maxiter << std::endl;
   if (iter>=maxiter) {
    //      edm::LogWarning("PulseChiSqSNNLS::Minimize") << "Max Iterations reached at iter " << iter <<  std::endl;
    break;
   }    
   
//    std::cout << " samplecor = " << samplecor << std::endl;
//    std::cout << " pederr = " << pederr << std::endl;
//    std::cout << " fullpulsecov = " << fullpulsecov << std::endl;
   
   status = updateCov(samplecor,pederr,fullpulsecov);    
   if (!status) break;    
   status = NNLS();
   if (!status) break;
   
   double chisqnow = ComputeChiSq();
   double deltachisq = chisqnow-_chisq;
   
   _chisq = chisqnow;
   if (std::abs(deltachisq)<1e-3) {
    break;
   }
   ++iter;    
  }  
  
  return status;  
  
}

bool PulseChiSqSNNLS::updateCov(const SampleMatrix &samplecor, double pederr, const FullSampleMatrix &fullpulsecov) {
 
//  std::cout << " :: PulseChiSqSNNLS::updateCov " << std::endl;
 
//   const unsigned int nsample = SampleVector::RowsAtCompileTime;
  const unsigned int nsample = _NSAMPLES;
  const unsigned int npulse = _bxs->rows();
  
  _invcov->triangularView<Eigen::Lower>() = (pederr*pederr)*samplecor;
//   std::cout << " _invcov->triangularView<Eigen::Lower>() done " << std::endl;
  bool atleastone = false;
  
  for (unsigned int ipulse=0; ipulse<npulse; ++ipulse) {
//    std::cout << " ipulse = " << ipulse << "::" << npulse << " _ampvec->coeff(ipulse) = " << _ampvec->coeff(ipulse)<< std::endl;
   
    if (_ampvec->coeff(ipulse)==0.) continue;
    
    atleastone = true;
    int bx = _bxs->coeff(ipulse);
//     int firstsamplet = std::max(0,bx + 3);
//     int offset = - bx;

    //--- YES
    //     int offset = _NSAMPLES - 5 * int(25. /_NFREQ) - bx;    
    //     int firstsamplet = std::max(0,bx + 3 * int(25. /_NFREQ));
    
    int offset = 4 * int(25. /_NFREQ) - bx;    
    int firstsamplet = std::max(0,bx + 3 * int(25. /_NFREQ));
    
    

//     int offset = _NSAMPLES - (7-2) * int(25 /_NFREQ) - bx;    
//     int firstsamplet = std::max(0,bx + 3 * int(25 /_NFREQ));
    
    
    
    //     int offset = 3 * int(25 /_NFREQ) - bx;
    //     int offset = 3 * int(25 /_NFREQ);
//     int offset = (_NSAMPLES/2 - int(1*25 /_NFREQ)) - bx;
    
    //     int offset = 7-3-bx;
    //     int offset = (_NSAMPLES/2 + 2) - 3 - bx;
    //     int offset =  _NSAMPLES/2 + int(2 * 25 /_NFREQ) - 3 - bx;
    //     int offset = (_NSAMPLES-3)-3-bx;
    
//     std::cout << " ipulse = " << ipulse << " nsample = " << nsample << " nsamplepulse = " <<  nsample-firstsamplet << " firstsamplet+offset= " << firstsamplet+offset << " = " << firstsamplet << " + " << offset << " (= " << 3 * int(25 /_NFREQ) << " - " << bx << " )" << std::endl; 
    
    double ampsq = (_ampvec->coeff(ipulse)) * (_ampvec->coeff(ipulse));
    
    const unsigned int nsamplepulse = nsample-firstsamplet;    
    _invcov->block(firstsamplet,firstsamplet,nsamplepulse,nsamplepulse).triangularView<Eigen::Lower>() += ampsq*fullpulsecov.block(firstsamplet+offset,firstsamplet+offset,nsamplepulse,nsamplepulse);    
  }
  
//   if (atleastone) _covdecomp->compute(*_invcov);

  _covdecomp->compute(*_invcov);
  
//   std::cout << " :: PulseChiSqSNNLS::updateCov (end) " << std::endl; 
  //   _covdecomp_support->llt().compute(*_invcov);
  
  bool status = true;
  return status;
    
}

double PulseChiSqSNNLS::ComputeChiSq() {
 
//  std::cout << " PulseChiSqSNNLS::ComputeChiSq " << std::endl;
 
//   SampleVector resvec = _pulsemat*_ampvec - _sampvec;
//   return resvec.transpose()*_covdecomp->solve(resvec);
  
//  return _covdecomp->matrixL().solve((*_pulsemat) * (*_ampvec) - (*_sampvec) ).squaredNorm();
 return _covdecomp_support->llt().matrixL().solve((*_pulsemat) * (*_ampvec) - (*_sampvec) ).squaredNorm();
 
 
 //   return _covdecomp->matrixL().solve(_pulsemat*_ampvec - _sampvec).squaredNorm();
  
}

double PulseChiSqSNNLS::ComputeApproxUncertainty(unsigned int ipulse) {
  //compute approximate uncertainties
  //(using 1/second derivative since full Hessian is not meaningful in
  //presence of positive amplitude boundaries.)
      
//   return 1./_covdecomp->matrixL().solve(_pulsemat->col(ipulse)).norm();
  return 1./_covdecomp_support->llt().matrixL().solve(_pulsemat->col(ipulse)).norm();
  
}

bool PulseChiSqSNNLS::NNLS() {
  
  //Fast NNLS (fnnls) algorithm as per http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.157.9203&rep=rep1&type=pdf
  
 if (_debug) std::cout << " PulseChiSqSNNLS::NNLS " << std::endl;
 
//   std::cout << " :: PulseChiSqSNNLS::NNLS " << std::endl;
 
  const unsigned int npulse = _bxs->rows();
  
//   std::cout << " _pulsemat  = " << std::endl << (*_pulsemat) << std::endl;
//   std::cout << " _covdecomp = " << _covdecomp << std::endl;
//   std::cout << " _covdecomp_support = " << _covdecomp_support << std::endl;  
//   std::cout << " _invcov = " << _invcov << std::endl;  
 if (_debug) std::cout << " _invcov = " << std::endl << (*_invcov) << std::endl;  
  
//   SamplePulseMatrix invcovp = _covdecomp->matrixL().solve(*_pulsemat);
//   SamplePulseMatrix invcovp = _covdecomp_support->llt().matrixL().solve(*_pulsemat);
  SamplePulseMatrix invcovp = _invcov->llt().matrixL().solve(*_pulsemat);
  if (_debug) std::cout << " invcovp  = " << invcovp << std::endl;
  
  PulseMatrix aTamat(npulse,npulse);
  if (_debug) std::cout << " aTamat  = " << aTamat << std::endl;
  
  aTamat.triangularView<Eigen::Lower>() = invcovp.transpose() * invcovp;
  aTamat = aTamat.selfadjointView<Eigen::Lower>();
  if (_debug) std::cout << " aTamat  = " << aTamat << std::endl;
  
  if (_debug) std::cout << " *_sampvec = " << (*_sampvec) << std::endl;
  
//   PulseVector aTbvec = invcovp.transpose() * (_covdecomp->matrixL()).solve(*_sampvec);  
//   PulseVector aTbvec = invcovp.transpose() * (_covdecomp_support->llt().matrixL()).solve(*_sampvec);  
  PulseVector aTbvec = invcovp.transpose() * (_invcov->llt().matrixL()).solve(*_sampvec);  
  
  if (_debug) std::cout << " aTbvec = " << aTbvec << std::endl;
  
//   PulseVector wvec(npulse);
  PulseVector wvec;
  wvec.resize(npulse,1);
  
  if (_debug) std::cout << " PulseChiSqSNNLS::NNLS here " << std::endl;
  
//   std::cout << " PulseChiSqSNNLS::NNLS here " << std::endl;
  
  const int maxiter = 5000;
  
  int iter = 0;
  while (true) {    
    //can only perform this step if solution is guaranteed viable
    if (iter>0 || _nP==0) {
      if ( _nP==npulse ) break;                  
      
      const unsigned int nActive = npulse - _nP;
      if (_debug) std::cout << " PulseChiSqSNNLS::NNLS nActive = " << nActive << " = " << npulse << " - " << _nP << std::endl;
      
//       std::cout << " PulseChiSqSNNLS::NNLS nActive = " << nActive << " = " << npulse << " - " << _nP << std::endl;
      
//    wvec.tail(nActive) = aTbvec.tail(nActive) - (aTamat.selfadjointView<Eigen::Lower>()*_ampvec).tail(nActive);       
      wvec.tail(nActive) = aTbvec.tail(nActive) - (aTamat.selfadjointView<Eigen::Lower>() * (*_ampvec)).tail(nActive);       
      
      Index idxwmax;
      double wmax = wvec.tail(nActive).maxCoeff(&idxwmax);
      
      if (_debug) std::cout << " PulseChiSqSNNLS::NNLS wmax = " << wmax << std::endl;
      
      //convergence
      if (wmax<1e-11) break;
      
      //unconstrain parameter
      Index idxp = _nP + idxwmax;
      //printf("adding index %i, orig index %i\n",int(idxp),int(_bxs->coeff(idxp)));
      aTamat.col(_nP).swap(aTamat.col(idxp));
      aTamat.row(_nP).swap(aTamat.row(idxp));
      _pulsemat->col(_nP).swap(_pulsemat->col(idxp));
      std::swap(aTbvec.coeffRef(_nP),aTbvec.coeffRef(idxp));
      std::swap(_ampvec->coeffRef(_nP),_ampvec->coeffRef(idxp));
      std::swap(_bxs->coeffRef(_nP),_bxs->coeffRef(idxp));
      
      ++_nP;
    }

//     std::cout << " PulseChiSqSNNLS::NNLS next one ..." << std::endl;
    
    while (true) {
      //printf("iter in, idxsP = %i\n",int(_idxsP.size()));
      
      if (_nP==0) break;     
      
      PulseVector ampvecpermtest = *_ampvec;
      
      //solve for unconstrained parameters      
      ampvecpermtest.head(_nP) = aTamat.topLeftCorner(_nP,_nP).ldlt().solve(aTbvec.head(_nP));     
      
      //check solution
      if (ampvecpermtest.head(_nP).minCoeff()>0.) {
        _ampvec->head(_nP) = ampvecpermtest.head(_nP);
        break;
      }      

      //update parameter vector
      Index minratioidx=0;
      
      double minratio = std::numeric_limits<double>::max();
      for (unsigned int ipulse=0; ipulse<_nP; ++ipulse) {
        if (ampvecpermtest.coeff(ipulse)<=0.) {
          double ratio = _ampvec->coeff(ipulse) / (_ampvec->coeff(ipulse) - ampvecpermtest.coeff(ipulse));
          if (ratio<minratio) {
            minratio = ratio;
            minratioidx = ipulse;
          }
        }
      }

      _ampvec->head(_nP) += minratio*(ampvecpermtest.head(_nP) - _ampvec->head(_nP));
      
      //avoid numerical problems with later ==0. check
      _ampvec->coeffRef(minratioidx) = 0.;
            
      //printf("removing index %i, orig idx %i\n",int(minratioidx),int(_bxs->coeff(minratioidx)));
      aTamat.col(_nP-1).swap(aTamat.col(minratioidx));
      aTamat.row(_nP-1).swap(aTamat.row(minratioidx));
      _pulsemat->col(_nP-1).swap(_pulsemat->col(minratioidx));
      std::swap(aTbvec.coeffRef(_nP-1),aTbvec.coeffRef(minratioidx));
      std::swap(_ampvec->coeffRef(_nP-1),_ampvec->coeffRef(minratioidx));
      std::swap(_bxs->coeffRef(_nP-1),_bxs->coeffRef(minratioidx));
      --_nP;
      
    }
    
//     std::cout << " PulseChiSqSNNLS::NNLS iter = " << iter << std::endl;
    ++iter;
    if (iter > maxiter) break;

  }
  
  return true;
  
  
}
