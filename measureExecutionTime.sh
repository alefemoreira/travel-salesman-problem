instances=("dantzig42" "swiss42" "att48" "gr48" "hk48" "eil51" "berlin52" "brazil58" "st70" "eil76" "pr76" "gr96" "rat99" "kroB100" "kroC100" "kroD100" "kroE100" "rd100" "kroA100" "eil101" "lin105" "pr107" "gr120" "pr124" "bier127" "ch130" "pr136" "gr137" "pr144" "kroA150" "kroB150" "ch150" "pr152" "u159" "si175" "brg180" "rat195" "d198" "kroA200" "kroB200" "gr202" "ts225" "tsp225" "pr226" "gr229" "gil262" "pr264" "a280" "pr299" "lin318" "linhp318" "rd400")
# instances=("dantzig42" "swiss42" "att48" "gr48" "hk48" "eil51" "berlin52" "brazil58" "st70" "eil76" "pr76" "pr76r" "gr96" "rat99" "kroA100" "kroB100" "kroC100" "kroD100" "kroE100" "rd100" "eil101" "lin105" "pr107")
make
# instances=("rat99")

executions=10
seeders=(2 3 5 7 11 13 17 19 23 29)

for instance in "${instances[@]}"; do
    echo Executando $instance
    echo "instancia;custo;tempo" > "executions-tsplist/$instance.csv"
    for i in {1..10}; do
        echo "  Execução $i"
        ./tsp instances/$instance.tsp ${seeders[i-1]} 1 >> "executions-tsplist/$instance.csv"
    done
done

