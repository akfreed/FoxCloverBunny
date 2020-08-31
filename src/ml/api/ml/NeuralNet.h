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

#pragma once

#include <array>

#include <Eigen/Dense>

namespace fcb { namespace ml {

//! A neural network with 1 hidden layer.
class NeuralNet
{
public:
    // static consts
    static unsigned constexpr NUM_INPUTS  = 4;
    static unsigned constexpr NUM_OUTPUTS = 2;

    //! Manages indexing operations to avoid the bias node.
    struct InputHelper
    {
        InputHelper() { m_input(0) = 1; }  // 1 for bias.
        float  operator()(Eigen::Index index) const { return m_input(index + 1); }
        float& operator()(Eigen::Index index)       { return m_input(index + 1); }
        Eigen::RowVectorXf m_input = Eigen::RowVectorXf(NeuralNet::NUM_INPUTS + 1);
    };

    // public typedefs
    using InputType         = InputHelper;
    using OutputType        = Eigen::Matrix<float, 1, NUM_OUTPUTS>;
    using WeightsType       = Eigen::MatrixXf;
    using WeightsCollection = std::array<WeightsType, 2>;

    // public functions
    NeuralNet() = default;
    explicit NeuralNet(unsigned const numHidden);

    WeightsCollection const& Weights() const;
    WeightsCollection& Weights();

    void FeedForward(InputType const& inputs, OutputType& out_outputs) const;
    static void Crossover(NeuralNet const& m, NeuralNet const& f, NeuralNet& out_c);

private:
    // private functions
    WeightsCollection generateWeightsRandom() const;
    WeightsCollection generateWeightsZero() const;

    // private data
    unsigned          m_numHidden = 6;
    WeightsCollection m_weights   = generateWeightsRandom();
};

} }
