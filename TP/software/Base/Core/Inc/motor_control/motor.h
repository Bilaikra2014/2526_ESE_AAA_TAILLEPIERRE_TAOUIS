/*
 * motor.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_MOTOR_CONTROL_MOTOR_H_
#define INC_MOTOR_CONTROL_MOTOR_H_

#include "main.h"
#include "user_interface/shell.h"

#define PWM_ARR          4249U            // f_PWM = 170 MHz / (2 x 4250) = 20 kHz
#define PWM_MAX          (PWM_ARR + 1U)   // 4250 pas
#define PWM_DUTY_INIT    60U              // rapport cyclique de test, en %
#define PERCENT_MAX      100U

#define PWM_CCR_LIMIT_PERCENT  95U                                    // valeur max autorisée
#define PWM_CCR_MAX            ((PWM_MAX * PWM_CCR_LIMIT_PERCENT) / PERCENT_MAX)  // 4037

#define SET_CCR_ARGC           2     // "SET_CCR" + la valeur
#define DECIMAL_BASE           10

HAL_StatusTypeDef motor_init(void);
HAL_StatusTypeDef motor_pwm_start(void);
HAL_StatusTypeDef motor_pwm_stop(void);
void motor_set_duty(uint32_t ccr);
int sh_set_ccr(h_shell_t* h_shell, int argc, char** argv);

#endif /* INC_MOTOR_CONTROL_MOTOR_H_ */
