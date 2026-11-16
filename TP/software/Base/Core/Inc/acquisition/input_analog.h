#ifndef INPUT_ANALOG_H
#define INPUT_ANALOG_H

#include "main.h"
#include "user_interface/shell.h"

/* --- ADC (12 bits, référence = VREF+ de la Nucleo) --- */
#define ADC_VREF_MV             3250U    // JP8 en [1-2] et SB14 ouvert : 3,25 V (à vérifier)
#define ADC_FULL_SCALE          4095
#define ADC_MID_SCALE           2048
#define ADC_CAL_SAMPLES         64U

/* --- Chaîne Timer/ADC/DMA --- */
#define ADC_NB_CHANNELS         2U
#define ADC_IDX_U               0U       // rang 1 : U_Imes (PA1)
#define ADC_IDX_BUS             1U       // rang 2 : Bus_Imes (PC2)
#define ADC_TRIGGER_CCR         250U     // instant de mesure (~1,5 µs avant le creux)

/* --- Capteur de courant GO 10-SME/SP3 --- */
#define SENSOR_SENS_MV_PER_A    50
#define MA_PER_A                1000
#define SENSOR_MA_PER_MV        (MA_PER_A / SENSOR_SENS_MV_PER_A)   // 20 mA par mV

typedef struct
{
    uint8_t index;        // position dans le buffer DMA
    int32_t offset_raw;   // valeur ADC à courant nul
} current_sensor_t;

extern current_sensor_t current_u;
extern current_sensor_t current_bus;

HAL_StatusTypeDef input_analog_init(void);
int32_t current_get_ma(const current_sensor_t * sensor);

int sh_current(h_shell_t* h_shell, int argc, char** argv);
int sh_current_zero(h_shell_t* h_shell, int argc, char** argv);

#endif /* INPUT_ANALOG_H */
