#include "project2.h"
#include "student_common.h"

#define SEND_STATE 0
#define WAIT_RES_STATE 1
#define A_TIME_UNIT 100

struct pkt* current_packet;
int current_seq;
int current_state;

struct msg_queue;
struct msg_queue{
  struct msg message;
  struct msg_queue* next;
};

struct msg_queue* queue_start;
struct msg_queue* queue_end;

void addToQueue(struct msg message);
struct msg* queuePop();
void sendMessage(struct msg* message);

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
  current_seq = SEQ0;
  current_state = SEND_STATE;
  queue_start = NULL;
  queue_end = NULL;
}

/*
 * A_output(message), where message is a structure of type msg, containing
 * data to be sent to the B-side. This routine will be called whenever the
 * upper layer at the sending side (A) has a message to send. It is the job
 * of your protocol to insure that the data in such a message is delivered
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
  // If sender is waiting for a response add message to the queue
  if(current_state == WAIT_RES_STATE){
    addToQueue(message);
  }else{ // Send message if sender is ready to send
    sendMessage(&message);
  }
}

// Makes a packet and sends it to EntityB
void sendMessage(struct msg* message){
  // Create packet
  int checksum = calculateChecksum(message->data, ACK, current_seq);
  current_packet = makePacket(current_seq, ACK, checksum, message->data);

  // Send packet and start timer
  tolayer3(AEntity, *current_packet);
  startTimer(AEntity, A_TIME_UNIT);

  // Change state to waiting for response
  current_state = WAIT_RES_STATE;
}

/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
  // If sender is not expecting a response do nothing
  if(current_state == SEND_STATE)
    return;

  // Stop timer and check if response is corrupted or NAK
  stopTimer(AEntity);
  if(isCorrupt(&packet) == TRUE || packet.acknum == NAK){
    // Resend packet if NAK or corrupted
    tolayer3(AEntity, *current_packet);
    startTimer(AEntity, A_TIME_UNIT);

  }else if(packet.acknum == ACK){
    // Change state to be ready to send
    free(current_packet);
    current_state = SEND_STATE;

    // Change sequence number
    current_seq = changeSEQ(current_seq);

    // Send any messages in the queue
    if(queue_start != NULL){
      struct msg* message = queuePop();
      sendMessage(message);
      free(message);
    }
  }
}


/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
  // A packet was lost resend the packet
  tolayer3(AEntity, *current_packet);
  startTimer(AEntity, A_TIME_UNIT);
}

// Adds a message to the message queue
void addToQueue(struct msg message){
  // Create a new queue node
  struct msg_queue* new = malloc(sizeof(struct msg_queue));
  memcpy(&new->message, &message, sizeof(struct msg));
  new->next = NULL;

  if(queue_start == NULL){
    // Queue emtpy
    queue_start = new;
    queue_end = new;
  }else{
    // Add node to the end of list
    queue_end->next = new;
    queue_end = new;
  }
}

// Pop a message out of the queue
struct msg* queuePop(){
  // Return null if queue is empty
  if(queue_start == NULL)
    return NULL;

  // Make a copy of the message in the queue
  struct msg* message = malloc(sizeof(struct msg));
  memcpy(message, &queue_start->message, sizeof(struct msg));

  // Move the head of the queue to the next node
  struct msg_queue* old = queue_start;
  queue_start = queue_start->next;
  free(old);

  return message;
}
