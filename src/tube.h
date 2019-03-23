#include "pin.h"
#include "buttons.h"

template <uint8_t address, class Button, class Start, class UVlevel, class Overheat>
class Tube
{
    bool on {false};
    bool overheat {false};
    
    Pin start;
    Pin alarm_uv;
    Pin alarm_heat;
    Button button;

    uint16_t& uv_level;
    uint16_t& temperature;
    uint16_t max_temperature {55};
    uint16_t recovery_temperature {20};
    uint16_t min_uv_level {40};

public:

    Tube() : start       {Pin::make<Start>()}
           , alarm_uv    {Pin::make<UVlevel>()}
           , alarm_heat  {Pin::make<Overheat>()}
           , button      {mcu::Button::make<Button>()}
           , uv_level    {uv_level}
           , temperature {temperature}
    {}

    void operator()(){}
    
};