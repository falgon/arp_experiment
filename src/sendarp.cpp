#include <arp_experimental/arp_packet.hpp>

bool cmdarg_check(const int argc, const char* const progname)
{
    if (argc != 3) {
        std::cerr << "Usage: " << progname << " <interface> <target ip address>" << std::endl;
        return false;
    } else if (::getuid() && geteuid()) {
        std::cerr << "Need superuser" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (!cmdarg_check(argc, argv[0])) return EXIT_FAILURE;

    arp_experimental::arp_packet packet(argv[1], argv[2]);
    packet.send();
}
