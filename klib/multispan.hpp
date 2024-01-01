#ifndef KLIB_MULTISPAN_HPP
#define KLIB_MULTISPAN_HPP

#include <cstdint>
#include <span>

namespace klib::detail {
    /**
     * @brief Multispan base for checking if we have a multispan 
     * with type checking
     * 
     * @tparam T 
     */
    template <typename T>
    class multispan_base_t {};
}

namespace klib {
    /**
     * @brief Returns if the template is a multispan a span with a specific type
     * 
     * @tparam T 
     * @tparam G 
     */
    template <typename T, typename G>
    concept is_span_type = (
        std::is_same_v<std::span<T>, G> || 
        std::is_base_of<detail::multispan_base_t<T>, G>::value
    );

    /**
     * @brief Returns if the template is a multispan a span with a specific type 
     * (checks both for the normal type and the const type)
     * 
     * @tparam T 
     * @tparam G 
     */
    template <typename T, typename G>
    concept is_span_type_c = (
        std::is_same_v<std::span<T>, G> || std::is_same_v<std::span<const T>, G> ||
        std::is_base_of<detail::multispan_base_t<T>, G>::value || 
        std::is_base_of<detail::multispan_base_t<const T>, G>::value
    );

    /**
     * @brief Non owning wrapper to map two std::span in a single array
     * 
     * @tparam T 
     */
    template <
        typename T, 
        typename G = std::span<T>, 
        typename H = std::span<T>
    > requires is_span_type<T, G> && is_span_type<T, H>
    class multispan: public detail::multispan_base_t<T> {
    protected:
        // the two internal spans
        G first;
        H second;

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
         * @brief Construct a new multiptr object using a initializer list, raw 
         * array(not a pointer) or std::array
         * 
         * for raw pointers {ptr, size} should be used as a parameter
         * 
         * @param first 
         * @param second 
         */
        multispan(const G& first, const H& second):
            first(first), second(second)
        {}

        /**
         * @brief Operator to get data from the two spans
         * 
         * @param index 
         * @return constexpr T& 
         */
        constexpr reference operator[](const uint32_t index) const {
            // check what pointer we should use
            if (index > first.size()) {
                return second[index - first.size()];
            }

            // return the first pointer
            return first[index];
        }

        /**
         * @brief Returns the total size of both spans
         * 
         * @return uint32_t 
         */
        uint32_t size() const {
            return first.size() + second.size();
        }

        /**
         * @brief Returns if the span is empty
         * 
         * @return uint32_t 
         */
        uint32_t empty() const {
            return size() == 0;
        }
    };
}

#endif