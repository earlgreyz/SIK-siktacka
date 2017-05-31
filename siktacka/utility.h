#ifndef SIK_SIKTACKA_UTILITY_H
#define SIK_SIKTACKA_UTILITY_H

#include <type_traits>

/**
 * Performs pointer cast disqualifying const classifier on the base type.
 * Safer version of standard C-style cast.
 * @tparam T return pointer type
 * @tparam U value type
 * @param value
 * @return pointer to the value of a different type.
 */
template <typename T, typename U>
inline T ptr_cast(U* value) noexcept {
    using P = typename std::remove_pointer_t<T>;
    using C = typename std::add_pointer_t<std::add_const_t<P>>;
    return const_cast<T>(reinterpret_cast<C>(value));
};

#endif
