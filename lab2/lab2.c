#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern unsigned int timer_counter; // Global counter incremented on every timer interrupt
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  
  u_int8_t configs = 0;

  if (timer_get_conf(timer, &configs) != 0) return -1;

  // Display the requested configuration field (all, mode, base, etc.)
  if (timer_display_conf(timer, configs, field) != 0) return -1;

  return 0;
}


int(timer_test_time_base)(uint8_t timer, uint32_t freq) {

  if (timer_set_frequency(timer, freq) != 0) return -1;

  return 0;
}


int(timer_test_int)(uint8_t time) {

  uint8_t bit_no = 0;                
  uint32_t irq_set = BIT(bit_no);     
    
  int r;                              
  int ipc_status;                     
  message msg;                        

  if (timer_subscribe_int(&bit_no) != 0) return -1;

  // Loop until the desired number of seconds has passed
  // (60 timer interrupts per second)
  while (timer_counter < (uint32_t)time * 60) {
        
        // Wait for a message (interrupt or other IPC)
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { 
            printf("driver_receive failed with: %d", r);
            continue;
        }

        // Check if the message is a notification
        if (is_ipc_notify(ipc_status)) {

            // Identify the source of the notification
            switch (_ENDPOINT_P(msg.m_source)) {

                case HARDWARE:
				
                    // Check if it was the timer interrupt
                    if (msg.m_notify.interrupts & irq_set) {

                        // Call the timer interrupt handler
                        timer_int_handler();

                        // Print elapsed time every second (60 interrupts)
                        if (timer_counter % 60 == 0) {
                            timer_print_elapsed_time();
                        }
                    }
                    break;

                default:
                    break;
            }
        }
    }

    if (timer_unsubscribe_int() != 0) return -1;

    return 0;
}
