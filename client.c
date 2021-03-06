#include "common.h"
#include <signal.h>

char *srv_name = "localhost";
int clt_sock;

int DFLAG;

void sig_handler(int s)
{
  msg_t msg;

  switch (s)
    {
    case SIGINT:
      msg.code = END_OK;
      msg.size = 0;

      send(clt_sock, &msg, HEADSIZE, 0);

      close(clt_sock);
      exit(EXIT_SUCCESS);
    default: return;
    }
}

/* Établie une session TCP vers srv_name sur le port srv_port
 * char *srv_name: nom du serveur (peut-être une adresse IP)
 * int srv_port: port sur lequel la connexion doit être effectuée
 *
 * renvoie: descripteur vers le socket
 */
int connect_to_server(char *srv_name, int srv_port){
  int clt_sock = -1;

  /* Code nécessaires à la création d'une socket en
     écoute :

     - résolution du nom avec gethostbyname

     - appel à socket()

     - appel à connect()

     avec les bons paramètres et contrôles d'erreurs.

     La fonction retourne l'identifiant de la socket cliente ou -1 en cas d'erreur
  */
  struct hostent *host;
  host = gethostbyname(srv_name);

  clt_sock = socket(AF_INET,SOCK_STREAM, 0);

  struct sockaddr_in socket_addr;
  struct in_addr ip_addr;
  memcpy(&(ip_addr.s_addr), host->h_addr_list[0], host->h_length);
  socket_addr.sin_family = AF_INET;
  socket_addr.sin_port = htons(srv_port);
  socket_addr.sin_addr = ip_addr;
  connect(clt_sock, (struct sockaddr *)&socket_addr ,sizeof(struct sockaddr_in));
  return clt_sock;
}

int authenticate(int clt_sock){

  /* Code nécessaire à l'authentification auprès du serveur :

     - attendre un paquet AUTH_REQ

     - répondre avec un paquet AUTH_RESP

     - attendre un paquet ACCESS_OK / ACCESS_DENIED / AUTH_REQ

     - agir en conséquence ...

  */

  unsigned char code = 0;
  unsigned char size;
  char login[] = "gleroy";
  char *msg = malloc(100);
  printf("Authentication\n");
  recv_msg(clt_sock, &code, &size, &msg);
  printf("%d\n", code);
  if(code == AUTH_REQ){
    printf("Receive AUTH_REQ \n");
    // printf("Enter login : \n" );
    // scanf("%s", login);
    // printf("\n");
    send_msg(clt_sock, AUTH_RESP, sizeof(login), login);
    unsigned char waited_code = 0;
    recv_msg(clt_sock, &waited_code, &size, &msg);
    switch (waited_code) {
      case ACCESS_OK:
        printf("Access is ok \n");
        return 0;
        break;
      case ACCESS_DENIED:
        printf("Acces refused \n");
        return 1;
        break;
      case AUTH_REQ:
        break;
      default:
        break;
    }
  }


  return -1;
}

int instant_messaging(int clt_sock){

  while(1){
    fd_set rset;
    unsigned char code;
    unsigned char size;
    char *data;

    FD_ZERO(&rset);
    FD_SET(clt_sock, &rset);
    FD_SET(STDIN_FILENO, &rset);

    /* pour les étapes 2 à 4 se contenter de recevoir les messages
       envoyés par le serveur et les afficher à l'utilisateur
    */

    if (select(clt_sock+1, &rset, NULL, NULL, NULL) < 0){
      PERROR("select");
      exit(EXIT_FAILURE);
    }

    if (FD_ISSET(STDIN_FILENO, &rset)){
    /* l'utilisateur a tapé un nouveau message */
      DEBUG("STDIN_FILENO isset");
      data = malloc(BUFFSIZE);
      if (fgets(data, BUFFSIZE, stdin) == NULL){
    	/* gérer feof et ferror */

   //   <COMPLÉTER>

    	return 0;
      }
      size = strlen(data)+1;

      DEBUG("sending MESG %s(%d)", data, size);

    //  <COMPLÉTER>

      free(data);

    }

     if (FD_ISSET(clt_sock, &rset)){
      /* réception d'un message du serveur */
      /* expected: <code><datalen>[<data>] */

      //  <COMPLÉTER>
      recv(clt_sock, data, size, 0);
     }

  } /* while (1) */
  return 0;
}

int main(int argc, char *argv[]){
  // char srv_name[BUFFSIZE];
  int srv_port = 4444;

  DFLAG = 1;

  signal(SIGINT, sig_handler);

  clt_sock = connect_to_server(srv_name, srv_port);
  if (clt_sock < 0)
    exit(EXIT_FAILURE);

  if (authenticate(clt_sock) < 0){
    close(clt_sock);
    eprintf("connexion closed\n");
    exit(EXIT_FAILURE);
  }


  /*
  if (instant_messaging(clt_sock) < 0){
    close(clt_sock);
    eprintf("connexion closed\n");
    exit(EXIT_FAILURE);
  }
  */
  // void *buf=malloc(100);
  // unsigned char code;
  // unsigned char size;
  // recv_msg(clt_sock, &code, &size, buf);
  // printf("Code : %d \n", code);
  // printf("Size : %d \n", size);
  // printf("%s \n", buf);
  //
  // char text[20] = "I'm a new client";
  // send_msg(clt_sock,31, sizeof(text),text);

  close(clt_sock);

  eprintf("connexion closed\n");
  exit(EXIT_SUCCESS);
}
