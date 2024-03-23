#ifndef KLIB_MEMORY_HPP
#define KLIB_MEMORY_HPP

#include <cstdint>
#include <cstdlib>

#include <klib/entry/entry.hpp>
#include <klib/math.hpp>

namespace klib::allocator::detail {
    template <uint32_t MinSize = 4>
    class allocator {
    protected:
        // start address of the memory
        const uint32_t start_address;

        // size of the heap
        const uint32_t heap_size;

        // end of the currently allocated memory
        uint32_t end_address;

        /**
         * @brief Chunk used to store information
         * about a memory allocation
         * 
         */
        struct chunk {
            // magic header should always be
            // 0xdeadbeef. Used to check if 
            // a chunk is valid
            uint32_t magic_header;

            // pointer to the previous chunk
            chunk *previous; 

            // size of the current block (can also be used 
            // to determine the next chunk)
            uint32_t size;

            // flag to signal if a chunk is in use
            bool in_use;
        };

        // make sure the chunk is the expected size
        static_assert(sizeof(chunk) == 16, "Chunk size should be 16 bytes");

        void add_new_chunk(chunk& ch, chunk *const previous, const uint32_t size = 0) const {
            // mark the new allocation as not in use and 0 size
            ch.magic_header = 0xdeadbeef;
            ch.previous = previous;
            ch.in_use = false;
            ch.size = size;
        }

        void split_chunk(chunk& ch, const uint32_t size) const {
            // get the next chunk
            chunk &next_ch = (*reinterpret_cast<chunk*>(
                reinterpret_cast<uint32_t>(&ch) + ch.size + sizeof(chunk)
            ));

            // create a new chunk after the new size
            chunk &new_ch = (*reinterpret_cast<chunk*>(
                reinterpret_cast<uint32_t>(&ch) + size + sizeof(chunk)
            ));

            // set all the parameters of the new chunk
            add_new_chunk(new_ch, &ch, (reinterpret_cast<uint32_t>(&new_ch) - reinterpret_cast<uint32_t>(&ch)) - sizeof(chunk));

            // move the previous of the next chunk to the new chunk
            next_ch.previous = &new_ch;
        }

    public:
        allocator(const uint32_t heap_start, const uint32_t heap_end):
            start_address(heap_start), heap_size(heap_end - heap_start), 
            end_address(heap_start)
        {
            // set a chunk on the start address
            chunk &start_chunk = (*reinterpret_cast<chunk*>(heap_start));

            // create a new chunk at the start chunk
            add_new_chunk(start_chunk, nullptr);
        }

        void free(const void* ptr) noexcept {
            // make sure we are not dereferencing a nullptr
            if (ptr == nullptr) {
                return;
            }

            // get the chunk that is used for this pointer
            chunk &current_chunk = (*reinterpret_cast<chunk*>(
                reinterpret_cast<uint32_t>(ptr) - sizeof(chunk)
            ));

            // check if the header is valid
            if (current_chunk.magic_header != 0xdeadbeef) {
                return;
            }

            // do not free when the size is 0 or when not in use
            if (current_chunk.size == 0 && !current_chunk.in_use) {
                return;
            }

            // get the location of a new chunk header
            chunk &next_chunk = (*reinterpret_cast<chunk*>(
                (reinterpret_cast<uint32_t>(&current_chunk) + sizeof(chunk)) + 
                current_chunk.size
            ));

            // check if the next header is also free
            if (!next_chunk.in_use) {
                // check if the next chunk is the last chunk
                if (next_chunk.size == 0) {
                    // clear the size of the current chunk to mark 
                    // it as the new last chunk
                    current_chunk.size = 0;

                    // move the end to the current chunk
                    end_address = reinterpret_cast<const uint32_t>(&current_chunk);
                }
                else {
                    // add the size to the current chunk
                    current_chunk.size += sizeof(chunk) + next_chunk.size;
                }
            }

            // check if the previous header is also free
            if (current_chunk.previous != nullptr && !current_chunk.previous->in_use) {
                // check if we are the last chunk
                if (current_chunk.size = 0) {
                    // clear the size of the previous chunk to mark
                    // it as the new last chunk
                    current_chunk.previous->size = 0;

                    // move the end to the current chunk
                    end_address = reinterpret_cast<const uint32_t>(&current_chunk.previous);
                }
                else {
                    // add our size to the previous chunk (including 
                    // the size of the chunk) so we point to the next
                    // chunk
                    current_chunk.previous->size += sizeof(chunk) + current_chunk.size;
                }
            }

            // change the in use flag
            current_chunk.in_use = false;
        }

        void* allocate(const uint32_t size) noexcept {
            // do not allocate when size is 0
            if (!size) {
                return nullptr;
            }

            // change the size to match the min size
            const uint32_t allocate_size = klib::max(size, MinSize);

            // pointer to a chunk
            chunk *ch = reinterpret_cast<chunk*>(start_address);

            while (true) {
                // make sure the structure is still valid
                if (ch->magic_header != 0xdeadbeef) {
                    // allocator structure is broken. Exit
                    return nullptr;
                }

                // check if a chunk is in use
                if (ch->in_use) {
                    // go to the next chunk
                    ch = reinterpret_cast<chunk*>(
                        reinterpret_cast<const uint32_t>(ch) + ch->size + sizeof(chunk)
                    );

                    continue;
                }

                // check if the data fits in a existing chunk
                if (allocate_size < ch->size) {
                    // check if we can split and still have enough space
                    // for both the minsize and another chunk
                    if ((ch->size - allocate_size) > (MinSize + sizeof(chunk))) {
                        // split the current chunk using the size
                        split_chunk(*ch, allocate_size);

                        // set the correct size in the current chunk
                        ch->size = allocate_size;
                    }

                    // mark chunk as in use
                    ch->in_use = true;

                    // return a pointer to free memory
                    return reinterpret_cast<void*>(
                        reinterpret_cast<uint32_t>(ch) + sizeof(chunk)
                    );
                }

                // size is 0 so that means last item in currently allocated memory
                // and we could not fit in any space
                if (ch->size == 0) {
                    // check if we can allocate behind the current chunk 
                    // 
                    // chunk + sizeof(chunk) + chunk.size (= 0) = end of 
                    // current chunk. End of chunk + sizeof(chunk) + size 
                    // needs to fit before we allocate
                    if ((reinterpret_cast<uint32_t>(ch) + (sizeof(chunk) * 2) + size) >= 
                        (start_address + heap_size)) 
                    {
                        return nullptr;
                    }

                    // change the size to the allocated size
                    ch->size = allocate_size;
                    ch->in_use = true;

                    // create a new chunk after this chunk
                    chunk &new_end = (*reinterpret_cast<chunk*>(
                        reinterpret_cast<const uint32_t>(ch) + ch->size + sizeof(chunk))
                    );

                    // set all the parameters for the new chunk
                    add_new_chunk(new_end, ch);

                    // move the end address
                    end_address = reinterpret_cast<const uint32_t>(&new_end);

                    // return our current chunk
                    return reinterpret_cast<void*>(
                        reinterpret_cast<uint32_t>(ch) + sizeof(chunk)
                    );
                }

                // if we are not at the end but we could not fit any chunk yet
                // go to the next chunk
                ch = reinterpret_cast<chunk*>(
                    reinterpret_cast<const uint32_t>(ch) + ch->size + sizeof(chunk)
                );
            }
        }

        /**
         * @brief return the amount of allocated memory
         * 
         * @return uint32_t 
         */
        uint32_t size() const {
            return end_address - start_address;
        }
    };
}

namespace klib::allocator {
    // allocator for memory on the heap
    [[maybe_unused]]
    static auto allocator = detail::allocator<4>(
        reinterpret_cast<uint32_t>(&__heap_start), 
        reinterpret_cast<uint32_t>(&__heap_end)
    );
}

namespace klib {
    /**
     * @brief Allocate memory on the heap
     * 
     * @param size 
     * @return void* 
     */
    void* malloc(uint32_t size) {
        return allocator::allocator.allocate(size);
    }

    /**
     * @brief Free memory on the heap
     * 
     * @param ptr 
     */
    void free(const void *const ptr) {
        return allocator::allocator.free(ptr);
    }
}

/**
 * @brief Allocate size amount of memory
 * 
 * @param size 
 * @return void* 
 */
void* operator new(const size_t size) noexcept {
    return klib::allocator::allocator.allocate(size);
}

/**
 * @brief Allocate a array amount of memory
 * 
 * @param size 
 * @return void* 
 */
void* operator new[](const size_t size) noexcept {
    return klib::allocator::allocator.allocate(size);
}

/**
 * @brief Delete size amount of memory
 * 
 * @param size 
 * @return void* 
 */
void operator delete(void *const ptr) noexcept {
    return klib::allocator::allocator.free(ptr);
}

/**
 * @brief Delete a array amount of memory
 * 
 * @param size 
 * @return void* 
 */
void operator delete[](void *const ptr) noexcept {
    return klib::allocator::allocator.free(ptr);
}

#endif