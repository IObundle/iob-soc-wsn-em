// Functions

// Initialize Mixer
void mixer_init(int base_address);

// Set power down
void mixer_set_pd(char value);

// Set OTA
void mixer_set_ota(char value);

// Set Buff
void mixer_set_buff(char value);

// Turn Mixer On
#define mixer_on() mixer_set_pd(0)

// Turn Mixer Off
#define mixer_off() mixer_set_pd(1)
