/**
 * @file UniformLog.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ESTIMATES_CREATORS_UNIFORMLOG_H_
#define ESTIMATES_CREATORS_UNIFORMLOG_H_

// headers
#include "Estimates/Creators/Creator.h"

// namespaces
namespace isam {
namespace estimates {
namespace creators {

/**
 *
 */
class UniformLog : public estimates::Creator {
public:
  UniformLog();
  virtual                     ~UniformLog() = default;
  void                        DoCopyParameters(isam::EstimatePtr estimate, unsigned index) override final { };
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace isam */

#endif /* UNIFORMLOG_H_ */