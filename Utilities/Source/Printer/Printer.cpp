#include "Printer.h"


namespace printer
{

Printer& Printer::getInstance()
{
    // Guaranteed to be destroyed. Instantiated on first use.
    static Printer instance;
    return instance;
}

} // namespace printer
