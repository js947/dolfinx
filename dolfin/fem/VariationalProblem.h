// Copyright (C) 2008 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-12-26
// Last changed: 2008-12-26

#ifndef __VARIATIONAL_PROBLEM_H
#define __VARIATIONAL_PROBLEM_H

#include <vector>

#include <dolfin/la/Matrix.h>
#include <dolfin/la/Vector.h>
#include <dolfin/parameter/Parametrized.h>

namespace dolfin
{

  class Form;
  class BoundaryCondition;
  class Function;

  /// This class represents a (system of) partial differential
  /// equation(s) in variational form: Find u in V such that
  ///
  ///     F_u(v) = 0  for all v in V'.
  ///
  /// The variational problem is defined in terms of a bilinear
  /// form a(v, u) and a linear for L(v).
  ///
  /// For a linear variational problem, F_u(v) = a(v, u) - L(v),
  /// the forms should correspond to the canonical formulation
  ///
  ///     a(v, u) = L(v)  for all v in V'.
  ///
  /// For a nonlinear variational problem, the forms should
  /// be given by
  /// 
  ///     a(v, u) = F_u'(v) u = F_u'(v, u),
  ///     L(v)    = F(v),
  ///
  /// that is, a(v, u) should be the Frechet derivative of F_u
  /// with respect to u, and L = F.

  class VariationalProblem : public Parametrized
  {
  public:

    /// Define variational problem with natural boundary conditions
    VariationalProblem(const Form& a,
                       const Form& L,
                       bool nonlinear=false);

    /// Define variational problem with a single Dirichlet boundary conditions
    VariationalProblem(const Form& a,
                       const Form& L,
                       const BoundaryCondition& bc,
                       bool nonlinear=false);

    // FIXME: Pointers need to be const here to work with SWIG. Is there a fix for this?

    /// Define variational problem with a list of Dirichlet boundary conditions
    VariationalProblem(const Form& a,
                       const Form& L,
                       std::vector<BoundaryCondition*>& bcs,
                       bool nonlinear=false);

    /// Destructor
    ~VariationalProblem();

    /// Solve variational problem
    void solve(Function& u);

    /// Solve variational problem and extract sub functions
    void solve(Function& u0, Function& u1);

    /// Solve variational problem and extract sub functions
    void solve(Function& u0, Function& u1, Function& u2);

    /// Return system matrix
    const GenericMatrix& matrix() const;

    /// Return system vector
    const GenericVector& vector() const;

  private:

    // Solve linear variational problem
    void solve_linear(Function& u);

    // Solve nonlinear variational problem
    void solve_nonlinear(Function& u);

    // Bilinear form
    const Form& a;

    // Linear form
    const Form& L;

    // Boundary conditions
    std::vector<const BoundaryCondition*> bcs;

    // True if problem is nonlinear
    bool nonlinear;

    // Matrix
    Matrix A;

    // Vector
    Vector b;

  };

}

#endif
