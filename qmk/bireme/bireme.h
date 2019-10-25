#ifndef BIREME_H
#define BIREME_H

#include "quantum.h"

#define red_led_off   PORTF |= (1<<5)
#define red_led_on    PORTF &= ~(1<<5)
#define blu_led_off   PORTF |= (1<<1)
#define blu_led_on    PORTF &= ~(1<<1)
#define grn_led_off   PORTF |= (1<<4)
#define grn_led_on    PORTF &= ~(1<<4)
#define wht_led_off   PORTF |= (1<<0)
#define wht_led_on    PORTF &= ~(1<<0)


// This a shortcut to help you visually see your layout.
// The first section contains all of the arguments
// The second converts the arguments into a two-dimensional array
#define LAYOUT( \
  k00, k01, k02, k03, k04, k05, k06,      k07, k08, k09, k0A, k0B, k0C, k0D, \
  k10, k11, k12, k13, k14, k15, k16,      k17, k18, k19, k1A, k1B, k1C, k1D, \
  k20, k21, k22, k23, k24, k25,                k28, k29, k2A, k2B, k2C, k2D, \
  k30, k31, k32, k33, k34, k35, k36,      k37, k38, k39, k3A, k3B, k3C, k3D, \
  k40, k41, k42, k43,      k45, k46,      k47, k48,      k4A, k4B, k4C, k4D, \
                           k55, k56,      k57, k58 \
) \
  {                                                           \
    { k00,   k01,   k02,   k03,   k04,   k05, k06,     k07,   k08, k09,   k0A,   k0B,   k0C,   k0D   }, \
    { k10,   k11,   k12,   k13,   k14,   k15, k16,     k17,   k18, k19,   k1A,   k1B,   k1C,   k1D   }, \
    { k20,   k21,   k22,   k23,   k24,   k25, KC_NO,   KC_NO, k28, k29,   k2A,   k2B,   k2C,   k2D   }, \
    { k30,   k31,   k32,   k33,   k34,   k35, k36,     k37,   k38, k39,   k3A,   k3B,   k3C,   k3D   }, \
    { k40,   k41,   k42,   k43,   KC_NO, k45, k46,     k47,   k48, KC_NO, k4A,   k4B,   k4C,   k4D   }, \
    { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, k55, k56,     k57,   k58, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }  \
  }

/* Switch numbers as on pcb and represented in bitmap received
  L00, L01, L02, L03, L04, L05, L06,      R06, R05, R04, R03, R02, R01, R00, \
  L07, L08, L09, L10, L11, L12, L13,      R13, R12, R11, R10, R09, R08, R07, \
  L14, L15, L16, L17, L18, L19,                R19, R18, R17, R16, R15, R14, \
  L20, L21, L22, L23, L24, L25, L26,      R26, R25, R24, R23, R22, R21, R20, \
  L27, L28, L29, L30,      L31, L32,      R32, R31,      R30, R29, R28, R27, \
                           L33, L34,      R34, R33 )
*/
#endif


