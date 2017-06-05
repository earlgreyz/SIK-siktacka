#ifndef SIK_SIKTACKA_RANDOM_H
#define SIK_SIKTACKA_RANDOM_H

#include "../types.h"


namespace siktacka {
    constexpr std::uint64_t RANDOM_MUL = 279470273u;
    constexpr std::uint64_t RANDOM_MOD = 4294967291u;

    /**
     * Pseudo random number generator.
     */
    class Random {
    private:
        /// Last generated random value.
        rand_t last_rand;
    public:
        /**
         * Constructs new random generator.
         * @param seed random generator seed.
         */
        Random(rand_t seed) noexcept;
        /**
         * Calculates next random value.
         * @return pseudo random number.
         */
        rand_t next() noexcept;
    };
}


#endif
