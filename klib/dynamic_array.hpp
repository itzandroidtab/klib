#ifndef KLIB_DYNAMIC_ARRAY_HPP
#define KLIB_DYNAMIC_ARRAY_HPP

#include <cstddef>
#include <utility>
#include <iterator>

#include "math.hpp"

namespace klib {
    template <typename T, uint32_t Size>
    class dynamic_array {
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
         * A random access iterator for the dynamic array;
         * used for for-ranges or standard algorithms.
         */
        class random_access_iterator {
        protected:
            dynamic_array<T, Size> *subject;
            uint32_t at;

            // Allow access to the 'at' member
            friend dynamic_array;

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = value_type *;
            using const_pointer = value_type const *;
            using reference = value_type &;
            using iterator_category = std::random_access_iterator_tag;

            constexpr random_access_iterator(dynamic_array<T, Size> &subject)
                    : subject(&subject), at(0) {}

            constexpr random_access_iterator(dynamic_array<T, Size> &subject, uint32_t start)
                    : subject(&subject), at(start) { }

            constexpr random_access_iterator(const random_access_iterator &rhs)
                    : subject(rhs.subject), at(rhs.at) {}

            constexpr random_access_iterator(random_access_iterator &&rhs) noexcept = default;

            constexpr random_access_iterator &operator=(const random_access_iterator &rhs) {
                subject = rhs.subject;
                at = rhs.at;

                return *this;
            }

            constexpr random_access_iterator &operator=(random_access_iterator &&rhs) noexcept = default;

            /**
             * a == b
             *
             * @param rhs
             * @return
             */
            constexpr bool operator==(const random_access_iterator &rhs) const {
                return at == rhs.at && subject == rhs.subject;
            }

            /**
             * a != b
             *
             * @param rhs
             * @return
             */
            constexpr bool operator!=(const random_access_iterator &rhs) {
                return !operator==(rhs);
            }

            constexpr reference operator*() {
                return (*subject).at(at);
            }

            constexpr value_type operator*() const {
                return (*subject).at(at);
            }

            constexpr pointer operator->() {
                return &(*subject).at(at);
            }

            constexpr const_pointer operator->() const {
                return &(*subject).at(at);
            }

            /**
             * Pre-increment.
             *
             * @return
             */
            constexpr random_access_iterator &operator++() {
                at++;
                return *this;
            }

            /**
             * Post increment.
             *
             * @return
             */
            constexpr const random_access_iterator operator++(int) {
                random_access_iterator temp(*this);
                operator++();
                return temp;
            }

            constexpr random_access_iterator &operator--() {
                at--;
                return *this;
            }

            constexpr const random_access_iterator operator--(int) {
                random_access_iterator temp(*this);
                operator--();
                return temp;
            }

            /**
             * a + n
             *
             * @param n
             * @return
             */
            constexpr random_access_iterator operator+(const uint32_t n) const {
                return random_access_iterator(subject, at + n);
            }

            /**
             * n + a
             *
             * @param n
             * @param it
             * @return
             */
            constexpr friend random_access_iterator operator+(const uint32_t n, const random_access_iterator &it) {
                return it + n;
            }

            /**
             * a - n
             *
             * @param n
             * @return
             */
            constexpr random_access_iterator operator-(const uint32_t n) const {
                return random_access_iterator(subject, at - n);
            }

            /**
             * a - b
             *
             * @param other
             * @return
             */
            constexpr random_access_iterator operator-(const random_access_iterator &other) const {
                return random_access_iterator(subject, at - other.at);
            }

            /**
             * a < b
             *
             * @param other
             * @return
             */
            constexpr random_access_iterator operator<(const random_access_iterator &other) const {
                return at < other.at;
            }

            /**
             * a > b
             *
             * @param other
             * @return
             */
            constexpr random_access_iterator operator>(const random_access_iterator &other) const {
                return at > other.at;
            }

            /**
             * a <= b
             *
             * @param other
             * @return
             */
            constexpr random_access_iterator operator<=(const random_access_iterator &other) const {
                return operator<(other) || operator==(other);
            }

            /**
             * a >= b
             *
             * @param other
             * @return
             */
            constexpr random_access_iterator operator>=(const random_access_iterator &other) const {
                return operator>(other) || operator==(other);
            }

            /**
             * a += n
             *
             * @param n
             * @return
             */
            constexpr random_access_iterator operator+=(const uint32_t n) {
                at += n;
                return *this;
            }

            /**
             * a -= n
             *
             * @param n
             * @return
             */
            constexpr random_access_iterator operator-=(const uint32_t n) {
                at -= n;
                return *this;
            }

            /**
             * a[n]
             *
             * @param n
             * @return
             */
            constexpr reference operator[](const uint32_t n) {
                return subject[n];
            }
        };

        class const_random_access_iterator {
        protected:
            const dynamic_array<T, Size> *subject;
            uint32_t at;

            // Allow access to the 'at' member
            friend dynamic_array;

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = value_type *;
            using const_pointer = value_type const *;
            using reference = value_type &;
            using iterator_category = std::random_access_iterator_tag;

            constexpr const_random_access_iterator(const dynamic_array<T, Size> &subject)
                : subject(&subject), at(0) {}

            constexpr const_random_access_iterator(const dynamic_array<T, Size> &subject, uint32_t start)
                : subject(&subject), at(start) {}

            constexpr const_random_access_iterator(const const_random_access_iterator &rhs)
                : subject(rhs.subject), at(rhs.at) {}

            constexpr const_random_access_iterator &operator=(const const_random_access_iterator &rhs) = default;

            constexpr const_random_access_iterator &operator=(const_random_access_iterator &&rhs) noexcept = default;

            /**
             * a == b
             *
             * @param rhs
             * @return
             */
            constexpr bool operator==(const const_random_access_iterator &rhs) const {
                return at == rhs.at && subject == rhs.subject;
            }

            /**
             * a != b
             *
             * @param rhs
             * @return
             */
            constexpr bool operator!=(const const_random_access_iterator &rhs) {
                return !operator==(rhs);
            }

            constexpr value_type operator*() const {
                return subject->at(at);
            }

            constexpr const_pointer operator->() const {
                return &subject->at(at);
            }

            /**
             * Pre-increment.
             *
             * @return
             */
            constexpr const_random_access_iterator &operator++() {
                at++;
                return *this;
            }

            /**
             * Post increment.
             *
             * @return
             */
            constexpr const const_random_access_iterator operator++(int) {
                const_random_access_iterator temp(*this);
                operator++();
                return temp;
            }

            constexpr const_random_access_iterator &operator--() {
                at--;
                return *this;
            }

            constexpr const const_random_access_iterator operator--(int) {
                const_random_access_iterator temp(*this);
                operator--();
                return temp;
            }

            /**
             * a + n
             *
             * @param n
             * @return
             */
            constexpr const_random_access_iterator operator+(const uint32_t n) const {
                return const_random_access_iterator(subject, at + n);
            }

            /**
             * n + a
             *
             * @param n
             * @param it
             * @return
             */
            constexpr friend const_random_access_iterator operator+(const uint32_t n, const const_random_access_iterator &it) {
                return it + n;
            }

            /**
             * a - n
             *
             * @param n
             * @return
             */
            constexpr const_random_access_iterator operator-(const uint32_t n) const {
                return const_random_access_iterator(subject, at - n);
            }

            /**
             * a - b
             *
             * @param other
             * @return
             */
            constexpr const_random_access_iterator operator-(const const_random_access_iterator &other) const {
                return const_random_access_iterator(subject, at - other.at);
            }

            /**
             * a < b
             *
             * @param other
             * @return
             */
            constexpr const_random_access_iterator operator<(const const_random_access_iterator &other) const {
                return at < other.at;
            }

            /**
             * a > b
             *
             * @param other
             * @return
             */
            constexpr const_random_access_iterator operator>(const const_random_access_iterator &other) const {
                return at > other.at;
            }

            /**
             * a <= b
             *
             * @param other
             * @return
             */
            constexpr const_random_access_iterator operator<=(const const_random_access_iterator &other) const {
                return operator<(other) || operator==(other);
            }

            /**
             * a >= b
             *
             * @param other
             * @return
             */
            constexpr const_random_access_iterator operator>=(const const_random_access_iterator &other) const {
                return operator>(other) || operator==(other);
            }

            /**
             * a += n
             *
             * @param n
             * @return
             */
            constexpr const_random_access_iterator operator+=(const uint32_t n) {
                at += n;
                return *this;
            }

            /**
             * a -= n
             *
             * @param n
             * @return
             */
            constexpr const_random_access_iterator operator-=(const uint32_t n) {
                at -= n;
                return *this;
            }
        };

        /**
         * Get a reference to the element at the given index.
         * This does not do bounds checking.
         *
         * @param n
         * @return
         */
        T &at(uint32_t n) {
           return store[n];
        }

        /**
         * Get a const reference to the element at the given index.
         * This does not do bounds checking.
         *
         * @param n
         * @return
         */
        T const &at(uint32_t n) const {
            return store[n];
        }

        /**
         * Get a reference to the first element
         * in the array. Calling this function on
         * an empty container causes undefined behaviour.
         *
         * @return
         */
        constexpr T &front() {
            return store[0];
        }

        /**
         * Get a constant reference to the first element
         * in the array. Calling this function on
         * an empty container causes undefined behaviour.
         *
         * @return
         */
        constexpr const T &front() const {
            return store[0];
        }

        /**
         * Get a reference to the last element
         * in the array. Calling this function on
         * an empty container causes undefined behaviour.
         *
         * @return
         */
        constexpr T &back() {
            return store[index - 1];
        }

        /**
         * Get constant a reference to the last element
         * in the array. Calling this function on
         * an empty container causes undefined behaviour.
         *
         * @return
         */
        constexpr const T &back() const {
            return store[index - 1];
        }

        /**
         * Get a pointer to the
         * underlying data store.
         *
         * @return
         */
        constexpr T *data() {
            return store;
        }

        /**
         * Get a constant pointer to the
         * underlying data store.
         *
         * @return
         */
        constexpr const T *data() const {
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

        // Reduce visual noise with these aliases
        using iterator = random_access_iterator;
        using const_iterator = const_random_access_iterator;

        /**
         * Insert a single element.
         *
         * @param it
         * @param value
         * @return
         */
        constexpr iterator insert(const_iterator &it, const T &value) {
            free_position(it.at);
            store[it.at] = value;
            index++;

            return iterator(it);
        }

        /**
         * Insert the given value n types, starting
         * at iterator it.
         *
         * @param it
         * @param n
         * @param value
         * @return
         */
        constexpr iterator insert(const_iterator &it, uint32_t n, const T &value) {
            for (uint32_t i = 0; i < n; i++) {
                insert(it + i, value);
            }

            return it;
        }

        /**
         * Insert a range of values from the input iterator,
         * starting at the given iterator.
         *
         * @tparam InputIterator
         * @param it
         * @param first
         * @param last
         * @return
         */
        template <typename InputIterator>
        constexpr iterator insert(const_iterator &it, InputIterator first, InputIterator last) {
            uint32_t i = 0;

            for (auto iit = first; iit != last; ++i, ++iit) {
                insert(it + i, iit);
            }

            return it;
        }

        /**
         * Insert a single element by move at
         * the given position.
         *
         * @param it
         * @param value
         * @return
         */
        constexpr iterator insert(const_iterator &it, T &&value) {
            insert(it, std::move(value));

            return it;
        }

        /**
         * Insert a range of values from an initializer list,
         * starting at the given position.
         *
         * @param it
         * @param list
         * @return
         */
        constexpr iterator insert(const_iterator &it, std::initializer_list<T> list) {
            insert(it, list.begin(), list.end());

            return it;
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
            store[pos] = T(std::forward<Args>(args)...);
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
        constexpr void emplace(const_iterator &it, Args &&... args) {
            emplace(it.at, std::forward<Args>(args)...);
        }

        /**
         * Erase an element at the given position.
         *
         * @param pos
         */
        constexpr void erase(const_iterator &it) {
            if (it.at == this->index - 1) {
                this->index--;
                return;
            }

            // We don't set the data to zero; all items from the given
            // index on will be shifted one left in the array, overwriting the index to
            // remove. This makes removals expensive in the front of the array but prevents
            // fragmentation.
            for (uint32_t i = it.at; i < this->index - 1; ++i) {
                store[i] = store[i + 1];
            }

            this->index--;
        }

        /**
         * Erase a range of elements.
         *
         * @param start
         * @param end
         */
        constexpr void erase(const_iterator &start, const_iterator &end) {
            for (auto it = start; it != end; ++it) {
                erase(it);
            }
        }

        /**
         * Replace the contents of the array with
         * the values from the iterator.
         * Old values are destroyed.
         *
         * @tparam InputIterator
         * @param first
         * @param last
         */
        template <typename InputIterator>
        constexpr void assign(InputIterator first, InputIterator last) {
            clear();

            for (auto it = first; it != last; ++it) {
                push_back(it);
            }
        }

        /**
         * Replace the contents of the array with
         * the given value.
         * Old values are destroyed.
         *
         * @param n
         * @param val
         */
        constexpr void assign(uint32_t n, const T &val) {
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
        constexpr void push_back(const T &entry) {
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
            store[index++] = T(std::forward<Args>(args)...);
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
            using std::swap;

            swap(first.store, second.store);
            swap(first.index, second.index);
        }

        /**
         * Get a reference to the element
         * at the given index.
         *
         * @param index
         * @return
         */
        constexpr T &operator[](const uint32_t index) {
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
        constexpr void set(const uint32_t index, const T &val) {
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
         * Get an iterator to the start
         * of the array.
         *
         * @return
         */
        constexpr iterator begin() {
            return iterator(*this);
        }

        /**
         * Get a constant iterator to the start
         * of the array.
         *
         * @return
         */
        constexpr const_iterator begin() const {
            return const_iterator(*this);
        }

        /**
         * Get an iterator to the end
         * of the array.
         *
         * @return
         */
        constexpr iterator end() {
            return iterator(*this, index);
        }

        /**
         * Get a constant iterator to the end
         * of the array.
         *
         * @return
         */
        constexpr const_iterator end() const {
            return const_iterator(*this, index);
        }
    };
}

#endif
