/*
 * Author: Michele Di Lucchio
 * 29/04/25
 * Questa classe incapsula la logica per creare un server TCP multi-client con Qt
 * e permette di gestire tutte le azioni del server TCP:
 * - accettare connessioni da più client,
 * - ricevere dati da ciascun client,
 * - trasmettere messaggi a tutti gli altri client,
 * - inviare dati binari come la posizione di un pennino,
 * - emettere segnali verso la GUI (o altri oggetti) quando si verificano eventi significativi
*/

#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>                                                  // Classe che serve per abilitare segnali e slot
#include <QTcpServer>                                               // Classe Qt per creare un server TCP che può accettare connessioni da client TCP
#include <QTcpSocket>                                               // Classe Qt che rappresenta una connessione TCP individuale (per gestire ogni client connesso)
#include <QPointF>                                                  // Classe Qt che rappresenta una coppia (x, y) in floating point (utile per il pennino)


class MyTcpServer : public QObject                                  // Classe che ereditando da QObject può utilizzare segnali e slot beneficiare del sistema di gestione della memoria di Qt
{
    Q_OBJECT

    public:
        enum class SendMode { // Dichiarazione
            Live,
            Batch
        };

        explicit MyTcpServer(int port, QObject *parent = nullptr);      // Costruttore che crea un oggetto di tipo MyTcpServer e lo avvia su una certa porta
        bool isStarted () const;                                        // Metodo pubblico che restituisce lo stato interno del server: true se il server è partito correttamente ovvero il server ha avviato correttamente l’ascolto sulla porta specificata
        void sendToAll(QString message);                                // Invia un messaggio testuale a tutti i client connessi

        SendMode getSendMode() const;
        void setSendMode(SendMode mode);

    signals:                                                            // Segnali che si collegano a slot di MainWindow per poter aggiornare la GUI
        void newClientConnected();                                      // Segnale generato dal seguente evento: un client si è connesso
        void clientDisconnect();                                        // Segnale generato dal seguente evento: un client si è disconnesso
        void dataReceived(QString message);                             // Segnale generato dal seguente evento: è arrivato un messaggio da un client connesso

    public slots:
        void sendPenPosition(QPointF pos);                              // Riceve la posizione del pennino dal DrawingArea e la manda come pacchetto binario
        void sendFullTrajectoryToClient(const QList<QPointF> &trajectory, int trajDuration);

    private slots:
        /* Quando un client tenta di connettersi al server allora viene emesso un segnale chiamato newConnection.
        Creo quindi uno slot chiamto on_client_connecting per rispondere a questo tipo di eventi. Quindi
        ogni qual volta un client tenterà di connettersi al mio server TCP verrà invocato questo slot. */
        void on_client_connecting();

        void clientDataReady();                                         // Slot invocato quando un client vuole mandare dati (sottoforma di messaggio) al server
        void clientDisconnected();                                      // Slot invocato quando un client si disconnette dal server

    private:
        QTcpServer *_server;                                            // Puntatore a un oggetto QTcpServer ovvero un oggetto server TCP che ascolta richieste
        QList<QTcpSocket *> _socketsList;                               // Una lista di puntatori a oggetti QTcpSocket ovvero i client TCP connessi. In questo modo tengo traccia di tutte le connessioni attive (cioè i client collegati)
        bool  _isStarted;                                               // Flag che ci dice se il server ha iniziato a mettersi in ascolto per stabilire connessioni con potenziali client

        // Flag per decidere con quale modalità mandare i dati della traiettoria registrata al client
        //bool sendLive = true; // true = invio ogni punto in tempo reale; false = invio blocco unico
        SendMode sendMode = SendMode::Live; // ✅ default

        // Serializzazione dei dati da mandare al client
        QByteArray serializePenPosition(const QPointF &pos);            // Metodo che serializza i dati della posizione producendo un pacchetto binario (QByteArray) da 9 byte (intestazione [0xA1 | float x | float y]), che può essere direttamente inviato via socket
};

#endif // MYTCPSERVER_H
