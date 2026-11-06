#include "AdafruitRotaryQT.h"

namespace {
constexpr uint8_t gpio_module = 0x01;
constexpr uint8_t gpio_bulk_read = 0x04;
constexpr uint8_t gpio_direction_clear_bulk = 0x03;
constexpr uint8_t gpio_bulk_set = 0x05;
constexpr uint8_t gpio_pull_enable_set = 0x0B;
constexpr uint8_t encoder_module = 0x11;
constexpr uint8_t encoder_position_register = 0x30;
constexpr uint8_t button_pin = 24;
}

AdafruitRotaryQT::AdafruitRotaryQT(i2c_inst_t* i2c_inst, uint8_t i2c_adr)
	: StepEncBase(i2c_inst, i2c_adr)
{
}

AdafruitRotaryQT::~AdafruitRotaryQT() = default;

bool AdafruitRotaryQT::initEncoder()
{
	const uint8_t position_command[] = {encoder_module, encoder_position_register};
	const uint8_t initial_position[] = {0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t result[4];

	i2c_write_blocking(m_i2cInst, m_i2cAdr, position_command, sizeof(position_command), true);
	i2c_write_blocking(m_i2cInst, m_i2cAdr, initial_position, sizeof(initial_position), false);

	const uint32_t button_mask = 1u << button_pin;
	uint8_t gpio_command[] = {
		gpio_module,
		gpio_direction_clear_bulk,
		static_cast<uint8_t>(button_mask >> 24),
		static_cast<uint8_t>(button_mask >> 16),
		static_cast<uint8_t>(button_mask >> 8),
		static_cast<uint8_t>(button_mask)
	};

	i2c_write_blocking(m_i2cInst, m_i2cAdr, gpio_command, sizeof(gpio_command), false);
	gpio_command[1] = gpio_pull_enable_set;
	i2c_write_blocking(m_i2cInst, m_i2cAdr, gpio_command, sizeof(gpio_command), false);
	gpio_command[1] = gpio_bulk_set;
	i2c_write_blocking(m_i2cInst, m_i2cAdr, gpio_command, sizeof(gpio_command), false);

    sleep_ms(1);

   // get status
    gpio_command[1] = gpio_bulk_read;
    i2c_write_blocking(m_i2cInst, m_i2cAdr, gpio_command, 2, true);
    i2c_read_blocking(m_i2cInst, m_i2cAdr, result, 4, false);  // false - finished with bus
	return(result[0]&(1<<8));
}

bool AdafruitRotaryQT::getButtonState()
{
	// const uint8_t command[] = {gpio_module, gpio_bulk_read};
	// uint8_t result[4];

	// i2c_write_blocking(m_i2cInst, m_i2cAdr, command, sizeof(command), true);
	// sleep_us(100);
	// if (i2c_read_blocking(m_i2cInst, m_i2cAdr, result, sizeof(result), false) != sizeof(result)) 
	// {
	// 	return false;
	// }
	// else
	// {
	// 	return (result[0] & 0x01) == 0;
	// }
	uint8_t cmd[2];
    uint8_t buf[4];

    cmd[0] = gpio_module;
    cmd[1] = gpio_bulk_read;
    i2c_write_blocking(m_i2cInst, m_i2cAdr, cmd, 2, false);
    sleep_us(100);
    i2c_read_blocking(m_i2cInst, m_i2cAdr, buf, 4, false);  // false - finished with bus
    return(!(buf[0]&1));

}

uint32_t AdafruitRotaryQT::getPosition()
{
	const uint8_t command[] = {encoder_module, encoder_position_register};
	uint8_t result[4];
	uint32_t raw_position = 0;

	i2c_write_blocking(m_i2cInst, m_i2cAdr, command, sizeof(command), false);
	sleep_us(100);
	i2c_read_blocking(m_i2cInst, m_i2cAdr, result, sizeof(result), false) != sizeof(result);
	raw_position =
		(static_cast<uint32_t>(result[0]) << 24) |
		(static_cast<uint32_t>(result[1]) << 16) |
		(static_cast<uint32_t>(result[2]) << 8) |
		static_cast<uint32_t>(result[3]);
	return 0u - raw_position;
}


