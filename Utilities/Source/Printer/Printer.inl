#ifndef PRINTER_INL
#define PRINTER_INL


template<class OutputStream, typename... Args>
void Printer::operator ()(OutputStream& out, const Args&... args) noexcept
{
    std::lock_guard<std::mutex> lockGuard{ _mutex };
    (item::ItemPrinter<OutputStream, Args>(out, args), ...);
}

template<class OutputStream, typename... Args>
void Printer::write(OutputStream& out, const Args&... args) noexcept
{
    std::lock_guard<std::mutex> lockGuard{ _mutex };
    utils::print(out, args...);
}

template<class OutputStream, typename... Args>
void Printer::writeLine(OutputStream& out, const Args&... args) noexcept
{
    std::lock_guard<std::mutex> lockGuard{ _mutex };
    utils::println(out, args...);
}

#endif // PRINTER_INL
