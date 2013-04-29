/**
 * @file DoubleNormal.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "DoubleNormal.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace isam {

using ::testing::Return;

/**
 * Test the results of our selectivity are correct
 */
TEST(Selectivities, DoubleNormal) {
  boost::shared_ptr<MockModel> model = boost::shared_ptr<MockModel>(new MockModel);
  EXPECT_CALL(*model.get(), min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(*model.get(), max_age()).WillRepeatedly(Return(20));

  isam::selectivities::DoubleNormal double_normal(model);

  double_normal.parameters().Add(PARAM_LABEL, "unit_test_double_normal", __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_MU, "15",  __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_L, "1",  __FILE__, __LINE__);
  double_normal.parameters().Add(PARAM_SIGMA_R, "10",  __FILE__, __LINE__);
  double_normal.Validate();
  double_normal.Build();

  EXPECT_DOUBLE_EQ(0.0,                       double_normal.GetResult(9)); // Below model->min_age()
  EXPECT_DOUBLE_EQ(2.9802322387695312e-008,   double_normal.GetResult(10)); // At model->min_age()
  EXPECT_DOUBLE_EQ(1.52587890625e-005,        double_normal.GetResult(11));
  EXPECT_DOUBLE_EQ(0.001953125,               double_normal.GetResult(12));
  EXPECT_DOUBLE_EQ(0.0625,                    double_normal.GetResult(13));
  EXPECT_DOUBLE_EQ(0.5,                       double_normal.GetResult(14));
  EXPECT_DOUBLE_EQ(1.0,                       double_normal.GetResult(15));
  EXPECT_DOUBLE_EQ(0.99309249543703593,       double_normal.GetResult(16));
  EXPECT_DOUBLE_EQ(0.97265494741228553,       double_normal.GetResult(17));
  EXPECT_DOUBLE_EQ(0.9395227492140118,        double_normal.GetResult(18));
  EXPECT_DOUBLE_EQ(0.89502507092797245,       double_normal.GetResult(19));
  EXPECT_DOUBLE_EQ(0.8408964152537145,        double_normal.GetResult(20)); // At model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       double_normal.GetResult(21)); // This is above model->max_age()
  EXPECT_DOUBLE_EQ(0.0,                       double_normal.GetResult(22));
  EXPECT_DOUBLE_EQ(0.0,                       double_normal.GetResult(23));
}

}

#endif /* ifdef TESTMODE */