#ifndef __IMU_TASK_H__
#define __IMU_TASK_H__

#define INS_YAW_ADDRESS_OFFSET    0
#define INS_PITCH_ADDRESS_OFFSET  1
#define INS_ROLL_ADDRESS_OFFSET   2

void imu_task(void *argument);

#endif
