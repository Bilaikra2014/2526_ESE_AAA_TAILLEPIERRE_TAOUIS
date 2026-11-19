/*
 * input_encoder.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_ACQUISITION_INPUT_ENCODER_H_
#define INC_ACQUISITION_INPUT_ENCODER_H_

#include "main.h"
#include "user_interface/shell.h"

/* --- Codeur incrémental --- */
#define ENC_PPR                 1024     // impulsions par tour
#define ENC_EDGES_PER_PULSE     4        // mode encodeur x4 (TI1 et TI2)
#define ENC_COUNTS_PER_REV      (ENC_PPR * ENC_EDGES_PER_PULSE)   // 4096 pas par tour

/* --- Période de mesure (TIM7 : 170 MHz / 170 / 5000 = 5 ms) --- */
#define SPEED_TE_MS             5
#define MS_PER_MIN              60000    // 60 s x 1000 ms

HAL_StatusTypeDef input_encoder_init(void);
void input_encoder_update(void);         // appelée par TIM7 toutes les SPEED_TE_MS
int32_t input_encoder_get_rpm(void);

int sh_rpm(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_ACQUISITION_INPUT_ENCODER_H_ */
