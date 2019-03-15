void pwm_limit_calc(const bool armed, const uint16_t *disarmed_pwm, const uint16_t *min_pwm, const uint16_t *max_pwm, const float *output, uint16_t *effective_pwm)
{
	switch (limit->state) {
	case PWM_LIMIT_STATE_INIT:
		if (armed) {
			if (limit->time_armed == 0) {
				limit->time_armed = hrt_absolute_time();
			}
			if (hrt_elapsed_time(&limit->time_armed) >= INIT_TIME_US) {
				limit->state = PWM_LIMIT_STATE_OFF;
			}
		}
		break;

	case PWM_LIMIT_STATE_OFF:
		if (armed) {
			limit->state = PWM_LIMIT_STATE_RAMP;
			limit->time_armed = hrt_absolute_time();
		}
		break;

	case PWM_LIMIT_STATE_RAMP:
		if (!armed) {
			limit->state = PWM_LIMIT_STATE_OFF;
		}
		else if (hrt_elapsed_time(&limit->time_armed) >= RAMP_TIME_US) {
			limit->state = PWM_LIMIT_STATE_ON;
		}
		break;

	case PWM_LIMIT_STATE_ON:
		if (!armed) {
			limit->state = PWM_LIMIT_STATE_OFF;
		}
		break;

	default:
		break;
	}

	switch (local_limit_state) {
	case PWM_LIMIT_STATE_OFF:
	case PWM_LIMIT_STATE_INIT:
		for (unsigned i = 0; i < num_channels; i++) {
			effective_pwm[i] = disarmed_pwm[i];
		}
		break;

	case PWM_LIMIT_STATE_RAMP:
		for (unsigned i = 0; i < num_channels; i++) {
			float control_value = output[i];
			effective_pwm[i] = control_value * (max_pwm[i] - min_pwm[i]) / 2 + (max_pwm[i] + min_pwm[i]) / 2;
		}
		break;

	case PWM_LIMIT_STATE_ON:
		for (unsigned i = 0; i < num_channels; i++) {
			float control_value = output[i];
			effective_pwm[i] = control_value * (max_pwm[i] - min_pwm[i]) / 2 + (max_pwm[i] + min_pwm[i]) / 2;
		}
		break;

	default:
		break;
	}

}
