#include "usbstartconnection.h"
#include <QDebug>

#ifdef Q_OS_WIN
bool WacomChecker::isWacomAlreadyConnected()
{
    // GUID_DEVINTERFACE_USB_DEVICE => quando vogliamo enumerare i dispositivi USB
    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        /* Prende la GUID delle USB (GUID_DEVINTERFACE_USB_DEVICE) e ritorna un handle
             * a un elenco di device attualmente connessi.*/
        &GUID_DEVINTERFACE_USB_DEVICE,
        nullptr,
        nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE // significa: “prendi solo i dispositivi presenti (cioè attualmente connessi) e che supportano una device interface”
        );

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        qWarning() << "SetupDiGetClassDevs fallita!";
        return false;
    }

    // Creo una struttura che verrà riempita da SetupDiEnumDeviceInterfaces per fornire informazioni sul singolo dispositivo
    SP_DEVICE_INTERFACE_DATA interfaceData;
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // Scorri tutti i dispositivi dell'elenco t.c, se esiste un device con indice i, la funzione riempie la struttura interfaceData
    for (DWORD i = 0; ; ++i) {
        BOOL enumResult = SetupDiEnumDeviceInterfaces(hDevInfo, nullptr, &GUID_DEVINTERFACE_USB_DEVICE, i, &interfaceData);

        if (!enumResult)
        {
            // Se l'errore è ERROR_NO_MORE_ITEMS, significa che non ci sono più device
            if (GetLastError() == ERROR_NO_MORE_ITEMS)
            {
                // Fine enumerazione
                break;
            }
            // Altri errori: continuiamo semplicemente con il prossimo indice
            continue;
        }

        // Determinazione dello spazio necessario per ottenere i dettagli del device i-esimo
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, nullptr, 0, &requiredSize, nullptr);

        // Una volta conosciute le dimensioni, si alloca un buffer adeguato
        auto detailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(requiredSize));
        if (!detailData)
        {
            continue; // se l’allocazione fallisce, passiamo al prossimo device
        }
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // Otteniamo i dettagli (DevicePath), tipo "\\?\USB#VID_056A&PID_0376#..."
        if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, detailData, requiredSize, &requiredSize, nullptr)) {
            QString devicePath = QString::fromWCharArray(detailData->DevicePath);

            // Filtra sul VendorID (056A => Wacom) e ProductID (es. 0376)
            if (devicePath.contains("VID_056A", Qt::CaseInsensitive) &&
                devicePath.contains("PID_0376", Qt::CaseInsensitive))
            {
                // Se abbiamo trovato un dispositivo che corrisponde alla nostra tavoletta Wacom:
                free(detailData); // liberiamo la memoria detailData
                SetupDiDestroyDeviceInfoList(hDevInfo); // distruggiamo la lista di enumerazione con SetupDiDestroyDeviceInfoList(hDevInfo)
                return true; // ritorniamo true: trovata tavoletta con quell’ID
            }
        }

        free(detailData); // liberiamo la memoria detailData
    }

    // Se siamo usciti dal ciclo for senza trovare il device con le stringhe VID_056A e PID_0376, significa che non esiste nessun dispositivo Wacom con quell’ID collegato
    SetupDiDestroyDeviceInfoList(hDevInfo); //distruggiamo la lista
    return false; // ritorniamo false
}

#else // non Windows

bool WacomChecker::isWacomConnected()
{
    // Su altri sistemi potresti implementare udev (Linux) o IOKit (macOS)
    return false;
}

#endif

