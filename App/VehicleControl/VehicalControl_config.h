/*
 * VehicalControl_config.h
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#ifndef APP_VEHICLECONTROL_VEHICALCONTROL_CONFIG_H_
#define APP_VEHICLECONTROL_VEHICALCONTROL_CONFIG_H_

/*
 * Telemetry period in main loop iterations.
 * If your main loop runs ~1 ms, 100 = 100 ms.
 * Adjust to your actual cycle time.
 */
#define VEHICLECONTROL_TELEM_PERIOD     100

/* Default drive speed if phone sends 0 or invalid */
#define VEHICLECONTROL_DEFAULT_SPEED    80



#endif /* APP_VEHICLECONTROL_VEHICALCONTROL_CONFIG_H_ */
