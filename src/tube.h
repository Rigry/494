#pragma once
#include "pin.h"
#include "buttons.h"

template <class Button, class Start, class UVlevel, class Overheat>
class Tube
{
    enum Step {wait, work, overheating} step {Step::wait};
    
    bool on {false};
    bool overheat {false};
    
    Button button;
    Pin start;
    Pin alarm_uv;
    Pin alarm_heat;

    uint16_t& uv_level;
    uint16_t& temperature;
    uint16_t max_temperature {55};
    uint16_t recovery_temperature {20};
    uint16_t min_uv_level {40};

public:

    Tube(uint16_t& uv_level, uint16_t& temperature)
        : button      {mcu::Button::make<Button>()}
        , start       {Pin::make<Start>()}
        , alarm_uv    {Pin::make<UVlevel>()}
        , alarm_heat  {Pin::make<Overheat>()}
        , uv_level    {uv_level}
        , temperature {temperature}
    {}

    // Register<address, Modbus_function::read_03, 4> uv_level;
    // Register<address, Modbus_function::read_03, 5> temperature;

    void operator()(){
        // switch (step) {
        //     case wait:
        //         on ^= button;
        //         if (on) {
        //             start = true;
        //             step = work;
        //         } else
        //             start = false;
        //     break;
        //     case work:
        //         on ^= button;
        //         if (on)
        //             start = true;
        //         else {
        //             start = false;
        //             step = wait;
        //         }
        //     break;
        //     case overheating:
        //     break;
        // }
    }
    
};