# Serveur Command and Control

Ce serveur TCP est concu pour recevoir des identifiants (username@ipaddr / password) envoyés par le malware ld_preload une fois ceux-ci recuperer.

## Compilation
- Makefile
`make`
- Manuellement
`gcc -Wall - Wextra server.c server.h -o serv`

## Utilisation
- Execution
`./serv`
- Le serveur est directement en mode listening des connexions tcp entrantes sur le port 6000.
- Une fois "l'api call" command recu et valide, le mot de passe sera enregistrer dans la base de donnée.
