/**
 * @file Penalty.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 15/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * A penalty is an object that when trigger applies a score
 * to the objective function. Certain processes and objects
 * will trigger a penalty when extreme conditions are met
 * that are not encouraging the minimiser to find the minimum.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PENALTY_H_
#define PENALTY_H_

// Headers
#include "BaseClasses/Object.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {

using isam::utilities::Double;

/**
 * Class definition
 */
class Penalty : public isam::base::Object {
public:
  // Methods
  Penalty();
  virtual                     ~Penalty() = default;
  void                        Validate();
  void                        Build() { DoBuild(); };
  void                        Reset() {};
  virtual Double              GetScore() { return 0.0; }

  // accessors
  bool                        has_score() const { return has_score_; }

protected:
  // methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;


  // members
  bool                        has_score_ = true;
};

// Typedef
typedef boost::shared_ptr<isam::Penalty> PenaltyPtr;

} /* namespace isam */
#endif /* PENALTY_H_ */
