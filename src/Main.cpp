#include <blox/Blox.h>

red::Registrar* blox::getRegistrar() {
    static red::Registrar sRegistrar("blox");
    return &sRegistrar;
}

void main() {
    tk::println("Blox by domthewiz has loaded!");
}

#include <telkin/Telkin.h>
// tPatch8u(0x024D3A63, 0);