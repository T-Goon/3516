#include "node_common.h"

#define NODE_NUM 1

extern int TraceLevel;
extern float clocktime;

struct distance_table dt1;
struct NeighborCosts   *neighbor1;

/* students to write the following two routines, and maybe some others */

void rtinit1() {
  // Get initial neighbor costs
  neighbor1 = getNeighborCosts(NODE_NUM);

  // Initialize distance table
  int temp[MAX_NODES][MAX_NODES] = {
    {INFINITY, INFINITY, INFINITY, INFINITY},
    {neighbor1->NodeCosts[0], neighbor1->NodeCosts[1], neighbor1->NodeCosts[2], neighbor1->NodeCosts[3]},
    {INFINITY, INFINITY, INFINITY, INFINITY},
    {INFINITY, INFINITY, INFINITY, INFINITY}
  };

  memcpy(&dt1.costs, &temp, sizeof(int) * MAX_NODES * MAX_NODES);

  initMsg(NODE_NUM, &dt1, clocktime);

  // Broadcast current nodes distances to neighbors
  broadcast(NODE_NUM, neighbor1, &dt1, clocktime);
}


void rtupdate1( struct RoutePacket *rcvdpkt ) {
  rcvMsg(NODE_NUM, rcvdpkt->sourceid, clocktime);

  // Update this node's distance table
  int updated = updateDt(NODE_NUM, &dt1, rcvdpkt->sourceid, rcvdpkt->mincost);

  // Chck if distance table has been updated
  if(updated == YES){
    currentDTmsg(NODE_NUM, &dt1, clocktime);

    // If yes broadcast distance table to neighbors
    broadcast(NODE_NUM, neighbor1, &dt1, clocktime);
  }
}
