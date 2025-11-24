# TP_Reseau_ESE_Terlinden_Bonnet--Galand

![Le Pinout de la STM](/datasheet/nucleo-f411re-f446re-wifi-serial1.png)

# Capteur BMP280

1. Les addresses I2C possibles pour le BMP280 sont 0x76 et 0x77, selon la configuration du pin SDO/SDI. Sur notre capteur, l'adresse est 0x77.
2. Le registre permettant d'identifier le composant est le registre ID, situé à l'adresse 0xD0. La valeur attendue pour le BMP280 est 0x58.
3. Le registre et la valeur permattant de placer le composant en mode normal sont le registre CTRL_MEAS (0xF4) avec mode[1:0] = 11.
4. Les registres contenant l'étalonnage du composant sont les registres de 0x88 à 0xA1.
5. Les registres contenants les données de température sont les registres 0xFA (Contains the MSB part ut[19:12] of the raw temperature measurement output data.), 0xFB (Contains the LSB part ut[11:4] of the raw temperature measurement output data.) et 0xFC (Contains the XLSB part ut[3:0] of the raw temperature measurement output data.). 
6. Les registres contenants les données de pression sont les registres 0xF7 (Contains the MSB part up[19:12] of the raw pressure measurement output data.), 0xF8 (Contains the LSB part up[11:4] of the raw pressure measurement output data.) et 0xF9 (Contains the XLSB part up[3:0] of the raw pressure measurement output data.).
7. Les fonctions de conversion des données brutes en valeurs exploitables sont décrites dans la section 3.11.3 "Compensation formula" du datasheet du BMP280.

![Les fonctions](/datasheet/fonctions.png)


