#ifndef KLIB_LOOKUPTABLE_HPP
#define KLIB_LOOKUPTABLE_HPP

#include <cstdint>

namespace klib {
    /**
     * @brief Lookuptable with a size and a type
     * 
     * @tparam N 
     * @tparam T 
     */
    template <uint32_t N, typename T>
    class lookuptable {
        private:
            T table[N] = {};

        public:
            constexpr static uint32_t size = N;

            template <typename F>
            constexpr lookuptable(const F function) {
                for(uint32_t i = 0; i < N; ++i){
                    table[i] = function(i);
                }
            }

            constexpr T get(const uint32_t index) const {
                return table[index];
            }

            constexpr const T *const begin() const {
                return table;
            } 
    };
}

#endif