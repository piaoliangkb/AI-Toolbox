#include <AIToolbox/MDP/DynaQ.hpp>

#include <chrono>

#include <cassert>

namespace AIToolbox {
    namespace MDP {
        DynaQ::DynaQ(size_t s, size_t a, double discount, unsigned n) : DynaQInterface(s, a, discount, n),
                                                                        rand_(std::chrono::system_clock::now().time_since_epoch().count()) 
        {
            visitedStatesActionsInserter_.reserve(S*A);
            visitedStatesActionsSampler_.reserve(S*A);
        }

        void DynaQ::updateSamplingQueue(size_t s, size_t, size_t a, double ) {
            // O(1) insertion...
            auto result = visitedStatesActionsInserter_.insert(std::make_pair(s,a)); 
            if ( std::get<1>(result) )
                visitedStatesActionsSampler_.push_back(*std::get<0>(result));
        }

        void DynaQ::batchUpdateQ(const RLModel & m, QFunction * q) {
            assert(q != nullptr);

            std::uniform_int_distribution<size_t> sampleDistribution_(0, visitedStatesActionsSampler_.size()-1);

            for ( unsigned i = 0; i < N; i++ ) {
                size_t s, s1, a;
                double rew;
                // O(1) sampling...
                std::tie(s,a) = visitedStatesActionsSampler_[sampleDistribution_(rand_)];

                std::tie(s1, rew) = m.sample(s, a);
                updateQ(s, s1, a, rew, q);
            }
        } 
    }
}
