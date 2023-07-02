#ifndef KLIB_TOUCH_HPP
#define KLIB_TOUCH_HPP

#include <cstdint>
#include <array>

#include <klib/vector2.hpp>

namespace klib::hardware::touch {
    /**
     * @brief Touch event data. 
     * 
     */
    struct touch {
        // state of the touch event. (pressed = 1 or 
        // released = 0)
        bool pressed;

        // xy position of the touch point
        vector2<uint16_t> position;
    };

    /**
     * @brief Helper class to store Point amount of touch 
     * points. Is used to buffer touch point that are 
     * received in a interrupt.
     * 
     * @tparam Points 
     * @tparam Width 
     * @tparam Height 
     * @tparam DisplayWidth 
     * @tparam DisplayHeight 
     */
    template <
        uint32_t Points,
        uint32_t Width, uint32_t Height, 
        uint32_t DisplayWidth = Width, 
        uint32_t DisplayHeight = Height
    >
    class helper {
    protected:
        // array with the max amount of touch points
        static inline std::array<touch, Points> data;

        // make sure the points input is valid
        static_assert(Points <= 32 && Points != 0, "Invalid amount of points. Supported range 1 - 32");

    public:
        /**
         * @brief Init the touch helper
         * 
         */
        static void init() {
            // clear the data
            data = {};
        }

        /**
         * @brief Returns the amount of points we store
         * 
         * @return constexpr uint8_t 
         */
        constexpr static uint8_t size() {
            return Points;
        }

        /**
         * @brief Get the latest touch data for touch index
         * 
         * @param index 
         * @return const touch& 
         */
        static const touch& at(const uint32_t index) {
            return data[index];
        }

        /**
         * @brief Returns the active touch points in a bit mask
         * 
         * @return uint32_t 
         */
        static uint32_t active() {
            uint32_t m = 0;

            // unpress all the touch points
            for (uint8_t i = 0; i < Points; i++) {
                if (data[i].pressed) {
                    m |= (0x1 << i);
                }
            }

            return m;
        }

        /**
         * @brief Update a touch id with the new state 
         * (activates the touch point by pressing)
         * 
         * @param id 
         * @param pressed 
         * @param x 
         * @param y 
         */
        static void update(const uint8_t id, const vector2<uint16_t> position) {
            // make sure the id is valid
            if (id >= Points) {
                // invalid id. exit
                return;
            }
            
            // check if we should scale the touch area to the display area
            if constexpr (Width == DisplayWidth && Height == DisplayHeight) {
                // update the id in the array
                data[id] = {
                    true, position
                };
            }
            else {
                // scale the x and y to the display width/height
                const vector2<uint16_t> xy = (
                    (position * vector2<uint16_t>{DisplayWidth, DisplayHeight}) / 
                    vector2<uint16_t>{Width, Height}
                );

                // update the id in the array
                data[id] = {
                    true, xy
                };
            }
        }

        /**
         * @brief Update a touch id with the new state
         * (unpresses the touch point)
         * 
         * @param id 
         */
        static void update(const uint8_t id) {
            // make sure the id is valid
            if (id >= Points) {
                // invalid id. exit
                return;
            }
            
            // update the id in the array
            data[id].pressed = false;
        }

        /**
         * @brief Unpress all the touch points. Can be used 
         * when the touch controller does not send a unpress 
         * event
         * 
         */
        static void unpress_all() {
            // unpress all the touch points
            for (auto &t: data) {
                t.pressed = false;
            }
        }
    };
}

#endif