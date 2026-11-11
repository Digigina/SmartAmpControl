#pragma GCC optimize ("O0")

#include <stdio.h>

//#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "AdafruitRotaryQT.h"
#include "LED_8SEG.h"
#include "VolumeSelector.h"
#include "ChannelSelector.h"
#include "MenueNode.h"
#include "UINode.h"
#include "VolChControl.h"
#include "CurrentSensor.h"
#include "CurrentDisplay.h"
#include "Protocol.h"

constexpr i2c_inst_t* I2C_ATT  = i2c0;
constexpr i2c_inst_t* I2C_STEP = i2c1;

constexpr uint8_t STEP_I2C_SDA_PIN = 18;
constexpr uint8_t STEP_I2C_SCL_PIN = 19;

constexpr uint8_t ATT_I2C_SDA_PIN = 8;
constexpr uint8_t ATT_I2C_SCL_PIN = 9;

constexpr uint8_t SPI_CS_PIN = 13;
constexpr uint8_t SPI_CLK_PIN = 10;
constexpr uint8_t SPI_MOSI_PIN = 11;

extern VolumeSelector  myVolumeSelector;
extern MenueNode       myMenue;
extern ChannelSelector myChannelSelector;
extern CurrentDisplay  myCurrentDisplay;

// Instanz zur Steuerung des Volumen- und Kanalumschalters
VolChControl mySwitchingDevice(I2C_ATT, 0x24);
// Instanz zur Einlesung der Step-Encoder-Daten (Drehimpulsgeber)
AdafruitRotaryQT myStepEncoder(I2C_STEP, 0x36);
StepEncBase* g_stepEnc = &myStepEncoder;
// Instanz zur Einlesung der Stromwerte (Shunt-Widerstand)
CurrentSensor myCurrentSensor(5.);
// Instanz zur Einlesung der Fernbedienungssignale (NEC-Protokoll) über PIO
RemoteProtocol myRemoteCtrl(0, &mySwitchingDevice);


SpiPort dispSpiPort(spi1, SPI_CS_PIN, SPI_CLK_PIN, SPI_MOSI_PIN);
LED_8SEG myDisplay(&dispSpiPort, 2 ); // 2ms repetition rate

MenueNodeEntry myMenueEntries[] = {
    { { LED_8SEG::SegmentCode::Blank, LED_8SEG::SegmentCode::DigitA, LED_8SEG::SegmentCode::DigitH, LED_8SEG::SegmentCode::DigitC }, &myChannelSelector },
    { { LED_8SEG::SegmentCode::Digitd, LED_8SEG::SegmentCode::Digitu, LED_8SEG::SegmentCode::Digito, LED_8SEG::SegmentCode::DigitL }, &myVolumeSelector},
    { { LED_8SEG::SegmentCode::Digitr, LED_8SEG::SegmentCode::Digitr, LED_8SEG::SegmentCode::Digitu, LED_8SEG::SegmentCode::DigitC }, &myCurrentDisplay},
    { { LED_8SEG::SegmentCode::Blank, LED_8SEG::SegmentCode::Blank, LED_8SEG::SegmentCode::Blank, LED_8SEG::SegmentCode::Blank }, nullptr }
};

MenueNode myMenue(&myVolumeSelector, &myMenueEntries[0]);

VolumeSelector myVolumeSelector(&myMenue, &mySwitchingDevice);
ChannelSelector myChannelSelector(&myVolumeSelector, &mySwitchingDevice);
CurrentDisplay  myCurrentDisplay(&myVolumeSelector, &myCurrentSensor);

int main() {
    stdio_init_all();

    // useful information for picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("BMP280 I2C example for the Raspberry Pi Pico"));

    UINode::m_currentNode = &myVolumeSelector;

    myDisplay.init();   
    myCurrentSensor.init(); 
 
    // I2C is "open drain", pull ups to keep signal high when no data is being sent
    i2c_init(I2C_STEP, 100 * 1000);
    gpio_set_function(STEP_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(STEP_I2C_SCL_PIN, GPIO_FUNC_I2C);
    g_stepEnc->initEncoder();

    i2c_init(I2C_ATT, 100 * 1000);
    gpio_set_function(ATT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(ATT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    mySwitchingDevice.init(UINode::changeEvent);

    myVolumeSelector.initialize(g_stepEnc, &myDisplay);
    myVolumeSelector.onEnter();

   sleep_ms(250); // sleep so that data polling and register update don't collide

    while (1) 
    {
        myDisplay.run();
        UINode::m_currentNode->run();
        myRemoteCtrl.run();
        sleep_ms(1);
    }
}

