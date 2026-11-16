#include "acquisition/input_analog.h"
#include "adc.h"
#include "tim.h"
#include <stdio.h>

current_sensor_t current_u   = { ADC_IDX_U,   ADC_MID_SCALE };
current_sensor_t current_bus = { ADC_IDX_BUS, ADC_MID_SCALE };

static volatile uint16_t adc_dma_buffer[ADC_NB_CHANNELS];   // rempli par le DMA

/* Mesure d'offset, faite dans le callback (moteur arrêté) */
static volatile uint32_t cal_remaining = 0U;
static uint32_t cal_sum_u   = 0U;
static uint32_t cal_sum_bus = 0U;

/**
 * @brief Calibre l'ADC, lance la chaîne Timer/ADC/DMA et la mesure d'offset.
 * À appeler après les MX_..._Init() et avant le démarrage des PWM.
 */
HAL_StatusTypeDef input_analog_init(void)
{
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) return HAL_ERROR;

    /* Instant de mesure dans la période PWM */
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, ADC_TRIGGER_CCR);

    /* Offset mesuré sur les ADC_CAL_SAMPLES premières conversions */
    cal_sum_u = 0U;
    cal_sum_bus = 0U;
    cal_remaining = ADC_CAL_SAMPLES;

    /* L'ADC attend le trigger du timer, le DMA remplit le buffer en continu */
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer, ADC_NB_CHANNELS) != HAL_OK) return HAL_ERROR;

    /* CH4 sans sortie : fait tourner le compteur et génère le trigger */
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

/**
 * @brief Fin d'une séquence (2 conversions) : le buffer DMA est à jour.
 * Une impulsion sur ADC_DBG (PB11) marque ce moment pour l'oscilloscope.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc)
{
    if (hadc->Instance != ADC1)
    {
        return;
    }


    if (cal_remaining > 0U)
    {
        cal_sum_u   += adc_dma_buffer[ADC_IDX_U];
        cal_sum_bus += adc_dma_buffer[ADC_IDX_BUS];
        cal_remaining--;

        if (cal_remaining == 0U)
        {
            current_u.offset_raw   = (int32_t)(cal_sum_u   / ADC_CAL_SAMPLES);
            current_bus.offset_raw = (int32_t)(cal_sum_bus / ADC_CAL_SAMPLES);
        }
    }
}

/**
 * @brief Courant en mA à partir de la dernière séquence ADC.
 * I = (Uadc - Uref) / 0,05, en entiers.
 */
int32_t current_get_ma(const current_sensor_t * sensor)
{
    int32_t raw = (int32_t)adc_dma_buffer[sensor->index];

    return ((raw - sensor->offset_raw) * (int32_t)ADC_VREF_MV * SENSOR_MA_PER_MV) / ADC_FULL_SCALE;
}

/**
 * @brief Commande shell "current" : I_U et I_bus en mA, avec valeur brute et offset.
 */
int sh_current(h_shell_t* h_shell, int argc, char** argv)
{
    int size;

    (void)argc;
    (void)argv;

    size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                    "I_U = %ld mA (raw %u, off %ld)\r\n",
                    (long)current_get_ma(&current_u),
                    (unsigned)adc_dma_buffer[ADC_IDX_U], (long)current_u.offset_raw);
    h_shell->drv.transmit(h_shell->print_buffer, size);

    size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                    "I_bus = %ld mA (raw %u, off %ld)\r\n",
                    (long)current_get_ma(&current_bus),
                    (unsigned)adc_dma_buffer[ADC_IDX_BUS], (long)current_bus.offset_raw);
    h_shell->drv.transmit(h_shell->print_buffer, size);
    return 0;
}

/**
 * @brief Commande shell "zero" : relance la mesure d'offset (moteur arrêté).
 */
int sh_current_zero(h_shell_t* h_shell, int argc, char** argv)
{
    int size;

    (void)argc;
    (void)argv;

    cal_sum_u = 0U;
    cal_sum_bus = 0U;
    cal_remaining = ADC_CAL_SAMPLES;

    size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                    "Offset en cours (%u mesures)\r\n", (unsigned)ADC_CAL_SAMPLES);
    h_shell->drv.transmit(h_shell->print_buffer, size);
    return 0;
}
