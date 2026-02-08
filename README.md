# C SERVER

The encryption mechanism is work in progress and there are still bugs.

We create a server using C POSIX api. We handle manually threads to allow clients to join and exchange communication via end-2-end encryption at OPEN-SSL standards. This is a demo and not meant for production! 

Compile using 
```bash
gcc server.c -o server
gcc client.c -o client
```
