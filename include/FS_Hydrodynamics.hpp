// Copyright 2022 Monterey Bay Aquarium Research Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef FREESURFACEHYDRODYNAMICS__LIB__FS_HYDRODYNAMICS_HPP_
#define FREESURFACEHYDRODYNAMICS__LIB__FS_HYDRODYNAMICS_HPP_

#include <Eigen/Dense>
#include <vector>
#include <string>

#include "IncidentWave.hpp"

// Storage space for xddot and eta.
//  Should be >2, larger takes more memory but there's less data shuffling
#define STORAGE_MULTIPLIER 5

class FS_HydroDynamics
{
public:
  double m_gam = 0.15;
  // FS_HydroDynamics();
  explicit FS_HydroDynamics(IncidentWave & IncWave);
  FS_HydroDynamics(IncidentWave & IncWave, double L, double g, double rho);
  void ReadWAMITData_FD(std::string filenm);
  void ReadWAMITData_TD(std::string filenm);
  void Plot_FD_Coeffs();
  void Plot_TD_Coeffs();

  double AddedMass(double omega, int i, int j);
  Eigen::Matrix<double, 6, 6> AddedMass(double omega);

  double RadiationDamping(double omega, int i, int j);
  Eigen::Matrix<double, 6, 6> RadiationDamping(double omega);

  std::complex<double> WaveExcitingForceComponents(double omega, int j);
  Eigen::Matrix<std::complex<double>, 6, 1> WaveExcitingForceComponents(double omega);

  void SetTimestepSize(double dt);
  double GetTimestepSize();

  void SetDampingCoeffs(Eigen::VectorXd b);
  void SetDragCoeffs(Eigen::VectorXd Cd);
  void SetAreas(Eigen::VectorXd A);

  void SetWaterplane(double S, double S11, double S22);
  void SetCOB(double x, double y, double z);
  void SetCOG(double x, double y, double z);
  void SetVolume(double V);
  void SetMass(double m);
  void SetI(Eigen::Matrix<double, 3, 3> I);

  void operator()(const std::vector<double> & x, std::vector<double> & dxdt, const double /* t */);

  Eigen::VectorXd ViscousDragForce(Eigen::VectorXd xdot);
  Eigen::VectorXd LinearDampingForce(Eigen::VectorXd xdot);
  Eigen::VectorXd GravityForce(Eigen::VectorXd x);
  Eigen::VectorXd BuoyancyForce(Eigen::VectorXd x);
  Eigen::VectorXd RadiationForce(Eigen::VectorXd last_xddot);
  Eigen::VectorXd ExcitingForce();

  Eigen::Matrix<std::complex<double>, 6, 1> ComplexAmplitude(double omega);
  std::complex<double> ComplexAmplitude(double omega, int mode);


  friend std::ostream &
  operator<<(std::ostream & out, const FS_HydroDynamics & f);
  std::string m_fd_filename;
  std::string m_td_filename;
  IncidentWave & _IncWave;
  double m_L = 1;
  double m_grav = 9.81;
  double m_rho = 1025;

  //Linear and Viscous Damping coefficients
  Eigen::VectorXd m_b;
  Eigen::VectorXd m_Cd;
  Eigen::VectorXd m_Area;

  // Frequency domain coefficients
  Eigen::VectorXd fd_am_dmp_tps;
  Eigen::VectorXd fd_am_dmp_omega;
  std::vector<Eigen::Matrix<double, 6, 6>> fd_A;  // Note: fd_A is a_ij in Newman pp 295, eqn 152
  std::vector<Eigen::Matrix<double, 6, 6>> fd_B;  // Note: fd_B is b_ij in Newman pp 295, eqn 152
  Eigen::Matrix<double, 6, 6> fd_A_inf_freq;
  Eigen::Matrix<double, 6, 6> fd_B_inf_freq;
  Eigen::VectorXd fd_ext_tps;
  Eigen::VectorXd fd_ext_omega;
  Eigen::VectorXd fd_ext_beta;
  std::vector<Eigen::Matrix<double, Eigen::Dynamic, 1>> fd_Mod_Chi;
  std::vector<Eigen::Matrix<double, Eigen::Dynamic, 1>> fd_Pha_Chi;
  std::vector<Eigen::Matrix<double, Eigen::Dynamic, 1>> fd_Re_Chi;
  std::vector<Eigen::Matrix<double, Eigen::Dynamic, 1>> fd_Im_Chi;

  Eigen::Matrix<double, 6, 6> c;  // Hydrostatics that depend upon body position

  // Time domain coefficients
  Eigen::VectorXd m_tau_rad;
  std::array<std::array<Eigen::VectorXd, 6>, 6> m_IR_cosint;
  std::array<std::array<Eigen::VectorXd, 6>, 6> m_IR_sinint;

  double m_dtau_exc;
  Eigen::VectorXd m_tau_exc;
  std::array<Eigen::VectorXd, 6> m_IR_exc;

  // Vectors for numerical integration
  double m_dt = 0;
  // Eigen::Matrix<Eigen::VectorXd, 6, 6> m_L_rad;
  std::array<std::array<Eigen::VectorXd, 6>, 6> m_L_rad;
  std::array<Eigen::VectorXd, 6> m_L_exc;

  // Storage for accelerations for each of 6 DOF
  std::array<Eigen::VectorXd, 6> m_xddot;

  // Storage for wave-elevation at origin;
  Eigen::VectorXd _eta0;

  int _rad_tstep_index = 0;
  int _exc_tstep_index = 0;
  int _n_rad_intpts = 0;
  int _n_exc_intpts = 0;

  // wave-elevation evaluation time;
  double _t_eta;

  /// \brief Buoy WaterPlane Area
  double S;

  /// \brief Buoy WaterPlane Area Second Moment of Area around x
  double S11;

  /// \brief Buoy WaterPlane Area Second Moment of Area around y
  double S22;

  /// \brief Submerged Volume
  double Vol;

  /// \brief Center of Buoyancy relative to water-plane coordinate system
  Eigen::Vector3d COB;

  /// \brief Center of gravity relative to water-plane coordinate system
  Eigen::Vector3d COG;

  /// \brief Mass Matrix, required only for complex amplitude calculation
  Eigen::Matrix<double, 6, 6> M;

private:
  void Compute_cij(void);
};

#endif  // FREESURFACEHYDRODYNAMICS__LIB__FS_HYDRODYNAMICS_HPP_
