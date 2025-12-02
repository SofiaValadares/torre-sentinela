#include "./includes.h"

char deviceId[20];  

void gerarDeviceId() {
    uint64_t chipid = ESP.getEfuseMac();
    sprintf(deviceId, "%04X%08X",
            (uint16_t)(chipid >> 32),
            (uint32_t)chipid);
}
