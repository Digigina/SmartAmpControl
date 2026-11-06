#include "StepEncBase.h"

StepEncBase::StepEncBase(i2c_inst_t* i2c_inst, uint8_t i2c_adr)
		: m_i2cAdr(i2c_adr),
			m_i2cInst(i2c_inst)
{
}

StepEncBase::~StepEncBase() = default;
