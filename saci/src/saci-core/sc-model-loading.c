#include "saci-core/sc-gl.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"

//----------------------------------------------------------------------------//
// Helper declarations
//----------------------------------------------------------------------------//
#define __SACI_MAX_FACES_PER_FACE_LINE 16

#define __SACI_CHAR_IS_SPACE(x) ((x == ' ') || ((x == '\t')))
#define __SACI_CHAR_IS_DIGIT(x) ((unsigned int)((x) - '0') < (unsigned int)(10))
#define __SACI_CHAR_IS_NEWLINE(x) (((x) == '\r') || ((x) == '\n') || ((x) == '\0'))

#define __SACI_TRIANGULATE

typedef enum {
    SC_OBJ_COMMANDTYPE_EMPTY = 0,
    SC_OBJ_COMMANDTYPE_V,
    SC_OBJ_COMMANDTYPE_VN,
    SC_OBJ_COMMANDTYPE_VT,
    SC_OBJ_COMMANDTYPE_F,
    SC_OBJ_COMMANDTYPE_G,
    SC_OBJ_COMMANDTYPE_O,
    SC_OBJ_COMMANDTYPE_USEMTL,
    SC_OBJ_COMMANDTYPE_MTLLIB
} __sc_OBJ_CommandType;

struct __sc_OBJ_Command {
    __sc_OBJ_CommandType type;

    saci_Vec3 vertexPos;
    saci_Vec3 vertexNormal;
    saci_Vec2 vertexTexcoord;

    struct sc_VertexIndice faces[__SACI_MAX_FACES_PER_FACE_LINE];
    saci_u64 facesAmount;

    int verticeAmountInFace[__SACI_MAX_FACES_PER_FACE_LINE];
    saci_u64 verticeAmountInFaceSize;

    const char* groupName;
    saci_u32 groupNameLength;
    int pad0;

    const char* objectName;
    saci_u32 objectNameLength;
    int pad1;

#if 0
    const char* materialName;
    saci_u32 materialNameLength;
    int pad2;
#endif // if 0

    // const char* groupName;
    // saci_u32 groupNameLenght;
};

saci_Bool __sc_File_IsLineEnd(char c);

void __sc_String_SkipSpace(const char** token);

void __sc_String_SkipSpaceAndCR(const char** token);

int __sc_String_UntilSpace(const char* token);

saci_u64 __sc_File_CountLines(const char* buffer, saci_u64 bufferLength);

void __sc_File_PopulateLineInfo(const char* buffer, saci_u64 bufferLength, saci_u64** linePos,
                                saci_u64** lineLength);

void __sc_File_GetInfo(const char* buffer, saci_u64 bufferLenght, saci_u64** lineLenght,
                       saci_u64** linePos, saci_u64* lineAmount);

int __sc_OBJ_tryParseDouble(const char* s, const char* s_end, double* result);

struct sc_VertexIndice __sc_OBJ_ParseRawTriple(const char** token);

float __sc_OBJ_ParseFloat(const char** token);

void __sc_OBJ_ParseFloat2(float* x, float* y, const char** token);

void __sc_OBJ_ParseFloat3(float* x, float* y, float* z, const char** token);

saci_Bool __sc_OBJ_ParseLine(struct __sc_OBJ_Command* command, const char* line,
                             saci_u64 lineLenght);

//----------------------------------------------------------------------------//
// Base Definitions
//----------------------------------------------------------------------------//

saci_Bool sc_OBJ_Parse(const char* buffer, saci_u64 lenght, saci_Vec3** verticesPos,
                       saci_u64* verticesAmount, saci_Vec2** verticesTexCoords,
                       saci_u64* verticesTexCoordsAmount, struct sc_VertexIndice** indices,
                       saci_u64* indicesAmount);

//----------------------------------------------------------------------------//
// Helper function implementation
//----------------------------------------------------------------------------//

saci_Bool __sc_File_IsLineEnd(char c) {
    if (c == '\n') return SACI_TRUE;
    if (c == '\0') return SACI_TRUE;
    if (c == '\r') return SACI_TRUE;
    return SACI_FALSE;
}

void __sc_String_SkipSpace(const char** token) {
    while ((*token)[0] == ' ' || (*token)[0] == '\t') {
        (*token)++;
    }
}

void __sc_String_SkipSpaceAndCR(const char** token) {
    while ((*token)[0] == ' ' || (*token)[0] == '\t' || (*token)[0] == '\r') {
        (*token)++;
    }
}

int __sc_String_UntilSpace(const char* token) {
    const char* p = token;
    while (p[0] != '\0' && p[0] != ' ' && p[0] != '\t' && p[0] != '\r') {
        p++;
    }

    return (int)(p - token);
}

saci_u64 __sc_File_CountLines(const char* buffer, saci_u64 bufferLength) {
    saci_u64 lines = 0;
    for (saci_u64 i = 0; i < bufferLength; ++i) {
        if (__sc_File_IsLineEnd(buffer[i])) {
            lines++;
        }
    }
    if (bufferLength > 0 && !__sc_File_IsLineEnd(buffer[bufferLength - 1])) {
        lines++;
    }
    return lines;
}

void __sc_File_PopulateLineInfo(const char* buffer, saci_u64 bufferLength, saci_u64** linePos,
                                saci_u64** lineLength) {
    // Count the number of lines to allocate the arrays
    saci_u64 lines = __sc_File_CountLines(buffer, bufferLength);

    // Allocate memory for line positions and lengths
    *linePos = (saci_u64*)malloc(lines * sizeof(saci_u64));
    *lineLength = (saci_u64*)malloc(lines * sizeof(saci_u64));

    if (*linePos == NULL || *lineLength == NULL) {
        // Memory allocation failed
        return;
    }

    saci_u64 line_index = 0;
    saci_u64 line_start = 0;

    for (saci_u64 i = 0; i < bufferLength; ++i) {
        if (__sc_File_IsLineEnd(buffer[i])) {
            (*linePos)[line_index] = line_start;
            (*lineLength)[line_index] = i - line_start;
            line_index++;
            line_start = i + 1;
        }
    }
    // Handle the last line if it doesn't end with a newline or null terminator
    if (line_start < bufferLength) {
        (*linePos)[line_index] = line_start;
        (*lineLength)[line_index] = bufferLength - line_start;
    }
}

void __sc_File_GetInfo(const char* buffer, saci_u64 bufferLength, saci_u64** lineLenght,
                       saci_u64** linePos, saci_u64* lineAmount) {
    if (!buffer || bufferLength == 0 || !lineLenght || !linePos || !lineAmount) {
        *lineAmount = 0;
        return;
    }

    *lineAmount = __sc_File_CountLines(buffer, bufferLength);

    *linePos = (saci_u64*)malloc(*lineAmount * sizeof(saci_u64));
    *lineLenght = (saci_u64*)malloc(*lineAmount * sizeof(saci_u64));

    if (!*linePos || !*lineLenght) {
        free(*linePos);
        free(*lineLenght);
        *linePos = NULL;
        *lineLenght = NULL;
        *lineAmount = 0;
        return;
    }

    __sc_File_PopulateLineInfo(buffer, bufferLength, linePos, lineLenght);
}

int __sc_OBJ_tryParseDouble(const char* s, const char* s_end, double* result) {
    double mantissa = 0.0;
    /* This exponent is base 2 rather than 10.
     * However the exponent we parse is supposed to be one of ten,
     * thus we must take care to convert the exponent/and or the
     * mantissa to a * 2^E, where a is the mantissa and E is the
     * exponent.
     * To get the final double we will use ldexp, it requires the
     * exponent to be in base 2.
     */
    int exponent = 0;

    /* NOTE: THESE MUST BE DECLARED HERE SINCE WE ARE NOT ALLOWED
     * TO JUMP OVER DEFINITIONS.
     */
    char sign = '+';
    char exp_sign = '+';
    char const* curr = s;

    /* How many characters were read in a loop. */
    int read = 0;
    /* Tells whether a loop terminated due to reaching s_end. */
    int end_not_reached = 0;

    /*
       BEGIN PARSING.
       */

    if (s >= s_end) {
        return 0; /* fail */
    }

    /* Find out what sign we've got. */
    if (*curr == '+' || *curr == '-') {
        sign = *curr;
        curr++;
    } else if (__SACI_CHAR_IS_DIGIT(*curr)) { /* Pass through. */
    } else {
        goto fail;
    }

    /* Read the integer part. */
    end_not_reached = (curr != s_end);
    while (end_not_reached && __SACI_CHAR_IS_DIGIT(*curr)) {
        mantissa *= 10;
        mantissa += (int)(*curr - 0x30);
        curr++;
        read++;
        end_not_reached = (curr != s_end);
    }

    /* We must make sure we actually got something. */
    if (read == 0) goto fail;
    /* We allow numbers of form "#", "###" etc. */
    if (!end_not_reached) goto assemble;

    /* Read the decimal part. */
    if (*curr == '.') {
        curr++;
        read = 1;
        end_not_reached = (curr != s_end);
        while (end_not_reached && __SACI_CHAR_IS_DIGIT(*curr)) {
            /* pow(10.0, -read) */
            double frac_value = 1.0;
            int f;
            for (f = 0; f < read; f++) {
                frac_value *= 0.1;
            }
            mantissa += (int)(*curr - 0x30) * frac_value;
            read++;
            curr++;
            end_not_reached = (curr != s_end);
        }
    } else if (*curr == 'e' || *curr == 'E') {
    } else {
        goto assemble;
    }

    if (!end_not_reached) goto assemble;

    /* Read the exponent part. */
    if (*curr == 'e' || *curr == 'E') {
        curr++;
        /* Figure out if a sign is present and if it is. */
        end_not_reached = (curr != s_end);
        if (end_not_reached && (*curr == '+' || *curr == '-')) {
            exp_sign = *curr;
            curr++;
        } else if (__SACI_CHAR_IS_DIGIT(*curr)) { /* Pass through. */
        } else {
            /* Empty E is not allowed. */
            goto fail;
        }

        read = 0;
        end_not_reached = (curr != s_end);
        while (end_not_reached && __SACI_CHAR_IS_DIGIT(*curr)) {
            exponent *= 10;
            exponent += (int)(*curr - 0x30);
            curr++;
            read++;
            end_not_reached = (curr != s_end);
        }
        if (read == 0) goto fail;
    }

assemble :

{
    double a = 1.0; /* = pow(5.0, exponent); */
    double b = 1.0; /* = 2.0^exponent */
    int i;
    for (i = 0; i < exponent; i++) {
        a = a * 5.0;
    }

    for (i = 0; i < exponent; i++) {
        b = b * 2.0;
    }

    if (exp_sign == '-') {
        a = 1.0 / a;
        b = 1.0 / b;
    }

    *result =
        /* (sign == '+' ? 1 : -1) * ldexp(mantissa * pow(5.0, exponent),
           exponent); */
        (sign == '+' ? 1 : -1) * (mantissa * a * b);
}

    return 1;
fail:
    return 0;
}

float __sc_OBJ_ParseFloat(const char** token) {
    const char* end;
    double val = 0.0;
    float f = 0.0f;
    __sc_String_SkipSpace(token);
    end = (*token) + __sc_String_UntilSpace((*token));
    val = 0.0;
    __sc_OBJ_tryParseDouble((*token), end, &val);
    f = (float)(val);
    (*token) = end;
    return f;
}

void __sc_OBJ_ParseFloat2(float* x, float* y, const char** token) {
    (*x) = __sc_OBJ_ParseFloat(token);
    (*y) = __sc_OBJ_ParseFloat(token);
}

struct sc_VertexIndice __sc_OBJ_ParseRawTriple(const char** token) {
    struct sc_VertexIndice vi;
    /* 0x80000000 = -2147483648 = invalid */
    vi.vertexIndex = (int)(0x80000000);
    vi.normalIndex = (int)(0x80000000);
    vi.texCoordIndex = (int)(0x80000000);

    vi.vertexIndex = saci_Atoi((*token));
    while ((*token)[0] != '\0' && (*token)[0] != '/' && (*token)[0] != ' ' && (*token)[0] != '\t' &&
           (*token)[0] != '\r') {
        (*token)++;
    }
    if ((*token)[0] != '/') {
        return vi;
    }
    (*token)++;

    /* i//k */
    if ((*token)[0] == '/') {
        (*token)++;
        vi.normalIndex = saci_Atoi((*token));
        while ((*token)[0] != '\0' && (*token)[0] != '/' && (*token)[0] != ' ' &&
               (*token)[0] != '\t' && (*token)[0] != '\r') {
            (*token)++;
        }
        return vi;
    }

    /* i/j/k or i/j */
    vi.texCoordIndex = saci_Atoi((*token));
    while ((*token)[0] != '\0' && (*token)[0] != '/' && (*token)[0] != ' ' && (*token)[0] != '\t' &&
           (*token)[0] != '\r') {
        (*token)++;
    }
    if ((*token)[0] != '/') {
        return vi;
    }

    /* i/j/k */
    (*token)++; /* skip '/' */
    vi.normalIndex = saci_Atoi((*token));
    while ((*token)[0] != '\0' && (*token)[0] != '/' && (*token)[0] != ' ' && (*token)[0] != '\t' &&
           (*token)[0] != '\r') {
        (*token)++;
    }
    return vi;
}

void __sc_OBJ_ParseFloat3(float* x, float* y, float* z, const char** token) {
    (*x) = __sc_OBJ_ParseFloat(token);
    (*y) = __sc_OBJ_ParseFloat(token);
    (*z) = __sc_OBJ_ParseFloat(token);
}

saci_Bool __sc_OBJ_ParseLine(struct __sc_OBJ_Command* command, const char* line,
                             saci_u64 lineLenght) {
    char lineBuffer[4096];
    const char* token;
    if (lineLenght >= 4095) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_OBJ_LOADING,
                       "Invalid line when parsing");
        return 0;
    }
    memcpy(lineBuffer, line, lineLenght);
    lineBuffer[lineLenght] = '\0'; // end of the buffer
    token = lineBuffer;
    command->type = SC_OBJ_COMMANDTYPE_EMPTY;
    __sc_String_SkipSpace(&token);

    if (!token) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_OBJ_LOADING,
                       "Invalid token when parsing");
        return 0;
    }
    if (token[0] == '\0') { /* empty line */
        return 0;
    }

    if (token[0] == '#') { /* comment line */
        return 0;
    }

    // vertex
    if (token[0] == 'v' && __SACI_CHAR_IS_SPACE((token[1]))) {
        float x, y, z;
        token += 2;
        __sc_OBJ_ParseFloat3(&x, &y, &z, &token);
        command->vertexPos.x = x;
        command->vertexPos.y = y;
        command->vertexPos.z = z;
        command->type = SC_OBJ_COMMANDTYPE_V;
        return 1;
    }

    // normal
    if (token[0] == 'v' && token[1] == 'n' && __SACI_CHAR_IS_SPACE((token[2]))) {
        float x, y, z;
        token += 3;
        __sc_OBJ_ParseFloat3(&x, &y, &z, &token);
        command->vertexNormal.x = x;
        command->vertexNormal.y = y;
        command->vertexNormal.z = z;
        command->type = SC_OBJ_COMMANDTYPE_VN;
        return 1;
    }

    if (token[0] == 'v' && token[1] == 't' && __SACI_CHAR_IS_SPACE((token[2]))) {
        float x, y;
        token += 3;
        __sc_OBJ_ParseFloat2(&x, &y, &token);
        command->vertexTexcoord.x = x;
        command->vertexTexcoord.y = y;
        command->type = SC_OBJ_COMMANDTYPE_VT;
        return 1;
    }

    if (token[0] == 'f' && __SACI_CHAR_IS_SPACE((token[1]))) {
        size_t num_f = 0;

        struct sc_VertexIndice f[__SACI_MAX_FACES_PER_FACE_LINE];
        token += 2;
        __sc_String_SkipSpace(&token);

        while (!__SACI_CHAR_IS_NEWLINE(token[0])) {
            struct sc_VertexIndice vi = __sc_OBJ_ParseRawTriple(&token);
            __sc_String_SkipSpaceAndCR(&token);

            f[num_f] = vi;
            num_f++;
        }

        command->type = SC_OBJ_COMMANDTYPE_F;

#ifdef __SACI_TRIANGULATE // this whas a if(), but this library will only work with triangulated
                          // values... for now...
        size_t k;
        size_t n = 0;

        struct sc_VertexIndice i0 = f[0];
        struct sc_VertexIndice i1;
        struct sc_VertexIndice i2 = f[1];

        assert(3 * num_f < __SACI_MAX_FACES_PER_FACE_LINE);

        for (k = 2; k < num_f; k++) {
            i1 = i2;
            i2 = f[k];
            command->faces[3 * n + 0] = i0;
            command->faces[3 * n + 1] = i1;
            command->faces[3 * n + 2] = i2;

            command->verticeAmountInFace[n] = 3;
            n++;
        }
        command->facesAmount = n;
        command->verticeAmountInFaceSize = n;

#else // This should NOT run
        size_t k = 0;
        assert(num_f < TINYOBJ_MAX_FACES_PER_F_LINE);
        for (k = 0; k < num_f; k++) {
            command->f[k] = f[k];
        }

        command->num_f = num_f;
        command->f_num_verts[0] = (int)num_f;
        command->num_f_num_verts = 1;
#endif

        return 1;
    }

    return SACI_TRUE;
}

saci_Bool sc_OBJ_Parse(const char* buffer, saci_u64 lenght, saci_Vec3** verticesPos,
                       saci_u64* verticesAmount, saci_Vec2** verticesTexCoords,
                       saci_u64* verticesTexCoordsAmount, struct sc_VertexIndice** indices,
                       saci_u64* indicesAmount) {
    if (lenght < 1) return SACI_FALSE;
    if (!buffer) return SACI_FALSE;

    saci_u64* lineLenght = 0;
    saci_u64* linePos = 0;
    saci_u64 lineAmount;

    __sc_File_GetInfo(buffer, lenght, &lineLenght, &linePos, &lineAmount);

    struct __sc_OBJ_Command* objCommands =
        (struct __sc_OBJ_Command*)malloc(sizeof(struct __sc_OBJ_Command) * lineAmount);

    /* parsing each line */
    saci_u64 vertexCount = 0;
    saci_u64 normalCount = 0;
    saci_u64 texcoordCount = 0;
    saci_u64 facesCount = 0;
    {
        for (saci_u64 i = 0; i < lineAmount; ++i) {
            int result = __sc_OBJ_ParseLine(&objCommands[i], &buffer[linePos[i]], lineLenght[i]);
            if (result) {
                switch (objCommands[i].type) {
                    case SC_OBJ_COMMANDTYPE_V: {
                        ++vertexCount;
                        break;
                    }
                    case SC_OBJ_COMMANDTYPE_VN: {
                        ++normalCount;
                        break;
                    }
                    case SC_OBJ_COMMANDTYPE_VT: {
                        ++texcoordCount;
                        break;
                    }
                    case SC_OBJ_COMMANDTYPE_F: {
                        ++facesCount;
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
    }

    /* Allocate arrays based on counted command types */
    *verticesPos = (saci_Vec3*)malloc(sizeof(saci_Vec3) * vertexCount);
    *verticesTexCoords = (saci_Vec2*)malloc(sizeof(saci_Vec2) * texcoordCount);
    *indices = (struct sc_VertexIndice*)malloc(sizeof(struct sc_VertexIndice) * facesCount * 3);

    /* Populate arrays with parsed data */
    saci_u64 vIndex = 0, vnIndex = 0, vtIndex = 0, fIndex = 0;

    for (saci_u64 i = 0; i < lineAmount; ++i) {
        struct __sc_OBJ_Command* cmd = &objCommands[i];

        switch (cmd->type) {
            case SC_OBJ_COMMANDTYPE_V:
                (*verticesPos)[vIndex++] =
                    (saci_Vec3){cmd->vertexPos.x, cmd->vertexPos.y, cmd->vertexPos.z};
                break;
            case SC_OBJ_COMMANDTYPE_VT:
                (*verticesTexCoords)[vtIndex++] =
                    (saci_Vec2){cmd->vertexTexcoord.x, cmd->vertexTexcoord.y};
                break;
            case SC_OBJ_COMMANDTYPE_F:
                for (saci_u64 j = 0; j < cmd->facesAmount; ++j) {
                    (*indices)[fIndex++] = (struct sc_VertexIndice){cmd->faces[j].vertexIndex,
                                                                    cmd->faces[j].texCoordIndex,
                                                                    cmd->faces[j].normalIndex};
                }
                break;
            default:
                break;
        }
    }

    *verticesAmount = vertexCount;
    *verticesTexCoordsAmount = texcoordCount;
    *indicesAmount = facesCount * 3;

    if (lineLenght) {
        free(lineLenght);
    }
    if (linePos) {
        free(linePos);
    }

    return SACI_TRUE;
}

//----------------------------------------------------------------------------//
