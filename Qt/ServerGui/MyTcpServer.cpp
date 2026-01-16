 #include "MyTcpServer.h"

MyTcpServer::MyTcpServer(int port, QObject *parent)                                                 // Definizione del costruttore della classe MyTcpServer
    : QObject{parent}
{
    /*
    new QTcpServer(this) crea un oggetto QTcpServer da qualche parte nella memoria heap e come parent gli viene assegnato this,
    cioè l'istanza di MyTcpServer. Quindi _server viene collegato a quell'oggetto. In Qt, assegnare un parent garantisce che la
    memoria venga gestita automaticamente (quando this viene distrutto, anche _server verrà eliminato)
    */
    _server = new QTcpServer(this);

    connect(_server, &QTcpServer::newConnection, this, &MyTcpServer::on_client_connecting);         // Faccio la connessione fra il segnale emesso quando un client tenta di connettersi al server e lo slot che gestisce questo evento

    // Dico al server TCP quali sono gli indirizzi IP di potenziali client che possono connettersi con esso e su quale porta deve stare in ascolto
    _isStarted = _server->listen(QHostAddress::Any, port);                                          // Il server TCP inizierà ad ascoltare tutte le interfacce di rete sulla porta port! (listen restituisce un bool).
    if(!_isStarted)
    {
        qDebug() << "Server could not start";
    } else {
        qDebug() <<"Server started...";
    }
}

// ======================================================= Definizione dei Metodi =======================================================

bool MyTcpServer::isStarted() const
{
    /*
    Solitamente operazioni come: verificare connessioni, gestire socket, o fare attese di rete lunghe potrebbero bloccare il thread principale.
    Per questo di solito in linguaggi come C++ queste operazioni vengono svolte su thread paralleli così da poter fare verifiche (es. isStarted(),
    o waitForReadyRead()) senza bloccare la UI o la logica principale. In Qt grazie al meccanismo di seganli-slot possiamo gestire queste operazioni
    come operazioni asincrone ovvero: l'operazione parte in background, e il risultato viene notificato quando è pronto, ma sempre nello stesso thread
    (tipicamente il thread principale o GUI)
    */
    return _isStarted;
}

void MyTcpServer::sendToAll(QString message)
{
    foreach(auto socket, _socketsList){                                                         // Per mandare un messaggio testuale a tutti i client connessi iteriamo su ogni QTcpSocket* della lista
        socket->write(message.toUtf8());                                                        // Converte il testo UTF-16 di QString in UTF-8 (toUtf8()) e poi scive il testo sul socket (invio TCP)
    }
}

void MyTcpServer::setSendMode(SendMode mode) {
    sendMode = mode;
}

MyTcpServer::SendMode MyTcpServer::getSendMode() const {
    return sendMode;
}

QByteArray MyTcpServer::serializePenPosition(const QPointF &pos)
{
    QByteArray data;                                                                             // Crea un array di byte vuoto in cui inserirai i dati da inviare
    QDataStream stream(&data, QIODevice::WriteOnly);                                             // Passo il riferimento del QByteArray come destinazione in modalità scrittura
    stream.setByteOrder(QDataStream::LittleEndian);                                              // Definisco l'oridine dei bit in data: LittleEndian = Byte meno significativo per primo

    stream << static_cast<quint8>(0xA1);         // 1 byte: header                                                 // Header: tipo = posizione pennino (1 byte)
    stream << static_cast<float>(pos.x());       // 4 byte: X                                                 // Scrivo nel QByteArray data le coordinate x come un numero float di 4 byte
    stream << static_cast<float>(pos.y());       // 4 byte: Y                                                 // Scrivo nel QByteArray data le coordinate y come un numero float di 4 byte

    return data;                                                                                  // Posizione serializzata in 9 byte
}

// ======================================================= Definizione degli Slot =======================================================

void MyTcpServer::sendPenPosition(QPointF pos)
{
    // L’invio punto-per-punto
    QByteArray data = serializePenPosition(pos);                                                   // Converte la posizione del pennino in pacchetto binario
    for (QTcpSocket *socket : _socketsList) {                                                      // Manda la posizione serializzata a tutti i clients connessi
        if (socket->state() == QAbstractSocket::ConnectedState)
            socket->write(data);
    }
}

void MyTcpServer::sendFullTrajectoryToClient(const QList<QPointF> &trajectory, int trajDuration)
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

    for (QTcpSocket *socket : _socketsList) {
        if (socket->state() == QAbstractSocket::ConnectedState)
            socket->write(data); //  invia i byte a ogni client
    }
}

void MyTcpServer::on_client_connecting()
{
    qDebug() <<"a client connected to server";

    /* Quando un client ha fatto richiesta di connessione al nostro server TCP allora apriamo il socket di comunicazione.
    In particolare quando un client si connette, QTcpServer mette in coda un oggetto QTcpSocket che rappresenta quella connessione.
    Chiamando nextPendingConnection(), estrai il socket di quel client dalla coda, e lo puoi usare per comunicare.*/
    auto socket = _server->nextPendingConnection();                                                 // Restituisce un puntatore a un oggetto QTcpSocket che rappresenta il client con cui è stata stabilita la connessione

    // Nel momento in cui viene invocato questo slot allora partono le seguenti connessioni:
    connect(socket, &QTcpSocket::readyRead, this, &MyTcpServer::clientDataReady);                   // Per mandare messaggi al server e visualizzarli nella sua Console, possiamo connettere il segnale readyRead allo slot clientDataReady
    connect(socket, &QTcpSocket::disconnected, this, &MyTcpServer::clientDisconnected);             // Per chiudere la comunicazione fra client e server, possiamo connettere il segnale disconnected allo slot clientDisconnected

    _socketsList.append(socket);                                                                    // Il socket estratto lo memorizzo nella lista _socketsList (così posso tenerne traccia).
    socket->write("Welcome to CALM Server!");                                                       // Quando viene estratto il socket di comunicazione, il Server TCP da il benvenuto al Client
    sendMode = SendMode::Batch; // ✅ Passa in modalità Batch all’arrivo di un client

    emit newClientConnected();                                                                      // Una volta stabilito/estratto il socket di comunicazione con il client che si connette al server posso emettere il segnale di connessione
}

void MyTcpServer::clientDataReady()
{
    /* Qui vogliamo leggere i dati che sono stati mandati al server dal client attraverso il socket.
    Per capire quale client sta mandando un messaggio al server possiamo andare a considerare la sorgente dell'evento che ha causato
    la chiamata di questo slot usando la funzione "sender()". */
    auto socket = qobject_cast<QTcpSocket *>(sender());                                             // Faccio il cast della sorgente per identificare il socket mittente
    auto data = QString(socket->readAll());                                                         // Leggo il dato mandato dal client al server
    emit dataReceived(data);                                                                        // Emetto il segnale per segnalare alla mainWindow l'arrivo del messaggio

    /* Facciamo in modo che i messaggi che arrivano da un client al server vengano trasmessi anche a tutti gli altri client */
    //sendToAll(QString(data)); // se facessi così però il messaggio mandato da un client ritornerebbe anche allo stesso client oltre che ad essere trasmesso agli altri client
    // Per evitare il problema della riga precedente facciamo come segue
    foreach(auto s, _socketsList){
        // Andiamo a confrontare la s con socket di riga 137 che rappresenta la sorgente che ha causato l'emissione del segnale dataReceived();
        if(s != socket){
            s->write(data.toUtf8()); // inoltra il messaggio ricevuto da un certo client a tutti gli altri client meno che a chi lo ha mandato al server
        }
    }
}

void MyTcpServer::clientDisconnected()
{
    // Funzione chiamata automaticamente da Qt quando un client si disconnette
    emit clientDisconnect(); // Segnale ascoltato da MainWindow
}

