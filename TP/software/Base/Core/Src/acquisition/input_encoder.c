/*
 * input_encoder.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "acquisition/input_encoder.h"

#include "acquisition/input_encoder.h"
#include "tim.h"
#include <stdio.h>

static uint16_t enc_count_prev = 0U;
static volatile int32_t enc_delta = 0;
static volatile int32_t speed_rpm = 0;

/**
 * @brief Démarre le codeur (TIM3) puis le cadencement de la mesure (TIM7).
 */
HAL_StatusTypeDef input_encoder_init(void)
{
    if (HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL) != HAL_OK) return HAL_ERROR;

    enc_count_prev = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);

    if (HAL_TIM_Base_Start_IT(&htim7) != HAL_OK) return HAL_ERROR;
    return HAL_OK;
}

/**
 * @brief Calcule la vitesse à partir de la variation du compteur depuis la dernière mesure.
 * vitesse [tr/min] = delta x 60000 / (4096 x Te[ms])
 * Appelée toutes les SPEED_TE_MS par l'interruption de TIM7.
 */
void input_encoder_update(void)
{
    uint16_t count = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);
    int16_t delta = (int16_t)(count - enc_count_prev);   // gère le débordement et le sens

    enc_count_prev = count;
    enc_delta = delta;
    speed_rpm = ((int32_t)delta * MS_PER_MIN) / (ENC_COUNTS_PER_REV * SPEED_TE_MS);
}

int32_t input_encoder_get_rpm(void)
{
    return speed_rpm;
}

/**
 * @brief Commande shell "rpm" : vitesse en tr/min (signe = sens de rotation).
 */
int sh_rpm(h_shell_t* h_shell, int argc, char** argv)
{
    int size;

    (void)argc;
    (void)argv;

    size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                    "Vitesse = %ld tr/min (delta = %ld)\r\n",
                    (long)speed_rpm, (long)enc_delta);
    h_shell->drv.transmit(h_shell->print_buffer, size);
    return 0;
}
