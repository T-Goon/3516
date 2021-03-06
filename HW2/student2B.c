#include "project2.h"
#include "student_common.h"

#define RCV_STATE 0

int current_state;
int current_seq;
/*
 * The following routine will be called once (only) before any other
 * entity B routines are called. You can use it to do any initialization
 */
void B_init() {
  current_state = RCV_STATE;
  current_seq = SEQ0;
}
/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
  int acknowledgement;
  // Empty content of response messages
  char* empty = malloc(sizeof(MESSAGE_LENGTH));
  memset(empty, 0, MESSAGE_LENGTH);

  // Check for corruption
  if(isCorrupt(&packet) == TRUE){
    acknowledgement = NAK;
  } else{
    acknowledgement = ACK;
  }

  // Send a response to sender
  int checksum = calculateChecksum(empty, acknowledgement, current_seq);
  struct pkt* response = makePacket(current_seq, acknowledgement, checksum, empty);
  tolayer3(BEntity, *response);
  free(empty);
  free(response);

  // Don't deliver message wrong sequence or currupt
  if(acknowledgement == NAK ||  packet.seqnum != current_seq)
    return;

  // Deliver message
  struct msg* message = malloc(sizeof(message));
  memcpy(message->data, packet.payload, MESSAGE_LENGTH);
  tolayer5(BEntity, *message);

  // Change sequence number
  current_seq = changeSEQ(current_seq);

  free(message);
}

void B_output(struct msg message){

}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
}
