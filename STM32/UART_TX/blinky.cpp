#include "main.h" 

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init(); 

  while (1) {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_Delay(500);  
  }
}