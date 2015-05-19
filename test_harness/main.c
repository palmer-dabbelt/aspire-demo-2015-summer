#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <stdio.h>
#include <math.h>
#include <float.h>
#define max(a,b) ((a) > (b) ? a : b)
#define min(a,b) ((a) < (b) ? a : b)

void fn_1(float* Ix, float* denom, float* Iy, float* im1, float* It, float* im0) {
    for (int index = 0; index < 256; index++) {
        float _hm_generated_0;
        float _hm_generated_1;
        float _hm_generated_2;

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
    _hm_generated_1 = powf(Iy[index], 2);
    _hm_generated_2 = powf(Ix[index], 2);
    _hm_generated_0 = _hm_generated_2 + _hm_generated_1;
    denom[index] = _hm_generated_0 + 900.0;

    }
}

void fn_2(float* v, float* u, float* Ix, float* Iy, float* It, float* denom, float* u_new, float* v_new) {
    for (int index = 0; index < 256; index++) {
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

void check_err(char* name, float* actual, float* expected) {
  for (int i = 0; i < 256; i++) {
    if (fabs(actual[i] - expected[i]) > .00001) {
      printf("Error: Results not equal at %s[%d]\n\tActual: %f\n\tExpected: %f\n",
             name, i, actual[i], expected[i]);
      exit(1);
    }
  }
  printf("Array %s correct\n", name);
}

int main() {
  float im0[256];
  float im1[256];
  float denom[256];
  float Ix[256];
  float Iy[256];
  float It[256];
  float u_new[256];
  float v_new[256];
  float u[256];
  float v[256];

  float denom_expected[256];
  float Ix_expected[256];
  float Iy_expected[256];
  float It_expected[256];
  float u_new_expected[256];
  float v_new_expected[256];

  int err;
  cl_device_id device_id;
  cl_context context;
  cl_command_queue commands;
  cl_program program;

  for (int i = 0; i < 256; i++) {
      im0[i] = rand() / (float) RAND_MAX;
      im1[i] = rand() / (float) RAND_MAX;
      u[i] = rand() / (float) RAND_MAX;
      v[i] = rand() / (float) RAND_MAX;
  }

  cl_platform_id platforms[1];
  err = clGetPlatformIDs(1, platforms, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to get platform!\n");
    return EXIT_FAILURE;
  }

  int gpu = 1;  // set to 0 to use CPU
  err = clGetDeviceIDs(platforms[0], gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to create a device group!\n");
    return EXIT_FAILURE;
  }

  // Create a compute context
  //
  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
  if (!context) {
    printf("Error: Failed to create a compute context!\n");
    return EXIT_FAILURE;
  }
  // Create a command commands
  //
  commands = clCreateCommandQueue(context, device_id, 0, &err);
  if (!commands) {
    printf("Error: Failed to create a command commands!\n");
    return EXIT_FAILURE;
  }


  // Read the kernel into a string
  //
  FILE *kernelFile = fopen("./kernels.cl", "rb");
  if (kernelFile == NULL) {
    printf("Error: Coudn't open kernel file.\n");
    return err;
  }

  fseek(kernelFile, 0 , SEEK_END);
  long kernelFileSize = ftell(kernelFile);
  rewind(kernelFile);

  // Allocate memory to hold kernel
  //
  char *KernelSource = (char *)calloc(sizeof(char), kernelFileSize);
  if (KernelSource == NULL) {
    printf("Error: failed to allocate memory to hold kernel text.\n");
    return err;
  }

  // Read the kernel into memory
  //
  int result = fread(KernelSource, sizeof(char), kernelFileSize, kernelFile);
  if (result != kernelFileSize) {
    printf("Error: read fewer bytes of kernel text than expected.\n");
    return err;
  }
  fclose(kernelFile);

  // Create the compute program from the source buffer
  //
  program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
  if (!program)
  {
      printf("Error: Failed to create compute program!\n");
      return EXIT_FAILURE;
  }

  // Build the program executable
  //
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS)
  {
      size_t len;
      char buffer[2048];

      printf("Error: Failed to build program executable!\n");
      clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
      printf("%s\n", buffer);
      exit(1);
  }

  // Create the compute kernel in the program we wish to run
  //
  cl_kernel kernel_1 = clCreateKernel(program, "fn_1", &err);
  cl_kernel kernel_2 = clCreateKernel(program, "fn_2", &err);
  if (!kernel_1 || !kernel_2 || err != CL_SUCCESS)
  {
      printf("Error: Failed to create compute kernel!\n");
      exit(1);
  }

  // Create the input and output arrays in device memory for our calculation
  //
  cl_mem im0_buf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * 256, NULL, NULL);
  cl_mem im1_buf = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * 256, NULL, NULL);
  cl_mem Ix_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem Iy_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem It_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem denom_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem ubar_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem vbar_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem u_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem v_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem t_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem u_new_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  cl_mem v_new_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * 256, NULL, NULL);
  if (!im0_buf || !im1_buf || !Ix_buf || !Iy_buf || !It_buf || !denom_buf ||
      !ubar_buf || !vbar_buf || !u_buf || !v_buf || !t_buf)
  {
      printf("Error: Failed to allocate device memory!\n");
      exit(1);
  }

  // Write our data set into the input array in device memory 
  //
  err = clEnqueueWriteBuffer(commands, im0_buf, CL_TRUE, 0, sizeof(float) * 256, im0, 0, NULL, NULL);
  err = clEnqueueWriteBuffer(commands, im1_buf, CL_TRUE, 0, sizeof(float) * 256, im1, 0, NULL, NULL);
  err = clEnqueueWriteBuffer(commands, u_buf, CL_TRUE, 0, sizeof(float) * 256, u, 0, NULL, NULL);
  err = clEnqueueWriteBuffer(commands, v_buf, CL_TRUE, 0, sizeof(float) * 256, v, 0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to writing to array!\n");
      exit(1);
  }
 
  // Set the arguments to our compute kernel
  //
  err = 0;
  // kernel 1
  err |= clSetKernelArg(kernel_1, 0, sizeof(cl_mem), &im0_buf);
  err |= clSetKernelArg(kernel_1, 1, sizeof(cl_mem), &Ix_buf);
  err |= clSetKernelArg(kernel_1, 2, sizeof(cl_mem), &Iy_buf);
  err |= clSetKernelArg(kernel_1, 3, sizeof(cl_mem), &It_buf);
  err |= clSetKernelArg(kernel_1, 4, sizeof(cl_mem), &im1_buf);
  err |= clSetKernelArg(kernel_1, 5, sizeof(cl_mem), &denom_buf);

  // kernel 2
  err |= clSetKernelArg(kernel_2, 0, sizeof(cl_mem), &Ix_buf);
  err |= clSetKernelArg(kernel_2, 1, sizeof(cl_mem), &v_buf);
  err |= clSetKernelArg(kernel_2, 2, sizeof(cl_mem), &It_buf);
  err |= clSetKernelArg(kernel_2, 3, sizeof(cl_mem), &Iy_buf);
  err |= clSetKernelArg(kernel_2, 4, sizeof(cl_mem), &denom_buf);
  err |= clSetKernelArg(kernel_2, 5, sizeof(cl_mem), &u_buf);
  err |= clSetKernelArg(kernel_2, 6, sizeof(cl_mem), &u_new_buf);
  err |= clSetKernelArg(kernel_2, 7, sizeof(cl_mem), &v_new_buf);
  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to set kernel arguments! %d\n", err);
      exit(1);
  }

  size_t local[1] = {64};
  size_t global[1] = {256};

  // Get the maximum work group size for executing the kernel on the device
  //
  err = clGetKernelWorkGroupInfo(kernel_1, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to retrieve kernel work group info! %d\n", err);
      exit(1);
  }
  err = clGetKernelWorkGroupInfo(kernel_2, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
  if (err != CL_SUCCESS)
    {
      printf("Error: Failed to retrieve kernel work group info! %d\n", err);
      exit(1);
    }

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  //
  err = clEnqueueNDRangeKernel(commands, kernel_1, 1, NULL, global, NULL, 0, NULL, NULL);
  err |= clEnqueueNDRangeKernel(commands, kernel_2, 1, NULL, global, NULL, 0, NULL, NULL);
  if (err)
  {
      printf("Error: Failed to execute kernel!\n");
      return EXIT_FAILURE;
  }

  // Wait for the command commands to get serviced before reading back results
  //
  clFinish(commands);

  // Read back the results from the device to verify the output
  //
  err = clEnqueueReadBuffer( commands, Ix_buf, CL_TRUE, 0, sizeof(float) * 256, Ix, 0, NULL, NULL );
  err |= clEnqueueReadBuffer( commands, Iy_buf, CL_TRUE, 0, sizeof(float) * 256, Iy, 0, NULL, NULL );
  err |= clEnqueueReadBuffer( commands, It_buf, CL_TRUE, 0, sizeof(float) * 256, It, 0, NULL, NULL );
  err |= clEnqueueReadBuffer( commands, denom_buf, CL_TRUE, 0, sizeof(float) * 256, denom, 0, NULL, NULL );
  err |= clEnqueueReadBuffer( commands, u_new_buf, CL_TRUE, 0, sizeof(float) * 256, u_new, 0, NULL, NULL );
  err |= clEnqueueReadBuffer( commands, v_new_buf, CL_TRUE, 0, sizeof(float) * 256, v_new, 0, NULL, NULL );
  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to read output arrays! %d\n", err);
      exit(1);
  }

  printf("===== Checking fn_1 =====\n");
  fn_1(Ix_expected, denom_expected, Iy_expected, im1, It_expected, im0);
  check_err("Ix", Ix, Ix_expected);
  check_err("Iy", Iy, Iy_expected);
  check_err("It", It, It_expected);
  check_err("denom", denom, denom_expected);
  printf("===== fn_1 correct  =====\n");

  printf("===== Checking fn_2 =====\n");
  fn_2(v, u, Ix_expected, Iy_expected, It_expected, denom_expected,
       u_new_expected, v_new_expected);
  check_err("u_new", u_new, u_new_expected);
  check_err("v_new", v_new, v_new_expected);
  printf("===== fn_2 correct  =====\n");


  // Shutdown and cleanup
  //
  clReleaseMemObject(Ix_buf);
  clReleaseMemObject(Iy_buf);
  clReleaseMemObject(It_buf);
  clReleaseMemObject(im0_buf);
  clReleaseMemObject(im1_buf);
  clReleaseMemObject(denom_buf);
  clReleaseProgram(program);
  clReleaseKernel(kernel_1);
  clReleaseKernel(kernel_2);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);
  printf("SUCCESS\n");

  return 0;
}
