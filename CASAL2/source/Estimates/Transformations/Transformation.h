/**
 * @file Transformation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a modifier that can be applied to an estimate. This allows
 * the estimate to be modified before it's sent off to the minimiser.
 */
#ifndef ESTIMATES_TRANSFORMATION_H_
#define ESTIMATES_TRANSFORMATION_H_

// headers
#include <string>

#include "BaseClasses/Object.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace estimates {

using niwa::utilities::Double;
using std::string;

/**
 * class definition
 */
class Transformation {
public:
  // methods
  Transformation() = default;
  virtual                     ~Transformation() = default;
  virtual void                Validate() { };
  virtual Double              Transform(Double initial_value) = 0;
  virtual Double              Untransform(Double initial_value) = 0;
//  virtual Double              Jocabian(Double initial_value) = 0;
  void                        SetParameters(vector<string> parameters) { parameters_ = parameters; }

protected:
  // members
  vector<string>              parameters_;
};
} /* namespace estimates */
} /* namespace niwa */

#endif /* ESTIMATES_TRANSFORMATION_H_ */