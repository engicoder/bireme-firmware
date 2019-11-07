
#ifdef LEFT_HAND

    /* Left hand key definitions */
    #define S00 {1,  0}
    #define S01 {1,  1}
    #define S02 {1, 15}
    #define S03 {0, 12}
    #define S04 {0, 16}
    #define S05 {0, 20}
    #define S06 {0, 22}
    #define S07 {0, 10}
    #define S08 {1,  2}
    #define S09 {0, 20}
    #define S10 {0,  5}
    #define S11 {0, 15}
    #define S12 {0, 19}
    #define S13 {0, 23}
    #define S14 {1, 11}
    #define S15 {1,  3}
    #define S16 {1, 14}
    #define S17 {0,  9}
    #define S18 {0, 14}
    #define S19 {0, 21}
    #define S20 {1, 10}
    #define S21 {1,  4}
    #define S22 {0,  3}
    #define S23 {0,  7}
    #define S24 {0, 13}
    #define S25 {0, 17}
    #define S26 {0, 24}
    #define S27 {0,  4}
    #define S28 {0, 31}
    #define S29 {1, 13}
    #define S30 {1, 12}
    #define S31 {0, 28}
    #define S32 {0, 26}
    #define S33 {0, 27}
    #define S34 {0, 25}

    #define PIPE_NUMBER BIREME_LEFT_HAND_PIPE

#elif defined (RIGHT_HAND)

    /* Right hand key definitions */
    #define S00 {1,  0}
    #define S01 {1,  6}
    #define S02 {0,  9}
    #define S03 {1, 13}
    #define S04 {0,  2}
    #define S05 {0, 28}
    #define S06 {0, 27}
    #define S07 {1,  1}
    #define S08 {1,  7}
    #define S09 {0, 11}
    #define S10 {0,  5}
    #define S11 {1, 12}
    #define S12 {0, 29}
    #define S13 {0, 26}
    #define S14 {1,  2}
    #define S15 {1,  8}
    #define S16 {0, 13}
    #define S17 {1, 15}
    #define S18 {0, 13}
    #define S19 {0, 30}
    #define S20 {1,  3}
    #define S21 {1,  9}
    #define S22 {0, 14}
    #define S23 {0,  7}
    #define S24 {0,  3}
    #define S25 {0, 31}
    #define S26 {0, 25}
    #define S27 {0, 15}
    #define S28 {0, 16}
    #define S29 {0, 17}
    #define S30 {0, 21}
    #define S31 {0, 19}
    #define S32 {0, 23}
    #define S33 {0, 20}
    #define S34 {0, 24}

    #define PIPE_NUMBER BIREME_RIGHT_HAND_PIPE

#else
    #error Side not defined. Must be one of LEFT_HAND or RIGHT_HAND.
#endif



// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
