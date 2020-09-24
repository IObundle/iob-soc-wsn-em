// Functions

// Initialize LPF
void lpf_init(int base_address);

// Set power down
void lpf_set_pd(char value);

// Set FC
void lpf_set_fc(char value);

// Turn LPF On
#define lpf_on() lpf_set_pd(0)

// Turn LPF Off
#define lpf_off() lpf_set_pd(1)
