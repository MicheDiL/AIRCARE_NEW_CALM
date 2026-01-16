/*La tua applicazione Qt è ancora un "server" UDP, perché:

L'app Qt ascolta passivamente su una porta UDP (es. 5000) → udpSocket->bind(QHostAddress::Any, port)

Il client embedded (TMS570 con lwIP) invia il primo pacchetto UDP verso l’app Qt (es. un "ciao")

L'app Qt riceve quel pacchetto, salva IP e porta, e poi risponde con pacchetti contenenti x,y del mouse
*/
#ifndef MYUDPSERVER_H
#define MYUDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QPointF>
#include <QHostAddress>
#include <QElapsedTimer>        // classe Qt pensata per misurare il tempo trascorso da un evento. Ci permette di creare un timer che viene controllato regolarmente per sapere se il client è ancora “vivo”, quindi è un modo elegante, leggero ed efficiente per simulare una “connessione” UDP
#include <QTimer>

class MyUdpServer : public QObject
{
    Q_OBJECT
public:

    explicit MyUdpServer(int port, QObject *parent = nullptr);
    void sendToLastClient(const QString &message); // Invia solo all’ultimo client UDP contattato
    bool isStarted() const;

signals:
    void newClientConnected();
    void clientDisconnect();            // Segnale generato dal seguente evento: il client si è disconnesso
    void dataReceived(QString message); // Segnala alla UI i messaggi ricevuti
    void UpdatePenPositionLabel(QPointF pos); // Segnale per aggiornare la label delle coordinate del pennino sulla UI

public slots:
    void sendPenPosition(QPointF pos);                              // Riceve la posizione del pennino dal DrawingArea e la manda come pacchetto binario
    void sendFullTrajectoryToClient(const QList<QPointF> &trajectory, int trajDuration);

private slots:
    void processPendingDatagrams(); // Quando arriva un pacchetto UDP
    void checkAlive();

private:
    QUdpSocket *_udpSocket;

    // Informazioni dell’ultimo client che ci ha mandato un pacchetto
    QHostAddress _lastClientAddr;
    quint16 _lastClientPort;

    // Serializzazione dei dati da mandare al client
    QByteArray serializePenPosition(const QPointF &pos);

    QTimer *_aliveTimer;     // nuovo timer per monitorare alive
    QElapsedTimer _keepAliveTimer; // oggetto della classe QElapsedTimer, usato per misurare il tempo trascorso da un certo evento
    bool _clientAlive = false;
};

#endif // MYUDPSERVER_H
