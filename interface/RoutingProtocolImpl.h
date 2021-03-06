#ifndef ROUTINGPROTOCOLIMPL_H
#define ROUTINGPROTOCOLIMPL_H

#include "RoutingProtocol.h"
#include "Node.h"
#include "Port.h"
#include <netinet/in.h>
#include <map>
#include <set>

using namespace std;

class RoutingProtocolImpl : public RoutingProtocol {
  public:
    RoutingProtocolImpl(Node *n);
    ~RoutingProtocolImpl();

    void init(unsigned short num_ports, unsigned short router_id, eProtocolType protocol_type);
    // As discussed in the assignment document, your RoutingProtocolImpl is
    // first initialized with the total number of ports on the router,
    // the router's ID, and the protocol type (P_DV or P_LS) that
    // should be used. See global.h for definitions of constants P_DV
    // and P_LS.

    void handle_alarm(void *data);
    // As discussed in the assignment document, when an alarm scheduled by your
    // RoutingProtoclImpl fires, your RoutingProtocolImpl's
    // handle_alarm() function will be called, with the original piece
    // of "data" memory supplied to set_alarm() provided. After you
    // handle an alarm, the memory pointed to by "data" is under your
    // ownership and you should free it if appropriate.

    void recv(unsigned short port, void *packet, unsigned short size);
    // When a packet is received, your recv() function will be called
    // with the port number on which the packet arrives from, the
    // pointer to the packet memory, and the size of the packet in
    // bytes. When you receive a packet, the packet memory is under
    // your ownership and you should free it if appropriate. When a
    // DATA packet is created at a router by the simulator, your
    // recv() function will be called for such DATA packet, but with a
    // special port number of SPECIAL_PORT (see global.h) to indicate
    // that the packet is generated locally and not received from 
    // a neighbor router.

 private:
    Node *sys; // To store Node object; used to access GSR9999 interfaces

		/********** variables **********/

	// currently using protocol
	eProtocolType protocol;

	// number of ports in this router
	unsigned short numOfPorts;

	// router ID
	unsigned short myID;

	// data for distinguishing types of alarm
	char* alarm_exp;	// expiration checking
	char* alarm_pp;		// ping-pong update
	char* alarm_update;	// protocol update

	// port informations
	Port *ports;

	// forwarding maping (table): from destination ID to forwarding port
	map<unsigned short, unsigned short> Forwarding;
	
	// map of Distance Vector table
	map<unsigned short, DVCell> DVMap;

	// map of Link State table
	map<unsigned short, Vertice> nodeVec;
	unsigned int mySequence;		// my sequence number

		/********** functions **********/

	// set the expiration checking alarm
	void setExpAlarm();
	// set the ping-pong message alarm
	void setPPAlarm();
	// set the protocol update alarm
	void setUpdateAlarm();

	// handle the expiration checking alarm; return true if succeed
	bool handleExp();
	// handle the ping-pong message alarm; return true if succeed
	bool handlePP();
	// handle the protocol update alarm; return true if succeed
	bool handleUpdate();

	// update status (tables) for Distance Vector Protocol
	bool DVUpdate();
	// update status (tables) for Link State Protocol
	bool LSUpdate();

	// initialize all ports
	void initPorts(int number);

	// find forwarding entry
	unsigned short findForward(unsigned short dest);
	// update table entry
	void updateForward(unsigned short destID, unsigned short fwdPort);
	// disable a link in entry
	void disableForward(unsigned short destID);
	// forward DATA packet
	void forwardData(void* packet, unsigned short destID, unsigned short size);
	// free forwarding table
	void printForward();

	// Deal with the DATA packet
	void recvDATA(unsigned short port, void *packet, unsigned short size);
	// Deal with PING or PONG packet
	void recvPP(unsigned short port, void *packet, unsigned short size);

	// Deal with DV packet
	void recvDV(unsigned short port, void *packet, unsigned short size);
	// update an entry of DV table
	bool updateDVTable(unsigned short nodeId, unsigned short cost, unsigned short sourceId);
	// flood DV update message
	void sendDVUpdateMessage();
	// use the newest DV to update forwarding table
	void updateForwardUsingDV();

	// Deal with LS packet
	void recvLS(unsigned short port, void *packet, unsigned short size);
	// get shortest path by using Dijkstra algorithm
	void dijkstra();
	// get the nearest node that is not in the work set
	pair<unsigned short, unsigned short> nodeIDWithMinDistance(map<unsigned short, unsigned short> tempMap);
	// check expirations of LS Vertices
	void checkLSExp();
	// forward LS packet that is received from others
	void sendReceivedLSPck(unsigned short port, char *packet, unsigned short size);
	// flood LS packet to all alive neighbors
	void sendLSTable();
};

#endif

