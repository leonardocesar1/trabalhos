using JuMP
using Gurobi

function leitura(path::String)
    linhas = readlines(path)

    partes = split(linhas[1], '\t')
    n = parse(Int, partes[2])

    id = String[]
    num = Int[]
    valor = Float64[]

    for linha in linhas[2:end]
        partes = split(linha, '\t')

        push!(id, partes[1])
        push!(num, parse(Int, partes[2]))
        push!(valor, parse(Float64, partes[3]))
    end

    return n, id, num, valor
end

function lotsizing(path::String)
    n, id, num, valor = leitura(path)
    model = Model(Gurobi.Optimizer)
    JuMP.set_optimizer_attribute(model, "OutputFlag", 0)

    c = zeros(Float64, n)
    d = zeros(Float64, n)
    h = zeros(Float64, n)
    p = zeros(Float64, n)

    for i in 1:length(id)
        periodo = num[i]
        if id[i] == "c"
            c[periodo] = valor[i]
        elseif id[i] == "d"
            d[periodo] = valor[i]
        elseif id[i] == "s"
            h[periodo] = valor[i]
        elseif id[i] == "p"
            p[periodo] = valor[i]
        end
    end

    @variable(model, x[1:n] >= 0)
    @variable(model, s[1:n] >= 0)
    @variable(model, b[1:n] >= 0)

    @objective(model, Min, sum(c[i]*x[i] + h[i]*s[i] + p[i]*b[i] for i in 1:n))

    @constraint(model, x[1] - s[1] + b[1] == d[1])
    @constraint(model, [i=2:n], x[i] + s[i-1] - b[i-1] - s[i] + b[i] == d[i])
    @constraint(model, s[n] == 0)
    @constraint(model, b[n] == 0)

    optimize!(model)
    valor = JuMP.objective_value(model)
    return valor
end

function main()
    entrada = ARGS[1]

    valor = lotsizing(entrada)
    println("TP1 2023087923 = $(valor)")
end

main()
