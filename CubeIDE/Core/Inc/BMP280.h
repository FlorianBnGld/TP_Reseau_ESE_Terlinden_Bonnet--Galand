#ifndef __BMP280_DRIVER_H
#define __BMP280_DRIVER_H

#include <stdint.h> // Nécessaire pour les types entiers standards (si BMP280_S32_t et BMP280_U32_t ne sont pas définis ailleurs)

// -----------------------------------------------------------------------------
// 1. Définitions des types (Adaptées aux types utilisés dans le .c)
// -----------------------------------------------------------------------------

// Si vos types sont définis de manière personnalisée (comme dans l'exemple),
// vous devez les définir ici pour que le .h soit autonome.
// Supposons que S32 = signed 32-bit integer et U32 = unsigned 32-bit integer.

typedef int32_t   BMP280_S32_t;
typedef uint32_t  BMP280_U32_t;
typedef int16_t   BMP280_S16_t;
typedef uint16_t  BMP280_U16_t;


// -----------------------------------------------------------------------------
// 2. Coefficients de Calibration du BMP280
// -----------------------------------------------------------------------------

// Ces coefficients doivent être lus dans la mémoire du capteur (registres 0x88 à 0xA1)
// et doivent être accessibles par les fonctions de compensation.
// Ils doivent être déclarés comme extern si vous les initialisez dans le fichier .c

// Coefficients de Température
extern BMP280_U16_t dig_T1;
extern BMP280_S16_t dig_T2;
extern BMP280_S16_t dig_T3;

// Coefficients de Pression
extern BMP280_U16_t dig_P1;
extern BMP280_S16_t dig_P2;
extern BMP280_S16_t dig_P3;
extern BMP280_S16_t dig_P4;
extern BMP280_S16_t dig_P5;
extern BMP280_S16_t dig_P6;
extern BMP280_S16_t dig_P7;
extern BMP280_S16_t dig_P8;
extern BMP280_S16_t dig_P9;


// -----------------------------------------------------------------------------
// 3. Variable Globale de Compensation
// -----------------------------------------------------------------------------

// t_fine est la "fine temperature" et DOIT être globale car elle est calculée
// par la fonction de compensation de température et utilisée par celle de pression.
extern BMP280_S32_t t_fine;


// -----------------------------------------------------------------------------
// 4. Prototypes des Fonctions de Compensation
// -----------------------------------------------------------------------------

/**
 * @brief Compense la température brute (adc_T) et calcule la température fine (t_fine).
 * @param adc_T Lecture brute (ADC) de la température.
 * @return Température compensée en DegC * 100 (résolution 0.01 DegC).
 */
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);


/**
 * @brief Compense la pression brute (adc_P) en utilisant t_fine (doit être calculée juste avant).
 * @param adc_P Lecture brute (ADC) de la pression.
 * @return Pression compensée en Pa (Pascals).
 */
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P);


#endif // __BMP280_DRIVER_H
