__kernel void jacobi(global const float* input, global float* output) {
  int index = get_global_id(0);
  if (index < 65536) {
    int y = index / 256;
    int x = index - y * 256;
    float accum = 0.0f;
    accum += 0.083333f * input[max(min(y + -1, 255), 0) * 256 + max(min(x + -1, 255), 0)];
    accum += 0.166667f * input[max(min(y + -1, 255), 0) * 256 + max(min(x + 0, 255), 0)];
    accum += 0.083333f * input[max(min(y + -1, 255), 0) * 256 + max(min(x + 1, 255), 0)];

    accum += 0.166667f * input[max(min(y + 0, 255), 0) * 256 + max(min(x + -1, 255), 0)];
    accum += 0.166667f * input[max(min(y + 0, 255), 0) * 256 + max(min(x + 1, 255), 0)];

    accum += 0.083333f * input[max(min(y + 1, 255), 0) * 256 + max(min(x + -1, 255), 0)];
    accum += 0.166667f * input[max(min(y + 1, 255), 0) * 256 + max(min(x + 0, 255), 0)];
    accum += 0.083333f * input[max(min(y + 1, 255), 0) * 256 + max(min(x + 1, 255), 0)];
    output[index] = accum;
  }
}
