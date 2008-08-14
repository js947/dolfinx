// Copyright (C) 2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2008.
//
// First added:  2008-01-07
// Last changed: 2008-08-13

#ifdef HAS_PETSC
#include <petsc.h>
#endif

#ifdef HAS_SLEPC
#include <slepc.h>
#endif

#ifdef HAS_MPI
#include <mpi.h>
#endif

#include <dolfin/common/constants.h>
#include <dolfin/log/dolfin_log.h>
#include "SubSystemsManager.h"

using namespace dolfin;

// Initialise static data
dolfin::SubSystemsManager dolfin::SubSystemsManager::sub_systems_manager;

//-----------------------------------------------------------------------------
SubSystemsManager::SubSystemsManager() : petsc_initialized(false),
                                         control_mpi(false)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
SubSystemsManager::SubSystemsManager(const SubSystemsManager& sub_sys_manager)
{
  error("Should not be using copy constructor of SubSystemsManager.");
}
//-----------------------------------------------------------------------------
SubSystemsManager::~SubSystemsManager()
{
  // Finalize subsystems in the correct order
  finalizePETSc();
  finalizeMPI();
}
//-----------------------------------------------------------------------------
void SubSystemsManager::initMPI()
{
#ifdef HAS_MPI
  std::cout << "MPI status in initMPI() " << MPI::Is_initialized() << std::endl;
  if( MPI::Is_initialized() )
    return;

  // Initialise MPI and take responsibility
  MPI::Init();
  sub_systems_manager.control_mpi = true;
#else
  // Do nothing
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::initPETSc()
{
#ifdef HAS_PETSC
  if ( sub_systems_manager.petsc_initialized )
    return;

  message(1, "Initializing PETSc (ignoring command-line arguments).");

  // Dummy command-line arguments for PETSc. This is needed since
  // PetscInitializeNoArguments() does not seem to work.
  int argc = 0;
  char** argv = NULL;

  // Initialize PETSc
  initPETSc(argc, argv, false);
#else
  error("DOLFIN has not been configured for PETSc.");
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::initPETSc(int argc, char* argv[], bool cmd_line_args)
{
#ifdef HAS_PETSC
  if ( sub_systems_manager.petsc_initialized )
    return;

  std::cout << "MPI status in initPETSc() " << MPI::Is_initialized() << std::endl;

  // Get status of MPI before PETSc initialisation
  const bool mpi_init_status = MPI::Is_initialized();

  // Print message if PETSc is intialised with command line arguments
  if(cmd_line_args)
    message(1, "Initializing PETSc with given command-line arguments.");

  // Initialize PETSc
  PetscInitialize(&argc, &argv, PETSC_NULL, PETSC_NULL);

#ifdef HAS_SLEPC
  // Initialize SLEPc
  SlepcInitialize(&argc, &argv, PETSC_NULL, PETSC_NULL);
#endif

  sub_systems_manager.petsc_initialized = true;

  // Determine if PETSc initialised MPI (and is therefore responsible for MPI finalization)
  if(MPI::Is_initialized() and !mpi_init_status)
    sub_systems_manager.control_mpi = false;
#else
  error("DOLFIN has not been configured for PETSc.");
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::finalizeMPI()
{
#ifdef HAS_MPI
  //Finalise MPI if required
  if ( MPI::Is_initialized() and sub_systems_manager.control_mpi )
    MPI::Finalize();
#else
  // Do nothing
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::finalizePETSc()
{
#ifdef HAS_PETSC
 if ( sub_systems_manager.petsc_initialized )
  {
    PetscFinalize();
 
    #ifdef HAS_SLEPC
    SlepcFinalize();
    #endif
  }
#else
  // Do nothing
#endif
}
//-----------------------------------------------------------------------------

