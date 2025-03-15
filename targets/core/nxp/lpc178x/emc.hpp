#ifndef KLIB_NXP_LPC178X_EMC_HPP
#define KLIB_NXP_LPC178X_EMC_HPP

#include <tuple>
#include <bit>

#include <klib/klib.hpp>
#include <klib/units.hpp>
#include <klib/delay.hpp>
#include <klib/io/peripheral.hpp>
#include <klib/io/port.hpp>
#include <klib/io/core_clock.hpp>

#include <io/power.hpp>

namespace klib::core::lpc178x::io {
    template <typename Emc, uint8_t AddressBits, uint8_t DataBits, bool HalfCPUClock = false>
    class emc {
    public:
        enum class mode: uint8_t {
            dynamic_memory,
            static_memory
        };

        struct emc_config {
            // programmable delay for the emc output. 
            // Actual delay is roughly 250 ps * (value + 1)
            uint8_t cmd_delay: 5;

            // programmable feedback clock delay that
            // controls input data sampling. 
            // Actual delay is roughly 250 ps * (value + 1)
            uint8_t feedback_clk_delay: 5;

            // programmable delay for the clock out 0 output. 
            // Actual delay is roughly 250 ps * (value + 1)
            uint8_t clk_out0_delay: 5;

            // programmable delay for the clock out 1 output. 
            // Actual delay is roughly 250 ps * (value + 1)
            uint8_t clk_out1_delay: 5;
        };

        /**
         * @brief Available memory sizes
         * 
         */
        enum class memory_size: uint8_t {
            mbit_16 = 0b000,
            mbit_64 = 0b001,
            mbit_128 = 0b010,
            mbit_256 = 0b011,
            mbit_512 = 0b100,
        };

        /**
         * @brief Available bus widths
         * 
         */
        enum class bus_width: uint8_t {
            x8 = 0b00,
            x16 = 0b01,
            x32 = 0b10
        };

        /**
         * @brief The mapping
         * 
         */
        enum class column_row_mapping: uint8_t {
            row_bank_column = 0b0,
            bank_row_column = 0b1,
        };

        /**
         * @brief Allowed emc latencies for the ras and cas
         * 
         */
        enum class latency: uint8_t {
            one_cycle = 0x1,
            two_cycles = 0x2,
            three_cycles = 0x3,
        };

        struct dynamic_memory {
            // flag if we are using 32 bit external address mapping
            bool bit32;

            // column row mapping
            column_row_mapping mapping;

            // bus width
            bus_width width;

            // memory size
            memory_size size;

            // ras and cas latencies
            latency ras;
            latency cas;

            // sdram timing in nsec
            klib::time::ns trp;
            klib::time::ns tras;
            klib::time::ns tsrex;
            klib::time::ns tapr;
            klib::time::ns tdal;
            klib::time::ns twr;
            klib::time::ns trc;
            klib::time::ns trfc;
            klib::time::ns txsr;
            klib::time::ns trrd;
            klib::time::ns tmrd;

            // the refresh period
            klib::time::ns refresh_period;

            // flag if the memory is write protected
            bool write_protected = false;

            // flag if we are using low power sdram
            bool low_power_sdram = false;
        };

    protected:
        static_assert(AddressBits <= std::tuple_size<typename Emc::address_pins>::value, "Invalid amount of address bits given");
        static_assert(DataBits <= std::tuple_size<typename Emc::data_pins>::value, "Invalid amount of data bits given");

        /**
         * @brief Helper struct for easier access to the dynamic 
         * memory configuration
         * 
         */
        struct dyn_memory {
            // memory configuration
            volatile uint32_t config;
            
            // ras and cas latencies
            volatile uint32_t ras_cas;

            // padding between fields
            const uint32_t padding[6];
        };

        /**
         * @brief Get a pointer to the correct dynamic memory config
         * 
         * @tparam Index 
         * @return volatile* 
         */
        template <uint8_t Index>
        constexpr static volatile dyn_memory* get_dynamic_memory_config() {
            return reinterpret_cast<volatile dyn_memory*>(&Emc::port->DYNAMICCONFIG0);
        }

        /**
         * @brief Helper function to set a tuple of pins to the 
         * correct peripheral
         * 
         * @tparam Pins 
         * @tparam Bits 
         * @tparam Index 
         */
        template <typename Pins, uint8_t Bits, uint8_t Index = 0>
        constexpr static void set_peripheral_helper() {
            static_assert(Bits != 0, "Invalid bits input");
            static_assert(Bits <= std::tuple_size<Pins>::value, "Invalid amount of address bits given");

            // get the type of the current pin
            using pin = std::tuple_element<Index, Pins>::type;

            // configure all the required pins
            target::io::detail::pins::set_peripheral<typename pin::pin, typename pin::periph>();

            // check if any bits are left
            if constexpr ((Index + 1) < Bits) {
                // set the next pin
                set_peripheral_helper<Pins, Bits, Index + 1>();
            }
        }

        template <typename Pin, typename Pins>
        constexpr static void set_peripheral_pin() {
            // get the index of the pin in the array
            constexpr uint32_t index = klib::io::peripheral::get_index<Pin, Pins>();

            // get the type of the pin using the index
            using pin = std::tuple_element<index, Pins>::type;

            // setup the pin to the correct alternate function
            target::io::detail::pins::set_peripheral<typename pin::pin, typename pin::periph>();
        }

        constexpr static uint32_t ns_to_cycles(const klib::time::ns time, const uint32_t ps_per_cycle) {
            // special case for 0 ns
            if (!time.value) {
                return 0;
            }

            // convert ns to picoseconds (add 1 cycle for the rounding)
            return ((time.value * 1000) / ps_per_cycle) + 1;
        }

        static void update_dynamic_registers(const uint32_t ps_per_cycle, const dynamic_memory& memory) {
            // create a array with all the options in order
            const klib::time::ns options[] = {
                memory.trp, memory.tras, memory.tsrex, memory.tapr,
                memory.twr, memory.trc, memory.trfc, memory.txsr, 
                memory.trrd, memory.tmrd,
            };

            // array to all the options.
            volatile uint32_t* option_register[] = {
                &Emc::port->DYNAMICRP, &Emc::port->DYNAMICRAS, 
                &Emc::port->DYNAMICSREX, &Emc::port->DYNAMICAPR,
                &Emc::port->DYNAMICWR, &Emc::port->DYNAMICRC, 
                &Emc::port->DYNAMICRFC, &Emc::port->DYNAMICXSR, 
                &Emc::port->DYNAMICRRD, &Emc::port->DYNAMICMRD,
            };

            // make sure the size is the same
            static_assert((sizeof(option_register) / sizeof(option_register[0])) == (sizeof(options) / sizeof(options[0])), "arrays are not the same size");

            // write all the options
            for (uint32_t i = 0; i < (sizeof(options) / sizeof(options[0])); i++) {
                // calculate the amount of cycles we need for this specific option
                // we remove 1 cycle as the hardware adds 1 cycle
                const uint32_t res = ns_to_cycles(options[i], ps_per_cycle) - 1;
                const uint32_t value = (*option_register[i]);

                // only update the value if it is higher than the one in the register
                if (res > value) {
                    (*option_register[i]) = res;
                }
            }

            // special case for the tdal as that does not have + 1 cycle in hardware
            const uint32_t res = ns_to_cycles(memory.tdal, ps_per_cycle);
            const uint32_t value = Emc::port->DYNAMICDAL;
        
            // only update the value if it is higher than the one in the register
            if (res > value) {
                Emc::port->DYNAMICDAL = res;
            }
        }

        constexpr static uint32_t calculate_dynamic_period_cycles(const uint32_t ps_per_cycle, const klib::time::ns period) {
            // get the amount of cycles in the period. We need to convert the ns to ps
            // but we need to do a divide by 1000 afterwards so no need to do it. 
            // Afterwards we do a divide by 16 as the hardware does a multiply by 16
            const uint32_t p = ((period.value / ps_per_cycle) / 16);

            if (p > 0x7ff) {
                return 0x7ff;
            }

            return p;
        }

        template <mode Mode, uint8_t Index>
        constexpr static uint32_t get_address_helper() {
            if constexpr (Mode == mode::dynamic_memory) {
                return Emc::dynamic_addresses[Index];
            }
            else {
                return Emc::static_addresses[Index];
            }
        }

    public:
        /**
         * @brief Init the emc peripheral. This function should always 
         * be called before calling any of the other init functions
         * 
         * @details this only inits the shared features of the peripheral 
         * (from both static and dynamic)
         * 
         */
        template <std::endian Endian = std::endian::native>
        static void init(const emc_config& config) {
            // enable power to the emc peripheral
            target::io::power_control::enable<Emc>;

            // configure the emc clock. If we the half clock flag is 
            // true we need to use half the cpu clock
            SYSCON->EMCCLKSEL = HalfCPUClock;

            // call the periperal helpers to setup all the address and data pins
            set_peripheral_helper<typename Emc::address_pins, AddressBits>();
            set_peripheral_helper<typename Emc::data_pins, DataBits>();

            // configure the write enable pin
            target::io::detail::pins::set_peripheral<typename Emc::we::pin, typename Emc::we::periph>();

            // enable the emc using the control register
            Emc::port->CONTROL = 0x1;

            // set the endian mode
            Emc::port->CONFIG = (Endian == std::endian::big);

            // reset all the dynamic configurations to the minimum value
            SYSCON->EMCDLYCTL = (
                config.cmd_delay | 
                (config.feedback_clk_delay << 8) |
                (config.clk_out0_delay << 16) | 
                (config.clk_out1_delay << 24)
            );

            // we set this to the minimum value so we can overwrite them 
            // later if higher values are needed for a specific sdram
            Emc::port->DYNAMICRP = 0x00;
            Emc::port->DYNAMICRAS = 0x00;
            Emc::port->DYNAMICSREX = 0x00;
            Emc::port->DYNAMICAPR = 0x00;
            Emc::port->DYNAMICDAL = 0x00;
            Emc::port->DYNAMICWR = 0x00;
            Emc::port->DYNAMICRC = 0x00;
            Emc::port->DYNAMICRFC = 0x00;
            Emc::port->DYNAMICXSR = 0x00;
            Emc::port->DYNAMICRRD = 0x00;
            Emc::port->DYNAMICMRD = 0x00;
        }

        template <std::endian Endian = std::endian::native>
        static void init() {
            // call the other init function with the default values
            init<Endian>({0x10, 0x02, 0x00, 0x00});
        }

        template <mode Mode, typename Cs, typename Cke, typename Clk>
        static void init(const dynamic_memory& memory) {
            // make sure we are the correct mode
            static_assert(Mode == mode::dynamic_memory, "Invalid init called for static memory");

            // calculate the clocks based on the current clock frequency
            const auto current_clock = klib::io::clock::get();

            // calculate how many pico seconds every clock takes
            const uint32_t ps = 1'000'000'000'000 / (current_clock / (HalfCPUClock + 1));

            // setup all the dynamic memory pins
            target::io::detail::pins::set_peripheral<typename Emc::cas::pin, typename Emc::cas::periph>();
            target::io::detail::pins::set_peripheral<typename Emc::ras::pin, typename Emc::ras::periph>();

            // configure the correct amount of byte lane select pins
            set_peripheral_helper<typename Emc::dqm_pins, DataBits / 8>();

            // setup the cke pin
            set_peripheral_pin<Cke, typename Emc::cke_pins>();

            // setup the clk pin
            set_peripheral_pin<Clk, typename Emc::clk_pins>();

            // setup the chip select pin
            set_peripheral_pin<Cs, typename Emc::dynamic_cs_pins>();

            // get the chip select pin index
            constexpr static uint32_t chip_select = klib::io::peripheral::get_index<Cs, typename Emc::dynamic_cs_pins>();

            // set the memory configuration to use the command delayed strategy
            Emc::port->DYNAMICREADCONFIG = 0x1;

            // configure the memory
            auto *const dyn = get_dynamic_memory_config<chip_select>(); 

            // create the memory configuration
            const uint32_t config = (
                (memory.low_power_sdram << 3) | (static_cast<uint32_t>(memory.width) << 7) | 
                (static_cast<uint32_t>(memory.size) << 9) | (static_cast<uint32_t>(memory.mapping) << 12) | 
                (memory.bit32 << 14)
            );

            // set the memory configuration (with the buffer disabled)
            dyn->config = config;

            // setup the ras and cas latencies
            dyn->ras_cas = static_cast<uint32_t>(memory.ras) | (static_cast<uint32_t>(memory.cas) << 8);

            // update the global timing registers if our current values are higher 
            // than the one in the register
            update_dynamic_registers(ps, memory);

            // wait 100us
            klib::delay(klib::time::us(100));

            Emc::port->DYNAMICCONTROL = 0x00000183; /* Issue NOP command */

            // wait 200us
            klib::delay(klib::time::us(200));

            Emc::port->DYNAMICCONTROL = 0x00000103; /* Issue PALL command */
            Emc::port->DYNAMICREFRESH = 0x00000002; /* ( n * 16 ) -> 32 clock cycles */

            /* wait 128 AHB clock cycles */
            for (uint32_t i = 0; i < 0x80; i++) {
                asm volatile("");
            }

            // set the refresh period to the one provided
            Emc::port->DYNAMICREFRESH = calculate_dynamic_period_cycles(ps, memory.refresh_period);

            Emc::port->DYNAMICCONTROL = 0x00000083; /* Issue MODE command */

            const uint32_t address = get_address_helper<Mode, chip_select>();

            (void) *((volatile uint32_t *)(address | (0x22<<(2+2+9)))); /* 4 burst, 2 CAS latency */


            // // 
            // if (memory.mapping == column_row_mapping::row_bank_column) {
            //     (void) *((volatile uint32_t *)(get_address_helper<Mode, chip_select>() | (0x33<<(2+2+9)))); /* 4 burst, 2 CAS latency */
            // }
            // else {

            // }

            Emc::port->DYNAMICCONTROL = 0x00000000; /* Issue NORMAL command */




            // enable the buffers with the same config
            dyn->config = config | (0x1 << 19);

            // TODO: calibrate the sdram
        }

        template <mode Mode, typename Cs>
        static void init() {
            // make sure we are the correct mode
            static_assert(Mode == mode::static_memory, "Invalid init called for static memory");

            // setup all the static memory pins
            target::io::detail::pins::set_peripheral<typename Emc::oe::pin, typename Emc::oe::periph>();

            // configure the correct amount of byte lane select pins
            set_peripheral_helper<typename Emc::bls_pins, DataBits / 8>();

            // get and check if the chip select pin is available in 
            // the dynamic cs pins tuple
            using cs_pin = std::tuple_element<
                klib::io::peripheral::get_index<Cs, typename Emc::cs_pins>(), typename Emc::cs_pins
            >::type;

            // confgure the dynamic chip select pin
            target::io::detail::pins::set_peripheral<typename cs_pin::pin, typename cs_pin::periph>();
        }

        template <mode Mode, typename Cs, typename T>
        static T* get_address() {
            if constexpr (Mode == mode::dynamic_memory) {
                // get the chip select pin index
                constexpr static uint32_t chip_select = klib::io::peripheral::get_index<Cs, typename Emc::dynamic_cs_pins>();

                // return the address as a the requested type
                return reinterpret_cast<T*>(get_address_helper<Mode, chip_select>());
            }
            else {
                // get the chip select pin index
                constexpr static uint32_t chip_select = klib::io::peripheral::get_index<Cs, typename Emc::cs_pins>();

                // return the address as a the requested type
                return reinterpret_cast<T*>(get_address_helper<Mode, chip_select>());
            }
        }
    };
}

#endif
