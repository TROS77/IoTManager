#include "Global.h"
#include "classes/IoTItem.h"
#include <Arduino.h>
#include "OpenThermLib.h"

OpenThermLib *OT_lib;
IoTItem *tmp;

    static uint8_t _debug;
    static bool _telegramInfo;

class OpenThermMonitor : public IoTItem
{
private:


public:
    //=======================================================================================================
    // setup()
    OpenThermMonitor(String parameters) : IoTItem(parameters)
    {
        _debug = 0;
        _telegramInfo = false;
        _debug = jsonReadInt(parameters, "debag");
        jsonRead(parameters, "telegram", _telegramInfo);

       OpenThermLib::initFunction(publishData, sendTelegramm, _SerialPrint);
               OT_lib = new OpenThermLib(parameters);
    }

    static void _SerialPrint(const String& errorLevel, const String& module, const String& msg){
        SerialPrint (errorLevel, module, msg, "");    
    }

    void doByInterval()
    {
        OT_lib->doByInterval();
    }

    // Основной цикл программы
    void loop()
    {
        OT_lib->loop(isNetworkActive(), mqttIsConnect());
        // для новых версий IoTManager
        IoTItem::loop();
    }

    // Исполнительные комманды
    IoTValue execute(String command, std::vector<IoTValue> &param)
    {
        OT_lib->execute(command, param);

        return {};
    }

    static void publishData(String widget, String status)
    {

        tmp = findIoTItem(widget);
        if (tmp)
        {
            tmp->setValue(status, true);
        }
        else
        {
            if (_debug > 0)
            {
                SerialPrint("i", "🆕", widget + " = " + status);
            }
        }
    }

    static void sendTelegramm(String msg)
    {
        if (_telegramInfo == 1)
        {
            /*
            // TODO переделать на драйвер, но сначала его сделать в "TelegramLT" и "Telegram"
            // подумать как сделать поддержку старых версий без драйвера
            for (std::list<IoTItem *>::iterator it = IoTItems.begin(); it != IoTItems.end(); ++it)
            {
                if ((*it)->getSubtype() == "TelegramLT" || "Telegram" || "Telegram_v2")
                {
                    (*it)->sendTelegramMsg(false, msg);
                }
            }
            */
            if (tlgrmItem)
            {
                tlgrmItem->sendTelegramMsg(false, msg);
            }
        }
    }

    ~OpenThermMonitor()
    {
        delete OT_lib;
        OT_lib = nullptr;
    };
};

void *getAPI_OpenThermMonitor(String subtype, String param)
{
    if (subtype == F("OpenThermMonitor"))
    {
        return new OpenThermMonitor(param);
    }
    else
    {
        return nullptr;
    }
}
