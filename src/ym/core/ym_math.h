#pragma once

#define YM_PI 3.14159265358979323846f
#define YM_DEG_TO_RAD(deg) (deg * YM_PI / 180.0f)
#define YM_RAD_TO_DEG(rad) (180.0f * rad / YM_PI)

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
    float val[16];
} ym_mat4;

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
ym_mat4
ym_translate(ym_vec4 vec)
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
ym_scale(ym_vec4 vec)
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
