// Copyright (C) 2008-2020 Garth N. Wells
//
// This file is part of DOLFINX (https://www.fenicsproject.org)
//
// SPDX-License-Identifier:    LGPL-3.0-or-later

#pragma once

namespace dolfinx::common
{

/// Function in this namesspace are convenience functtions for the
/// initialisation and finalisation of various sub systems, such as MPI
/// and PETSc.

namespace SubSystemsManager
{

/// Initialise MPI
void init_mpi();

/// Initialise MPI with required level of thread support
int init_mpi(int argc, char* argv[], int required_thread_level);

/// Initialise loguru
void init_logging(int argc, char* argv[]);

/// Initialize PETSc without command-line arguments
void init_petsc();

/// Initialize PETSc with command-line arguments. Note that PETSc
/// command-line arguments may also be filtered and sent to PETSc by
/// parameters.parse(argc, argv).
void init_petsc(int argc, char* argv[]);

/// Check if MPI has been initialised (returns true if MPI has been
/// initialised, even if it is later finalised)
bool mpi_initialized();

/// Check if MPI has been finalized (returns true if MPI has been
/// finalised)
bool mpi_finalized();

/// Finalize MPI
void finalize_mpi();

/// Finalize PETSc
void finalize_petsc();

}; // namespace SubSystemsManager
} // namespace dolfinx::common
