instances=("burma14" "ulysses16" "gr17" "gr21" "ulysses22" "gr24" "fri26" "bayg29" "bays29" "dantzig42" "swiss42" "att48" "gr48" "hk48" "eil51" "berlin52" "brazil58" "st70" "eil76" "pr76" "gr96" "rat99" "kroB100" "kroC100" "kroD100" "kroE100" "rd100" "kroA100" "eil101" "lin105" "pr107" "gr120" "pr124" "bier127" "ch130" "pr136" "gr137" "pr144" "kroA150" "kroB150" "ch150" "pr152" "u159" "si175" "brg180" "rat195" "d198" "kroA200" "kroB200" "gr202" "ts225" "tsp225" "pr226" "gr229" "gil262" "pr264" "a280" "pr299" "lin318" "linhp318" "rd400" "fl417" "gr431" "pcb442" "d493" "att532" "ali535" "si535")

executions=10

for instance in "${instances[@]}"; do
    echo Executando $instance
    echo "instancia;custo;tempo" > "executions/$instance.csv"
    for i in {1..10}; do
        echo "  Execução $i"
        ./tsp instances/$instance.tsp >> "executions/$instance.csv"
    done
done

