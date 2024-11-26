import random
import string
import os
import json

def random_string(length):
    return ''.join(random.choices(string.ascii_lowercase, k=length))

def random_int():
    return random.randint(-10000, 10000)

def random_float():
    return round(random.uniform(-10000, 10000), 2)

def generate_create_database():
    db_name = f"db_{random_string(8)}"
    return f"CREATE DATABASE {db_name};\nUSE DATABASE {db_name};\n", db_name

def generate_create_table():
    table_name = f"table_{random_string(6)}"
    num_columns = random.randint(3, 8)
    columns = []
    column_types = []
    
    types = ['INTEGER', 'FLOAT', 'TEXT']
    for i in range(num_columns):
        col_name = f"col_{random_string(4)}"
        col_type = random.choice(types)
        columns.append(f"{col_name} {col_type}")
        column_types.append((col_name, col_type))
    
    return f"CREATE TABLE {table_name} (\n    " + ",\n    ".join(columns) + "\n);\n", table_name, column_types

def generate_insert(table_name, columns):
    values = []
    row_data = {}
    for col_name, col_type in columns:
        if col_type == 'INTEGER':
            val = random_int()
            values.append(str(val))
            row_data[col_name] = val
        elif col_type == 'FLOAT':
            val = random_float()
            values.append(str(val))
            row_data[col_name] = val
        else:  # TEXT
            val = random_string(10)
            values.append(f"'{val}'")
            row_data[col_name] = val
    
    return f"INSERT INTO {table_name} VALUES ({', '.join(values)});\n", row_data

def generate_select(table_name, columns):
    num_cols = random.randint(1, len(columns))
    selected_cols = random.sample([col[0] for col in columns], num_cols)
    
    if random.random() < 0.3:  # 30% chance to use *
        query = f"SELECT * FROM {table_name};\n"
        selected_cols = [col[0] for col in columns]
    else:
        query = f"SELECT {', '.join(selected_cols)} FROM {table_name};\n"
    
    return query, selected_cols

def generate_test_file(num_tables=3, num_inserts_per_table=10):
    output_dir = "test"
    os.makedirs(output_dir, exist_ok=True)
    
    test_name = f"test_{random_string(6)}"
    sql_filename = os.path.join(output_dir, f"{test_name}.sql")
    expected_filename = os.path.join(output_dir, f"{test_name}_expected.json")
    
    # Store expected results
    expected = {
        "tables": {},
        "queries": []
    }
    
    with open(sql_filename, 'w') as f:
        # Create database
        create_db, db_name = generate_create_database()
        f.write(create_db)
        expected["database"] = db_name
        
        # Create tables and generate data
        tables_info = []
        for _ in range(num_tables):
            create_table_sql, table_name, columns = generate_create_table()
            f.write(create_table_sql)
            
            # Initialize table in expected results
            expected["tables"][table_name] = {
                "columns": {name: type for name, type in columns},
                "rows": []
            }
            
            # Generate inserts
            for _ in range(num_inserts_per_table):
                insert_sql, row_data = generate_insert(table_name, columns)
                f.write(insert_sql)
                expected["tables"][table_name]["rows"].append(row_data)
            
            # Generate some select queries
            for _ in range(3):
                select_sql, selected_cols = generate_select(table_name, columns)
                f.write(select_sql)
                
                # Add query results to expected output
                query_result = []
                for row in expected["tables"][table_name]["rows"]:
                    query_result.append({col: row[col] for col in selected_cols})
                
                expected["queries"].append({
                    "query": select_sql.strip(),
                    "result": query_result
                })
            
            # Add some drop table queries
            if random.random() < 0.2:  # 20% chance to drop table
                f.write(f"DROP TABLE {table_name};\n")
                expected["tables"][table_name]["dropped"] = True
    
    # Write expected results to JSON file
    with open(expected_filename, 'w') as f:
        json.dump(expected, f, indent=2)

if __name__ == "__main__":
    generate_test_file()