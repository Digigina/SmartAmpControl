#include "VolChControl.h"

constexpr uint8_t ADR_GPIOA = 0x12;
constexpr uint8_t ADR_GPIOB = 0x13;
constexpr uint8_t ADR_DIRA = 0x00;
constexpr uint8_t ADR_DIRB = 0x01;

constexpr uint8_t VOL_MASK = 0x7F;
constexpr uint8_t CHA_MASK = 0x0F;



VolChControl::VolChControl(i2c_inst_t* i2c_inst, uint8_t i2c_adr) : m_i2cInst(i2c_inst), m_i2cAdr(i2c_adr)
{
    m_curVol = 0;
    m_curCha = 1;
    m_updateCB = nullptr;
}

void VolChControl::init(void (*updateCB)())
{
    bool result;
    uint8_t init_cmd[] = {ADR_DIRA, 0x00, 0x00};
    // select on-chip register addr 0;
    // IODIRA = 0 (all output)
    // IODIRB = 0 (all output)

    m_updateCB = updateCB;

    result = i2c_write_blocking(m_i2cInst, m_i2cAdr, init_cmd, 2, false);
    if(result == false)
        printf("initialization of MCP23017 failed \n");
    init_cmd[0] = ADR_DIRB;
    result = i2c_write_blocking(m_i2cInst, m_i2cAdr, init_cmd, 2, false);
    if(result == false)
        printf("initialization of MCP23017 failed \n");
}

void VolChControl::setVolume(uint8_t newVol)
{
    if(newVol < 64)
    {
        m_curVol = newVol;
    }
    updateRegister();
    // signal any menue node that volume has changed, so it can update the display
    if(m_updateCB)
        m_updateCB();

}
void VolChControl::setChannel(uint8_t newCha)
{
    if(newCha > 4)
        newCha = 1;
    else if(newCha < 1)
        newCha = 4;
    if(newCha != m_curCha)
    {
        m_curCha = newCha;
        // write new channel to MCP23017
        updateRegister();
        // signal any menue node that channel has changed, so it can update the display
        if(m_updateCB)
            m_updateCB();
    }
}

//===========================================================================================
// Description
// calculate the MCP device bit pattern for port A and B based ob volume and channel settings
// update the port via i2c if necessary
// return: none
//===========================================================================================
void VolChControl::updateRegister()
{
    uint8_t mcpPortA;
    uint8_t mcpPortB;
    uint8_t cmd_buffer[4];
    uint8_t channel;

    // if volume is zero, disable input
    if(m_curVol == 0)
        channel = 0;
    else
        channel = m_curCha;
    
    // calculatate bit pattern for volume control on port B
    // volume relais are driven by DRV777DE
    // because DRV777 inverts the signal and relais switches on low signal, no invertion is needed
	mcpPortB = m_curVol;
    if(mcpPortB != m_gpioPortB)
    {
        cmd_buffer[0] = ADR_GPIOB;
        cmd_buffer[1] = mcpPortB;
        i2c_write_blocking(m_i2cInst, m_i2cAdr, cmd_buffer, 2, false);
        m_gpioPortB = mcpPortB;
    }

    // calculate bit pattern for channel selection
    // because realais needs low signal to switch, invertion is needed
    if(channel == 0)
        mcpPortA = CHA_MASK;
    else
        mcpPortA = ~(1 << (channel-1));  
 	    //mcpPortA = m_gpioPortA & ~CHA_MASK | ~(1 << (channel-1)) && CHA_MASK;  
    if(mcpPortA != m_gpioPortA)
    {
        cmd_buffer[0] = ADR_GPIOA;
        cmd_buffer[1] = mcpPortA;
        i2c_write_blocking(m_i2cInst, m_i2cAdr, cmd_buffer, 2, false);
        m_gpioPortA = mcpPortA;
    }
   
//     // test routine
//     uint8_t test_input = m_curVol;
//     if(test_input < 7)
//     {
//         mcpPortB = 1 << m_curVol;
//         cmd_buffer[0] = ADR_GPIOB;
//         cmd_buffer[1] = mcpPortB;
//         i2c_write_blocking(m_i2cInst, m_i2cAdr, cmd_buffer, 2, false);
//     }
//     else if(test_input >= 10 && test_input < 15)
//     {
//         test_input %= 10;
//         mcpPortA = 1 << test_input;
//         mcpPortA = ~mcpPortA;
//         cmd_buffer[0] = ADR_GPIOA;
//         cmd_buffer[1] = mcpPortA;
//         i2c_write_blocking(m_i2cInst, m_i2cAdr, cmd_buffer, 2, false);
//    }        
 }
