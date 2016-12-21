/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#ifdef PAIR_CLASS

PairStyle(exp6/rx/kk,PairExp6rxKokkos<LMPDeviceType>)
PairStyle(exp6/rx/kk/device,PairExp6rxKokkos<LMPDeviceType>)
PairStyle(exp6/rx/kk/host,PairExp6rxKokkos<LMPHostType>)

#else

#ifndef LMP_PAIR_EXP6_RX_KOKKOS_H
#define LMP_PAIR_EXP6_RX_KOKKOS_H

#include "pair_exp6_rx.h"
#include "kokkos_type.h"
#include "pair_kokkos.h"

namespace LAMMPS_NS {

// Create a structure to hold the parameter data for all
// local and neighbor particles. Pack inside this struct
// to avoid any name clashes.

template<class DeviceType>
struct PairExp6ParamDataTypeKokkos
{
  typedef ArrayTypes<DeviceType> AT;

   int n;
   typename AT::t_float_1d epsilon1, alpha1, rm1, fraction1,
          epsilon2, alpha2, rm2, fraction2,
          epsilonOld1, alphaOld1, rmOld1, fractionOld1,
          epsilonOld2, alphaOld2, rmOld2, fractionOld2;

   // Default constructor -- nullify everything.
   PairExp6ParamDataTypeKokkos<DeviceType>(void)
      : n(0)
   {}
};

struct TagPairExp6rxgetParamsEXP6{};

template<int NEIGHFLAG, int NEWTON_PAIR, int EVFLAG>
struct TagPairExp6rxCompute{};

template<class DeviceType>
class PairExp6rxKokkos : public PairExp6rx {
 public:
  typedef DeviceType device_type;
  typedef ArrayTypes<DeviceType> AT;
  typedef EV_FLOAT value_type;

  PairExp6rxKokkos(class LAMMPS *);
  virtual ~PairExp6rxKokkos();
  void compute(int, int);
  void init_style();

  KOKKOS_INLINE_FUNCTION
  void operator()(TagPairExp6rxgetParamsEXP6, const int&) const;

  template<int NEIGHFLAG, int NEWTON_PAIR, int EVFLAG>
  KOKKOS_INLINE_FUNCTION
  void operator()(TagPairExp6rxCompute<NEIGHFLAG,NEWTON_PAIR,EVFLAG>, const int&, EV_FLOAT&) const;

  template<int NEIGHFLAG, int NEWTON_PAIR, int EVFLAG>
  KOKKOS_INLINE_FUNCTION
  void operator()(TagPairExp6rxCompute<NEIGHFLAG,NEWTON_PAIR,EVFLAG>, const int&) const;

  template<int NEIGHFLAG, int NEWTON_PAIR>
  KOKKOS_INLINE_FUNCTION
  void ev_tally(EV_FLOAT &ev, const int &i, const int &j,
      const F_FLOAT &epair, const F_FLOAT &fpair, const F_FLOAT &delx,
                  const F_FLOAT &dely, const F_FLOAT &delz) const;

  KOKKOS_INLINE_FUNCTION
  int sbmask(const int& j) const;

 protected:
  int eflag,vflag;
  int nlocal,newton_pair,neighflag;
  double special_coul[4];
  double special_lj[4];

  typename AT::t_x_array_randomread x;
  typename AT::t_f_array f;
  typename AT::t_int_1d_randomread type;
  typename AT::t_efloat_1d uCG, uCGnew;
  typename AT::t_float_2d dvector;

  DAT::tdual_efloat_1d k_eatom;
  DAT::tdual_virial_array k_vatom;
  DAT::t_efloat_1d d_eatom;
  DAT::t_virial_array d_vatom;

  DAT::tdual_int_scalar k_error_flag;

  typename AT::t_neighbors_2d d_neighbors;
  typename AT::t_int_1d_randomread d_ilist;
  typename AT::t_int_1d_randomread d_numneigh;

  PairExp6ParamDataTypeKokkos<DeviceType> PairExp6ParamData;

  void allocate();
  DAT::tdual_int_1d k_mol2param;               // mapping from molecule to parameters
  typename AT::t_int_1d_randomread d_mol2param;

  typedef Kokkos::DualView<Param*,Kokkos::LayoutRight,DeviceType> tdual_param_1d;
  typedef typename tdual_param_1d::t_dev_const_randomread t_param_1d_randomread;

  tdual_param_1d k_params;                // parameter set for an I-J-K interaction
  t_param_1d_randomread d_params;                // parameter set for an I-J-K interaction

  typename ArrayTypes<DeviceType>::tdual_ffloat_2d k_cutsq;
  typename ArrayTypes<DeviceType>::t_ffloat_2d d_cutsq;

  void read_file(char *);
  void setup();

  KOKKOS_INLINE_FUNCTION
  void getParamsEXP6(int, double &, double &, double &, double &, double &, double &, double &, double &, double &, double &, double &, double &, double &, double &, double &, double &) const;

  KOKKOS_INLINE_FUNCTION
  double func_rin(const double &) const;

  KOKKOS_INLINE_FUNCTION
  double expValue(const double) const;

  friend void pair_virial_fdotr_compute<PairExp6rxKokkos>(PairExp6rxKokkos*);
};

}

#endif
#endif

/* ERROR/WARNING messages:

E:  alpha_ij is 6.0 in pair exp6

Self-explanatory

E: Illegal ... command

Self-explanatory.  Check the input script syntax and compare to the
documentation for the command.  You can use -echo screen as a
command-line option when running LAMMPS to see the offending line.

E: Incorrect args for pair coefficients

Self-explanatory.  Check the input script or data file.

E: PairExp6rxKokkos requires a fix rx command

The fix rx command must come before the pair style command in the input file

E:  There are no rx species specified

There must be at least one species specified through the fix rx command

E:  Site1 name not recognized in pair coefficients

The site1 keyword does not match the species keywords specified throug the fix rx command

E: All pair coeffs are not set

All pair coefficients must be set in the data file or by the
pair_coeff command before running a simulation.

E:  Cannot open exp6/rx potential file %s

Self-explanatory

E:  Incorrect format in exp6/rx potential file

Self-explanatory

E:  Illegal exp6/rx parameters.  Rm and Epsilon must be greater than zero.  Alpha cannot be negative.

Self-explanatory

E:  Illegal exp6/rx parameters.  Interaction potential does not exist.

Self-explanatory

E:  Potential file has duplicate entry.

Self-explanatory

E:  The number of molecules in CG particle is less than 1e-8.

Self-explanatory.  Check the species concentrations have been properly set
and check the reaction kinetic solver parameters in fix rx to more for
sufficient accuracy.


*/