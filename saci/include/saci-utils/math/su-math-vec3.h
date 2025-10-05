
typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
} su_Vec3;

/**
 * @fn su_vec3_subtract
 * @brief Returns a new vector from a vector subtracted from another
 *
 * @param[in] a The base vector that will be subtracted
 * @param[in] b The vetor that will subtract
 *
 * @return su_Vec3 The result of the subtraction
 */
su_Vec3 su_vec3_subtract(su_Vec3 a, su_Vec3 b);

/**
 * @fn su_vec3_add
 * @brief Returns a new vector from a vector added from another
 *
 * @param[in] a The vector that will be added
 * @param[in] b The vetor that will added
 *
 * @return su_Vec3 The result of the subtraction
 */
su_Vec3 su_vec3_add(su_Vec3 a, su_Vec3 b);

/**
 * @fn su_vec3_scale
 * @brief Returns a new scaled vector
 *
 * @param[in] a The vector that will be added
 * @param[in] b The vetor that will added
 *
 * @return su_Vec3 The result of the scalation
 */
su_Vec3 su_vec3_scale(su_Vec3 v, float scalar);

/**
 * @fn su_vec3_normalize
 * @brief Returns a normalized vector to unit length (1)
 *
 * @param[in] v The vector to normalize
 *
 * @return su_Vec3 The normalized vector with a length of 1
 */
su_Vec3 su_vec3_normalize(su_Vec3 v);

/**
 * @fn su_vec3_cross
 * @brief Returns the computed cross product of two vectors
 *
 * @param[in] a The first vector
 * @param[in] b The second vector
 *
 * @return su_Vec3 The result of the cross product of vectors a and b
 */

su_Vec3 su_vec3_cross(su_Vec3 a, su_Vec3 b);

/**
 * @fn su_vec3_dot
 * @brief Returns the computed dot product of two vectors
 *
 * @param[in] a The first vector
 * @param[in] b The second vector
 *
 * @return float The result of the dot product of vectors a and b
 */
float su_vec3_dot(su_Vec3 a, su_Vec3 b);
