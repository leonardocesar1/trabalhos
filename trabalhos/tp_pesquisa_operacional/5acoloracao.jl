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

function acoloracao(path::String)
    n, v, u = leitura(path)
    model = Model(Gurobi.Optimizer)
    JuMP.set_optimizer_attribute(model, "OutputFlag", 0)

    cores = 1:n
    vertices = 1:n
    arestas = [(v[i], u[i]) for i in 1:length(v)]

    @variable(model, x[vertices, cores], Bin)
    @variable(model, y[cores], Bin)
    @variable(model, z[1:length(arestas), cores, cores], Bin)

    @objective(model, Max, sum(y))

    @constraint(model, [i in vertices], sum(x[i, c] for c in cores) == 1)
    @constraint(model, [k in 1:length(arestas), c in cores], x[arestas[k][1], c] + x[arestas[k][2], c] <= 1)
    @constraint(model, [i in vertices, c in cores], x[i, c] <= y[c])
    @constraint(model, [k in 1:length(arestas), i in cores, j in cores; i != j], z[k, i, j] >= x[arestas[k][1], i] + x[arestas[k][2], j] - 1)
    @constraint(model, [k in 1:length(arestas), i in cores, j in cores; i != j], z[k, i, j] <= x[arestas[k][1], i])
    @constraint(model, [k in 1:length(arestas), i in cores, j in cores; i != j], z[k, i, j] <= x[arestas[k][2], j])
    @constraint(model, [i in cores, j in (i+1):last(cores)], y[i] + y[j] - 1 <= sum(z[k, i, j] + z[k, j, i] for k in 1:length(arestas)))

    optimize!(model)
    valor = JuMP.objective_value(model)
    return valor
end

function main()
    entrada = ARGS[1]

    valor = acoloracao(entrada)
    println("TP1 2023087923 = $(valor)")
end

main()
