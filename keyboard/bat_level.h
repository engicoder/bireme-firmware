#ifndef BAT_LEVEL_H
#define BAT_LEVEL_H

void bat_level_init(void);
uint8_t bat_level_read(void);
void bat_level_update(void);
void bat_level_uninit(void);
#endif