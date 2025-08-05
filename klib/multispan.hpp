#ifndef KLIB_MULTISPAN_HPP
#define KLIB_MULTISPAN_HPP

#include <cstdint>
#include <type_traits>
#include <span>
#include <array>

namespace klib {
    /**
     * @brief Base class to a multispan, implementation dependend
     *
     * @tparam T
     */
    template <typename T>
    class multispan {
    protected:
        /**
         * @brief Default constructor
         * 
         */
        constexpr multispan() {};

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
         * @brief Operator to get data from a multispan
         *
         * @param index
         * @return constexpr T&
         */
        virtual reference operator[](const uint32_t index) const = 0;

        /**
         * @brief Returns the total size of both spans
         *
         * @return uint32_t
         */
        virtual uint32_t size() const = 0;

        /**
         * @brief Returns if the span is empty
         *
         * @return uint32_t
         */
        uint32_t empty() const {
            return size() == 0;
        }

        /**
         * @brief Returns the amount of bytes the combined spans use
         *
         * @return constexpr uint32_t
         */
        virtual uint32_t size_bytes() const = 0;
    };

    /**
     * @brief Non owning wrapper for multiple arrays
     *
     * @tparam T
     */
    template <typename T, uint32_t Amount>
    class span_array: public klib::multispan<T> {
    protected:
        std::array<std::span<T>, Amount> storage = {};

    public:
        /**
         * @brief Span_array constructor from multple std::spans
         * 
         */
        constexpr span_array(const std::initializer_list<std::span<T>>& list) {
            uint32_t index = 0;

            // copy the spans into the storage
            for (const auto& l: list) {
                storage[index] = l;
                index++;
            }
        }

        /**
         * @brief Construct a multispan using variadic templates
         * 
         * @tparam Args 
         */
        template<typename... Args>
        constexpr span_array(Args... args): 
            span_array({args...})
        {}

        /**
         * @brief Operator to get data from the two spans
         *
         * @param index
         * @return constexpr T&
         */
        constexpr klib::multispan<T>::reference operator[](const uint32_t index) const {
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
            return *storage[Amount - 1].end();
        }

        /**
         * @brief Returns the total size of both spans
         *
         * @return uint32_t
         */
        constexpr uint32_t size() const override {
            uint32_t count = 0;

            for (auto& s: storage) {
                count += s.size();
            }

            return count;
        }

        /**
         * @brief Returns the amount of bytes the combined spans use
         *
         * @return constexpr uint32_t
         */
        constexpr uint32_t size_bytes() const override {
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

    /**
     * @brief Helper to create a span_array from multiple std::span. Uses the first 
     * argument as the base type for all the elements
     * 
     * @tparam Args 
     * @param args 
     * @return auto 
     */
    template<typename... Args>
    auto make_span_array(Args... args) {
        // return a span_array with the correct size and initalize it
        return klib::span_array<
            typename std::tuple_element_t<0, std::tuple<Args...>>::element_type,
            sizeof...(Args)
        >(args...);
    }
}

#endif
