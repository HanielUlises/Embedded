#include "RTE_Components.h"
#include CMSIS_device_header

int main() {
    // Set Region 0 using Attr 0
    ARM_MPU_SetMemAttr(0UL, ARM_MPU_ATTR(
        ARM_MPU_ATTR_MEMORY_(0UL, 1UL, 1UL, 1UL),
        ARM_MPU_ATTR_MEMORY_(0UL, 1UL, 1UL, 1UL)
    ));

    ARM_MPU_SetRegion(0UL,
        ARM_MPU_RBAR(0x08000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 1UL),
        ARM_MPU_RLAR(0x080FFFFFUL, 0UL)
    );

    // Enable MPU
    ARM_MPU_Enable(0);

    // Execute application code that is access protected by the MPU
    ARM_MPU_Disable();

    return 0;
}
