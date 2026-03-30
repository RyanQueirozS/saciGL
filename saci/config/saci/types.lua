local M = {
  UNKOWN = 0,
  VOIDPTR = 1,
  U8 = 2,
  U16 = 3,
  U32 = 4,
  U64 = 5,
  S8 = 6,
  S16 = 7,
  S32 = 8,
  S64 = 9,

  CHAR = 10,
  STRING = 11,

  FLOAT = 12,
  DOUBLE = 13,

  BOOL = 14,

  UV = 18,
  VEC2 = 19,
  VEC3 = 20,
  VEC4 = 21,

  COLOR = 22,

  MAT2 = 23,
  MAT3 = 24,
  MAT4 = 25,

  MAT2X3 = 26,
  MAT2X4 = 27,
  MAT3X2 = 28,
  MAT3X4 = 29,
  MAT4X2 = 30,
  MAT4X3 = 31,

  NULL = 32,

  SAMPLER2D = 33,
}

return M;
