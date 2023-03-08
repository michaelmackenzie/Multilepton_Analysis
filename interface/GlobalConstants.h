#ifndef __GLOBALCONSTANTS__
#define __GLOBALCONSTANTS__

//ROOT includes
#include "Math/Math.h"
#include "Math/GenVector/DisplacementVector3D.h"
#include "Math/GenVector/Cartesian3D.h"
#include "Math/Vector4D.h"
#include "Math/Vector4Dfwd.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
#include "Math/LorentzVector.h"

namespace CLFV {
  enum {kMaxSystematics = 200};

  const double TAUMASS = 1.77686;
  const double MUONMASS = 0.105658;
  const double ELECMASS = 0.511e-3;
  const double HIGGSMASS = 125.;
  const double ZMASS = 91.1876;
  const double WMASS = 80.379;

  typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>> PtEtaPhiMVector;
  typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double>> Vector3;

}
#endif
