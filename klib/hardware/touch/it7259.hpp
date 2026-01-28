#ifndef KLIB_IT7259_HPP
#define KLIB_IT7259_HPP

#include <optional>
#include <algorithm>
#include <array>

#include <klib/vector2.hpp>
#include <klib/multispan.hpp>
#include <klib/math.hpp>
#include <klib/delay.hpp>

namespace klib::hardware::touch {
    template <typename Bus, typename RstPin>
    class it7259 {
    public:
        enum class format_tag: uint8_t {
            point_data = 0b0000,
            gesture = 0b1000,
            touch_event = 0b0100,
            wakeup = 0b0001,
        };

        /**
         * @brief Pressure contact of a touch point
         * 
         */
        enum class pressure_contact: uint8_t {
            no_contact = 0x0,
            hovering = 0x1 << 0,
            light = 0x1 << 1,
            normal = 0x1 << 2,
            high = 0x1 << 3,
            heavy = 0xf,
        };

        /**
         * @brief Single touch point
         *
         */
        struct touch_point {
            // x coordinate
            uint16_t x:12;

            // y coordinate
            uint16_t y:12;

            // touch information for this point
            pressure_contact contact;

            /**
             * @brief Construct a new touch point from seperate values
             *
             * @param contact
             * @param x
             * @param y
             */
            constexpr touch_point(pressure_contact contact, uint16_t x, uint16_t y):
                contact(contact), x(x), y(y)
            {}

            /**
             * @brief 
             * 
             */
            constexpr touch_point(const uint32_t packet):
                touch_point(
                    static_cast<pressure_contact>((packet >> 24) & 0xf),
                    (packet & 0xfff),
                    ((packet >> 12) & 0xfff)
                )
            {}
        };

        /**
         * @brief Point data report format
         * 
         */
        struct point_data {
            // flag if a finger is pressing the screen
            bool finger;

            // flag if a palm is detected 
            bool palm;

            // bitfield for the points that are available
            uint8_t point_info: 4;

            // all the points
            std::array<touch_point, 3> points;
        };

        /**
         * @brief Full data report. Data is based on the tag
         * 
         */
        struct data_report {
            // format tag
            format_tag tag;

            // the data. Data is based on the tag
            union {
                point_data point;
            };
        };

    protected:
        // 7-bit base address of the it7259
        const uint8_t address = 0x46;

        // using for the callback function
        using interrupt_callback = void (*)(const data_report&);

        // callback when a touch packet happens
        interrupt_callback touch_callback = nullptr;

        /**
         * @brief Available buffers
         * 
         * @note only the command buffer can be written to
         * 
         */
        enum class buffer: uint8_t {
            command = 0x20,
            query = 0x80,
            command_response = 0xa0,
            point_info = 0xe0 
        };

        /**
         * @brief Available commands
         * 
         */
        enum class command {
            device_name = 0x00,
            get_cap_sensor_information = 0x01,
            set_cap_sensor_information = 0x02,
            set_power_mode = 0x04,
            get_variable_value = 0x05,
            set_variable_value = 0x06,
            reset_queue = 0x07,
            enter_exit_pure_command_mode = 0x08,
            set_start_offset_of_flash = 0x09,
            read_flash = 0x0b,
            reinitialize_firmware = 0x0c,
            write_memory_register = 0x0d,
            read_memory_register = 0x0e,
            enter_exit_charge_mode = 0x0f,
            enter_exit_gsm_mode = 0x10,
            get_algorithm_parameter = 0x14,
            set_algorithm_parameter = 0x15,
            write_start = 0x16,
            write_continue = 0x17,
            read_start = 0x18,
            read_continue = 0x19,
            function_test = 0x1a,
            auto_tune_cdc = 0x1c,
        };

        enum class query_result: uint8_t {
            done = 0b00,
            busy = 0b01,
            error = 0b10,
            reserved = 0b11,
        };

        /**
         * @brief Read the current data in the query buffer
         * 
         * @return std::optional<query_result> 
         */
        std::optional<query_result> get_query_buffer() {
            const buffer query_buffer = buffer::query;
            query_result response = query_result::busy;

            // read the response from the device
            if (!Bus::write_read(address,
                std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(&query_buffer), sizeof(query_buffer)),
                std::span<uint8_t>(reinterpret_cast<uint8_t*>(&response), sizeof(response)))) 
            {
                return {};
            }

            return response;
        }

        /**
         * @brief Reads the bus until the command is done
         * 
         * @return query_result 
         */
        query_result wait_for_query_not_busy() {
            while (true) {
                // get the current query buffer data
                const auto res = get_query_buffer();

                // check if we got a nack. If we have
                // a nack the hardware is not ready
                // and we need to retry again. Same for
                // if we are busy
                if (!res || *res == query_result::busy) {
                    continue;
                }

                // we have some other result. Return
                // it so the callee can do what they 
                // want with it
                return *res;
            }
        }

        /**
         * @brief function to write commands to the i2c bus. length of the data is calculated automaticly
         *
         * @tparam Args
         * @param args
         */
        template <typename T, typename G>
        bool write_read_command(const command cmd, const T& tx, const G& rx) {
            // create a 2-byte array with buffer type and command
            const uint8_t header[] = {
                static_cast<uint8_t>(buffer::command),
                static_cast<uint8_t>(cmd)
            };

            // create a multispan with the header and the data
            const auto command = make_span_array(
                std::span<const uint8_t>{header, sizeof(header)}, 
                std::span<const uint8_t>(tx)
            );

            // send the array to the bus
            if (!Bus::write(address, command)) {
                return false;
            }

            // klib::delay(klib::time::ms(1));

            // wait until the command is done
            while (true) {
                bool complete = false;

                // wait until we are not busy anymore
                switch (wait_for_query_not_busy()) {
                    case query_result::done:
                        complete = true;
                        break;
                    default:
                        // for all other cases we
                        // return a error
                        return false;
                }

                if (complete) {
                    break;
                }
            }

            // check if we have data to read
            if (!rx.size()) {
                return true;
            } 

            // convert all the parameters to an array with the length at the front
            const uint8_t response_cmd = (static_cast<uint8_t>(buffer::command_response));

            // read the data from the command
            return Bus::write_read(address, std::span<const uint8_t>(&response_cmd, sizeof(response_cmd)), rx);
        }
        
    public:
        /**
         * @brief Construct a it7259 object
         *
         * @param address address of the it7259 on the bus
         */
        it7259(const uint8_t address = 0x46): 
            address(address) 
        {}
        
        /**
         * @brief Init the it7259
         * 
         * @return true 
         * @return false 
         */
        bool init(const interrupt_callback callback = nullptr) {
            // make sure we are not busy
            if (wait_for_query_not_busy() != query_result::done) {
                return false;
            }

            // check if the device name matches
            const auto name = get_device_name();

            // the expected manufacturer name
            constexpr std::array<uint8_t, 3> manufacturer = {'I', 'T', 'E'};

            // check if the device name looks correct
            if (!name || !std::equal(manufacturer.begin(), manufacturer.end(), name->begin())) {
                return false;
            }

            // register the callback
            touch_callback = callback;

            return true;
        }

        /**
         * @brief Get the device name of the it7259
         * 
         * @return std::optional<std::array<uint8_t, 0xa - 1>> 
         */
        std::optional<std::array<uint8_t, 0x9>> get_device_name() {
            // buffer includes length byte at position 0
            std::array<uint8_t, 0x1> length;
            std::array<uint8_t, 0x9> buffer;

            // create a multispan that has both arrays inside. This
            // is so we dont have to remove the length in the 
            // received payload.
            auto buf = make_span_array(
                std::span<uint8_t>(length), std::span<uint8_t>(buffer)
            );

            // try to read the device name from the device
            if (!write_read_command(command::device_name, std::span<const uint8_t>{}, buf)) {
                return {};
            }
            
            return buffer;
        }

        /**
         * @brief Poll the active data from the it7259
         * 
         * @return std::optional<data_report> 
         */
        std::optional<data_report> poll() {
            const std::array<uint8_t, 1> tx = {static_cast<uint8_t>(buffer::point_info)};
            std::array<uint8_t, 13> buffer;

            // read the data from the pointer buffer
            if (!Bus::write_read(address, tx, buffer)) {
                return {};
            }

            data_report ret = {
                .tag = static_cast<format_tag>((buffer[0] >> 4) & 0xf)
            };

            switch (ret.tag) {
                case format_tag::point_data: {
                    const uint8_t count = buffer[0] & 0x3;

                    ret.point.finger = (buffer[0] >> 3) & 0x1;
                    ret.point.palm = buffer[1] & 0x1;
                    ret.point.point_info = count;

                    // check if we have 
                    if (klib::popcount(count) == 3 || !klib::popcount(count)) {
                        return ret;
                    }

                    // copy all the points 
                    for (uint8_t i = 0; i < ret.point.points.size(); i++) {
                        // check if this point is active
                        if (!(count & (0x1 << i))) {
                            continue;
                        }

                        // get the raw data from the buffer
                        const uint32_t& packet = *reinterpret_cast<const uint32_t*>(
                            &buffer[i * 4 + 2]
                        );

                        ret.point.points[i] = touch_point(packet);
                    }

                    break;
                }
                // case format_tag::gesture:
                //     break;
                // case format_tag::touch_event:
                //     break;
                // case format_tag::wakeup:
                //     break;
                default:
                    break;
            }

            return ret;
        }

        /**
         * @brief Get the resolution of the touch screen
         * 
         * @return klib::vector2u 
         */
        std::optional<klib::vector2u> get_resolution() {
            // buffer to receive the capacitive sensor information
            std::array<uint8_t, 1> sub_command = {0x02};
            std::array<uint8_t, 14> buffer;

            // read the capacitive sensor information
            if (!write_read_command(command::get_cap_sensor_information, sub_command, std::span<uint8_t>(buffer))) {
                return {};
            }

            // extract resolution from the response
            const uint16_t x_res = buffer[2] | (buffer[3] << 8);
            const uint16_t y_res = buffer[4] | (buffer[5] << 8);

            return klib::vector2u{x_res, y_res};
        }

        /**
         * @brief Get the scale of the touch screen
         * 
         * @return uint8_t 
         */
        std::optional<uint8_t> get_scale() {
            // buffer to receive the capacitive sensor information
            std::array<uint8_t, 1> sub_command = {0x02};
            std::array<uint8_t, 14> buffer;

            // read the capacitive sensor information
            if (!write_read_command(command::get_cap_sensor_information, sub_command, std::span<uint8_t>(buffer))) {
                return {};
            }

            return buffer[6];
        }

        /**
         * @brief Enter/Exit sleep mode
         * 
         * @tparam Enter 
         */
        template <bool Enter>
        void sleep() {
            // set the irq pin low
            RstPin::template set<Enter>();
        }

        /**
         * @brief IRQ handler to call when the it7259 interrupt pin gets triggered.
         * User needs to call this function periodically or use a falling interrupt
         * on the irq pin.
         *
         */
        template <typename TouchHelper>
        void irq_handler() {
            // poll the it7259
            const auto data = poll();

            if (!data) {
                return;
            }

            // ignore all tags that are not point data
            if (data->tag != format_tag::point_data) {
                return;
            }

            // check if we have any points
            if (!data->point.point_info) {
                TouchHelper::unpress_all();

                return;
            }

            // iterate through all the points
            for (uint8_t i = 0; i < data->point.points.size(); i++) {
                const auto& p = data->point.points[i];

                if (p.contact == pressure_contact::no_contact) {
                    // unpress the touch point
                    TouchHelper::update(i);
                }
                else {
                    TouchHelper::update(i, {p.x, p.y});
                }
            }
        }
    };
}

#endif
