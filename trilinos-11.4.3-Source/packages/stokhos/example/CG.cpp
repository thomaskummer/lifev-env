/*
// @HEADER
// ***********************************************************************
// 
//                           Stokhos Package
//                 Copyright (2009) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Eric T. Phipps (etphipp@sandia.gov).
// 
// ***********************************************************************
// @HEADER
*/

#include "Teuchos_SerialDenseMatrix.hpp"
#include "Teuchos_LAPACK.hpp"
#include "Stokhos.hpp"
#include "Teuchos_GlobalMPISession.hpp"
#include "Teuchos_CommandLineProcessor.hpp"
#include "Teuchos_TestForException.hpp"
#include "Stokhos_Operator.hpp"
#include "Stokhos_DiagPreconditioner.hpp"

// CG
int CG(const  Teuchos::SerialDenseMatrix<int, double> &  A, Teuchos::SerialDenseMatrix<int,double>   X,const Teuchos::SerialDenseMatrix<int,double> &   B, int max_iter, double tolerance, Stokhos::DiagPreconditioner<int,double> prec)

{
  int n; 
  int k=0;
  double resid;
  
  n=A.numRows();
  std::cout << "A= " << A << std::endl;
  std::cout << "B= " << B << std::endl;
  Teuchos::SerialDenseMatrix<int, double> Ax(n,1);
  Ax.multiply(Teuchos::NO_TRANS,Teuchos::NO_TRANS,1.0, A, X, 0.0);

  Teuchos::SerialDenseMatrix<int, double> r(B);
  r-=Ax;  
  resid=r.normFrobenius(); 
  Teuchos::SerialDenseMatrix<int, double> rho(1,1);
  Teuchos::SerialDenseMatrix<int, double> oldrho(1,1);
  Teuchos::SerialDenseMatrix<int, double> pAp(1,1);
  Teuchos::SerialDenseMatrix<int, double> Ap(n,1);
  
  double b;
  double a;
  Teuchos::SerialDenseMatrix<int, double> p(r);

  
 
  while (resid > tolerance && k < max_iter){
 
     Teuchos::SerialDenseMatrix<int, double> z(r);
     
     //z=M-1r
//     prec.ApplyInverse(r,z);

     rho.multiply(Teuchos::TRANS,Teuchos::NO_TRANS,1.0, r, z, 0.0);
  
     if (k==0){
       p.assign(z);
       rho.multiply(Teuchos::TRANS, Teuchos::NO_TRANS, 1.0, r, z, 0.0);
      }  
      else {
        b=rho(0,0)/oldrho(0,0);
        p.scale(b);
        p+=z;
      }
      Ap.multiply(Teuchos::NO_TRANS,Teuchos::NO_TRANS,1.0, A, p, 0.0);
      pAp.multiply(Teuchos::TRANS,Teuchos::NO_TRANS,1.0, p, Ap, 0.0);
      a=rho(0,0)/pAp(0,0);
      Teuchos::SerialDenseMatrix<int, double> scalep(p);
      scalep.scale(a);
      X+=scalep;
      Ap.scale(a);
      r-=Ap;
      oldrho.assign(rho);
      resid=r.normFrobenius();
   
 
     k++;
  } 
  
 std::cout << "X=  " << X << std::endl;

 return 0;
}

int main()
{
  Teuchos::SerialDenseMatrix<int, double> A(5,5);
  Teuchos::SerialDenseMatrix<int, double> B(5,1);
  Teuchos::SerialDenseMatrix<int, double> X(5,1);

/* for (double i=0; i<A.numRows();i++){
      for (double j=0; j<A.numRows();j++){
           A(i,j)=1/(i+j+1);
     }}*/ 

for (int i=0; i<A.numRows(); i++){
       A(0,i)=1;
       A(i,0)=1;
     }
     A(1,1)=2; A(1,2)=3;A(1,3)=4;A(1,4)=5;
     A(2,1)=3; A(2,2)=6;A(2,3)=10;A(2,4)=15;
     A(3,1)=4; A(3,2)=10;A(3,3)=20;A(3,4)=35;
     A(4,1)=5; A(4,2)=15;A(4,3)=35;A(4,4)=70;
      B.putScalar(1.0);
  X.putScalar(0.0);
  
  int maxit=40;
  double tolerance=1e-6;
  

  Stokhos::DiagPreconditioner<int,double> prec(A);

  CG(A,X,B,maxit,tolerance, prec);
 
  return 0;
}





