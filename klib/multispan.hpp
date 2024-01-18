#ifndef KLIB_MULTISPAN_HPP
#define KLIB_MULTISPAN_HPP

#include <cstdint>
#include <span>
#include <array>

namespace klib {
    /**
     * @brief Non owning wrapper to map two std::span in a single array
     * 
     * @tparam T 
     */
    template <typename T> 
    class multispan {
    protected:
        // amount of items in the buffer
        constexpr static uint32_t Amount = 2;

        // the two internal spans
        std::array<std::span<T>, Amount> storage;

    public:
        // member types (no iterators as it is not a continuous 
        // block of memory)
        using element_type        = T;
        using value_type          = std::remove_cv_t<T>;
        using size_type           = size_t;
        using difference_type     = ptrdiff_t;
        using pointer             = T*;
        using const_pointer       = const T*;
        using reference           = element_type&;
        using const_reference     = const element_type&;

        /**
         * @brief Construct a new multispan object using a initializer list, raw 
         * array(not a pointer) or std::array
         * 
         * for raw pointers {ptr, size} should be used as a parameter
         * 
         * @param first 
         * @param second 
         */
        template <typename... E> 
        constexpr multispan(const std::span<T>& first, const std::span<T>& second):
            storage{first, second}
        {}

        /**
         * @brief Copy constructor for the multispan
         * 
         */
        constexpr multispan(const multispan&) noexcept = default;

        /**
         * @brief Construct a multispan from a different multispan
         * 
         * @tparam G 
         */
        template <typename G>
        constexpr multispan(const multispan<G>& other) noexcept:
            storage(other.store())
        {}

        /**
         * @brief Operator to get data from the two spans
         * 
         * @param index 
         * @return constexpr T& 
         */
        constexpr reference operator[](const uint32_t index) const {
            uint32_t offset = 0;

            // check where we need to point to
            for (auto& s: storage) {
                const auto x = index - offset;

                if (x < s.size()) {
                    return s[x];
                }
                
                offset += s.size();
            }

            // we are out of bounds. This is undefined behaviour
            return storage[Amount - 1].end();
        }

        /**
         * @brief Returns the total size of both spans
         * 
         * @return uint32_t 
         */
        constexpr uint32_t size() const {
            uint32_t count = 0;

            for (auto& s: storage) {
                count += s.size();
            }

            return count;
        }

        /**
         * @brief Returns if the span is empty
         * 
         * @return uint32_t 
         */
        constexpr uint32_t empty() const {
            return size() == 0;
        }

        /**
         * @brief Returns the amount of bytes the combined spans use
         * 
         * @return constexpr uint32_t 
         */
        constexpr uint32_t size_bytes() const {
            uint32_t count = 0;

            for (auto& s: storage) {
                count += s.size_bytes();
            }

            return count;
        }

    public:
        /**
         * @brief Get a reference to the storage
         * 
         * @details needed for the copy constructor
         * 
         * @return constexpr const std::span<T>& 
         */
        constexpr const std::array<std::span<T>, Amount>& store() const {
            return storage;
        }
    };
}

#endif
