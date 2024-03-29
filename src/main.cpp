#define STM32F030x6
#define F_CPU   48000000UL

#include "periph_rcc.h"
#include "flash.h"
#include "pin.h"
#include "buttons.h"
#include "modbus_master.h"
// #include "tube.h"
#include "literals.h"
#include "init_clock.h"
#include <bitset>
#include <algorithm>

/// эта функция вызываеться первой в startup файле
extern "C" void init_clock () { init_clock<8_MHz,F_CPU>(); }

using DI1 = mcu::PA9;  using Button_1 = DI1;
using DI2 = mcu::PA8;  using Button_2 = DI2;
using DI3 = mcu::PB15;
using DI4 = mcu::PB14;

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
    auto [button_1, button_2, reset_1, reset_2] = make_pins<mcu::PinMode::Input,DI1,DI2,DI3,DI4>();
    auto [alarm_heat_1, alarm_uv_1, alarm_heat_2, alarm_uv_2, start_1, start_2] 
        = make_pins<mcu::PinMode::Output,DO1,DO2,DO3,DO4,DO7,DO8>();

    struct Flash_data 
    {
        int max_uv_level_1{10};
        int max_uv_level_2{10};
    };

    Flash<Flash_data, mcu::FLASH::Sector::_28> flash{};
    
    constexpr bool parity_enable {true};
    constexpr int  timeout       {500_ms};
    constexpr UART::Settings set {
          not parity_enable
        , UART::Parity::even
        , UART::DataBits::_8
        , UART::StopBits::_1
        , UART::Baudrate::BR9600
    };

    constexpr auto address_1 {0x0A};
    constexpr auto address_2 {0x0C};

    struct Modbus {
        
        Register<address_1, Modbus_function::read_03, 0> uv_level_1;
        Register<address_2, Modbus_function::read_03, 0> uv_level_2;
        Register<address_1, Modbus_function::read_03, 1> temperature_1;
        Register<address_2, Modbus_function::read_03, 1> temperature_2;
        
    } modbus;

    decltype(auto) modbus_master =
        make_modbus_master <mcu::Periph::USART1, TX, RX, RTS> (
            timeout, set, modbus
        );

    bool on_1 {false};
    bool on_2 {false};
    bool overheat_1 {false};
    bool overheat_2 {false};
    constexpr auto recovery_temperature {20};
    constexpr auto max_temperature {55};
    constexpr auto min_uv_level {20}; //%

    while (1) {
        modbus_master();

        flash.max_uv_level_1 = modbus.uv_level_1 > flash.max_uv_level_1 ? modbus.uv_level_1 : flash.max_uv_level_1;
        flash.max_uv_level_2 = modbus.uv_level_2 > flash.max_uv_level_2 ? modbus.uv_level_2 : flash.max_uv_level_2;
    
        on_1 = on_2 ? false : button_1;
        on_2 = on_1 ? false : button_2;

        start_1 = (on_1 and not overheat_1);
        start_2 = (on_2 and not overheat_2);

        if (overheat_1 |= modbus.temperature_1 > max_temperature)
            overheat_1  = modbus.temperature_1 > recovery_temperature;
        
        if (overheat_2 |= modbus.temperature_2 > max_temperature)
            overheat_2  = modbus.temperature_2 > recovery_temperature;

        alarm_heat_1 = overheat_1;
        alarm_heat_2 = overheat_2;

        alarm_uv_1 = (on_1 and modbus.uv_level_1 < (flash.max_uv_level_1 * min_uv_level)/100);
        alarm_uv_2 = (on_2 and modbus.uv_level_2 < (flash.max_uv_level_2 * min_uv_level)/100);

        flash.max_uv_level_1 = reset_1 ? 10 : flash.max_uv_level_1;
        flash.max_uv_level_2 = reset_2 ? 10 : flash.max_uv_level_2;

        __WFI();
    }
}

