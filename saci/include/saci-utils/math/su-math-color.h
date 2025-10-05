typedef struct {
    float r; /**< Red compoonent */
    float g; /**< Green compoonent */
    float b; /**< Blue compoonent */
    float a; /**< Alpha (transparency) compoonent */
} su_Color;

/* === Color Operations === */

/**
 * @fn su_color_from_hex
 * @brief Returns a new su_Color from a given hex code
 *
 * @param[in] hex The hex code 0x12345678
 *
 * @return su_Color The new color based on the hex code
 */
su_Color su_color_from_hex(su_U32 hex);

/**
 * @fn su_color_from_u8
 * @brief Returns a new color from given rgba values
 *
 * @param[in] r The r component of the color
 * @param[in] g The g component of the color
 * @param[in] b The b component of the color
 * @param[in] a The a component of the color
 *
 * @return su_Color The new color based on the rgba value
 */
su_Color su_color_from_u8(su_U8 r, su_U8 g, su_U8 b, su_U8 a);

/**
 * @fn su_color_to_hex
 * @brief Returns a new hex code from a color
 *
 * @param[in] color The color to be evaluated
 *
 * @return su_U32 The new hex code
 */
su_U32 su_color_to_hex(su_Color color);
