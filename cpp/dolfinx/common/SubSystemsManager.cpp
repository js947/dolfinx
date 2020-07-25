// Copyright (C) 2008-2017 Garth N. Wells, Anders Logg, Jan Blechta
//
// This file is part of DOLFINX (https://www.fenicsproject.org)
//
// SPDX-License-Identifier:    LGPL-3.0-or-later

#define MPICH_IGNORE_CXX_SEEK 1

#include "SubSystemsManager.h"
#include <boost/algorithm/string/trim.hpp>
#include <dolfinx/common/log.h>
#include <iostream>
#include <mpi.h>
// #include <petsc.h>
#include <petscsys.h>

#ifdef HAS_SLEPC
#include <slepcsys.h>
#endif

using namespace dolfinx::common;

// Return singleton instance. Do NOT make the singleton a global static
// object; the method here ensures that the singleton is initialised
// before use. (google "static initialization order fiasco" for full
// explanation)

// SubSystemsManager& SubSystemsManager::singleton()
// {
//   static SubSystemsManager the_instance;
//   return the_instance;
// }
//-----------------------------------------------------------------------------
// SubSystemsManager::SubSystemsManager()
//     : petsc_err_msg(""), petsc_initialized(false), control_mpi(false)
// {
//   // Do nothing
// }
//-----------------------------------------------------------------------------
// SubSystemsManager::~SubSystemsManager() { finalize(); }
//-----------------------------------------------------------------------------
void SubSystemsManager::init_mpi()
{
  int mpi_initialized;
  MPI_Initialized(&mpi_initialized);
  if (mpi_initialized)
    return;

  // Init MPI with highest level of thread support
  std::string s("");
  char* c = const_cast<char*>(s.c_str());
  SubSystemsManager::init_mpi(0, &c, MPI_THREAD_MULTIPLE);
}
//-----------------------------------------------------------------------------
int SubSystemsManager::init_mpi(int argc, char* argv[],
                                int required_thread_level)
{
  int mpi_initialized;
  MPI_Initialized(&mpi_initialized);
  if (mpi_initialized)
    return -100;

  // Initialise MPI and take responsibility
  int provided = -1;
  MPI_Init_thread(&argc, &argv, required_thread_level, &provided);
  return provided;
}
//-----------------------------------------------------------------------------
void SubSystemsManager::init_logging(int argc, char* argv[])
{
  loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;
  loguru::Options options = {"-v", "main thread", false};
  loguru::init(argc, argv, options);
}
//-----------------------------------------------------------------------------
void SubSystemsManager::init_petsc()
{
  // Initialize PETSc
  int argc = 0;
  char** argv = nullptr;
  init_petsc(argc, argv);
}
//-----------------------------------------------------------------------------
void SubSystemsManager::init_petsc(int argc, char* argv[])
{
  // Print message if PETSc is initialised with command line arguments
  if (argc > 1)
    LOG(INFO) << "Initializing PETSc with given command-line arguments.";

  PetscBool is_initialized;
  PetscInitialized(&is_initialized);
  if (!is_initialized)
    PetscInitialize(&argc, &argv, nullptr, nullptr);

#ifdef HAS_SLEPC
  SlepcInitialize(&argc, &argv, nullptr, nullptr);
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::finalize_mpi()
{
  int mpi_initialized;
  MPI_Initialized(&mpi_initialized);

  // Finalise MPI if required
  if (mpi_initialized)
  {
    // Check in MPI has already been finalised (possibly incorrectly by
    // a 3rd party library). If it hasn't, finalise as normal.
    int mpi_finalized;
    MPI_Finalized(&mpi_finalized);
    if (!mpi_finalized)
      MPI_Finalize();
    else
    {
      // Use std::cout since log system may fail because MPI has been shut down.
      std::cout << "DOLFINX is responsible for MPI, but it has been finalized "
                   "elsewhere prematurely."
                << std::endl;
      std::cout << "This is usually due to a bug in a 3rd party library, and "
                   "can lead to unpredictable behaviour."
                << std::endl;
    }
  }
}
//-----------------------------------------------------------------------------
void SubSystemsManager::finalize_petsc()
{
  PetscFinalize();
#ifdef HAS_SLEPC
  SlepcFinalize();
#endif
}
//-----------------------------------------------------------------------------
bool SubSystemsManager::mpi_initialized()
{
  // This function is not affected if MPI_Finalize has been called. It
  // returns true if MPI_Init has been called at any point, even if
  // MPI_Finalize has been called.
  int mpi_initialized;
  MPI_Initialized(&mpi_initialized);
  return mpi_initialized;
}
//-----------------------------------------------------------------------------
bool SubSystemsManager::mpi_finalized()
{
  int mpi_finalized;
  MPI_Finalized(&mpi_finalized);
  return mpi_finalized;
}
//-----------------------------------------------------------------------------
// PetscErrorCode SubSystemsManager::PetscDolfinErrorHandler(
//     MPI_Comm, int line, const char* fun, const char* file, PetscErrorCode n,
//     PetscErrorType, const char* mess, void*)
// {
//   // Store message for printing later (by PETScObject::petsc_error) only
//   // if it's not empty message (passed by PETSc when repeating error)
//   std::string _mess = mess;
//   boost::algorithm::trim(_mess);
//   if (_mess != "")
//     singleton().petsc_err_msg = _mess;

//   // Fetch PETSc error description
//   const char* desc;
//   PetscErrorMessage(n, &desc, nullptr);

//   // Log detailed error info
//   LOG(ERROR)
//       << "PetscDolfinErrorHandler: line '{}', function '{}', file '{}',\n"
//          "                       : error code '{}' ({}), message follows:"
//       << line << fun << file << n << desc;
//   LOG(ERROR) << (_mess);

//   // Continue with error handling
//   PetscFunctionReturn(n);
// }
//-----------------------------------------------------------------------------
