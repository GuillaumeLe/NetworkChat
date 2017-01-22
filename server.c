#include "common.h"
#include "chatroom.h"
#include <signal.h>
#include <pthread.h>

#define MAX_CONN 10            // nombre maximale de requêtes en attentes

int DFLAG;
int srv_sock;

void sig_handler(int s)
{
  switch (s)
    {
    case SIGINT:
      stop_chat_room();
      sleep(1); /* wait for client to close */
      close(srv_sock);
      exit(0);
    case SIGPIPE:
      break;
    default: break;
    }
}

int create_a_listening_socket(int srv_port, int maxconn){
  int srv_sock = -1;
  /* Code nécessaires à la création d'une socket en
     écoute :

     - appel à socket()

     - appel à bind()

     - appel à listen()

     avec les bons paramètres et contrôles d'erreurs.

     La fonction retourne l'identifiant de la socket serveur ou -1 en
     cas d'erreur.
  */
  srv_sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in sock_addr;
  struct in_addr ip_addr;

  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(srv_port);
  ip_addr.s_addr = INADDR_ANY;
  sock_addr.sin_addr = ip_addr;

  bind(srv_sock, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr_in));

  listen(srv_sock, maxconn);

  return srv_sock;
}

int accept_clt_conn(int srv_sock, struct sockaddr_in *clt_sockaddr){
  int clt_sock =-1;

  /* Code nécessaire à l'acception d'une connexion sur
     la socket en écoute (passée en argument via le paramètre srv_sock :

     - appel à accept()

     avec les bons paramètres et contrôles d'erreurs.

     La fonction retourne l'identifiant de la socket cliente ou -1 en
     cas d'erreur.

  */
  clt_sock = accept(srv_sock, NULL, NULL);

  DEBUG("connexion accepted");

  return clt_sock;
}

int main(void)
{
  signal(SIGINT, sig_handler);
  signal(SIGPIPE, sig_handler);

  DFLAG = 1;

  /* create a listening socket */
  srv_sock = create_a_listening_socket(SRV_PORT, MAX_CONN);
  if (srv_sock < 0)
    {
      DEBUG("failed to create a listening socket");
      exit(EXIT_FAILURE);
    }

  /* initialize the chat room with no client */
  initialize_chat_room();

  while (1){
    int clt_sock;
    struct sockaddr_in clt_sockaddr;
    char *clt_ip;
    int clt_port;

    /* wait for new incoming connection */
    if ((clt_sock = accept_clt_conn(srv_sock, &clt_sockaddr)) < 0 )
    {
	    perror("accept_clt_conn");
	    exit(EXIT_FAILURE);
    }

    clt_ip = inet_ntoa(clt_sockaddr.sin_addr);
    clt_port = ntohs(clt_sockaddr.sin_port);

    /* register new buddies in the chat room */
    if ( login_chatroom(clt_sock, clt_ip, clt_port) != 0 )
    {
	    DEBUG("client %s:%d not accepted", clt_ip, clt_port);
	    close(clt_sock);
	    DEBUG("close clt_sock %s:%d", clt_ip, clt_port);
    }

   // broadcast_text("admin", "hello guys !");
    // unsigned char code = 31;
    // char text [12] = "Hello World";
    // send_msg(clt_sock,code,sizeof(text), text);
    //
    // void *buf = malloc(100);
    // unsigned char size;
    // recv_msg(clt_sock, &code, &size, buf);
    // printf("%s \n", buf);
    // printf("%d \n", code);
    // printf("%d \n", size);

	  close(clt_sock);
	  DEBUG("close clt_sock %s:%d", clt_ip, clt_port);

  } /* while */
  return EXIT_SUCCESS;
}
