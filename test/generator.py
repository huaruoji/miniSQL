import random
import string
import os

def random_string(length):
    return ''.join(random.choices(string.ascii_lowercase, k=length))

def random_int():
    return random.randint(-10000, 10000)

def random_float():
    return round(random.uniform(-10000, 10000), 2)

def generate_create_database():
    db_name = f"db_{random_string(8)}"
    return f"CREATE DATABASE {db_name};\nUSE DATABASE {db_name};\n"

def generate_create_table():
    table_name = f"table_{random_string(6)}"
    num_columns = random.randint(3, 8)
    columns = []
    
    types = ['INTEGER', 'FLOAT', 'TEXT']
    for i in range(num_columns):
        col_name = f"col_{random_string(4)}"
        col_type = random.choice(types)
        columns.append(f"{col_name} {col_type}")
    
    return f"CREATE TABLE {table_name} (\n    " + ",\n    ".join(columns) + "\n);\n", table_name, columns

def generate_insert(table_name, columns):
    values = []
    for col in columns:
        if 'INTEGER' in col:
            values.append(str(random_int()))
        elif 'FLOAT' in col:
            values.append(str(random_float()))
        else:  # TEXT
            values.append(f"'{random_string(10)}'")
    
    return f"INSERT INTO {table_name} VALUES ({', '.join(values)});\n"

def generate_select(table_name, columns):
    num_cols = random.randint(1, len(columns))
    selected_cols = random.sample([col.split()[0] for col in columns], num_cols)
    
    if random.random() < 0.3:  # 30% chance to use *
        return f"SELECT * FROM {table_name};\n"
    return f"SELECT {', '.join(selected_cols)} FROM {table_name};\n"

def generate_test_file(num_tables=5, num_inserts_per_table=1000):
    output_dir = "test_cases"
    os.makedirs(output_dir, exist_ok=True)
    
    filename = os.path.join(output_dir, f"large_test_{random_string(6)}.sql")
    with open(filename, 'w') as f:
        # Create database
        f.write(generate_create_database())
        
        # Create tables and generate data
        tables_info = []
        for _ in range(num_tables):
            create_table_sql, table_name, columns = generate_create_table()
            f.write(create_table_sql)
            tables_info.append((table_name, columns))
            
            # Generate inserts
            for _ in range(num_inserts_per_table):
                f.write(generate_insert(table_name, columns))
            
            # Generate some select queries
            for _ in range(5):
                f.write(generate_select(table_name, columns))
            
            # Add some drop table queries
            if random.random() < 0.2:  # 20% chance to drop table
                f.write(f"DROP TABLE {table_name};\n")

if __name__ == "__main__":
    generate_test_file()