#include "map_obj/BlockMgr.h"
#include "red/profile/ProfileEx.h"
#include "system/TouchDrcMgr.h"
#include "telkin/Hooks.h"
#include <telkin/Print.h>
#include <blox/Blox.h>

red::Registrar* blox::getRegistrar() {
    static red::Registrar sRegistrar("blox");
    return &sRegistrar;
}

void main() {
    tk::println("Welcome to Domthewiz' Blox!");
}

// tPatch8u(0x024D3A63, 0);