#ifndef KLIB_NXP_LPC178X_EMC_HPP
#define KLIB_NXP_LPC178X_EMC_HPP

#include <tuple>
#include <optional>

#include <klib/delay.hpp>
#include <klib/io/core_clock.hpp>

#include <io/power.hpp>
#include <io/port.hpp>

namespace klib::core::lpc178x::io {
    template <typename Emc, uint8_t AddressBits, uint8_t DataBits, bool HalfCPUClock = false>
    class emc {
    public:
        /**
         * @brief Different types of memory that are supported
         * 
         */
        enum class mode: uint8_t {
            dynamic_memory,
            static_memory
        };

        /**
         * @brief EMC timing configuration
         * 
         */
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

        /**
         * @brief Configuration for dynamic memory
         * 
         */
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

        /**
         * @brief Convert memory size to bytes
         * 
         * @param size 
         * @return constexpr uint32_t 
         */
        constexpr static uint32_t mbit_to_bytes(const memory_size size) {
            switch (size) {
                case memory_size::mbit_16:
                    return 2 * 1024 * 1024;
                case memory_size::mbit_64:
                    return 8 * 1024 * 1024;
                case memory_size::mbit_128:
                    return 16 * 1024 * 1024;
                case memory_size::mbit_256:
                    return 32 * 1024 * 1024;
                case memory_size::mbit_512:
                    return 64 * 1024 * 1024;
            }

            return 0;
        }

    protected:
        static_assert(AddressBits <= std::tuple_size<typename Emc::address_pins>::value, "Invalid amount of address bits given");
        static_assert(DataBits <= std::tuple_size<typename Emc::data_pins>::value, "Invalid amount of data bits given");

        /**
         * @brief dynamic memory commands
         * 
         */
        class dyn_cmd {
        public:
            enum {
                normal = 0x0,
                mode = 0x1,
                pall = 0x2,
                nop = 0x3,
            };
        };

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

        template <uint32_t Cycles = 10>
        static uint32_t emc_calibration() {
            uint32_t ret = 0;

            // run the calibration cycles
            for (uint32_t i = 0; i < Cycles; i++) {
                // set the bit to start calibration cycle
                SYSCON->EMCCAL = SYSCON->EMCCAL | (0x1 << 14);

                // wait until the calibration cycle is done
                while (!(SYSCON->EMCCAL & (0x1 << 15))) {}

                // read the result
                ret += (SYSCON->EMCCAL & 0xFF);
            }

            return (ret / Cycles);
        }

        static bool find_command_delay(uint32_t* address, const uint32_t size) {
            // we want to find the first value that works. If all the values work
            // we take the lowest value
            for (uint32_t current = 0x0; current < 32; current++) {
                // only update the feedback clock delay
                SYSCON->EMCDLYCTL = (SYSCON->EMCDLYCTL & ~0x1f) | current;

                // do a memory test
                if (memory_test_impl(address, size)) {
                    return true;
                }
            }

            return false;
        }

        static bool find_feedback_delay(uint32_t* address, const uint32_t size) {
            // we want to find the first value that works. If all the values work
            // we take the lowest value
            for (uint32_t current = 0x0; current < 32; current++) {
                // only update the feedback clock delay
                SYSCON->EMCDLYCTL = (SYSCON->EMCDLYCTL & ~0x1f00) | (current << 8);

                // do a memory test
                if (memory_test_impl(address, size)) {
                    return true;
                }
            }

            return false;
        }

        /**
         * @brief Internal memory test implementation
         * 
         * @param address 
         * @param size in bytes
         * @return true 
         * @return false 
         */
        static bool memory_test_impl(uint32_t* address, const uint32_t size) {
            const uint32_t s = size / sizeof(uint32_t);

            // initialize the address with a pattern
            for (uint32_t i = 0; i < s; i++) {
                address[i] = i;
            }

            // verify the pattern
            for (uint32_t i = 0; i < s; i++) {
                if (address[i] != i) {
                    return false;
                }
            }

            return true;
        }

    public:
        /**
         * @brief Init the emc peripheral. This function should always 
         * be called before calling any of the other init functions and
         * should only be called once.
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

            // set all the dynamic configurations
            SYSCON->EMCDLYCTL = *reinterpret_cast<const uint32_t*>(&config);

            // enable the emc using the control register
            Emc::port->CONTROL = 0x1;

            // set the endian mode
            Emc::port->CONFIG = (Endian == std::endian::big);

            // call the periperal helpers to setup all the address and data pins
            set_peripheral_helper<typename Emc::address_pins, AddressBits>();
            set_peripheral_helper<typename Emc::data_pins, DataBits>();

            // configure the write enable pin
            target::io::detail::pins::set_peripheral<typename Emc::we::pin, typename Emc::we::periph>();

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
            Emc::port->DYNAMICREFRESH = 0x00;
        }

        /**
         * @brief Init a specific dynamic memory region
         * 
         * @tparam Mode 
         * @tparam Cs 
         * @tparam Cke 
         * @tparam Clk 
         * @tparam Memory
         * @param memory 
         * @return true 
         * @return false 
         */
        template <mode Mode, typename Cs, typename Cke, typename Clk, typename Memory>
        static bool init(const dynamic_memory& memory) {
            // make sure we are the correct mode
            static_assert(Mode == mode::dynamic_memory, "Invalid init called for static memory");

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

            // set the memory configuration to use the command delayed strategy
            Emc::port->DYNAMICREADCONFIG = 0x1;

            // calculate the clocks based on the current clock frequency
            const auto current_clock = klib::io::clock::get();

            // calculate how many pico seconds every clock takes
            const uint32_t ps = 1'000'000'000'000 / (current_clock / (HalfCPUClock + 1));

            // update the global timing registers if our current values are higher 
            // than the one in the register
            update_dynamic_registers(ps, memory);

            // wait 100us
            klib::delay(klib::time::us(100));

            // issue a nop command
            Emc::port->DYNAMICCONTROL = (dyn_cmd::nop << 7) | (0b11);

            // wait 200us
            klib::delay(klib::time::us(200));

            // issue a pall command
            Emc::port->DYNAMICCONTROL = (dyn_cmd::pall << 7) | (0b11);

            // set the refresh period if it is higher than the current one. We 
            // need to set the worst case value for all the chip selects
            Emc::port->DYNAMICREFRESH = klib::max(
                calculate_dynamic_period_cycles(ps, memory.refresh_period),
                Emc::port->DYNAMICREFRESH
            );

            // issue a mode command
            Emc::port->DYNAMICCONTROL = (dyn_cmd::mode << 7) | (0b11);

            // get the address for the current chip select
            uint32_t* address = get_address<mode::dynamic_memory, Cs, uint32_t>();

            // setup the dynamic memory using the memory template
            Memory::setup(address);

            // issue a normal command
            Emc::port->DYNAMICCONTROL = dyn_cmd::normal; 

            // enable the buffers with the same config
            dyn->config = config | (0x1 << 19);

            return true;
        }

        /**
         * @brief Get a pointer to the start address of the memory region
         * 
         * @tparam Mode 
         * @tparam Cs 
         * @tparam T 
         * @return T* 
         */
        template <mode Mode, typename Cs, typename T = uint32_t>
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

        /**
         * @brief Run a memory test on the given memory region
         * 
         * @param offset to the start of the memory region in bytes 
         * @param size of the memory test in bytes
         * @return result 
         */
        template <mode Mode, typename Cs>
        static bool memory_test(const uint32_t offset, const uint32_t size) {
            // get a pointer to the start address
            uint32_t *const address = get_address<Mode, Cs, uint32_t>() + (offset / sizeof(uint32_t));

            // return the result of the memory test
            return memory_test_impl(address, size);
        }

       /**
         * @brief Calibrate the emc delay control on a memory region
         * 
         * @param offset to the start of the memory region in bytes 
         * @param size of the memory test in bytes
         * @return result
         */
        template <mode Mode, typename Cs>
        static std::optional<emc_config> calibrate_delay_control(const uint32_t offset, const uint32_t size) {
            // get a pointer to the start address
            uint32_t *const address = get_address<Mode, Cs, uint32_t>() + (offset / sizeof(uint32_t));

            // store the original emc delay control register so we can restore it later
            const auto original = *const_cast<uint32_t*>(&(SYSCON->EMCDLYCTL));

            // do an initial calibration on the emc
            const uint32_t cal_start = emc_calibration();

            // find the command delay
            if (!find_command_delay(address, size)) {
                // restore the original emc delay control register
                SYSCON->EMCDLYCTL = original;

                return {};
            }

            // find the feedback clock delay
            if (!find_feedback_delay(address, size)) {
                // restore the original emc delay control register
                SYSCON->EMCDLYCTL = original;

                return {};
            }

            // do another calibration after setting the delays
            const uint32_t cal_end = emc_calibration();

            // get the current emc delay control register
            auto c = *reinterpret_cast<const emc_config*>(
                const_cast<uint32_t*>(&(SYSCON->EMCDLYCTL))
            );

            // update the command and feedback clock delays. Correct using the
            // change in calibration values
            c.cmd_delay = ((c.cmd_delay * cal_start) / cal_end);
            c.feedback_clk_delay = ((c.feedback_clk_delay * cal_start) / cal_end);

            // restore the original emc delay control register
            SYSCON->EMCDLYCTL = original;

            // return the configuration
            return c;
        }
    };
}

#endif