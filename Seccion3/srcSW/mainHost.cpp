#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <unistd.h>

#include "specs.h"
#include "mmultSW.h"
#include "tictoc.h"
//using namespace std;

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#include <CL/cl2.hpp>

#define DISPLAY

extern "C"{
void mmultHW(T A_in[M_MMULT][N_MMULT], T B_in[N_MMULT][P_MMULT], T C_out[M_MMULT][P_MMULT]);
}

void genRandArray(T min, T max, int size, T *array);
int compare(T* gold, T* result, int size, double th);


int main(int argc, char* argv[]) {

	std::cout << std::setprecision(15);
	//int iterPDIP = 20;
	//int iterLS = 30;
	int errors = 0;
	T min = 0.5;
	T max = 300;

	//double expectedResult[P_SYS];

    //TARGET_DEVICE macro needs to be passed from gcc command line
    if(argc != 2) {
		std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
		return EXIT_FAILURE;
	}

    char* xclbinFilename = argv[1];

    // Compute the size of array in bytes
    //size_t size_in_bytes = DATA_SIZE * sizeof(int);

    // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
    // using customized allocator for getting buffer alignment to 4k boundary

    std::vector<cl::Device> devices;
    cl::Device device;
    std::vector<cl::Platform> platforms;
    bool found_device = false;

    //traversing all Platforms To find Xilinx Platform and targeted
    //Device in Xilinx Platform
    cl::Platform::get(&platforms);
    for(size_t i = 0; (i < platforms.size() ) & (found_device == false) ;i++){
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
        if ( platformName == "Xilinx"){
            devices.clear();
            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
	    if (devices.size()){
		    device = devices[0];
		    found_device = true;
		    break;
	    }
        }
    }
    if (found_device == false){
       std::cout << "Error: Unable to find Target Device "
           << device.getInfo<CL_DEVICE_NAME>() << std::endl;
       return EXIT_FAILURE;
    }

    // Creating Context and Command Queue for selected device
    cl::Context context(device);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

    // Load xclbin
    std::cout << "Loading: '" << xclbinFilename << "'\n";
    std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
    bin_file.seekg (0, bin_file.end);
    unsigned nb = bin_file.tellg();
    bin_file.seekg (0, bin_file.beg);
    char *buf = new char [nb];
    bin_file.read(buf, nb);
    std::cout << "Done loading xclbin file\n";

    // Creating Program from Binary File
    std::cout << "Creating Program from Binary File\n";
    cl::Program::Binaries bins;
    bins.push_back({buf,nb});
    devices.resize(1);
    cl::Program program(context, devices, bins);
    std::cout << "DONE Creating Program from Binary File\n";

    // This call will get the kernel object from program. A kernel is an
    // OpenCL function that is executed on the FPGA.
    cl::Kernel krnl_mmult(program,"mmultHW");


    // These commands will allocate memory on the Device. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    std::cout << "Creating Buffers\n";
    cl::Buffer buffer_A(context, CL_MEM_READ_ONLY, M_MMULT*N_MMULT * sizeof(T));
    cl::Buffer buffer_B(context, CL_MEM_READ_ONLY, N_MMULT*P_MMULT * sizeof(T));

    cl::Buffer buffer_C_HW(context, CL_MEM_WRITE_ONLY, M_MMULT*P_MMULT * sizeof(T));

    //set the kernel Arguments
    std::cout << "Setting kernel arguments\n";
    int narg=0;
    krnl_mmult.setArg(narg++,buffer_A);
    krnl_mmult.setArg(narg++,buffer_B);

    krnl_mmult.setArg(narg++,buffer_C_HW);

    std::cout << "Mapping Buffers\n";
    //We then need to map our OpenCL buffers to get the pointers
    std::cout << "Buffer: A\n";
	T *A= (T *) q.enqueueMapBuffer (buffer_A , CL_TRUE , CL_MAP_WRITE , 0, M_MMULT*N_MMULT * sizeof(T));

	std::cout << "Buffer: B\n";
	T *B = (T *) q.enqueueMapBuffer (buffer_B , CL_TRUE , CL_MAP_WRITE , 0, N_MMULT*P_MMULT * sizeof(T));

	std::cout << "Buffer: C\n";
	T *C_HW = (T *) q.enqueueMapBuffer (buffer_C_HW , CL_TRUE , CL_TRUE , 0,  M_MMULT*P_MMULT * sizeof(T));

	std::cout << "Buffers mapped\n";

	genRandArray(min, max, M_MMULT*N_MMULT, A);
	genRandArray(min, max, N_MMULT*P_MMULT, B);
    q.enqueueMigrateMemObjects({buffer_A,buffer_B},0/* 0 means from host*/);
    //Launch the Kernel
    q.enqueueTask(krnl_mmult);

    q.enqueueMigrateMemObjects({buffer_C_HW},CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();

    // threshold for the difference between the calculated results and the correct ones.
    double th = 1e-4;

    // number of samples to test
    int nSamples = 1000;

    T C_SW[M_MMULT*P_MMULT];

    // tic toc timer
    tictoc HWTimer(nSamples);
    tictoc SWTimer(nSamples);

    std::cout << "Starting tests!\n";
    for (int sample=0; sample<nSamples; sample++){
    	genRandArray(min, max, M_MMULT*N_MMULT, A);
    	genRandArray(min, max, N_MMULT*P_MMULT, B);

        HWTimer.tic();
        q.enqueueMigrateMemObjects({buffer_A,buffer_B},0/* 0 means from host*/);
        //Launch the Kernel
        q.enqueueTask(krnl_mmult);

        q.enqueueMigrateMemObjects({buffer_C_HW},CL_MIGRATE_MEM_OBJECT_HOST);
        q.finish();
        HWTimer.toc();

        SWTimer.tic();
        mmultSW (A, B, C_SW);
        SWTimer.toc();

        errors += compare(C_SW, C_HW, M_MMULT*P_MMULT, th);
        //std::cout << "A[0]: " << A[0] << "   B[0]: " << B[0] << std::endl;

    }


    // print important data
    std::cout << "Finished processing " << nSamples << " samples" << std::endl;
    std::cout << "Number of differences between expected and calculated " << errors << std::endl;
    std::cout << "Threshold: " << th << std::endl;
    std::cout << std::endl;

    HWTimer.rmWarmup(100);
    SWTimer.rmWarmup(100);

    std::cout << "Mean time running application in hardware: \t\t" << HWTimer.mean() / 1e3 << "\xC2\xB5s" << std::endl;
    std::cout << "Standard deviation running application in hardware: \t" << HWTimer.stdev() / 1e3 << "\xC2\xB5s" << std::endl;
    std::cout << "Max running application in hardware: \t\t\t" << HWTimer.max() / 1e3 << "\xC2\xB5s" << std::endl;
    std::cout << "Min running application in hardware: \t\t\t" << HWTimer.min() / 1e3 << "\xC2\xB5s" << std::endl;

    std::cout << "Mean time running application in software: \t\t" << SWTimer.mean() / 1e3 << "\xC2\xB5s" << std::endl;
    std::cout << "Standard deviation running application in software: \t" << SWTimer.stdev() / 1e3 << "\xC2\xB5s" << std::endl;
    std::cout << "Max running application in software: \t\t\t" << SWTimer.max() / 1e3 << "\xC2\xB5s" << std::endl;
    std::cout << "Min running application in software: \t\t\t" << SWTimer.min() / 1e3 << "\xC2\xB5s" << std::endl;

    q.enqueueUnmapMemObject(buffer_A , A);
    q.enqueueUnmapMemObject(buffer_B , B);
    q.enqueueUnmapMemObject(buffer_C_HW , C_HW);
    q.finish();


    return errors;

}

void genRandArray(T min, T max, int size, T *array){
    for(int i=0; i<size; i++){
        array[i] = min + static_cast <T> (rand()) / ( static_cast <T> (RAND_MAX/(max-min)));
    }
}

int compare(T* gold, T* result, int size, double th){
        int errors = 0;
        double dif = 0;
        for (int i=0; i<size; i++){
                dif = fabs((double)gold[i] - (double)result[i]);
                // a comparison with NaN will always be false
                if (!(dif < th)){
                        errors++;
                        std::cout << "sw: " << gold[i] << "   hw: " << result[i] << "   dif: "<< dif << std::endl;
                }
        }
        return errors;
}

