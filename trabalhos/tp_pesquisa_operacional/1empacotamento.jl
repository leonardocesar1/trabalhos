using JuMP
using Gurobi

function leitura(path::String)
    linhas = readlines(path)

    partes = split(linhas[1], '\t')
    n = parse(Int, partes[2])

    id = Int[]
    peso = Float64[]

    for linha in linhas[2:end]
        partes = split(linha, '\t')

        push!(id, parse(Int, partes[2]))
        push!(peso, parse(Float64, partes[3]))
    end

    return n, id, peso
end

function empacotamento(path::String)
    n, id, peso = leitura(path)
    model = Model(Gurobi.Optimizer)
    JuMP.set_optimizer_attribute(model, "OutputFlag", 0)

    @variable(model, x[1:n, 1:n], Bin)
    @variable(model, y[1:n], Bin)

    @constraint(model, [i=1:n], sum(x[i,j] for j in 1:n) == 1)
    @constraint(model, [j=1:n], sum(peso[i]*x[i,j] for i in 1:n) <= 20.0 * y[j])
    
    @objective(model, Min, sum(y[j] for j in 1:n))

    optimize!(model)
    valor = JuMP.objective_value(model)
    return valor
end

function main()
    entrada = ARGS[1]

    valor = empacotamento(entrada)
    println("TP1 2023087923 = $(valor)")
end

main()
