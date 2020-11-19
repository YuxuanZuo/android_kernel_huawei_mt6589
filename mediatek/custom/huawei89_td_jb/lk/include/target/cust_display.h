#ifndef __CUST_DISPLAY_H__
#define __CUST_DISPLAY_H__

#define BAR_LEFT            (215)
#define BAR_TOP             (156)
#define BAR_RIGHT           (265)
#define BAR_BOTTOM          (278)

#define BAR_OCCUPIED_COLOR  (0x07E0)    // Green
#define BAR_EMPTY_COLOR     (0xFFFF)    // White
#define BAR_BG_COLOR        (0x0000)    // Black

//customize for cmmc_fwvga
#define CAPACITY_LEFT                (172) // battery capacity center
#define CAPACITY_TOP                 (357)
#define CAPACITY_RIGHT               (307)
#define CAPACITY_BOTTOM              (573)

#define NUMBER_LEFT                  (182) // number
#define NUMBER_TOP                   (214)
#define NUMBER_RIGHT                 (220)
#define NUMBER_BOTTOM                (268)

// percent number_left + 2*number_width
#define PERCENT_LEFT                 (259)
#define PERCENT_TOP                  NUMBER_TOP
#define PERCENT_RIGHT                (307)
#define PERCENT_BOTTOM               NUMBER_BOTTOM

// top animation
#define TOP_ANIMATION_LEFT           CAPACITY_LEFT
#define TOP_ANIMATION_TOP            (0)
#define TOP_ANIMATION_RIGHT          CAPACITY_RIGHT
#define TOP_ANIMATION_BOTTOM         (24)

/* The option of new charging animation */
#define ANIMATION_NEW

#endif // __CUST_DISPLAY_H__