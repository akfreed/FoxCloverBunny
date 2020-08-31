// ==================================================================
// Copyright 2020 Alexander K. Freed
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ==================================================================

// Language: ISO C++17

#include "ml/NeuralNet.h"

#include "util/Util.h"

#include <cassert>

namespace fcb { namespace ml {


//! Anonymous namespace for local functions.
namespace {

    //! Force the sigmoid function to be inlined by writing it as a lambda.
    auto const sigmoid = [](float const z) -> float { return (1.0f / (1.0f + expf(-z))); };

    //! Combine the weights from two matrices into one.
    //! Copy the weights from one, occasionally switching to the other.
    //! Uses the array view of the matrices.
    //! @param[in]  m     Parent one.
    //! @param[in]  f     Parent two.
    //! @param[out] out_c A matrix to write the results to. Should be the same dimensions.
    template <typename Derived>
    static void crossoverMatrix(Eigen::MatrixBase<Derived> const& m, Eigen::MatrixBase<Derived> const& f, Eigen::MatrixBase<Derived>& out_c)
    {
        double constexpr crossoverRate = 0.7;

        // sanity check. Matrices should have the same shape... but they at least need to have the same number of elements.
        assert(m.array().size() == f.array().size() && f.array().size() == out_c.array().size());

        std::uniform_real_distribution<> distReal(0, 1);
        std::uniform_int_distribution<Eigen::Index> distInt(0, out_c.size());

        // Generate some crossover points.
        std::vector<Eigen::Index> crossoverPoints({ 0, out_c.size() });
        while (distReal(util::rng()) < crossoverRate)
            crossoverPoints.push_back(distInt(util::rng()));
        std::sort(crossoverPoints.begin(), crossoverPoints.end());

        // 50% chance to pick either parent to start.
        bool useM = (distReal(util::rng()) < .5);

        for (size_t i = 0; i < crossoverPoints.size() - 1; ++i)
        {
            // Pick a parent and make a pointer to the beginning of its range.
            float const* const parentBegin = (useM ? &m(0, 0) : &f(0, 0));
            // Switch parent for next time.
            useM = !useM;

            // Setup transcription range given indexes.
            float const* const first = crossoverPoints[i]     + parentBegin;
            float const* const last  = crossoverPoints[i + 1] + parentBegin;
            float* const cIter       = crossoverPoints[i]     + &out_c(0, 0);

            // Transcribe.
            std::copy(first, last, cIter);
        }
    }

    //! Chance to mutate a matrix.
    //! To mutate, add a value between -1 and 1 to 0 or more random weights.
    //! Uses the array view of the matrix.
    //! @param[in/out] c The matrix with a chance to mutate.
    template <typename Derived>
    static void mutateMatrix(Eigen::MatrixBase<Derived>& c)
    {
        double constexpr mutationRate = .15;

        std::uniform_real_distribution<> distRoll(0, 1);
        std::uniform_real_distribution<float> distMutation(-.5f, .5f);

        while (distRoll(util::rng()) < mutationRate)
        {
            //size_t const mutationIndex = util::rng()() % c.size();  // for mutating only 1 weight (instead of a whole column)
            //c.array()(mutationIndex) += distMutation(util::rng());
            size_t const mutationColumn = util::rng()() % c.cols();
            c.col(mutationColumn) = c.col(mutationColumn).unaryExpr([&distMutation](float const in) -> float { return in + distMutation(util::rng()); });
        }
    }

}  // Anonymous namespace.


//! Argument Constructor.
//! Sets the number of neurons in the hidden layer.
//! Initializes the weights.
//! @param[in] numHidden The number of nodes to put in the hidden layer.
NeuralNet::NeuralNet(unsigned const numHidden)
    : m_numHidden(numHidden)
{ }

//! Create weights as a collection of matrices.
//! Initializes the weights and bias randomly.
//! Didn't want to use Eigen's setRandom() function because it uses old C++ rand.
//! @return A set of new matrices of randomly generated weights.
NeuralNet::WeightsCollection NeuralNet::generateWeightsRandom() const
{
    std::uniform_real_distribution<float> distribution(-0.8f, 0.8f);
    WeightsCollection weights;
    // Weights for input->hidden.
    weights[0] = WeightsType::NullaryExpr(static_cast<size_t>(NUM_INPUTS)  + 1, m_numHidden, [&distribution]() { return distribution(util::rng()); });
    // Weights for hidden->output.
    weights[1] = WeightsType::NullaryExpr(static_cast<size_t>(m_numHidden) + 1, NUM_OUTPUTS, [&distribution]() { return distribution(util::rng()); });
    return weights;
}

//! Create weights as a collection of matrices.
//! Initializes the weights and bias to 0.
//! @return A set of new matrices of randomly generated weights.
NeuralNet::WeightsCollection NeuralNet::generateWeightsZero() const
{
    WeightsCollection weights;
    // weights for input->hidden.
    weights[0] = WeightsType::Zero(static_cast<size_t>(NUM_INPUTS)  + 1, m_numHidden);
    // weights for hidden->output.
    weights[1] = WeightsType::Zero(static_cast<size_t>(m_numHidden) + 1, NUM_OUTPUTS);
    return weights;
}

//! @return The weights for this neural network.
NeuralNet::WeightsCollection const& NeuralNet::Weights() const
{
    // The individual elements are const protected if the weights collection is an std::array.
    return m_weights;
}
//! @return The weights for this neural network.
NeuralNet::WeightsCollection& NeuralNet::Weights()
{
    return m_weights;
}

//! Feed the input forward and return the outputs.
//! @param[in]  inputs  A vector of input values.
//! @param[out] outputs A vector to hold output results.
void NeuralNet::FeedForward(InputType const& inputs, OutputType& out_outputs) const
{
    // The bias must be set to 1.
    assert(inputs.m_input(0) == 1);
    // Create a place to hold the activation of input->hidden layer.
    Eigen::RowVectorXf hiddenActivation(m_numHidden + 1);
    // The bias is the first element.
    hiddenActivation(0) = 1;
    // Need to map the activation result onto the rest of the holding space.
    {
        Eigen::Map<Eigen::RowVectorXf> activationMap(&hiddenActivation(1), m_numHidden);
        activationMap = (inputs.m_input * m_weights[0]).unaryExpr(sigmoid);
    }

    // Activate hidden->output layer.
    out_outputs = (hiddenActivation * m_weights[1]).unaryExpr(sigmoid);
}

//! Combine the weights from two neural nets to make a new one.
//! @param[in]  m     Parent one.
//! @param[in]  f     Parent two. Can be the same.
//! @param[out] out_c A neural net to write the results to.
void NeuralNet::Crossover(NeuralNet const& m, NeuralNet const& f, NeuralNet& out_c)
{
    crossoverMatrix(m.m_weights[0], f.m_weights[0], out_c.m_weights[0]);
    crossoverMatrix(m.m_weights[1], f.m_weights[1], out_c.m_weights[1]);
    mutateMatrix(out_c.m_weights[0]);
    mutateMatrix(out_c.m_weights[1]);
}


} }
