#include "headers/lib.hpp"
#include "headers/mac.hpp"

uint16_t checksum(void* data, int len){
	uint16_t* ptr = (uint16_t*)data;
	uint32_t sum = 0;

	while(len > 1){
		sum += *ptr++;
		len -= 2;
	}
	if(len == 1)
		sum += *(uint8_t*)ptr;

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);

	return ~((uint16_t)sum);
}


std::string getIP(){
	std::cout << "Enter IP: ";
	std::string ip;
	std::cin >> ip;
	return ip;
}
std::string getInterface(){
	std::cout << "Enter Iface: ";
	std::string iface;
	std::cin >> iface;
	return iface;
}

int main(){

	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock < 0) {
		perror("socket");
		return 1;
	}

	// Устанавливаем таймаут приёма (2 секунды), чтобы не зависало
	timeval timeout = {2, 0};
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	sockaddr_in to = {};
	to.sin_family = AF_INET;

	std::string ip = getIP();
	std::string iface = getInterface();

	inet_pton(AF_INET, ip.c_str(), &to.sin_addr);

	char packet[8] = {};
	packet[0] = 8;              // Type = Echo
	packet[1] = 0;              // Code = 0

	uint16_t id = getpid() & 0xFFFF;  // Динамический ID
	packet[4] = id & 0xFF;
	packet[5] = (id >> 8) & 0xFF;

	packet[6] = 0x00;           // SEQ low
	packet[7] = 0x01;           // SEQ high

	uint16_t sum = checksum(packet, 8);
	packet[2] = sum & 0xFF;
	packet[3] = (sum >> 8) & 0xFF;

	char buf[1024];
	sockaddr_in from = {};
	socklen_t fromlen = sizeof(from);

	auto start = std::chrono::high_resolution_clock::now();

	sendto(sock, packet, 8, 0, (sockaddr*)&to, sizeof(to));

	while (true){
		ssize_t r = recvfrom(sock, buf, sizeof(buf), 0, (sockaddr*)&from, &fromlen);
		if (r <= 0) {
			std::cerr << "Timeout or error receiving packet.\n";
			return 1;
		}

		iphdr* ip = (iphdr*)buf;
		icmphdr* icmp = (icmphdr*)(buf + ip->ihl * 4);

		if (icmp->type == 0 && icmp->un.echo.id == id) {
			break;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "Ping: " << ms << " ms" << std::endl;

	std::string myMac = MAC::getMyMac(iface);
	std::string serverMac = MAC::getMacServer(ip);

	std::cout << "My mac: " << myMac << std::endl;
	std::cout << "Server mac: " << serverMac << std::endl;

	close(sock);
	return 0;
}
