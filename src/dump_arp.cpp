#include <arp_experimental/arp_receive.hpp>
#include <srook/cstring/memcpy.hpp>
#include <iostream>

int main(const int argc, const char** const argv)
{
    if (argc <= 1) return EXIT_FAILURE;

    bool b = false;
    if (!srook::string_view("-p").compare(argv[2])) {
        b = true;
    }

    arp_experimental::initialize_raw_socket(argv[1], srook::move(b)) >>= [&](int soc) -> srook::optional<int> {
        ::u_char buf[2048];

        for (int size = 0;;) {
            if ((size = ::read(soc, buf, sizeof(buf))) <= 0) {
                srook::process::perror("read");
                return {};
            } else {
                if (static_cast<std::size_t>(size) >= sizeof(ether_header)) {
                    ether_header eth;
                    srook::cstring::memcpy(&eth, buf, sizeof(eth));
                    std::cout << arp_experimental::show_ether_header{eth} << std::endl;
                } else {
                    std::cerr << "read size(" << size << ") < " << sizeof(::ether_header) << std::endl;
                }
            }
        }
        ::close(soc);
        return { EXIT_SUCCESS };
    };
}
