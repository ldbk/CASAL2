/**
 * @file Data.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGELENGTHS_DATA_H_
#define AGELENGTHS_DATA_H_

// headers
#include "AgeLengths/AgeLength.h"
#include "SizeWeights/SizeWeight.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * Class definition
 */
class Data : public AgeLength {
public:
  // methods
  Data();
  virtual                     ~Data() = default;
  void                        DoValidate() override final {};
  void                        DoBuild() override final;
  void                        DoReset() override final {};
  Double                      mean_length(unsigned year, unsigned age) override final;
  Double                      mean_weight(unsigned year, unsigned age) override final;

private:
  // methods
  void                        FillExternalGaps();
  void                        FillInternalGaps();

  // members
  parameters::TablePtr          data_table_;
  string                        external_gaps_;
  string                        internal_gaps_;
  map<unsigned, vector<Double>> data_by_year_;
  vector<Double>                means_;
  Double                        cv_;
  string                        distribution_;
  string                        size_weight_label_;
  SizeWeightPtr                 size_weight_;
};

} /* namespace agelengths */
} /* namespace niwa */

#endif /* AGELENGTHS_DATA_H_ */