using JuMP
using Gurobi

function leitura(path::String)
    linhas = readlines(path)

    partes = split(linhas[1], '\t')
    n = parse(Int, partes[2])

    v = Int[]
    u = Int[]

    for linha in linhas[2:end]
        partes = split(linha, '\t')

        push!(v, parse(Int, partes[2]))
        push!(u, parse(Int, partes[3]))
    end

    return n, v, u
end

function coloracao(path::String)
    n, v, u = leitura(path)
    model = Model(Gurobi.Optimizer)
    JuMP.set_optimizer_attribute(model, "OutputFlag", 0)

    N_cores = n
    adj = falses(n, n)
    for i in 1:length(v)
        adj[v[i], u[i]] = true
        adj[u[i], v[i]] = true
    end

    @variable(model, y[1:N_cores], Bin)
    @variable(model, x[1:n, 1:N_cores], Bin)

    @constraint(model, [i=1:n], sum(x[i, :]) == 1)
    @constraint(model, [i=1:n, j=(i+1):n, k=1:N_cores; adj[i, j]], x[i, k] + x[j, k] <= 1)
    @constraint(model, x .<= y')

    @objective(model, Min, sum(y))

    optimize!(model)
    valor = JuMP.objective_value(model)
    return valor
end

function main()
    entrada = ARGS[1]

    valor = coloracao(entrada)
    println("TP1 2023087923 = $(valor)")
end

main()
