#include "MyUdpServer.h"
#include "qdebug.h"

MyUdpServer::MyUdpServer(int port, QObject *parent)
    : QObject(parent)
{
    _udpSocket = new QUdpSocket(this);
    // _udpSocket->bind(QHostAddress::AnyIPv4, port)
    if (_udpSocket->bind(QHostAddress("192.168.1.30"), port)) { // L'app Qt ascolta passivamente su una porta UDP. bind() dice al sistema operativo: “Voglio ricevere datagrammi UDP su questa porta (es. 5000), da qualsiasi indirizzo IP
        qDebug() << "UDP Server in ascolto sulla porta" << port;
        /* Il server è ora in ascolto passivo su quella porta (port) e quando un pacchetto UDP arriva, Qt emette il segnale readyRead() */
        connect(_udpSocket, &QUdpSocket::readyRead, this, &MyUdpServer::processPendingDatagrams);
    } else {
        qDebug() << "Errore: bind() fallito!";
    }

    _aliveTimer = new QTimer(this);
    connect(_aliveTimer, &QTimer::timeout, this, &MyUdpServer::checkAlive);
}

// ======================================================= Definizione dei Metodi =======================================================

bool MyUdpServer::isStarted() const {
    /* Restituisce true se il socket è attualmente in stato BoundState, cioè:
     * - La porta UDP è stata riservata con successo.
     * - Il socket è pronto a ricevere datagrammi in ingresso.
     * - Il server è considerato "avviato".
     */
    return _udpSocket->state() == QAbstractSocket::BoundState;
}

void MyUdpServer::sendToLastClient(const QString &message) {
    if (!_lastClientAddr.isNull()) {
        _udpSocket->writeDatagram(message.toUtf8(), _lastClientAddr, _lastClientPort);
    }
}

QByteArray MyUdpServer::serializePenPosition(const QPointF &pos)
{
    QByteArray data;                                                                             // Crea un array di byte vuoto in cui inserirai i dati da inviare
    QDataStream stream(&data, QIODevice::WriteOnly);                                             // Passo il riferimento del QByteArray come destinazione in modalità scrittura
    stream.setByteOrder(QDataStream::LittleEndian);                                              // Definisco l'oridine dei bit in data: LittleEndian = Byte meno significativo per primo

    stream << static_cast<quint8>(0xA1);         // 1 byte: header                                                 // Header: tipo = posizione pennino (1 byte)
    stream << static_cast<qint16>(pos.x());       // 4 byte: X                                                 // Scrivo nel QByteArray data le coordinate x come un numero float di 4 byte
    stream << static_cast<qint16>(pos.y());       // 4 byte: Y                                                 // Scrivo nel QByteArray data le coordinate y come un numero float di 4 byte
    data.append(static_cast<char>(0xFF));

    return data;                                                                                  // Posizione serializzata in 9 byte
}


// ======================================================= Definizione degli Slot =======================================================

void MyUdpServer::sendPenPosition(QPointF pos)
{
    if (_clientAlive && !_lastClientAddr.isNull()) {
        QByteArray data = serializePenPosition(pos);
        _udpSocket->writeDatagram(data, _lastClientAddr, _lastClientPort);

        emit UpdatePenPositionLabel(pos);
    }
}

void MyUdpServer::processPendingDatagrams() {

    while (_udpSocket->hasPendingDatagrams()) { // Ciclo di lettura di tutti i datagrammi pendenti

        /* Un socket UDP può ricevere più datagrammi prima che il segnale readyRead venga gestito.
        *  Il while garantisce che vengano elaborati tutti*/

        QByteArray datagram; // Preparazione buffer per il datagramma in arrivo
        datagram.resize(int(_udpSocket->pendingDatagramSize())); // Alloca un buffer della dimensione esatta del datagramma
        QHostAddress sender;    // variabile locale per memorizzare IP del client
        quint16 senderPort;     // variabile locale per memorizzare Pott del client

        // Lettura effettiva del datagramma
        _udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort); // Legge i dati dal socket e li inserisce nel buffer

        // === Rileva primo handshake ===
        if (!_clientAlive && datagram.startsWith("Ciao da TMS570")) {
            _lastClientAddr = sender;       // Salva l’indirizzo del client nella variabile membro
            _lastClientPort = senderPort;   // Salva la porta del client nella variabile membro
            _clientAlive = true;            // Il client è vivo
            _keepAliveTimer.restart();      // Riavvia il timer KeepAliveTimer perchè l'handshake conferma che il client è vivo
            emit newClientConnected();
            qDebug() << "Handshake iniziale da TMS570";
            _aliveTimer->start(200);        // Dopo il primo handshake, inizia un controllo periodico (ogni 200 ms) per verificare se il client sta ancora inviando i keep-alive
            return;
        }

        // === Keep-alive ===
        if (datagram.size() == 2 &&
            static_cast<quint8>(datagram[0]) == 0xAA &&
            static_cast<quint8>(datagram[1]) == 0x55)
        {
            if (_clientAlive) _keepAliveTimer.restart();
            return;
        }
    }
}

void MyUdpServer::checkAlive()
{
    if (_clientAlive && _keepAliveTimer.elapsed() > 3000) {
        qDebug() << "TMS570 disconnessa: timeout keep-alive";
        _clientAlive = false;
        _lastClientAddr.clear();
        emit dataReceived("Client Offline: timeout keep-alive");
        emit clientDisconnect();
    }
}

void MyUdpServer::sendFullTrajectoryToClient(const QList<QPointF> &trajectory, int trajDuration)
{
    QByteArray data; // contenitore binario da spedire
    QDataStream stream(&data, QIODevice::WriteOnly); // struttura per serializzare i dati in formato compatto


    stream.setByteOrder(QDataStream::LittleEndian); // ordine dei byte: little-endian

    // Cosa inserisco nel pacchetto
    stream << static_cast<quint8>(0xB1);                // header per "traiettoria completa" (1 byte)
    stream << static_cast<quint16>(trajectory.size()); // Numero di punti (2 byte)
    stream << static_cast<quint16>(trajDuration);     // ✅ nuova informazione: durata totale

    for (const QPointF &pt : trajectory) {
        stream << static_cast<float>(pt.x()); // scrivo nel paccheto i 4 byte per X
        stream << static_cast<float>(pt.y()); // scrivo nel paccheto i 4 byte per Y
    }

    /*for (QTcpSocket *socket : _socketsList) {
        if (socket->state() == QAbstractSocket::ConnectedState)
            socket->write(data); //  invia i byte a ogni client
    }*/
}

