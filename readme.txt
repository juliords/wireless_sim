
Server (port: 9999)
- conects nodes using an undirected graph
- adding edge: "a <node-id-from> <node-id-to>"
- deleting edge: "d <node-id-from> <node-id-to>"
- console: graph construction commands (two above)

Client
- must pass server port and address via argv
- on client startup it sends an init message to server
- after that, server replies informing its id
- Console: message to be sent to server

How it works:
Client send message to server
Server redirect message to its neighbours (1 hop)

