#include "common.h"

/* send_msg send a message on socket sock
   sock: the socket
   code: message's protocol code
   size: message's size
   msg: message to be sent
*/
int send_msg(int sock, unsigned char code, unsigned char size, char *body)
{
  msg_t msg;

  msg.code = code;
  msg.size = size;

  /* Code nécessaire à envoyer le message correspondant au protocle
     sur la socket
  */

  send(sock, &msg, sizeof(msg), 0);
  printf("Sent on %d, code: %d, size: %d\n", sock, msg.code, msg.size);
  send(sock, body, size, 0);
  printf("Sent on %d, body: %s, size: %d\n", sock, body, size);
  return 0;
}

/* recv_msg recv a message from the socket sock
   sock: the socket
   code: message's protocol code
   size: message's size
   msg: message to be received
*/
int recv_msg(int sock, unsigned char *code, unsigned char *size, char **body)
{
  msg_t msg;


  recv(sock, &msg, sizeof(msg), 0);
  *code = msg.code;
  *size = msg.size;
  printf("Receive on %d, code: %d, size: %d\n",sock, msg.code, msg.size);
  recv(sock, *body, *size, 0);
  printf("Receive on %d, body: %s \n", sock, *body);
  return 0;
}
