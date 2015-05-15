#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <stdio.h>
#include <math.h>
#include <float.h>

#define max(x, y) (x) > (y) ? (x) : (y)
#define min(x, y) (x) < (y) ? (x) : (y)

int main() {
  float input[256][256];
  float filters[3][3] = {
    {1.0f/12.0f, 1.0f/6.0f, 1.0f/12.0f},
    {1.0f/6.0f,       0.0f, 1.0f/6.0f},
    {1.0f/12.0f, 1.0f/6.0f, 1.0f/12.0f},
  };
  float output[256][256];

  int err;
  cl_device_id device_id;
  cl_context context;
  cl_command_queue commands;
  cl_program program;
  cl_kernel kernel;

  cl_mem input_buf;
  cl_mem output_buf;

  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 256; j++) {
      input[i][j] = rand() / (float) RAND_MAX;
    }
  }

  int gpu = 1;  // set to 0 to use CPU
  err = clGetDeviceIDs(NULL, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
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
  kernel = clCreateKernel(program, "jacobi", &err);
  if (!kernel || err != CL_SUCCESS)
  {
      printf("Error: Failed to create compute kernel!\n");
      exit(1);
  }

  // Create the input and output arrays in device memory for our calculation
  //
  input_buf = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * 256 * 256, NULL, NULL);
  output_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * 256 * 256, NULL, NULL);
  if (!input || !output_buf)
  {
      printf("Error: Failed to allocate device memory!\n");
      exit(1);
  }

  // Write our data set into the input array in device memory 
  //
  err = clEnqueueWriteBuffer(commands, input_buf, CL_TRUE, 0, sizeof(float) * 256 * 256, input, 0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to writing to array!\n");
      exit(1);
  }
 
  // Set the arguments to our compute kernel
  //
  err = 0;
  err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buf);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buf);
  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to set kernel arguments! %d\n", err);
      exit(1);
  }

  size_t local[1] = {64};
  size_t global[1] = {256 * 256};

  // Get the maximum work group size for executing the kernel on the device
  //
  err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to retrieve kernel work group info! %d\n", err);
      exit(1);
  }

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  //
  err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, global, NULL, 0, NULL, NULL);
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
  err = clEnqueueReadBuffer( commands, output_buf, CL_TRUE, 0, sizeof(float) * 256 * 256, output, 0, NULL, NULL );  
  if (err != CL_SUCCESS)
  {
      printf("Error: Failed to read output array! %d\n", err);
      exit(1);
  }

  for (int y = 0; y < 256; y++) {
    for (int x = 0; x < 256; x++) {
      float expected = 0.0f;
      for (int p = 0; p < 3; p++) {
        for (int q = 0; q < 3; q++) {
          expected += filters[p][q] * input[max(min(y + p - 1, 255), 0)][max(min(x + q - 1, 255), 0)];
        }
      }
      if (fabs(output[y][x] - expected) > .00001) {
        printf("Error: Results not equal at index (%d, %d)\n \
                          Actual: %f\n \
                          Expected: %f\n",
               x, y,
               output[y][x], expected);
        exit(1);
      }
    }
  }
  // Shutdown and cleanup
  //
  clReleaseMemObject(input_buf);
  clReleaseMemObject(output_buf);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);
  printf("SUCCESS\n");

  return 0;
}
