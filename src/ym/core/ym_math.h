#pragma once

typedef struct
{
    float val[4];
} ym_vec4;

typedef struct
{
    float val[16];
} ym_mat4;

inline
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

inline
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
