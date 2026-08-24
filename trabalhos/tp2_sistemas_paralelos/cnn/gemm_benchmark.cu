#include <iostream>     // Para imprimir no console
#include <vector>       // Para facilitar a alocação de memória na CPU (Host)
#include <chrono>       // Para cronometrar o tempo
#include <cstdlib>      // Para números aleatórios (rand)
#include <cmath>        // Para checar a precisão (fabs)

// --- Bibliotecas Específicas ---
#include <omp.h>            // 1. Para o OpenMP (CPU Paralela)
#include <cuda_runtime.h>   // 2. Para runtime do CUDA (malloc, memcpy, etc.)
#include <cublas_v2.h>      // 3. Para a biblioteca NVIDIA (cuBLAS)

// --- Nossas Configurações ---

// Tamanho da Matriz (N x N). 4096 é um bom teste de estresse.
#define N 8192

// Tamanho do Bloco (Tile) para o Kernel Otimizado. 
// 32x32 = 1024 threads por bloco. É um número padrão e eficiente.
#define TILE_DIM 32

// --- Função Auxiliar de Verificação de Erros CUDA ---
// (Uma boa prática para debugar seu código)
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

// --- Função Auxiliar para Inicializar Matrizes ---
void init_matrices(std::vector<float>& A, std::vector<float>& B) {
    for (int i = 0; i < N * N; ++i) {
        A[i] = static_cast<float>(rand()) / RAND_MAX;
        B[i] = static_cast<float>(rand()) / RAND_MAX;
    }
}

// --- Função Auxiliar para Verificar o Resultado ---
// Compara o resultado de uma implementação com a da CPU Serial (referência)
void verify_result(const std::vector<float>& ref, const std::vector<float>& test) {
    float max_error = 0.0f;
    float tolerance = 1e-2; // Tolerância para erros de ponto flutuante

    for (int i = 0; i < N * N; ++i) {
        max_error = fmax(max_error, fabs(ref[i] - test[i]));
    }

    if (max_error > tolerance) {
        std::cout << "  [FALHOU!] Erro máximo: " << max_error << std::endl;
    } else {
        std::cout << "  [OK!] Resultado correto." << std::endl;
    }
}


// #############################################################################
// ### IMPLEMENTAÇÃO 1: CPU SERIAL (O BASAL)
// #############################################################################
void gemm_cpu_serial(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            float soma = 0.0f;
            for (int k = 0; k < N; ++k) {
                soma += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = soma;
        }
    }
}

// #############################################################################
// ### IMPLEMENTAÇÃO 2: CPU PARALELA (OPENMP)
// #############################################################################
void gemm_cpu_openmp(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C) {
    // collapse(2) diz ao OpenMP para paralelizar os DOIS loops externos
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            float soma = 0.0f;
            for (int k = 0; k < N; ++k) {
                soma += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = soma;
        }
    }
}

// #############################################################################
// ### IMPLEMENTAÇÃO 3: GPU INGÊNUA (KERNEL)
// #############################################################################
__global__ void kernel_gpu_naive(const float* A, const float* B, float* C) {
    
    // 1. Descobre quem é (qual célula (i, j) ele deve calcular)
    int i = blockIdx.y * blockDim.y + threadIdx.y; // Linha
    int j = blockIdx.x * blockDim.x + threadIdx.x; // Coluna

    // 2. Garante que não está fora da matriz
    if (i < N && j < N) {
        float soma = 0.0f;
        
        // 3. Faz a operação MAC (Produto Escalar) lendo da memória global
        for (int k = 0; k < N; ++k) {
            soma += A[i * N + k] * B[k * N + j];
        }
        
        // 4. Escreve o resultado na memória global
        C[i * N + j] = soma;
    }
}

// #############################################################################
// ### IMPLEMENTAÇÃO 4: GPU OTIMIZADA (TILING COM __shared__)
// #############################################################################
__global__ void kernel_gpu_tiled(const float* A, const float* B, float* C) {
    
    // 1. Define as matrizes de cache na Memória Compartilhada
    __shared__ float sA[TILE_DIM][TILE_DIM];
    __shared__ float sB[TILE_DIM][TILE_DIM];

    // 2. Identifica o thread e suas coordenadas globais (onde ele vai escrever)
    int i = blockIdx.y * blockDim.y + threadIdx.y; // Linha global
    int j = blockIdx.x * blockDim.x + threadIdx.x; // Coluna global

    // 3. Identifica as coordenadas locais do thread (dentro do bloco)
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    float soma = 0.0f;

    // 4. Loop pelos "blocos" (tiles) da matriz inteira
    for (int tile_k = 0; tile_k < N; tile_k += TILE_DIM) {
        
        // 5. CARREGAR O BLOCO PARA A MEMÓRIA COMPARTILHADA (Cooperação)
        sA[ty][tx] = (i < N && (tile_k + tx) < N) ? A[i * N + (tile_k + tx)] : 0.0f;
        sB[ty][tx] = ((tile_k + ty) < N && j < N) ? B[(tile_k + ty) * N + j] : 0.0f;

        // 6. BARREIRA DE SINCRONIZAÇÃO: Espera TODOS os threads do bloco terminarem de carregar
        __syncthreads();

        // 7. CALCULAR USANDO APENAS A MEMÓRIA RÁPIDA
        for (int k = 0; k < TILE_DIM; ++k) {
            soma += sA[ty][k] * sB[k][tx];
        }

        // 8. BARREIRA DE SINCRONIZAÇÃO: Espera TODOS os threads terminarem de calcular
        __syncthreads();
    }

    // 9. Escreve o resultado final na memória global
    if (i < N && j < N) {
        C[i * N + j] = soma;
    }
}


// #############################################################################
// ### IMPLEMENTAÇÃO 4.5: GPU OTIMIZADA (TILING + THREAD COARSENING)
// #############################################################################
// Otimização: Cada thread calcula 2 elementos da matriz C em vez de 1.
// Isso aumenta a "Intensidade Aritmética" e reutiliza dados carregados de A.
__global__ void kernel_gpu_coarsened(const float* __restrict__ A, 
                                     const float* __restrict__ B, 
                                     float* __restrict__ C) {
    
    // TILE_DIM deve ser 32.
    // Mas lançaremos o bloco com dimensão (16, 32).
    // Cada thread cuida de 2 colunas.
    const int COARSE_FACTOR = 2;

    __shared__ float sA[TILE_DIM][TILE_DIM];
    __shared__ float sB[TILE_DIM][TILE_DIM];

    int bx = blockIdx.x; int by = blockIdx.y;
    int tx = threadIdx.x; int ty = threadIdx.y;

    // Coordenadas globais da linha
    int row = by * TILE_DIM + ty;
    // Coordenadas globais da coluna (base)
    // Note que multiplicamos tx por COARSE_FACTOR porque cada thread cobre 2 espaços
    int colStart = bx * TILE_DIM + (tx * COARSE_FACTOR);

    float sum0 = 0.0f;
    float sum1 = 0.0f;

    for (int tile_k = 0; tile_k < N; tile_k += TILE_DIM) {
        
        // 1. CARREGAMENTO COLABORATIVO PARA SHARED MEMORY
        // Precisamos carregar um tile 32x32 inteiro.
        // Como temos apenas 32x16 threads (512 threads), cada thread precisa carregar 2 floats de A e 2 de B.
        
        // Carrega A
        sA[ty][tx * COARSE_FACTOR]     = A[row * N + (tile_k + tx * COARSE_FACTOR)];
        sA[ty][tx * COARSE_FACTOR + 1] = A[row * N + (tile_k + tx * COARSE_FACTOR + 1)];

        // Carrega B (Cuidado com índices aqui)
        // Para B, queremos carregar as colunas correspondentes
        sB[ty][tx * COARSE_FACTOR]     = B[(tile_k + ty) * N + colStart];
        sB[ty][tx * COARSE_FACTOR + 1] = B[(tile_k + ty) * N + colStart + 1];

        __syncthreads();

        // 2. CÁLCULO (A Mágica do Reuso)
        for (int k = 0; k < TILE_DIM; ++k) {
            float a_val = sA[ty][k]; // Lemos A UMA VEZ
            
            // E usamos DUAS VEZES
            sum0 += a_val * sB[k][tx * COARSE_FACTOR];
            sum1 += a_val * sB[k][tx * COARSE_FACTOR + 1];
        }

        __syncthreads();
    }

    // Escreve os 2 resultados
    if (row < N && colStart < N) {
        C[row * N + colStart]     = sum0;
        C[row * N + colStart + 1] = sum1;
    }
}


// #############################################################################
// ### FUNÇÃO PRINCIPAL (O "CHEFE")
// #############################################################################
int main() {
    std::cout << "Iniciando Benchmark GEMM (Tamanho: " << N << "x" << N << ")" << std::endl;

    // --- 1. Alocação de Memória no HOST (CPU) ---
    size_t memSize = N * N * sizeof(float);
    std::vector<float> h_A(N * N);
    std::vector<float> h_B(N * N);
    std::vector<float> h_C_serial(N * N);
    std::vector<float> h_C_parallel(N * N);
    std::vector<float> h_C_gpu(N * N); // Host C para receber dados da GPU

    // --- 2. Inicialização das Matrizes ---
    init_matrices(h_A, h_B);

    // --- 3. Alocação de Memória no DEVICE (GPU) ---
    float *d_A, *d_B, *d_C;
    gpuErrchk(cudaMalloc(&d_A, memSize));
    gpuErrchk(cudaMalloc(&d_B, memSize));
    gpuErrchk(cudaMalloc(&d_C, memSize));

    // --- 4. Copiar Matrizes Iniciais (A, B) para a GPU ---
    std::cout << "Copiando dados para a GPU..." << std::endl;
    gpuErrchk(cudaMemcpy(d_A, h_A.data(), memSize, cudaMemcpyHostToDevice));
    gpuErrchk(cudaMemcpy(d_B, h_B.data(), memSize, cudaMemcpyHostToDevice));

    // --- 5. Configuração do Lançamento CUDA ---
    dim3 threadsPerBlock(TILE_DIM, TILE_DIM); // 32x32 = 1024 threads
    dim3 numBlocks(N / TILE_DIM, N / TILE_DIM); // 4096/32 = 128 blocos

    // --- INÍCIO DOS BENCHMARKS ---
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    std::chrono::duration<double> diff;

    //--- TESTE 1: CPU SERIAL ---
    std::cout << "\nRodando Implementação 1: CPU Serial..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    gemm_cpu_serial(h_A, h_B, h_C_serial);
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "Tempo CPU Serial: " << diff.count() << " s" << std::endl;
    // (h_C_serial é nossa referência "correta")

    // --- TESTE 2: CPU PARALELA (OPENMP) ---
    std::cout << "\nRodando Implementação 2: CPU OpenMP..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    gemm_cpu_openmp(h_A, h_B, h_C_parallel);
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "Tempo CPU OpenMP: " << diff.count() << " s" << std::endl;
    verify_result(h_C_serial, h_C_parallel);
    

    // --- TESTE 3: GPU INGÊNUA ---
    std::cout << "\nRodando Implementação 3: GPU Ingênua..." << std::endl;
    
    // 1. AQUECIMENTO (Acorda a GPU)
    kernel_gpu_naive<<<numBlocks, threadsPerBlock>>>(d_A, d_B, d_C);
    gpuErrchk(cudaDeviceSynchronize());

    // 2. MEDIÇÃO REAL
    start = std::chrono::high_resolution_clock::now();
    kernel_gpu_naive<<<numBlocks, threadsPerBlock>>>(d_A, d_B, d_C);
    gpuErrchk(cudaDeviceSynchronize()); // Espera a GPU terminar
    end = std::chrono::high_resolution_clock::now();
    
    diff = end - start;
    std::cout << "Tempo GPU Ingênua: " << diff.count() << " s" << std::endl;
    gpuErrchk(cudaMemcpy(h_C_gpu.data(), d_C, memSize, cudaMemcpyDeviceToHost)); // Traz resultado
    verify_result(h_C_serial, h_C_gpu);

    // --- TESTE 4: GPU OTIMIZADA (TILED) ---
    std::cout << "\nRodando Implementação 4: GPU Otimizada (Tiled)..." << std::endl;
    
    // 1. AQUECIMENTO (Acorda a GPU)
    kernel_gpu_tiled<<<numBlocks, threadsPerBlock>>>(d_A, d_B, d_C);
    gpuErrchk(cudaDeviceSynchronize());

    // 2. MEDIÇÃO REAL
    start = std::chrono::high_resolution_clock::now();
    kernel_gpu_tiled<<<numBlocks, threadsPerBlock>>>(d_A, d_B, d_C);
    gpuErrchk(cudaDeviceSynchronize()); // Espera a GPU terminar
    end = std::chrono::high_resolution_clock::now();
    
    diff = end - start;
    std::cout << "Tempo GPU Otimizada: " << diff.count() << " s" << std::endl;
    gpuErrchk(cudaMemcpy(h_C_gpu.data(), d_C, memSize, cudaMemcpyDeviceToHost)); // Traz resultado
    verify_result(h_C_serial, h_C_gpu);

    // --- TESTE 4.5: GPU COARSENED ---
    std::cout << "\nRodando Implementação 4.5: GPU Coarsening (2x)..." << std::endl;
    
    // Configuração Especial:
    // Bloco de 32 de altura, mas apenas 16 de largura (total 512 threads)
    dim3 threadsCoarse(TILE_DIM / 2, TILE_DIM); 
    // O número de blocos continua o mesmo (cobrindo a matriz toda)
    dim3 blocksCoarse(N / TILE_DIM, N / TILE_DIM);

    // Aquecimento
    kernel_gpu_coarsened<<<blocksCoarse, threadsCoarse>>>(d_A, d_B, d_C);
    gpuErrchk(cudaDeviceSynchronize());

    start = std::chrono::high_resolution_clock::now();
    kernel_gpu_coarsened<<<blocksCoarse, threadsCoarse>>>(d_A, d_B, d_C);
    gpuErrchk(cudaDeviceSynchronize());
    end = std::chrono::high_resolution_clock::now();
    
    diff = end - start;
    std::cout << "Tempo GPU Coarsened: " << diff.count() << " s" << std::endl;
    
    // Validar
    gpuErrchk(cudaMemcpy(h_C_gpu.data(), d_C, memSize, cudaMemcpyDeviceToHost));
    verify_result(h_C_serial, h_C_gpu);


    // --- TESTE 5: GPU cuBLAS (NVIDIA) ---
    std::cout << "\nRodando Implementação 5: GPU cuBLAS (Profissional)..." << std::endl;
    cublasHandle_t handle;
    cublasCreate(&handle);
        
    const float alpha = 1.0f;
    const float beta = 0.0f;

    // -------> INÍCIO DA MUDANÇA <-------

    // 1. CHAMADA DE "AQUECIMENTO" (Warm-up) - FORA DO CRONÔMETRO
    // (Vamos rodar uma vez para a biblioteca se preparar)
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, N, N, &alpha, d_B, N, d_A, N, &beta, d_C, N);
    gpuErrchk(cudaDeviceSynchronize()); // Sincroniza o aquecimento

    // 2. CHAMADA REAL (CRONOMETRADA)
    auto start_cublas = std::chrono::high_resolution_clock::now();
        
    // C = alpha * A * B + beta * C
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, N, N, &alpha, d_B, N, d_A, N, &beta, d_C, N);
    gpuErrchk(cudaDeviceSynchronize()); // Espera a GPU terminar
        
    auto end_cublas = std::chrono::high_resolution_clock::now();

    // -------> FIM DA MUDANÇA <-------

    std::chrono::duration<double> diff_cublas = end_cublas - start_cublas;
    std::cout << "Tempo GPU cuBLAS: " << diff_cublas.count() << " s" << std::endl;
    gpuErrchk(cudaMemcpy(h_C_gpu.data(), d_C, memSize, cudaMemcpyDeviceToHost)); // Traz resultado
    verify_result(h_C_serial, h_C_gpu);

    cublasDestroy(handle);

    // --- FIM DOS BENCHMARKS E LIMPEZA ---
    std::cout << "\n--- Limpando memória da GPU ---" << std::endl;
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    std::cout << "Benchmark concluído." << std::endl;
    return 0;
}


//cuda-F526788A-keyring.gpg