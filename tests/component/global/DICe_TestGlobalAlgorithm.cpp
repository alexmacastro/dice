// @HEADER
// ************************************************************************
//
//               Digital Image Correlation Engine (DICe)
//                 Copyright 2015 Sandia Corporation.
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
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
// Questions? Contact: Dan Turner (dzturne@sandia.gov)
//
// ************************************************************************
// @HEADER

#include <DICe.h>
#include <DICe_Global.h>
#include <DICe_Parser.h>

#include <Teuchos_RCP.hpp>
#include <Teuchos_oblackholestream.hpp>
#include <Teuchos_ParameterList.hpp>

#include <iostream>

using namespace DICe;

int main(int argc, char *argv[]) {

  DICe::initialize(argc, argv);

  // only print output if args are given (for testing the output is quiet)
  int_t iprint     = argc - 1;
  int_t errorFlag  = 0;
  Teuchos::RCP<std::ostream> outStream;
  Teuchos::oblackholestream bhs; // outputs nothing
  if (iprint > 0)
    outStream = Teuchos::rcp(&std::cout, false);
  else
    outStream = Teuchos::rcp(&bhs, false);

  *outStream << "--- Begin test ---" << std::endl;

  *outStream << " TESTING HORN_SCHUNCK FORMULATION " << std::endl;

  *outStream << "creating the global parameter list" << std::endl;

  Teuchos::RCP<Teuchos::ParameterList> global_params = Teuchos::rcp(new Teuchos::ParameterList());
  global_params->set(DICe::mesh_size,1000.0);
  global_params->set(DICe::global_regularization_alpha,1.0);
  global_params->set(DICe::global_formulation,HORN_SCHUNCK);
  global_params->set(DICe::global_solver,GMRES_SOLVER);
  global_params->set(DICe::output_folder,"");
  global_params->set(DICe::output_prefix,"test_global_alg_hs");
  Teuchos::ParameterList mms_sublist;
  mms_sublist.set(DICe::problem_name,"div_curl_modulator");
  mms_sublist.set(DICe::phi_coeff,10.0);
  mms_sublist.set(DICe::b_coeff,2.0);
  global_params->set(DICe::mms_spec,mms_sublist);
  global_params->print(*outStream);

  *outStream << "creating a global algorithm" << std::endl;

  Teuchos::RCP<DICe::global::Global_Algorithm> global_alg = Teuchos::rcp(new DICe::global::Global_Algorithm(global_params));

  *outStream << "pre-execution tasks" << std::endl;

  global_alg->pre_execution_tasks();

  *outStream << "executing" << std::endl;

  global_alg->execute();

  *outStream << "post execution tasks" << std::endl;

  global_alg->post_execution_tasks(1.0);

  *outStream << "evaluating the error" << std::endl;

  scalar_t error_bx = 0.0;
  scalar_t error_by = 0.0;
  scalar_t error_lambda = 0.0;
  global_alg->evaluate_mms_error(error_bx,error_by,error_lambda);

  const scalar_t error_max = 0.02;
  if(error_bx > error_max || error_by > error_max){
    *outStream << "error, the solution error is too large" << std::endl;
    errorFlag++;
  }
// deactivating Levenberg Marquardt because it is not stable....

//  *outStream << " TESTING LEVENBERG_MARQUARDT FORMULATION " << std::endl;
//
//  *outStream << "creating the global parameter list" << std::endl;
//
//  global_params->set(DICe::mesh_size,500.0);
//  global_params->set(DICe::global_regularization_alpha,5.0);
//  global_params->set(DICe::global_formulation,LEVENBERG_MARQUARDT);
//  global_params->set(DICe::global_solver,GMRES_SOLVER);
//  global_params->set(DICe::output_folder,"");
//  global_params->set(DICe::output_prefix,"test_global_alg_lm");
//  global_params->set(DICe::mms_spec,mms_sublist);
//  global_params->print(*outStream);
//
//  *outStream << "creating a global algorithm" << std::endl;
//
//  Teuchos::RCP<DICe::global::Global_Algorithm> global_alg_lm = Teuchos::rcp(new DICe::global::Global_Algorithm(global_params));
//
//  *outStream << "pre-execution tasks" << std::endl;
//
//  global_alg_lm->pre_execution_tasks();
//
//  *outStream << "executing" << std::endl;
//
//  global_alg_lm->execute();
//
//  *outStream << "post execution tasks" << std::endl;
//
//  global_alg_lm->post_execution_tasks(1.0);
//
//  *outStream << "evaluating the error" << std::endl;
//
//  error_bx = 0.0;
//  error_by = 0.0;
//  error_lambda = 0.0;
//  global_alg_lm->evaluate_mms_error(error_bx,error_by,error_lambda);
//
//  if(error_bx > error_max || error_by > error_max){
//    *outStream << "error, the solution error is too large" << std::endl;
//    errorFlag++;
//  }

  *outStream << "--- End test ---" << std::endl;

  DICe::finalize();

  if (errorFlag != 0)
    std::cout << "End Result: TEST FAILED\n";
  else
    std::cout << "End Result: TEST PASSED\n";

  return 0;

}

