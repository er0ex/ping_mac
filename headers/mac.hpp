#ifndef _MAC_H_
#define _MAC_H_

#include <string>

namespace MAC{
    
    std::string getMyMac(const std::string& interface);

    std::string getMacServer(std::string ip);
}

#endif //_MAC_H_