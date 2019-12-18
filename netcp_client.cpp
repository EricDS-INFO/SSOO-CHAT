// This program should open "prueba.txt", read it and send it to the receiver socket
// librerías:

#include<iostream>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include "socket.cpp"

// Constantes:

#define BUFLEN  1024
#define PORT    8888    

// Prototipos:

sockaddr_in make_ip_address(const std::string& ip_address, int port);

int protected_main (int argc, char *argv[])
{  
  if ( argc == 2) {
    // variables:
      struct sockaddr_in  server_address, local_address;
      int                 addrlen = sizeof(server_address), recv_len, ret;
      char                buffer[BUFLEN];

    //

    // File oppening:
      int fileFd;
      if (fileFd = open(argv[1], O_RDONLY) < 0) {
        throw std::system_error(errno, std::system_category(),
                            "no se pudo abrir el fichero.");
  
      }
    //

    // Address: 
      memset((char *) &server_address, 0, sizeof(server_address));
      server_address = make_ip_address("127.1.1.1", PORT);
      if (server_address.sin_addr.s_addr < 0) {
        throw std::system_error(errno, std::system_category(),
            "no se pudo crear una dirección para el socket");
      }

      memset((char *) &local_address, 0, sizeof(local_address));
      local_address = make_ip_address("", 8081);
      if (local_address.sin_addr.s_addr < 0) {
        throw std::system_error(errno, std::system_category(),
            "no se pudo crear una dirección para el socket");
      }

    //

    // Socket:
      Socket<Message> local_socket(local_address);

    //

      struct Message message;

      std::cout << "Sending file ... \n";
      
      
      while((ret = read(fileFd, message.text.data(), message.text.size()-1)) > 0) {

        message.text.data()[ret] = 0x00;
        local_socket.send_to(message, server_address); 
      }

    
    if ( int is_closed = close(fileFd) < 0) {
      throw std::system_error(errno, std::system_category(),
                            "no se pudo cerrar el fichero.");
  
    }
  } else {
    std::cout << "falta el nombre del fichero a enviar " << std::endl;
  
  }

  return 0;
}

int main (int argc, char *argv[])
{
  try
  {
    return protected_main(argc, argv);
  }
  catch(const std::bad_alloc& e)
  {
    std::cerr << "mitalk :" << "memoria insuficiente\n";
    return 1;
  }
  catch(const std::system_error& e)
  {
    std::cerr << "mitalk :" << e.what() << "\n";
    return 2;
  }

  catch(...)
  {
    std::cout << "Error desconocido\n";
    return 99;
  }
  
}


sockaddr_in make_ip_address(const std::string& ip_address, int port)
{
  sockaddr_in address;

  address.sin_family = AF_INET;  
  
  address.sin_port = htons(port);  
  // Dependiendo de la dirección dada se 
  if (ip_address == "") {
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    
  } else {

    // opción igualmente válida (al parecer)
    // local_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    inet_aton(ip_address.c_str(), &(address.sin_addr));
    
  }

  return address;
}
