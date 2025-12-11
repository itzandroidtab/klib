#ifndef KLIB_RTOS_WAITABLE_HPP
#define KLIB_RTOS_WAITABLE_HPP

namespace klib::rtos {
    /**
     * @brief Interface for waitable objects. A waitable object can be
     * waited on by a task
     * 
     */
    class waitable {
    public:
        /**
         * @brief Check if the waitable object is currently
         * being waited on.
         * 
         * @return true 
         * @return false 
         */
        virtual bool is_waiting() const = 0;
    };
}

#endif