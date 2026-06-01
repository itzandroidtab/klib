#ifndef KLIB_NXP_LPC178X_LCD_HPP
#define KLIB_NXP_LPC178X_LCD_HPP

#include <bit>

#include <klib/io/core_clock.hpp>
#include <klib/graphics/color.hpp>

#include <io/power.hpp>

namespace klib::core::lpc178x::io {
    /**
     * @brief Lcd driver
     * 
     * @note Only TFT is supported
     * 
     * @tparam Lcd 
     * @tparam Mode 
     */
    template <typename Lcd, graphics::mode Mode = graphics::mode::rgb565>
    class lcd {
    public:
        // using for the array of callbacks
        using interrupt_callback = void(*)();

        /**
         * @brief The clock source for the LCD clock
         * 
         */
        enum class clock_source {
            cclk = 0,
            clk_in = 1,
        };

        /**
         * @brief LCD timing configuration
         * 
         */
        struct timing_config {
            // horizontal synchronization width
            uint8_t hsync;
            
            // vertical synchronization height
            uint16_t vsync;

            // horizontal back porch
            uint16_t hbp;

            // vertical back porch
            uint8_t vbp;

            // active width
            uint16_t active_width;

            // active height
            uint16_t active_height;

            // total width
            uint16_t total_width;

            // total height
            uint16_t total_height;

            // the target refresh rate of the display
            uint16_t refresh_rate;
        };

        struct io_polarity {
            // invert vertical synchronization
            bool invert_vsync;

            // invert horizontal synchronization
            bool invert_hsync;

            // invert the panel clock
            bool invert_panel_clock;

            // invert the output enable
            bool invert_output_enable;
        };

    protected:
        // Make sure we support the graphics mode
        static_assert(
            (Mode == graphics::mode::mono) || 
            (Mode == graphics::mode::rgb444) || 
            (Mode == graphics::mode::rgb565) || 
            (Mode == graphics::mode::bgr565) || 
            (Mode == graphics::mode::rgb888) || 
            (Mode == graphics::mode::mono),
            "Invalid graphics mode for controller or not supported"
        );

        // pointer to a callback
        static inline interrupt_callback lcd_callback = nullptr;

        constexpr static uint8_t graphics_to_raw() {
            // get the bits the mode supports
            constexpr static uint32_t bits = graphics::detail::pixel_conversion<Mode>::bits;

            // return the raw value for every bit amount
            switch (bits) {
                case 1:
                    return 0b000;
                case 12:
                    return 0b111;
                case 16:
                    return 0b110;
                case 24:
                default:
                    return 0b101;
            }
        }

        static void interrupt_handler() {
            // get the mask and status
            const uint32_t mask = Lcd::port->INTMSK;
            const uint32_t status = Lcd::port->INTRAW;
            
            // get the masked status
            const uint32_t masked = status & mask;

            // clear the interrupt bits
            Lcd::port->INTCLR = masked;

            // call the callback
            if (lcd_callback) {
                lcd_callback();
            }
        }

    public:
        template <
            std::endian ByteEndian = std::endian::native, 
            std::endian PixelEndian = std::endian::native, 
            clock_source Source = clock_source::cclk
        >
        static bool init(const timing_config& config, const io_polarity& polarity, const interrupt_callback& callback = nullptr) {
            // check for correct values in the active height and width fields
            if (config.active_width < 16 || config.active_width > 1024 || !config.hsync) {
                return false;
            }

            if (config.active_height == 0 || config.active_height > 1024) {
                return false;
            }

            // calculate the front porch for the horizontal and vertical side
            const int32_t hfp = (config.total_width - config.active_width - config.hsync - config.hbp);
            const int32_t vfp = (config.total_height - config.active_height - config.vsync - config.vbp);

            // check if the range is valid
            if (hfp < 1 || hfp > 256 || config.hbp < 1 || config.hbp > 256) {
                return false;
            }

            if (vfp < 0 || vfp > 255) {
                return false;
            }

            const uint32_t clock = klib::io::clock::get();
            const uint32_t pixel_clock = config.total_width * config.total_height * config.refresh_rate;
            const uint32_t pcd = (clock / pixel_clock) - 2;

            if (pixel_clock == 0 || (clock / pixel_clock) < 2) {
                return false;
            }

            if (pcd > 0x3ff) {
                return false;
            }

            // enable power to the lcd peripheral
            target::io::power_control::enable<Lcd>();
            
            // disable lcd controller output
            Lcd::port->CTRL = 0x0;

            // setup the pins we always need
            target::io::detail::pins::set_peripheral<typename Lcd::power::pin, typename Lcd::power::periph>();
            target::io::detail::pins::set_peripheral<typename Lcd::line_end::pin, typename Lcd::line_end::periph>();
            target::io::detail::pins::set_peripheral<typename Lcd::clock::pin, typename Lcd::clock::periph>();
            target::io::detail::pins::set_peripheral<typename Lcd::clock_in::pin, typename Lcd::clock_in::periph>();
            target::io::detail::pins::set_peripheral<typename Lcd::vsync::pin, typename Lcd::vsync::periph>();
            target::io::detail::pins::set_peripheral<typename Lcd::hsync::pin, typename Lcd::hsync::periph>();
            target::io::detail::pins::set_peripheral<typename Lcd::output_enable::pin, typename Lcd::output_enable::periph>();

            // always init the vd0 
            target::io::detail::pins::set_peripheral<typename Lcd::vd0::pin, typename Lcd::vd0::periph>();

            // setup the pins based on how many bits we have
            if constexpr (graphics::detail::pixel_conversion<Mode>::bits >= 12) {
                target::io::detail::pins::set_peripheral<typename Lcd::vd1::pin, typename Lcd::vd1::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd2::pin, typename Lcd::vd2::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd3::pin, typename Lcd::vd3::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd4::pin, typename Lcd::vd4::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd5::pin, typename Lcd::vd5::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd6::pin, typename Lcd::vd6::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd7::pin, typename Lcd::vd7::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd8::pin, typename Lcd::vd8::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd9::pin, typename Lcd::vd9::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd10::pin, typename Lcd::vd10::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd11::pin, typename Lcd::vd11::periph>();
            }
            
            if constexpr (graphics::detail::pixel_conversion<Mode>::bits >= 16) {
                target::io::detail::pins::set_peripheral<typename Lcd::vd12::pin, typename Lcd::vd12::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd13::pin, typename Lcd::vd13::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd14::pin, typename Lcd::vd14::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd15::pin, typename Lcd::vd15::periph>();
            }
            
            if constexpr (graphics::detail::pixel_conversion<Mode>::bits >= 24) {
                target::io::detail::pins::set_peripheral<typename Lcd::vd16::pin, typename Lcd::vd16::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd17::pin, typename Lcd::vd17::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd18::pin, typename Lcd::vd18::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd19::pin, typename Lcd::vd19::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd20::pin, typename Lcd::vd20::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd21::pin, typename Lcd::vd21::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd22::pin, typename Lcd::vd22::periph>();
                target::io::detail::pins::set_peripheral<typename Lcd::vd23::pin, typename Lcd::vd23::periph>();
            }

            // setup the clock source
            const uint32_t pol = (
                (pcd & 0x1f) |
                ((Source != clock_source::cclk) << 5) |
                (polarity.invert_vsync << 11) |
                (polarity.invert_hsync << 12) |
                (polarity.invert_panel_clock << 13) |
                (polarity.invert_output_enable << 14) |
                (((config.active_width - 1) & 0x3ff) << 16) |
                (((pcd >> 5) & 0x1f) << 27)
            );

            // write the new value to the register
            Lcd::port->POL = pol;
            SYSCON->LCD_CFG = 0;

            // setup the lcd configuration         
            Lcd::port->TIMH = (
                ((((config.active_width / 16) - 1) & 0x3f) << 2) |
                ((config.hsync - 1) & 0xff) << 8 |
                (static_cast<uint32_t>(hfp - 1) & 0xff) << 16 |
                ((config.hbp - 1) & 0xff) << 24
            );

            Lcd::port->TIMV = (
                (((config.active_height - 1) & 0x3ff)) |
                (((config.vsync - 1) & 0x3f) << 10) |
                ((static_cast<uint32_t>(vfp) & 0xff) << 16) |
                ((config.vbp & 0xff) << 24)
            );

            // check if we need any interrupt or not
            if (callback) {
                // register our handler if we have a callback
                lcd_callback = callback;
                target::irq::register_irq<Lcd::interrupt_id>(interrupt_handler);

                // enable the interrupt
                target::enable_irq<Lcd::interrupt_id>();

                // enable the lnbuin interrupt
                Lcd::port->INTMSK = 0x1 << 2;
            }
            else {
                target::disable_irq<Lcd::interrupt_id>();

                // clear all the enabled interrupts
                Lcd::port->INTMSK = 0;
            }

            // set the control register
            Lcd::port->CTRL = (
                (0x1 << 0) |
                (graphics_to_raw() << 1) |
                (0x1 << 5) |
                ((Mode != graphics::mode::bgr565) << 8) |
                ((ByteEndian != std::endian::little) << 9) |
                ((PixelEndian != std::endian::little) << 10)
            );

            // clear the line end
            Lcd::port->LE = 0x00;

            return true;
        }

        /**
         * @brief Set the framebuffer address for a layer
         * 
         * @param layer2
         * @param address Framebuffer address
         */
        static void set_framebuffer(const std::span<const uint8_t>& address) {
            // convert the span to a raw address we can load into the hardware
            Lcd::port->UPBASE = reinterpret_cast<uint32_t>(address.data()) & (~0x7);
        }

        /**
         * @brief enable/disable the Lcd power
         * 
         */
        template <bool Enable>
        static void enable() {
            if constexpr (Enable) {
                Lcd::port->CTRL |= (0x1 << 11);
            }
            else {
                Lcd::port->CTRL &= ~(0x1 << 11);
            }
        }
    };
}

#endif