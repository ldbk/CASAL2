/**
 * @file BaseObject.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is the highest level class in our object tree. All objects
 * that have to be created from a configuration block need to inherit from this class.
 *
 * This class creates access to some central global objects within the system
 * including: GlobalConfiguration, TheModel, ParameterList etc
 *
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef BASEOBJECT_H_
#define BASEOBJECT_H_

// Headers
#include <string>

#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../Translations/Translations.h"

// Namespaces
using std::string;

namespace iSAM {
namespace Base {

/**
 * Class Definition
 */
class Object {
public:
  // Methods
  Object();
  virtual                     ~Object();
  string                      getLabel() { return sLabel; }

protected:
  // Members
  string                      sLabel;
  GlobalConfigurationPtr      pGlobalConfig;
};

} /* namespace Base */
} /* namespace iSAM */
#endif /* BASEOBJECT_H_ */