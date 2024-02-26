#pragma once
// #include "Global.h"
// #include "classes/IoTItem.h"

#include "OpenTherm.h"
#include <queue>
#include <map>

// #define ESP32_FREERTOS
#define TIMEOUT_TRESHOLD 5
#define TEMP_OFFLINE_SEND 60

struct IoTValue;

#ifdef ESP32_FREERTOS
portMUX_TYPE serialOTMux = portMUX_INITIALIZER_UNLOCKED;
static void openthermSendHandler(void *pvParam);
#endif

void IRAM_ATTR handleInterrupt();

// static void publishData(String widget, String status);
static void (*_publishData_)(String, String);

// static void sendTelegramm(String msg);
static void (*_sendTelegramm_)(String);

static void (*_SerialPrint_)(const String &, const String &, const String &); //, const String& itemId = ""

class OpenThermLib
{
private:
    uint8_t _tempDHWmin;
    uint8_t _tempDHWmax;
    uint8_t _tempCHmin;
    uint8_t _tempCHmax;

    // Что бы проредить отправку при offline в loop
    uint8_t _offlineSend = TEMP_OFFLINE_SEND; // Если мы offline и вкл _offline---Enable, то устанавливаем в котёл температуру раз в минуту
    uint8_t _offlineCHTemp = 50;
    bool _useOfflineMode = false;

    unsigned long ts = 0;
    int md = 0;
    int md2 = 0;
    bool firstSendFlag = true;
    String _license;

public:
    //=======================================================================================================
    // setup()
    OpenThermLib(String parameters);

    ~OpenThermLib();

    static OpenTherm *instanceot(int _RX_pin, int _TX_pin);

    static void initFunction(void (*_publishData_)(String, String), void (*_sendTelegramm_)(String), void (*_SerialPrint_)(const String &, const String &, const String &)); //, const String&

    void doByInterval();

    // Основной цикл программы
    void loop(bool isNetworkActive, bool mqttIsConnect);

    // Исполнительные комманды
    void execute(String command, std::vector<IoTValue> &param);

    // Функция колбэка, возвращает ответ на асинхронные сообщения, с проверкой статуса, для последующей обработки
    static void responseCallback(unsigned long result, OpenThermResponseStatus status);

    // Обработка Асинхронных ответов котла на основные команды
    static void HandleReply(unsigned long response);

    // проверяем если пришедшее значение отличается от предыдущего регистрируем событие
    static void checkNew(String openthermID, String value);

    // Постановка в очередь запросов для котла, для последующей асинхронной отправки
    // В очереди хранится максимум 45 запросов (для полного теста), если они не были отправлены,
    //  то старые запросы затираются новыми
    static void addRequestQueue(unsigned long request);

    // Отправка асинхронных запросов непосредственно в канал передачи данных (в котёл)
    bool sendRequest2Channel();

    // Отправка тестовых запросов, полного теста котла (а точнее поставнока в очередь, для отправки в соответствии с протоколом)
    void testSupportedIDs();

    uint32_t getFlashChipIdNew();

    uint32_t ESP_getChipId(void);

    const String getChipId();
};
