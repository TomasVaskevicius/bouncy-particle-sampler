#include <gtest/gtest.h>

#include <memory>

#include <Eigen/Core>

#include "mcmc/pdmp_builder_base.h"
#include "core/policies/linear_flow.h"
#include "core/policies/poisson_process.h"
#include "core/state_space/position_and_velocity_state.h"

using namespace std;
using namespace pdmp;
using namespace pdmp::dependencies_graph;
using namespace pdmp::mcmc;

using Flow = LinearFlow;
using State = DynamicPositionAndVelocityState<double>;
using RealVector = State::DynamicRealVector;

const int kPdmpDimension = 4;

struct PdmpBuilder : PdmpBuilderBase<State, Flow> {

 public:

  PdmpBuilder() : PdmpBuilderBase<State, Flow>(kPdmpDimension) {
  }

  auto getVariableNodes() {
    return this->variableNodes_;
  }

  auto getFactorNodes() {
    return this->factorNodes_;
  }

  auto getMarkovKernelNodes() {
    return this->markovKernelNodes_;
  }
};

TEST(PdmpBuilderBaseTests, TestAddingMarkovKernelNodes) {
  PdmpBuilder builder;
  // A Markov kernel that sets the first coordinate to 0.
  auto kernel = [] (const auto& subvector) {
    auto newSubvector = subvector;
    newSubvector(0) = 0.0;
    return newSubvector;
  };
  std::vector<int> neededVariables{0, 1};
  std::vector<int> modifiableVariables{0};
  builder.addMarkovKernelNode(neededVariables, modifiableVariables, kernel);
  State state{
    (RealVector(2) << 1, 2).finished(), (RealVector(2) << 3, 4).finished()};
  State expectedState{
    (RealVector(2) << 0, 2).finished(), (RealVector(2) << 3, 4).finished()};
  EXPECT_TRUE(builder.getFactorNodes().size() == 0);
  EXPECT_TRUE(builder.getMarkovKernelNodes().size() == 1);
  EXPECT_TRUE(expectedState == builder.getMarkovKernelNodes()[0]->jump(state));
  EXPECT_TRUE(builder.getMarkovKernelNodes()[0]->dependentVariableIds ==
    vector<int>{0});
}

TEST(PdmpBuilderBaseTests, TestAddingFactorNodes) {
  PdmpBuilder builder;
  // A Poisson process strategy lambda.
  auto ppStrategy = [] (const auto& subvector, auto&, auto&) {
    shared_ptr<PoissonProcessResultBase> result = make_shared<
      PoissonProcessResult<>>(subvector.squaredNorm());
    return result;
  };
  std::vector<int> neededVariables{0, 2};
  builder.addFactorNode(neededVariables, ppStrategy);
  State state{
    (RealVector(2) << 1, 2).finished(), (RealVector(2) << 3, 4).finished()};
  EXPECT_TRUE(builder.getFactorNodes().size() == 1);
  EXPECT_TRUE(builder.getMarkovKernelNodes().size() == 0);
  EXPECT_DOUBLE_EQ(
    10.0, builder.getFactorNodes()[0]->getPoissonProcessResult(state)->time);
  EXPECT_TRUE(
    builder.getVariableNodes()[0]->dependentFactorIds == vector<int>{0}
    && builder.getVariableNodes()[1]->dependentFactorIds.size() == 0
    && builder.getVariableNodes()[2]->dependentFactorIds == vector<int>{0}
    && builder.getVariableNodes()[3]->dependentFactorIds.size() == 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

