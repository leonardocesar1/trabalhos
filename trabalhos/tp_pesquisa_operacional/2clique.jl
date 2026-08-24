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

function clique(path::String)
    n, v, u = leitura(path)
    model = Model(Gurobi.Optimizer)
    JuMP.set_optimizer_attribute(model, "OutputFlag", 0)

    adj = falses(n, n)
    for i in 1:length(v)
        adj[v[i], u[i]] = true
        adj[u[i], v[i]] = true
    end

    @variable(model, x[1:n], Bin)

    @constraint(model, [i=1:n, j=(i+1):n; !adj[i, j]], x[i] + x[j] <= 1)

    @objective(model, Max, sum(x))

    optimize!(model)
    valor = JuMP.objective_value(model)
    return valor
end

function main()
    entrada = ARGS[1]

    valor = clique(entrada)
    println("TP1 2023087923 = $(valor)")
end

main()
