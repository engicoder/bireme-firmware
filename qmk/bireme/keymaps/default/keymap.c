// this is the style you want to emulate.
// This is the canonical layout file for the Quantum project. If you want to add another keyboard,

#include QMK_KEYBOARD_H

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
enum bireme_layers
{
	_MALT,
	_SHIFTED,
	_FUNCTION,
	_FUNCSHIFT
};

enum bireme_keycodes
{
  FNKEY = SAFE_RANGE,
  SHIFT,
  M_VOLU,
  M_VOLD,
  M_ESCM
};

#define LONGPRESS_DELAY 150
#define LAYER_TOGGLE_DELAY 300

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = LAYOUT(
  KC_ESC,	KC_1,  	 KC_2, 	 KC_3, 	  KC_4, KC_5, 	KC_EQL,      		KC_MINS,KC_6,	KC_7,	KC_8, 	 KC_9, 	  KC_0,    KC_BSPC,
  KC_TAB,	KC_Q, 	 KC_W, 	 KC_E, 	  KC_R, KC_T, 	KC_LBRC,      		KC_RBRC,KC_Y,	KC_U,	KC_I, 	 KC_O, 	  KC_P,	   KC_BSLS,
  KC_CAPS,	KC_A, 	 KC_S, 	 KC_D, 	  KC_F,	KC_G,              				 	KC_H,	KC_J,	KC_K,	 KC_L, 	  KC_SCLN, KC_QUOT,
  KC_LSFT,	KC_Z, 	 KC_X, 	 KC_C, 	  KC_V, KC_B, 	KC_DEL,      		KC_NO,	KC_N,	KC_M,	KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
  KC_LCTL, 	KC_LGUI, KC_GRV, KC_LALT,       KC_SPC,	KC_SPC,      	    KC_SPC, KC_SPC,	        KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT,
									        KC_NO, 	KC_NO,      		KC_NO,	KC_NO
  ),
/*
  [2] = LAYOUT(
  k00, k01, k02, k03, k04, k05, k06,      k07, k08, k09, k0A, k0B, k0C, k0D,
  k10, k11, k12, k13, k14, k15, k16,      k17, k18, k19, k1A, k1B, k1C, k1D,
  k20, k21, k22, k23, k24, k25, k26,      k27, k28, k29, k2A, k2B, k2C, k2D,
  k30, k31, k32, k33, k34, k35,                k38, k39, k3A, k3B, k3C, k3D,
  k50, k51, k52, k53,      k55, k56,      k57, k58,      k5A, k5B, k5C, k5D,
                           k65, k66,      k67, k68
  ),

  [3] = LAYOUT(
  k00, k01, k02, k03, k04, k05, k06,      k07, k08, k09, k0A, k0B, k0C, k0D,
  k10, k11, k12, k13, k14, k15, k16,      k17, k18, k19, k1A, k1B, k1C, k1D,
  k20, k21, k22, k23, k24, k25, k26,      k27, k28, k29, k2A, k2B, k2C, k2D,
  k30, k31, k32, k33, k34, k35,                k38, k39, k3A, k3B, k3C, k3D,
  k50, k51, k52, k53,      k55, k56,      k57, k58,      k5A, k5B, k5C, k5D,
                           k65, k66,      k67, k68
  ),
*/
};

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
  return true;
};

void matrix_scan_user(void) {
    uint8_t layer = biton32(layer_state);

    switch (layer) {
    	case 0:
    		break;
        default:
            break;
    }
};

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=true;
  debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}
