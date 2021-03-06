/**
 * @file DoubleExponential.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "DoubleExponential.h"

#include <boost/math/distributions/lognormal.hpp>
#include <cmath>

#include "Age/AgeLengths/AgeLength.h"
#include "Common/Model/Model.h"
#include "Common/TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Explicit Constructor
 */
DoubleExponential::DoubleExponential(Model* model)
: Selectivity(model) {

  parameters_.Bind<Double>(PARAM_X0, &x0_, "X0", "");
  parameters_.Bind<Double>(PARAM_X1, &x1_, "X1", "");
  parameters_.Bind<Double>(PARAM_X2, &x2_, "X2", "");
  parameters_.Bind<Double>(PARAM_Y0, &y0_, "Y0", "");
  parameters_.Bind<Double>(PARAM_Y1, &y1_, "Y1", "");
  parameters_.Bind<Double>(PARAM_Y2, &y2_, "Y2", "");
  parameters_.Bind<Double>(PARAM_ALPHA, &alpha_, "Alpha", "", 1.0);

  RegisterAsAddressable(PARAM_X0, &x0_);
  RegisterAsAddressable(PARAM_Y0, &y0_);
  RegisterAsAddressable(PARAM_Y1, &y1_);
  RegisterAsAddressable(PARAM_Y2, &y2_);
  RegisterAsAddressable(PARAM_ALPHA, &alpha_);
}

/**
 * Validate this selectivity. This will load the
 * values that were passed in from the configuration
 * file and assign them to the local variables.
 *
 * We'll then do some basic checks on the local
 * variables to ensure they are within the business
 * rules for the model.
 */
void DoubleExponential::DoValidate() {
  // Param: x0, x1, x2 - Check that x1 is between x0 and x2
  if (x0_ < x1_ || x0_ > x2_)
    LOG_ERROR_P(PARAM_X0) << "x0 ( " << AS_DOUBLE(x0_) << ") must be between x1 (" << AS_DOUBLE(x1_) << ") and x2 (" << AS_DOUBLE(x2_) << ")";

  // Param: y0, y1, y2
  if (y0_ < 0.0)
    LOG_ERROR_P(PARAM_Y0) << ": y0 (" << AS_DOUBLE(y0_) << ") is less than 0.0";
  if (y1_ < 0.0)
    LOG_ERROR_P(PARAM_Y1) << ": y1 (" << AS_DOUBLE(y1_) << ") is less than 0.0";
  if (y2_ < 0.0)
    LOG_ERROR_P(PARAM_Y2) << ": y2 (" << AS_DOUBLE(y2_) << ") is less than 0.0";

  // Param: alpha
  if (alpha_ <= 0.0)
    LOG_ERROR_P(PARAM_ALPHA) << ": alpha (" << AS_DOUBLE(alpha_) << ") is less than or equal to 0.0";
}

/**
 * Reset this selectivity so it's ready for the next execution
 * phase in the model.
 *
 * This method will rebuild the cache of selectivity values
 * for each age in the model.
 */
void DoubleExponential::RebuildCache() {
  if (model_->partition_type() == PartitionType::kAge) {
    for (unsigned age = model_->min_age(); age <= model_->max_age(); ++age) {
      if ((Double)age <= x0_) {
        values_[age - age_index_] = alpha_ * y0_ * pow((y1_ / y0_), ((Double)age - x0_)/(x1_ - x0_));
      } else if ((Double)age > x0_ && (Double)age <= x2_) {
        values_[age - age_index_] = alpha_ * y0_ * pow((y2_ / y0_), ((Double)age - x0_)/(x2_ - x0_));
      } else {
        values_[age - age_index_] = y2_;
      }
    }
  } else if (model_->partition_type() == PartitionType::kLength) {
    vector<unsigned> length_bins = model_->length_bins();
    for (unsigned length_bin_index = 0; length_bin_index < length_bins.size(); ++length_bin_index) {
      if ((Double)length_bins[length_bin_index] <= x0_) {
        length_values_[length_bin_index] = alpha_ * y0_ * pow((y1_ / y0_), ((Double)length_bins[length_bin_index] - x0_)/(x1_ - x0_));
      } else if ((Double)length_bins[length_bin_index] > x0_ && (Double)length_bins[length_bin_index] <= x2_) {
        length_values_[length_bin_index] = alpha_ * y0_ * pow((y2_ / y0_), ((Double)length_bins[length_bin_index] - x0_)/(x2_ - x0_));
      } else {
        length_values_[length_bin_index] = y2_;
      }
    }
  }
}

/**
 * GetLengthBasedResult function
 *
 * @param age
 * @param age_length AgeLength pointer
 * @return Double selectivity for an age based on age length distribution_label
 */
Double DoubleExponential::GetLengthBasedResult(unsigned age, AgeLength* age_length) {
  unsigned year = model_->current_year();
  unsigned time_step = model_->managers().time_step()->current_time_step();
  Double cv = age_length->cv(year, time_step, age);

  Double mean = age_length->mean_length(time_step, age);
  string dist = age_length->distribution_label();

  if (dist == PARAM_NONE || n_quant_ <= 1) {
    // no distribution_label just use the mu from age_length
    Double val;
    if ((Double)mean <= x0_)
      val = alpha_ * y0_ * pow((y1_ / y0_), ((Double)mean - x0_)/(x1_ - x0_));
    else
      val = alpha_ * y0_ * pow((y2_ / y0_), ((Double)mean - x0_)/(x2_ - x0_));
    return val;

  } else if (dist == PARAM_NORMAL) {

    Double sigma = cv * mean;
    Double size = 0.0;
    Double total = 0.0;

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mean + sigma * quantiles_at_[j];

      if ((Double)size <= x0_)
        total += alpha_ * y0_ * pow((y1_ / y0_), ((Double)size - x0_)/(x1_ - x0_));
      else
        total += alpha_ * y0_ * pow((y2_ / y0_), ((Double)size - x0_)/(x2_ - x0_));
    }
    return total / n_quant_;

  } else if (dist == PARAM_LOGNORMAL) {
    // convert paramters to log space
    Double sigma = sqrt(log(1 + cv * cv));
    Double mu = log(mean) - sigma * sigma * 0.5;
    Double size = 0.0;
    Double total = 0.0;
    boost::math::lognormal dist{AS_DOUBLE(mu), AS_DOUBLE(sigma)};

    for (unsigned j = 0; j < n_quant_; ++j) {
      size = mu + sigma * quantile(dist, AS_DOUBLE(quantiles_[j]));

      if ((Double)size <= x0_)
        total += alpha_ * y0_ * pow((y1_ / y0_), ((Double)size - x0_)/(x1_ - x0_));
      else
        total += alpha_ * y0_ * pow((y2_ / y0_), ((Double)size - x0_)/(x2_ - x0_));
    }
    return total / n_quant_;
  }
  LOG_CODE_ERROR() << "dist is invalid " << dist;
  return 0;
}
} /* namespace selectivities */
} /* namespace niwa */
