# TP_Reseau_ESE_Terlinden_Bonnet--Galand


# TP1 - Communication I2C avec un capteur BMP280
![Le Pinout de la STM](/datasheet/nucleo-f411re-f446re-wifi-serial1.png)

## Capteur BMP280

1. Les addresses I2C possibles pour le BMP280 sont 0x76 et 0x77, selon la configuration du pin SDO/SDI. Sur notre capteur, l'adresse est 0x77.
2. Le registre permettant d'identifier le composant est le registre ID, situé à l'adresse 0xD0. La valeur attendue pour le BMP280 est 0x58.
3. Le registre et la valeur permattant de placer le composant en mode normal sont le registre CTRL_MEAS (0xF4) avec mode[1:0] = 11.
4. Les registres contenant l'étalonnage du composant sont les registres de 0x88 à 0xA1.
5. Les registres contenants les données de température sont les registres 0xFA (Contains the MSB part ut[19:12] of the raw temperature measurement output data.), 0xFB (Contains the LSB part ut[11:4] of the raw temperature measurement output data.) et 0xFC (Contains the XLSB part ut[3:0] of the raw temperature measurement output data.). 
6. Les registres contenants les données de pression sont les registres 0xF7 (Contains the MSB part up[19:12] of the raw pressure measurement output data.), 0xF8 (Contains the LSB part up[11:4] of the raw pressure measurement output data.) et 0xF9 (Contains the XLSB part up[3:0] of the raw pressure measurement output data.).
7. Les fonctions de conversion des données brutes en valeurs exploitables sont décrites dans la section 3.11.3 "Compensation formula" du datasheet du BMP280.

![Les fonctions](/datasheet/fonctions.png)


# TP2


Nous nous connectons en SSH à notre Raspberry Pi:
```
PS C:\Users\ombeline> ssh thomas@192.168.4.193
The authenticity of host '192.168.4.193 (192.168.4.193)' can't be established.
ED25519 key fingerprint is SHA256:7C8a9UbbyhT8ioKEpNc8v6FPypgk1JN+8w4kZu0JQ6c.
This key is not known by any other names.
Are you sure you want to continue connecting (yes/no/[fingerprint])? yes
Warning: Permanently added '192.168.4.193' (ED25519) to the list of known hosts.
thomas@192.168.4.193's password:
Linux thomas 6.12.47+rpt-rpi-v6 #1 Raspbian 1:6.12.47-1+rpt1 (2025-09-16) armv6l

The programs included with the Debian GNU/Linux system are free software;
the exact distribution terms for each program are described in the
individual files in /usr/share/doc/*/copyright.

Debian GNU/Linux comes with ABSOLUTELY NO WARRANTY, to the extent
permitted by applicable law.
thomas@thomas:~ $
```
L'addresse IP du Raspberry Pi est 192.168.4.193, elle est attribuée par le routeur DHCP du routeur lors de la première connexion du Raspberry Pi au réseau WiFi.

## Minicom
Nous installons minicom sur le Raspberry Pi, et activons le port gpio AMA0:
Voila une petite demo de minicom:
```
Welcome to minicom 2.10

OPTIONS: I18n
Port /dev/ttyAMA0, 14:37:27 [U]

Press CTRL-A Z for help on special keys

coucou c thomas et florian  
```


<img width="462" height="141" alt="image" src="https://github.com/user-attachments/assets/3386fd8a-7d24-4e4d-bb64-b072afc41bda" />






# TP3


On suit les étapes, et on arrive à lancer le serveur, dans un premier temps seulement dans la loopback, puis ensuite sur le navigateur.  
(Insérer les photos dans le dossier serveur)  
Le décorateur @app.route sert à lier une fonction à une adresse. Grâce à ce décorateur, quand une application Web accède à l'adresse donnée dans le décorateur, la fonction associée s'éxécute.  
Le fragment <int:index> sert à extraire et convertir la valeur où est situé le fragment en entier int. Cela permet ensuite d'utiliser cette valeur pour l'appeler dans une fonction.  
(Insérer une capture d'écran avec le code python qui marche.)  
On modifie ensuite le code pour avoir une requête JSON.  
(Insérer une capture avec le JSON.)  
On implémente ensuite la page de l'erreur 404.  
(Insérer les captures de l'erreur 404.)  
Fin au niveau du test de la méthode POST.  

