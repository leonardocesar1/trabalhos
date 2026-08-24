// Função SIMD para um único núcleo (single-core)
export void saxpy_ispc(uniform int N,
                       uniform float scale,
                       // REMOVIDO: uniform float X[], uniform float Y[],
                       uniform float result[])
{
    foreach (i = 0 ... N) {
        // LÓGICA ALTERADA: Gera o valor 'i' na hora para evitar leitura da memória
        float val = i;
        result[i] = scale * val + val;
    }
}

// Função que cada tarefa executará
task void saxpy_ispc_task(uniform int N,
                         uniform int span,
                         uniform float scale,
                         // REMOVIDO: uniform float X[], uniform float Y[],
                         uniform float result[])
{
    uniform int indexStart = taskIndex * span;
    uniform int indexEnd = min(N, indexStart + span);

    foreach (i = indexStart ... indexEnd) {
        // LÓGICA ALTERADA: Gera o valor 'i' na hora para evitar leitura da memória
        float val = i;
        result[i] = scale * val + val;
    }
}

// Função "gerente" que lança as tarefas para o multicore
export void saxpy_ispc_withtasks(uniform int N,
                               uniform float scale,
                               // REMOVIDO: uniform float X[], uniform float Y[],
                               uniform float result[])
{
    uniform int span = N / 64; // 64 tasks

    // REMOVIDO: X, Y da chamada da tarefa
    launch[N/span] saxpy_ispc_task(N, span, scale, result);
}