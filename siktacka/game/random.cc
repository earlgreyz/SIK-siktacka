#include "random.h"

using namespace siktacka;

Random::Random(rand_t seed) noexcept : last_rand(seed) {

}

rand_t Random::next() noexcept {
    rand_t rand = last_rand;
    last_rand = static_cast<rand_t>((last_rand * RANDOM_MUL) % RANDOM_MOD);
    return rand;
}
