# Projet de C - POC malware ld_preload communiquant avec serveur C2
  Ce projet est un poc d'un malware ld_preload capable de communiquer avec un serveur C2 afin d'exfiltrer des identifiants SSH et les envoyer à l'attaquant.
`. Il intercepte les identifiants SSH, les exfiltre vers un serveur distant, et masque sa présence sur le système.

## Objectif

- Intercepter les identifiants SSH saisis dans un terminal.
- Cacher la bibliothèque malveillante injectée via `LD_PRELOAD`.
- Exfiltrer les identifiants à un serveur distant (C2).

## Architecture Docker

```bash
+------------------+      +------------------+      +------------------+
|  victimedebian   | <--→ |   victimessh     | -->  |     serverc2     |
| (LD_PRELOAD Lab) |      | (SSH Target)     |      | (Exfiltration)   |
| 172.21.0.10      |      | 172.21.0.11      |      | 172.21.0.12      |
+------------------+      +------------------+      +------------------+
```

## Lancer l’environnement

```bash
docker compose up --build
```

Ports exposés :

- `5000` → terminal de `victimedebian`
- `5001` → SSH vers `victimessh`
- `6000` → serveur C2 (réception des identifiants)

## Description des machines

- **victimedebian** : machine infectée avec le malware `LD_PRELOAD`, déclenche l'interception.
- **victimessh** : serveur SSH utilisé pour simuler une connexion distante.
- **serverc2** : serveur TCP simple recevant les identifiants capturés.

## Malware LD_PRELOAD

- Redéfinit `read()`, `write()`, `open()`, `access()` etc.
- Cible uniquement les sessions `ssh` actives via `/proc/self/comm`.
- Sauvegarde les identifiants dans `/tmp/nothingtosee.txt`.
- Utilise `sendc2` pour les envoyer à `serverc2`.

Exfiltration :

```
command username@ip password
```

