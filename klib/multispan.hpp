#ifndef KLIB_MULTISPAN_HPP
#define KLIB_MULTISPAN_HPP

#include <cstdint>
#include <span>

namespace klib::detail {
    /**
     * @brief Helper concept to check if two types match
     * 
     * @tparam T 
     * @tparam G 
     */
    template<typename T, typename G>
    concept same_as = std::same_as<std::remove_reference_t<T>, std::remove_reference_t<G>>;

    /**
     * @brief Concept to check if we have a size function
     * 
     * @tparam T 
     */
    template <class T, typename G>
    concept is_span_like = requires(T a) {
        // check if we have a size function
        // a.size();
        a.size();

        // check if we have a operator[] and the 
        // return matches the expected type
        { a[0] } -> same_as<G>;
    };
}

namespace klib {
    /**
     * @brief Non owning wrapper to map two std::span in a single array
     * 
     * @tparam T 
     */
    template <typename T> 
    class multispan {
    protected:
        // the two internal spans
        std::span<T> _first;
        std::span<T> _second;

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
        constexpr multispan(const std::span<T>& first, const std::span<T>& second) noexcept:
            _first(first), _second(second)
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
            _first(other.first()), _second(other.second())
        {}

        /**
         * @brief Operator to get data from the two spans
         * 
         * @param index 
         * @return constexpr T& 
         */
        constexpr reference operator[](const uint32_t index) const {
            // check what pointer we should use
            if (index > _first.size()) {
                return _second[index - _first.size()];
            }

            // return the first pointer
            return _first[index];
        }

        /**
         * @brief Returns the total size of both spans
         * 
         * @return uint32_t 
         */
        constexpr uint32_t size() const {
            return _first.size() + _second.size();
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
            return _first.size_bytes() + _second.size_bytes();
        }

    public:
        /**
         * @brief Get a reference to the first std::span
         * 
         * @details needed for the copy constructor
         * 
         * @return constexpr const std::span<T>& 
         */
        constexpr const std::span<T>& first() const {
            return _first;
        }

        /**
         * @brief Get a reference to the first std::span
         * 
         * @details needed for the copy constructor
         * 
         * @return constexpr const std::span<T>& 
         */
        constexpr const std::span<T>& second() const {
            return _second;
        }
    };
}

#endif
