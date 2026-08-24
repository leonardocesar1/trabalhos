const E = 1e-9

function entrada(caminho::String)
    linhas = readlines(caminho)
    
    partes = split(strip(linhas[1]))
    n = parse(Int, partes[1])
    m = parse(Int, partes[2])
    
    c = parse.(Float64, split(strip(linhas[2])))
    
    A = zeros(Float64, n, m)
    b = zeros(Float64, n)
    
    for i in 1:n
        linha = split(strip(linhas[2+i]))
        for j in 1:m
            A[i, j] = parse(Float64, linha[j])
        end
        b[i] = parse(Float64, linha[m+1])
    end
    
    return n, m, c, A, b
end

function formatar(val)
    return abs(val) < E ? 0.0 : round(val, digits=3)
end

function saida(io::IO, estado::String, valor::Float64)
    if estado == "otimo"
        println(io, "otima")
        println(io, formatar(valor))
    elseif estado == "inviavel"
        println(io, "inviavel")
    elseif estado == "ilimitado"
        println(io, "ilimitada")
    end
end

function pivotar(T, base, indice_linha, indice_coluna, num_linhas, num_cols)
    pivo = T[indice_linha, indice_coluna]
    
    for j in 1:num_cols
        T[indice_linha, j] /= pivo
    end
    
    for i in 1:num_linhas
        if i != indice_linha && abs(T[i, indice_coluna]) > E
            fator = T[i, indice_coluna]
            for j in 1:num_cols
                T[i, j] -= fator * T[indice_linha, j]
            end
        end
    end
    base[indice_linha-1] = indice_coluna 
end

function simplex(n, m, c_orig, A_orig, b_orig)
    A = copy(A_orig)
    b = copy(b_orig)
    
    for i in 1:n
        if b[i] < -E
            b[i] *= -1
            A[i, :] .*= -1
        end
    end

    num_cols = m + n + 1
    T = zeros(Float64, n + 1, num_cols)
    
    T[2:end, 1:m] = A
    for i in 1:n
        T[i+1, m + i] = 1.0 
        T[i+1, end] = b[i]
    end
    
    base = collect(m+1 : m+n) 
    
    T[1, m+1 : m+n] .= -1.0
    
    for i in 1:n
        for j in 1:num_cols
             T[1, j] += T[i+1, j]
        end
    end

    while true
        max_c, indice_coluna_entrar = -1.0, -1
        for j in 1:(m + n) 
            if T[1, j] > E && T[1, j] > max_c
                max_c = T[1, j]
                indice_coluna_entrar = j
            end
        end

        if indice_coluna_entrar == -1; break; end 

        min_razao, indice_linha_sair = Inf, -1
        indice_base_desempate = Inf 
        
        for i in 2:n+1
            if T[i, indice_coluna_entrar] > E
                razao = T[i, end] / T[i, indice_coluna_entrar]
                if razao < min_razao - E
                    min_razao = razao
                    indice_linha_sair = i
                    indice_base_desempate = base[i-1] 
                elseif abs(razao - min_razao) < E 
                    if base[i-1] < indice_base_desempate 
                        indice_linha_sair = i
                        indice_base_desempate = base[i-1]
                    end
                end
            end
        end

        if indice_linha_sair == -1; break; end 

        pivotar(T, base, indice_linha_sair, indice_coluna_entrar, n+1, num_cols)
    end

    valor_objetivo_fase_um = T[1, end] 
    if valor_objetivo_fase_um > E 
        return "inviavel", 0.0
    end

    c_full = [c_orig; zeros(n)] 
    
    T[1, :] .= 0.0 
    for j in 1:m 
        T[1, j] = c_full[j]
    end
    
    valor_z = 0.0
    for i in 1:n 
        c_base = c_full[base[i]] 
        if abs(c_base) > E 
            for j in 1:num_cols-1 
                T[1, j] -= c_base * T[i+1, j] 
            end
            valor_z += c_base * T[i+1, end]
        end
    end
    
    T[1, end] = -valor_z 
    
    while true
        max_c, indice_coluna_entrar = -1.0, -1
        for j in 1:m 
            if T[1, j] > E && T[1, j] > max_c
                max_c = T[1, j]
                indice_coluna_entrar = j
            end
        end

        if indice_coluna_entrar == -1 
            valor_final_otimo = -T[1, end] 
            return "otimo", valor_final_otimo
        end

        min_razao, indice_linha_sair = Inf, -1
        indice_base_desempate = Inf 
        
        for i in 2:n+1
            if T[i, indice_coluna_entrar] > E
                razao = T[i, end] / T[i, indice_coluna_entrar]
                if razao < min_razao - E
                    min_razao = razao
                    indice_linha_sair = i
                    indice_base_desempate = base[i-1]
                elseif abs(razao - min_razao) < E 
                    if base[i-1] < indice_base_desempate
                        indice_linha_sair = i
                        indice_base_desempate = base[i-1]
                    end
                end
            end
        end

        if indice_linha_sair == -1 
            return "ilimitado", 0.0
        end
        
        pivotar(T, base, indice_linha_sair, indice_coluna_entrar, n+1, num_cols)
    end
end

function main()
    if length(ARGS) < 2
        println("Uso: julia tp2_xxxxxx.jl <caminho_entrada> <caminho_saida>")
        return
    end
    
    caminho_entrada = ARGS[1]
    caminho_saida = ARGS[2]
    
    n, m, c, A, b = entrada(caminho_entrada)
    
    estado, valor = simplex(n, m, c, A, b)
    
    open(caminho_saida, "w") do io
        saida(io, estado, valor)
    end
end

if abspath(PROGRAM_FILE) == @__FILE__
    main()
end