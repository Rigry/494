#define STM32F030x6
#define F_CPU   48000000UL

#include "periph_rcc.h"
#include "pin.h"
#include "buttons.h"
#include "modbus_master.h"
#include "literals.h"
#include "init_clock.h"
#include <bitset>
#include <algorithm>

/// эта функция вызываеться первой в startup файле
extern "C" void init_clock () { init_clock<8_MHz,F_CPU>(); }

using DI1 = mcu::PA9;  using Button_1 = DI1;
using DI2 = mcu::PA8;  using Button_2 = DI2;

using DO1 = mcu::PB0;  using Alarm_heat_1 = DO1;
using DO2 = mcu::PA7;  using Alarm_uv_1   = DO2;
using DO3 = mcu::PA6;  using Alarm_heat_2 = DO3;
using DO4 = mcu::PA5;  using Alarm_uv_2   = DO4;
using DO7 = mcu::PB3;  using Start_1      = DO7;
using DO8 = mcu::PA15; using Start_2      = DO8;

using RX  = mcu::PA3;
using TX  = mcu::PA2;
using RTS = mcu::PA1;

int main()
{
    // auto button_1 = mcu::Button::make<DI1>();
    // auto button_2 = mcu::Button::make<DI2>();
    // auto [alarm_heat_1, alarm_uv_1, alarm_heat_2, alarm_uv_2, start_1, start_2] 
    //     = make_pins<mcu::PinMode::Output,DO1,DO2,DO3,DO4,DO7,DO8>();

    constexpr bool parity_enable {true};
    constexpr int  timeout       {50_ms};
    constexpr UART::Settings set {
          not parity_enable
        , UART::Parity::even
        , UART::DataBits::_8
        , UART::StopBits::_1
        , UART::Baudrate::BR9600
    };

  

    Tube<1,DI1> tube_1{};
    Tube<2,DI2> tube_2{};



    struct Modbus {
        
        tube_1.uv_level;
        tube_1.temperature;
        tube_2.uv_level;
        tube_2.temperature;
    } modbus;

    decltype(auto) modbus_master =
        make_modbus_master <mcu::Periph::USART1, TX, RX, RTS> (
            timeout, set, modbus
        );

    while (1) {
        modbus_master();

        tube_1.uv_on ^= tube_1.button;
        tube_2.uv_on ^= tube_2.button;
        
        // if (overheat_1 |= modbus.temperature_1 > max_temperature)
        //     overheat_1  = modbus.temperature_1 > recovery_temperature;
        
        // if (overheat_2 |= modbus.temperature_2 > max_temperature)
        //     overheat_2  = modbus.temperature_2 > recovery_temperature;

        // start_1 = uv_1_on ^= (button_1 and not overheat_1);
        
        if (tube_1.uv_on) start_1 = true;
        else start_1 = false;

        if (tube_2.uv_on) start_2 = true;
        else start_2 = false;

        // if (button_1 and not overheat_1)
        //     uv_1_on = start_1 = true;
        // if (button_1) uv_1_on = start_1 = false;

        // if ((button_2 or uv_2_on) and not overheat_2)
        //     uv_2_on = start_2 = true;
        // else uv_2_on = start_2 = false;

        // alarm_heat_1 = overheat_1;
        // alarm_heat_2 = overheat_2;

        // alarm_uv_1 = (uv_1_on and modbus.uv_level_1 < min_uv_level);
        // alarm_uv_2 = (uv_2_on and modbus.uv_level_2 < min_uv_level);

        __WFI();
    }
}

