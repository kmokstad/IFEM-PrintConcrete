// $Id$
//==============================================================================
//!
//! \file NonlinearElasticityBirth.h
//!
//! \date Aug 18 2026
//!
//! \author Knut Morten Okstad / SINTEF
//!
//! \brief Integrand implementations for nonlinear elasticity problems.
//!
//==============================================================================

#ifndef _NONLINEAR_ELASTICITY_BIRTH_H
#define _NONLINEAR_ELASTICITY_BIRTH_H

#include "NonlinearElasticityUL.h"
#include "Tensor.h"


/*!
  \brief Class representing the integrand of the nonlinear elasticity problem.
  \details This class inherits most of the NonlinearElasticityUL methods,
  but overrides the NonlinearElasticityUL::kinematics() method to account for
  initial stress-free deformation at element birth time.
*/

class NonlinearElasticityBirth : public NonlinearElasticityUL
{
public:
  //! \brief The constructor invokes the parent class constructor only.
  //! \param[in] n Number of spatial dimensions
  explicit NonlinearElasticityBirth(unsigned short int n)
    : NonlinearElasticityUL(n), iAmIntegrating(false) {}

  //! \brief Initializes the integrand with the number of integration points.
  //! \param[in] nGp Total number of interior integration points
  //! \param[in] nBp Total number of boundary integration points
  void initIntegration(size_t nGp, size_t nBp) override;
  //! \brief Initializes the integrand for a new integration loop.
  //! \param[in] prm Nonlinear solution algorithm parameters
  void initIntegration(const TimeDomain& prm, const Vector&, bool) override;
  //! \brief Initializes the integrand for a new result point loop.
  //! \param[in] lambda Load parameter
  //! \param[in] prinDir Option for computation of principle stress directions
  void initResultPoints(double lambda, char prinDir) override;

  //! \brief Calculates some kinematic quantities at current point.
  //! \param[in] eV Element solution vector
  //! \param[in] iP Global integration/result point counter
  //! \param[in] N Basis function values at current point
  //! \param[in] dNdX Basis function gradients at current point
  //! \param[in] r Radial coordinate of current point
  //! \param[out] F Deformation gradient at current point
  //! \param[out] E Green-Lagrange strain tensor at current point
  bool kinematics(const Vector& eV, size_t iP,
                  const Vector& N, const Matrix& dNdX, double r,
                  Tensor& F, Matrix*, SymmTensor* E) const override;

  //! \brief Checks if the specified number of integration points is valid.
  //! \param[in] nGp Total number of integration points in the quadrature.
  //!
  //! \details This method always returns \e true, unless internal integration
  //! point buffers are used. The size of the buffers (if any) then need to be
  //! equal to \a nGp, otherwise \a false is returned.
  bool checkItgBuffer(size_t nGp) const override;

private:
  bool iAmIntegrating; //!< Flag indicating integration or result evaluation

  mutable std::vector<Tensor> Fbirth; //!< Deformation gradients at birth
};

#endif
