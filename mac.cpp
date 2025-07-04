#include "headers/lib.hpp"
#include "headers/mac.hpp"

std::string MAC::getMyMac(const std::string& interface){
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0){
        perror("socket");
        return "";
    }

    ifreq ifr = {};
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);

    if(ioctl(fd, SIOCGIFHWADDR, &ifr) == -1){
        close(fd);
        return "---";
    }

    close(fd);

    unsigned char* mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;
    char macStr[18];

    snprintf(macStr, sizeof(macStr),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return std::string(macStr);
}


std::string MAC::getMacServer(std::string ip){

    std::ifstream arp("/proc/net/arp");
    std::string line;

    std::getline(arp, line);

    while (std::getline(arp, line)) {
        std::istringstream iss(line);
        std::string ipAddr, hwType, flags, macAddr, mask, device;

        iss >> ipAddr >> hwType >> flags >> macAddr >> mask >> device;

        if (ipAddr == ip)
            return macAddr;
    }

    return "---";
}