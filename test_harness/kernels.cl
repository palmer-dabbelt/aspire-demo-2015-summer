__kernel void fn_1(global const float* im0, global float* Ix, global float* Iy,
                   global float* It, global const float* im1,
                   global float* denom) {
    int index = get_global_id(0);
    if (index < 256) {
        float _hm_generated_2;
        float _hm_generated_0;
        float _hm_generated_1;

        It[index] = im1[index] - im0[index];

        {
            int x = index % 16;
            int y = index / 16;
            float accum = 0.0;
            accum += -0.0833333333333f * im1[max(y + -2, 0) * 16 + x];
            accum += -0.666666666667f * im1[max(y + -1, 0) * 16 + x];
            accum += 0.666666666667f * im1[min(y + 1, 16 - 1) * 16 + x];
            accum += 0.0833333333333f * im1[min(y + 2, 16 - 1) * 16 + x];
            Iy[index] = accum;
        }

        {
            int x = index % 16;
            int y = index / 16;
            float accum = 0.0;
            accum += -0.0833333333333f * im1[y * 16 + max(x + -2, 0)];
            accum += -0.666666666667f * im1[y * 16 + max(x + -1, 0)];
            accum += 0.666666666667f * im1[y * 16 + min(x + 1, 16 - 1)];
            accum += 0.0833333333333f * im1[y * 16 + min(x + 2, 16 - 1)];
            Ix[index] = accum;
        }
        _hm_generated_1 = pow(Iy[index], 2);
        _hm_generated_2 = pow(Ix[index], 2);
        _hm_generated_0 = _hm_generated_2 + _hm_generated_1;
        denom[index] = _hm_generated_0 + 900.0;
    }
}

__kernel void fn_2(global const float* Ix, global float* v,
                   global const float* It, global const float* Iy,
                   global const float* denom, global float* u,
                   global float* u_new, global float* v_new) {
    int index = get_global_id(0);
    if (index < 256) {
        float _hm_generated_6;
        float _hm_generated_7;
        float _hm_generated_5;
        float _hm_generated_4;
        float _hm_generated_8;
        float _hm_generated_3;
        float ubar, vbar, t;


        {
            int x = index % 16;
            int y = index / 16;
            float accum = 0.0;

            accum += 0.0833333333333f * u[max(y + -1, 0) * 16 + max(x + -1, 0)];
            accum += 0.166666666667f * u[max(y + -1, 0) * 16 + x];
            accum += 0.0833333333333f * u[max(y + -1, 0) * 16 + min(x + 1, 16 - 1)];
            accum += 0.166666666667f * u[y * 16 + max(x + -1, 0)];
            accum += 0.166666666667f * u[y * 16 + min(x + 1, 16 - 1)];
            accum += 0.0833333333333f * u[min(y + 1, 16 - 1) * 16 + max(x + -1, 0)];
            accum += 0.166666666667f * u[min(y + 1, 16 - 1) * 16 + x];
            accum += 0.0833333333333f * u[min(y + 1, 16 - 1) * 16 + min(x + 1, 16 - 1)];

            ubar = accum;
        }

        {
            int x = index % 16;
            int y = index / 16;
            float accum = 0.0;

            accum += 0.0833333333333f * v[max(y + -1, 0) * 16 + max(x + -1, 0)];
            accum += 0.166666666667f * v[max(y + -1, 0) * 16 + x];
            accum += 0.0833333333333f * v[max(y + -1, 0) * 16 + min(x + 1, 16 - 1)];
            accum += 0.166666666667f * v[y * 16 + max(x + -1, 0)];
            accum += 0.166666666667f * v[y * 16 + min(x + 1, 16 - 1)];
            accum += 0.0833333333333f * v[min(y + 1, 16 - 1) * 16 + max(x + -1, 0)];
            accum += 0.166666666667f * v[min(y + 1, 16 - 1) * 16 + x];
            accum += 0.0833333333333f * v[min(y + 1, 16 - 1) * 16 + min(x + 1, 16 - 1)];

            vbar = accum;
        }
        _hm_generated_5 = Iy[index] * vbar;
        _hm_generated_6 = Ix[index] * ubar;
        _hm_generated_4 = _hm_generated_6 + _hm_generated_5;
        _hm_generated_3 = _hm_generated_4 + It[index];
        t = _hm_generated_3 / denom[index];
        _hm_generated_7 = Ix[index] * t;
        u_new[index] = ubar - _hm_generated_7;
        _hm_generated_8 = Iy[index] * t;
        v_new[index] = vbar - _hm_generated_8;

    }
}