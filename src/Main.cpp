#include <telkin/Print.h>
#include <blox/Blox.h>

red::Registrar* blox::getRegistrar() {
    static red::Registrar sRegistrar("blox");
    return &sRegistrar;
}

void main() {
    tk::println("Welcome to Domthewiz' Blox!");
}
