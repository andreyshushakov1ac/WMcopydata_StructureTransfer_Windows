#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

 struct Header 
{
    int dataSizeBytes;
    time_t currentTime;
}; 
struct Body 
{
    char gg[14]; 
};
struct End 
{
    uint16_t crc16; 
}; 
//#################################
struct Packet 
{   
    Header H1;
    Body B1;
    End E1;
};
//#################################
// Функция для конвертации структуры Packet в байты
std::vector<char> packetToBytes(const Packet& packet) {
    std::vector<char> bytes(sizeof(Packet));
    memcpy(bytes.data(), &packet, sizeof(Packet));
    return bytes;
}

//###################################################################################
uint16_t calculateCRC16(const char* data, int dataSize) {
    
    // Генераторный полином для CRC16 (x^16 + x^15 + x^2 + 1)
const uint16_t CRC16_POLY = 0xA001;
    
    uint16_t crc = 0xFFFF; // init by max value


    for (int i = 0; i < dataSize; ++i) {
        crc ^= static_cast<uint8_t>(data[i]); // XOR
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ CRC16_POLY;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

//#######################################################################################
int main() {

    // Packet creation and init
    Packet msg; 
    strcpy(msg.B1.gg, "Hello, world!");
    msg.H1.dataSizeBytes = sizeof(msg.B1.gg);
    msg.H1.currentTime = time(nullptr); 
    msg.E1.crc16 = calculateCRC16(reinterpret_cast<const char*>(&msg.B1), sizeof(Body));

    // struct to byte arr
    std::vector<char> bmsg = packetToBytes(msg);




    HWND hwnd = FindWindow("MyWindowClass", "process 1 window");
    if (hwnd == NULL) {
        std::cout << "Error: Cannot find window" << std::endl;
        return 1;
    }

    std::string message = "\nHello from process 2!";
    COPYDATASTRUCT cds;
    cds.dwData = 0;
    cds.cbData = bmsg.size(); // ?? + 1;
    cds.lpData = (LPVOID)bmsg.data(); //c_str();

    SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)&cds);

    return 0;
}
