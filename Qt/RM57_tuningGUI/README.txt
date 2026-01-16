OBIETTIVO & MACRO-ARCHITETTURA

L’app è un tool di tuning + telemetria in tempo reale per una MCU via UART:

GUI layer (MainWindow + DeviceDialog + PlotZoomDialog)
Gestisce input utente, stato connessione, invio parametri di tuning e visualizzazione live dei segnali.

I/O layer (SerialWorker in QThread dedicato)
Gestisce QSerialPort, riceve stream, fa parsing e valida frame (magic + CRC), emette campioni “puliti”.

Protocol layer (protocol.h)
Definisce il wire contract: struct packed, CRC, scaling, e factory di comandi TX.

La scelta architetturale chiave è che il traffico seriale e il parsing non vivono nel GUI thread: così la UI resta reattiva anche in presenza di burst di dati.

Perché è “la soluzione giusta”:

- QSerialPort vive in un solo thread → zero errori di affinity
- RX parsing e TX write avvengono nello stesso posto → niente race
- GUI riceve dati solo tramite signal → slot (queued) plotting avviene solo nel thread GUI → QCustomPlot non viene mai aggiornato dal worker

COME GESTIRE UNA TELEMETRIA AD ALTA FREQUENZA SENZA AVERE LAG:

Quando facciamo [emit telemetryReceived(s);] per ogni frame che arriva (es. 8 frame * 50 Hz = 400 segnali al secondo), succede questo:
- SerialWorker vive nel thread seriale
- la GUI vive nel thread principale

la connessione SerialWorker::telemetryReceived -> MainWindow::onTelemetry è quasi sempre una Qt::QueuedConnection (implicita se i thread sono diversi)

Cosa significa “queued connection” in pratica:

Ogni emit crea un evento (un QMetaCallEvent) che viene messo nella event queue del thread destinatario (GUI).
Se arrivano più eventi di quanti la GUI riesca a processarne, la coda cresce:

- lag dell’interfaccia
- ritardo nei plot (plotti valori vecchi)
- mouse/zoom che “scatta”
- CPU alta (perché replotti troppo spesso)

In breve: non è il parsing a pesare, è il numero di eventi + replots.

Pattern consigliato: accumulo nel worker + invio a “batch”

L’idea è ridurre drasticamente:

- il numero di emit verso la GUI
- il numero di replot() (che è costoso)

Invece di:

1 segnale Qt per ogni sample

fai:

1 segnale Qt ogni 20–50 ms contenente un array di sample

Così la GUI riceve tipo:

20–50 aggiornamenti al secondo (fluido)
anziché

500–5000 aggiornamenti al secondo (satura)
