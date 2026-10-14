/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"

#include "motor.h"
#include "tim.h"
#include <stdlib.h>

void motor_set_duty(uint32_t ccr)
{
    if (ccr > PWM_MAX)
    {
        ccr = PWM_MAX;
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PWM_MAX - ccr);
}

HAL_StatusTypeDef motor_init(void)
{
    motor_set_duty((PWM_MAX * PWM_DUTY_INIT) / PERCENT_MAX);   // 60 % -> 2550
    return HAL_OK;
}

HAL_StatusTypeDef motor_pwm_start(void)
{
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1)    != HAL_OK) return HAL_ERROR;
    if (HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) return HAL_ERROR;
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2)    != HAL_OK) return HAL_ERROR;
    if (HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2) != HAL_OK) return HAL_ERROR;
    return HAL_OK;
}

HAL_StatusTypeDef motor_pwm_stop(void)
{
    if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1)    != HAL_OK) return HAL_ERROR;
    if (HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1) != HAL_OK) return HAL_ERROR;
    if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2)    != HAL_OK) return HAL_ERROR;
    if (HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2) != HAL_OK) return HAL_ERROR;
    return HAL_OK;
}

int sh_set_ccr(h_shell_t* h_shell, int argc, char** argv)
{
    char* end_ptr;
    unsigned long value;
    int size;

    if (argc != SET_CCR_ARGC)
    {
        size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                        "Usage : SET_CCR XXXX (0 a %lu)\r\n", (unsigned long)PWM_CCR_MAX);
        h_shell->drv.transmit(h_shell->print_buffer, size);
        return -1;
    }

    value = strtoul(argv[1], &end_ptr, DECIMAL_BASE);

    if ((end_ptr == argv[1]) || (*end_ptr != '\0'))
    {
        size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                        "Erreur : valeur non numerique\r\n");
        h_shell->drv.transmit(h_shell->print_buffer, size);
        return -1;
    }

    if (value > PWM_CCR_MAX)
    {
        value = PWM_CCR_MAX;   // saturation
    }

    motor_set_duty((uint32_t)value);

    size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                    "CCR = %lu\r\n", value);
    h_shell->drv.transmit(h_shell->print_buffer, size);
    return 0;
}
