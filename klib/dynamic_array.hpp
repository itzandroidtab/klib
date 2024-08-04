#ifndef KLIB_DYNAMIC_ARRAY_HPP
#define KLIB_DYNAMIC_ARRAY_HPP

#include <cstddef>
#include <utility>
#include <iterator>

#include "math.hpp"

namespace klib {
    template <typename T, uint32_t Size>
    class dynamic_array {
    public:
        // usings for the iterator
        using difference_type         = std::ptrdiff_t;
        using value_type              = T;
        using pointer                 = value_type*;
        using const_pointer           = const value_type*;
        using reference               = value_type&;
        using const_reference         = const value_type&;
        using iterator                = pointer;
        using const_iterator          = const_pointer;

    protected:
        T store[Size] = {};
        uint32_t index = 0;

        /**
         * Helper function that frees a spot at
         * the given position by shifting all elements
         * to the right by one.
         *
         * Does not update the index.
         *
         * @param pos
         */
        constexpr void free_position(const uint32_t pos) {
            for (uint32_t i = this->index; i != pos - 1; --i) {
                store[i + 1] = store[i];
            }
        }

        /**
         * @brief Get the index of the iterator
         *
         * @tparam G
         * @return constexpr uint32_t
         */
        template <typename G>
        constexpr uint32_t get_index(const G &it) {
            return it - begin();
        }

    public:
        /**
         * Default constructor.
         */
        constexpr dynamic_array() = default;

        /**
         * Construct from a initializer list.
         *
         * @param list
         */
        constexpr dynamic_array(std::initializer_list<T> list)
                : dynamic_array(list.begin(), list.end()) {}

        /**
         * Construct from a given InputIterator.
         *
         * @tparam InputIterator
         * @param first
         * @param last
         */
        template <typename InputIterator>
        constexpr dynamic_array(InputIterator first, InputIterator last) {
            for (auto it = first; it != last; ++it) {
                push_back(*it);
            }
        }

        /**
         * Copy constructor.
         *
         * @param x
         */
        constexpr dynamic_array(const dynamic_array &x) {
            for (uint32_t i = 0; i < x.size(); i++) {
                push_back(x[i]);
            }
        }

        /**
         * Move constructor.
         *
         * @param other
         */
        constexpr dynamic_array(dynamic_array &&other) : dynamic_array() {
            swap(*this, other);
        }

        /**
         * Assignment operator for two arrays.
         *
         * @param other
         * @return
         */
        constexpr dynamic_array &operator=(dynamic_array other) {
            swap(*this, other);

            return *this;
        }

        /**
         * Assignment operator for an array and an
         * initializer_list.
         *
         * @param list
         * @return
         */
        constexpr dynamic_array &operator=(std::initializer_list<T> list) {
            index = 0;

            for (const auto &item : list) {
                push_back(item);
            }

            return *this;
        }

        /**
         * Get a reference to the element at the given index.
         * This does not do bounds checking.
         *
         * @param n
         * @return
         */
        reference at(uint32_t n) {
           return store[n];
        }

        /**
         * Get a const reference to the element at the given index.
         * This does not do bounds checking.
         *
         * @param n
         * @return
         */
        const_reference at(uint32_t n) const {
            return store[n];
        }

        /**
         * Get a reference to the first element
         * in the array. Calling this function on
         * an empty container causes undefined behaviour.
         *
         * @return
         */
        constexpr reference front() {
            return store[0];
        }

        /**
         * Get a constant reference to the first element
         * in the array. Calling this function on
         * an empty container causes undefined behaviour.
         *
         * @return
         */
        constexpr const_reference front() const {
            return store[0];
        }

        /**
         * Get a reference to the last element
         * in the array. Calling this function on
         * an empty container causes undefined behaviour.
         *
         * @return
         */
        constexpr reference back() {
            return store[index - 1];
        }

        /**
         * Get constant a reference to the last element
         * in the array. Calling this function on
         * an empty container causes undefined behaviour.
         *
         * @return
         */
        constexpr const_reference back() const {
            return store[index - 1];
        }

        /**
         * Get a pointer to the
         * underlying data store.
         *
         * @return
         */
        constexpr pointer data() {
            return store;
        }

        /**
         * Get a constant pointer to the
         * underlying data store.
         *
         * @return
         */
        constexpr const_pointer data() const {
            return store;
        }

        /**
         * Check whether the array is
         * empty.
         *
         * @return
         */
        constexpr bool empty() const {
            return size() == 0;
        }

        /**
         * Get the amount of elements
         * in the array.
         *
         * @return
         */
        constexpr uint32_t size() const {
            return index;
        }

        /**
         * Get the maximum size of
         * the array.
         *
         * @return
         */
        constexpr uint32_t max_size() const {
            return Size;
        }

        /**
         * Resize the container.
         * This moves the internal index to the
         * given point.
         *
         * @param n
         * @return
         */
        constexpr void resize(const uint32_t n) {
            index = n;
        }

        /**
         * Remove all elements from the array.
         * These elements are destroyed.
         */
        constexpr void clear() {
            // Destruct elements.
            for (uint32_t i = 0; i < index; i++) {
                store[i].~T();
            }

            index = 0;
        }

        /**
         * Emplace an element at the given position.
         *
         * @tparam Args
         * @param pos
         * @param args
         */
        template <typename ...Args>
        constexpr void emplace(const uint32_t pos, Args &&... args) {
            free_position(pos);
            store[pos] = T{std::forward<Args>(args)...};
            index++;
        }

        /**
         * Emplace an element at the given position.
         *
         * @tparam Args
         * @param pos
         * @param args
         */
        template <typename ...Args>
        constexpr void emplace(const const_iterator &it, Args &&... args) {
            emplace(get_index(it), std::forward<Args>(args)...);
        }

        /**
         * Emplace an element at the given position.
         *
         * @tparam Args
         * @param pos
         * @param args
         */
        template <typename ...Args>
        constexpr void emplace(const iterator &it, Args &&... args) {
            emplace(get_index(it), std::forward<Args>(args)...);
        }

        /**
         * Replace the contents of the array with
         * the given value.
         * Old values are destroyed.
         *
         * @param n
         * @param val
         */
        constexpr void assign(uint32_t n, const_reference val) {
            clear();

            for (uint32_t i = 0; i < n; i++) {
                push_back(val);
            }
        }

        /**
         * Replace the contents of the array with
         * the values from the initializer list.
         * Old values are destroyed.
         *
         * @param list
         */
        constexpr void assign(std::initializer_list<T> list) {
            assign(list.begin(), list.end());
        }

        /**
         * Add the given value to the end
         * of the array.
         *
         * @param entry
         */
        constexpr void push_back(const_reference entry) {
            store[index++] = entry;
        }

        /**
         * Add the given value to the end
         * of the array.
         *
         * @param entry
         */
        constexpr void push_back(T &&entry) {
            store[index++] = entry;
        }

        /**
         * Emplace an element at
         * the back of the array.
         *
         * @tparam Args
         * @param args
         */
        template <typename ...Args>
        constexpr void emplace_back(Args &&... args) {
            store[index] = T{std::forward<Args>(args)...};
            index++;
        }

        /**
         * Remove the first element from
         * the array.
         * This destroys the removed element.
         */
        constexpr void pop_front() {
            store[0].~T();
            erase(begin());
        }

        /**
         * Remove the last element of the array,
         * reducing the array size by one.
         * This destroys the removed element.
         */
        constexpr void pop_back() {
            store[index--].~T();
        }

        /**
         * Swap the arrays.
         *
         * @param first
         * @param second
         */
        constexpr friend void swap(dynamic_array &first, dynamic_array &second) {
            std::swap(first.store, second.store);
            std::swap(first.index, second.index);
        }

        /**
         * Get a reference to the element
         * at the given index.
         *
         * @param index
         * @return
         */
        constexpr reference operator[](const uint32_t index) {
            return store[index];
        }

        /**
         * Set the item at the given index
         * to the given value.
         *
         * @param index
         * @param val
         * @return
         */
        constexpr void set(const uint32_t index, const_reference val) {
            store[index] = val;
        }

        /**
         * Set the item at the given index
         * to the given value.
         *
         * @param index
         * @param val
         * @return
         */
        constexpr void set(const uint32_t index, T &&val) {
            store[index] = std::move(val);
        }

        /**
         * Get a copy of the element
         * at the given index.
         *
         * @param index
         * @return
         */
        constexpr T operator[](const uint32_t index) const {
            return store[index];
        }

        /**
         * @brief Return a iterator to the
         * start of the dynamic array
         *
         * @return constexpr iterator
         */
        constexpr iterator begin() noexcept {
            return iterator(data());
        }

        /**
         * @brief Return a iterator to the
         * start of the dynamic array
         *
         * @return constexpr iterator
         */
        constexpr const_iterator begin() const noexcept {
            return const_iterator(data());
        }

        /**
         * @brief Returns a const iterator
         * to the start of the dynamic array
         *
         * @return constexpr const_iterator
         */
        constexpr const_iterator cbegin() const noexcept {
            return const_iterator(data());
        }

        /**
         * @brief Returns a iterator to the
         * end of the dynamic array
         *
         * @return constexpr iterator
         */
        constexpr iterator end() noexcept {
            return iterator(data() + index);
        }

        /**
         * @brief Returns a const iterator to
         * the end of the dynamic array
         *
         * @return constexpr iterator
         */
        constexpr const_iterator end() const noexcept {
            return const_iterator(data() + index);
        }

        /**
         * Erase an element at the given position.
         *
         * @param pos
         */
        constexpr void erase(const iterator &it) {
            // We don't set the data to zero; all items from the given
            // index on will be shifted one left in the array, overwriting the index to
            // remove. This makes removals expensive in the front of the array but prevents
            // fragmentation.
            for (iterator i = it; i < end(); ++i) {
                (*i) = *(i + 1);
            }

            this->index--;
        }

        /**
         * Erase an element at the given position.
         *
         * @warning Calling this function with a
         * wrong iterator causes undefined behaviour
         *
         * @param pos
         */
        constexpr void erase(const const_iterator &it) {
            // get a iterator from the const iterator
            iterator i = begin() + get_index(it);

            // erase using the normal iterator
            erase(i);
        }

        /**
         * Erase a range of elements.
         *
         * @param start
         * @param end
         */
        constexpr void erase(const_iterator &start, const_iterator &end) {
            for (auto it = start; it < end; ++it) {
                erase(it);
            }
        }
    };
}

#endif
