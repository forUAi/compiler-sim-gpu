#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>

namespace compiler_sim {

struct KernelConfig {
    std::string name;
    dim3 gridDim;
    dim3 blockDim;
    size_t sharedMemBytes;
};

struct MemoryAllocation {
    void* ptr;
    size_t size;
    std::string name;
};

class MockGPURuntime {
public:
    MockGPURuntime() : totalMemoryAllocated_(0), peakMemoryUsage_(0) {
        std::cout << "MockGPU: Initialized with 8GB memory\n";
    }
    
    void* allocate(size_t size, const std::string& name) {
        // Mock allocation
        void* ptr = reinterpret_cast<void*>(nextAddress_);
        nextAddress_ += size;
        
        allocations_.push_back({ptr, size, name});
        totalMemoryAllocated_ += size;
        currentMemoryUsage_ += size;
        
        if (currentMemoryUsage_ > peakMemoryUsage_) {
            peakMemoryUsage_ = currentMemoryUsage_;
        }
        
        std::cout << "MockGPU: Allocated " << formatBytes(size) 
                  << " for " << name 
                  << " at 0x" << std::hex << reinterpret_cast<uintptr_t>(ptr) 
                  << std::dec << "\n";
        
        return ptr;
    }
    
    void free(void* ptr) {
        for (auto it = allocations_.begin(); it != allocations_.end(); ++it) {
            if (it->ptr == ptr) {
                currentMemoryUsage_ -= it->size;
                std::cout << "MockGPU: Freed " << formatBytes(it->size) 
                          << " from " << it->name << "\n";
                allocations_.erase(it);
                return;
            }
        }
    }
    
    void launchKernel(const KernelConfig& config) {
        std::cout << "\nMockGPU: Launching kernel '" << config.name << "'\n";
        std::cout << "  Grid: (" << config.gridDim.x << ", " 
                  << config.gridDim.y << ", " << config.gridDim.z << ")\n";
        std::cout << "  Block: (" << config.blockDim.x << ", " 
                  << config.blockDim.y << ", " << config.blockDim.z << ")\n";
        std::cout << "  Shared Memory: " << config.sharedMemBytes << " bytes\n";
        
        // Simulate execution time
        auto start = std::chrono::high_resolution_clock::now();
        
        // Mock computation (sleep for realistic timing)
        std::this_thread::sleep_for(std::chrono::microseconds(
            100 + rand() % 900  // 0.1-1ms
        ));
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
        );
        
        std::cout << "  Execution time: " << duration.count() / 1000.0 << "ms\n";
        
        // Simulate performance metrics
        double tflops = (rand() % 50 + 50) / 10.0;  // 5-10 TFLOPS
        double bandwidth = (rand() % 400 + 100);     // 100-500 GB/s
        
        std::cout << "  Performance: " << tflops << " TFLOPS\n";
        std::cout << "  Memory bandwidth: " << bandwidth << " GB/s\n";
    }
    
    void synchronize() {
        std::cout << "MockGPU: Device synchronized\n";
    }
    
    void printStats() {
        std::cout << "\n=== GPU Runtime Statistics ===\n";
        std::cout << "Total memory allocated: " << formatBytes(totalMemoryAllocated_) << "\n";
        std::cout << "Peak memory usage: " << formatBytes(peakMemoryUsage_) << "\n";
        std::cout << "Current memory usage: " << formatBytes(currentMemoryUsage_) << "\n";
        std::cout << "Active allocations: " << allocations_.size() << "\n";
    }
    
private:
    std::vector<MemoryAllocation> allocations_;
    size_t totalMemoryAllocated_;
    size_t peakMemoryUsage_;
    size_t currentMemoryUsage_ = 0;
    uintptr_t nextAddress_ = 0x100000000;  // Mock GPU address
    
    std::string formatBytes(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB"};
        int unit = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024 && unit < 3) {
            size /= 1024;
            unit++;
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size << " " << units[unit];
        return ss.str();
    }
};

// Simulation helper for matmul kernel
void simulateMatmulKernel(MockGPURuntime& gpu, 
                         int M, int N, int K,
                         void* A, void* B, void* C) {
    // Calculate grid and block dimensions
    const int TILE_SIZE = 32;
    dim3 grid((N + TILE_SIZE - 1) / TILE_SIZE, 
              (M + TILE_SIZE - 1) / TILE_SIZE);
    dim3 block(TILE_SIZE, TILE_SIZE);
    
    KernelConfig config{
        "matmul_kernel",
        grid,
        block,
        2 * TILE_SIZE * TILE_SIZE * sizeof(float)  // Shared memory for tiles
    };
    
    gpu.launchKernel(config);
}

} // namespace compiler_sim