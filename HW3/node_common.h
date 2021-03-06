#include <stdio.h>
#include <string.h>
#include "project3.h"

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};

void printdt( int MyNodeNumber, struct NeighborCosts *neighbor,
   struct distance_table *dtptr);

void printdtALL(int MyNodeNumber,struct distance_table *dtptr);

void broadcast(int sourceid, struct NeighborCosts* neighbors,
  struct distance_table* dt, float time);

int updateDt(int rcvId, struct distance_table* rcvDt,
  int sndId, int sndCosts[MAX_NODES]);

void initMsg(int id, struct distance_table* dt, float time);

void sndMsg(int rcvid, int sndid, struct distance_table* dt, float time);

void rcvMsg(int rcvid, int sndid, float time);

void currentDTmsg(int id, struct distance_table* dt, float time);
