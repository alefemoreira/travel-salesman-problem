import os
import csv
import numpy as np

def get_min_cost_execution(csv_file):
    data = np.genfromtxt('executions2/' + csv_file, delimiter=';', dtype=None, names=True, encoding=None)
    cost_index = 1  # Index of the 'custo' column
    time_index = 2  # Index of the 'tempo' column

    min_cost = np.min(data[data.dtype.names[cost_index]])
    rows_with_min_cost = data[data[data.dtype.names[cost_index]] == min_cost]

    min_execution_time = np.min(rows_with_min_cost[data.dtype.names[time_index]])
    row_with_min_values = rows_with_min_cost[rows_with_min_cost[data.dtype.names[time_index]] == min_execution_time][0]

    return row_with_min_values

def read_csv_files_and_get_min_values(csv_files):
    min_values = []
    for file in csv_files:
        min_row = get_min_cost_execution(file)
        min_cost = min_row[1]  # Index of the 'custo' column
        min_execution_time = min_row[2]  # Index of the 'tempo' column
        min_values.append({'File': os.path.basename(file), 'Min Cost': min_cost, 'Min Execution Time': min_execution_time})

    return min_values

def save_to_csv(output_file, data):
    with open(output_file, mode='w', newline='') as file:
        fieldnames = ['File', 'Min Cost', 'Min Execution Time']
        writer = csv.DictWriter(file, fieldnames=fieldnames)

        writer.writeheader()
        writer.writerows(data)

if __name__ == '__main__':
    # List of CSV files to read
    csv_files = ["eil51.csv","berlin52.csv","brazil58.csv","st70.csv","eil76.csv","pr76.csv","gr96.csv","rat99.csv","kroB100.csv","kroC100.csv","kroD100.csv","kroE100.csv","rd100.csv","kroA100.csv","eil101.csv","lin105.csv","pr107.csv","gr120.csv","pr124.csv","bier127.csv","ch130.csv","pr136.csv","gr137.csv","pr144.csv","kroA150.csv","kroB150.csv","ch150.csv","pr152.csv","u159.csv","si175.csv","brg180.csv","rat195.csv","d198.csv","kroA200.csv","kroB200.csv","gr202.csv","ts225.csv","tsp225.csv","pr226.csv","gr229.csv","gil262.csv","pr264.csv","a280.csv","pr299.csv","lin318.csv","linhp318.csv","rd400.csv"]  # Add your file names here

    # Get min cost and execution time from each file
    min_values_data = read_csv_files_and_get_min_values(csv_files)

    # Save the data to a new CSV file
    output_csv_file = 'min_values_output_2.csv'
    save_to_csv(output_csv_file, min_values_data)

    print(f"Minimum values from each file have been saved to '{output_csv_file}'.")
