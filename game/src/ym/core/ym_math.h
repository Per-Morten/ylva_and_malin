#pragma once
#include <math.h>

#define YM_PI 3.14159265358979323846f
#define YM_DEG_TO_RAD(deg) (deg * YM_PI / 180.0f)
#define YM_RAD_TO_DEG(rad) (180.0f * rad / YM_PI)

typedef struct
{
    float x;
    float y;
} ym_vec2;

typedef struct
{
    union
    {
        float val[3];
        struct
        {
            float x;
            float y;
            float z;
        };
    };
} ym_vec3;

typedef struct
{
    union
    {
        float val[4];
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
    };
} ym_vec4;


typedef struct
{
    float val[9];
} ym_mat3;

typedef struct
{
    float val[16];
} ym_mat4;

//static const ym_mat4 ym_identity =
//{
//    .val =
//    {
//        1.0f, 0.0f, 0.0f, 0.0f,
//        0.0f, 1.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 1.0f, 0.0f,
//        0.0f, 0.0f, 0.0f, 1.0f,
//    },
//};

YM_INLINE
ym_mat4
ym_mul_mat4_mat4(ym_mat4 lhs, ym_mat4 rhs)
{
    ym_mat4 res;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            res.val[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++)
                res.val[i * 4 + j] += lhs.val[i * 4 + k] * rhs.val[k * 4 + j];
        }
    }
    return res;
}

YM_INLINE
ym_mat3
ym_mul_mat3_mat3(ym_mat3 lhs, ym_mat3 rhs)
{
    ym_mat3 res;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            res.val[i * 3 + j] = 0;
            for (int k = 0; k < 3; k++)
                res.val[i * 3 + j] += lhs.val[i * 3 + k] * rhs.val[k * 3 + j];
        }
    }
    return res;
}

YM_INLINE
ym_vec4
ym_mul_mat4_vec4(ym_mat4 lhs, ym_vec4 rhs)
{
    ym_vec4 res;
    for (int i = 0; i < 4; i++)
    {
        res.val[i] = 0;
        for (int j = 0; j < 4; j++)
            res.val[i] += lhs.val[i * 4 + j] * rhs.val[j];
    }

    return res;
}

YM_INLINE
ym_vec3
ym_mul_mat3_vec3(ym_mat3 lhs, ym_vec3 rhs)
{
    ym_vec3 res;
    for (int i = 0; i < 3; i++)
    {
        res.val[i] = 0;
        for (int j = 0; j < 3; j++)
            res.val[i] += lhs.val[i * 3 + j] * rhs.val[j];
    }

    return res;
}

YM_INLINE
ym_mat3
ym_translate_vec3(ym_vec3 vec)
{
    ym_mat3 mat =
    {
        .val =
        {
            1.0f, 0.0f,   0.0f,
            0.0f, 1.0f,   0.0f,
            vec.x, vec.y, 1.0f,
        },
    };

    return mat;
}

YM_INLINE
ym_mat3
ym_scale_vec3(ym_vec3 vec)
{
    ym_mat3 mat =
    {
        .val =
        {
            vec.x, 0.0f,  0.0f,
            0.0f,  vec.y, 0.0f,
            0.0f,  0.0f,  1.0f,
        },
    };

    return mat;
}


YM_INLINE
ym_mat4
ym_translate_vec4(ym_vec4 vec)
{
    ym_mat4 mat =
    {
        .val =
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            vec.x, vec.y, vec.z, 1.0f,
        },
    };

    return mat;
}

YM_INLINE
ym_mat4
ym_scale_vec4(ym_vec4 vec)
{
    ym_mat4 mat =
    {
        .val =
        {
            vec.x, 0.0f,  0.0f,  0.0f,
            0.0f,  vec.y, 0.0f,  0.0f,
            0.0f,  0.0f,  vec.z, 0.0f,
            0.0f,  0.0f,  0.0f,  1.0f,
        },
    };

    return mat;
}

// Taken from: http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf
YM_INLINE
ym_mat4
ym_lookat(ym_vec4 camera_pos, ym_vec4 target_pos, ym_vec4 up_direction)
{
    //  rx,  ry,  rz,  -px,
    //  ux,  uy,  uz,  -py,
    // -fx, -fy, -fz,  -pz,
    //   0,   0,   0,    1,

    //  rx,  ux,  -fx,    0,
    //  ry,  uy,  -fy,    0,
    //  rz,  uz,  -fz,    0,
    // -px, -py,  -pz,    1,

    ym_vec4 camera_neg =
    {
        .x = -camera_pos.x,
        .y = -camera_pos.y,
        .z = -camera_pos.z,
        .w = 1.0f,
    };

    ym_mat4 trans = ym_translate_vec4(camera_neg);

    ym_vec4 distance =
    {
        .x = target_pos.x - camera_pos.x,
        .y = target_pos.y - camera_pos.y,
        .z = target_pos.z - camera_pos.z,
        .w = 1.0f,
    };

    float magn = sqrtf(distance.x * distance.x +
                       distance.y * distance.y +
                       distance.z * distance.z);

    distance.x /= magn;
    distance.y /= magn;
    distance.z /= magn;

    // Cross product of forward and upward vector
    ym_vec4 r =
    {
        .x = distance.y * up_direction.z - distance.z * up_direction.y,
        .y = distance.z * up_direction.x - distance.x * up_direction.z,
        .z = distance.x * up_direction.y - distance.y * up_direction.x,
        .w = 1.0f,
    };

    ym_mat4 mat =
    {
        .val =
        {
            r.x, up_direction.x, -distance.x, 0.0f,
            r.y, up_direction.y, -distance.y, 0.0f,
            r.z, up_direction.z, -distance.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        },
    };

    return ym_mul_mat4_mat4(trans, mat);
}

// Taken from: http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf
YM_INLINE
ym_mat4
ym_project(float near, float far, float fov, float aspect)
{
    float range = tanf(fov * 0.5f) * near;
    float sx = (2.0f * near) / (range * aspect + range * aspect);
    float sy = near / range;
    float sz = -(far + near) / (far - near);
    float pz = -(2 * far * near) / (far - near);

    ym_mat4 mat =
    {
        .val =
        {
              sx,   0.0f,   0.0f, 0.0f,
            0.0f,     sy,   0.0f, 0.0f,
            0.0f,   0.0f,     sz, -1.0f,
            0.0f,   0.0f,     pz, 0.0f,
        },
    };

    return mat;
}

YM_INLINE
void
ym_print_mat3(ym_mat3 mat)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            printf("%f ", mat.val[i * 3 + j]);
        printf("\n");
    }
}
