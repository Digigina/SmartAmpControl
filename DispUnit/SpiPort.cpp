#include "SpiPort.h"

namespace {
constexpr uint32_t spi_frequency_hz = 1'000'000;
}

SpiPort::SpiPort(spi_inst_t* spi_inst, uint8_t cs_pin, uint8_t clk_pin, uint8_t mosi_pin)
	: m_spiInst(spi_inst),
	  m_csPin(cs_pin),
	  m_clkPin(clk_pin),
	  m_mosiPin(mosi_pin)
{
}

SpiPort::~SpiPort() = default;

void SpiPort::init()
{
	spi_init(m_spiInst, spi_frequency_hz);
	gpio_set_function(m_clkPin, GPIO_FUNC_SPI);
	gpio_set_function(m_mosiPin, GPIO_FUNC_SPI);

	gpio_init(m_csPin);
	gpio_set_dir(m_csPin, GPIO_OUT);
	gpio_put(m_csPin, 1);
}

void SpiPort::writeData(uint8_t data)
{
	spi_write_blocking(m_spiInst, &data, 1);
}

void SpiPort::writeDataBuffer(const uint8_t* data, size_t length)
{
	spi_write_blocking(m_spiInst, data, length);
}

void SpiPort::chipSelect(bool active)
{
	gpio_put(m_csPin, !active);
}
