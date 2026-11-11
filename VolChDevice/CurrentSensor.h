#pragma once
#include <stdint.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"

// ---------------------------------------------------------------------------
// Klasse zur Einlesung der Ruhestromwerte (Shunt-Widerstand) über den ADC
// ---------------------------------------------------------------------------
class CurrentSensor
{
    public:
    CurrentSensor(float r_ref);

    void init();

    // setzt den Kanal von dem der Ruhestrom ermittelt werden soll (0,1)
    void selectChannel(uint8_t chaNo);
    // liefert den Ruhestromwert in mA
    uint16_t getValueMilliAmpere();

    private:
    // Referenzwert des Shunt-Widerstands in Ohm
    float m_valueShuntResistor;
};