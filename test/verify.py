import json
import sys
import os
import subprocess
import tempfile
import csv
from io import StringIO

def parse_csv_output(csv_content):
    """Parse the CSV output into a list of dictionaries."""
    results = []
    csv_reader = csv.reader(StringIO(csv_content))
    
    try:
        # Read header row
        headers = next(csv_reader)
        
        # Read data rows
        for row in csv_reader:
            if not row:  # Skip empty rows
                continue
                
            # Convert values to appropriate types
            converted_row = {}
            for header, value in zip(headers, row):
                # Remove quotes if present
                if value.startswith('"') and value.endswith('"'):
                    value = value[1:-1]
                    
                try:
                    if '.' in value:
                        # Try float first
                        converted_row[header] = float(value)
                    else:
                        # Try integer, if fails keep as string
                        try:
                            converted_row[header] = int(value)
                        except ValueError:
                            converted_row[header] = value
                except ValueError:
                    # If conversion fails, keep as string
                    converted_row[header] = value
            
            results.append(converted_row)
            
    except Exception as e:
        print(f"Error parsing CSV: {e}")
        return []
        
    return results

def run_minidb(sql_file):
    """Run minidb.exe with the given SQL file and return the output."""
    try:
        # Get the directory containing the SQL file
        sql_dir = os.path.dirname(os.path.abspath(sql_file))
        minidb_path = os.path.join(sql_dir, "minidb.exe")
        
        # Create a temporary file for output
        with tempfile.NamedTemporaryFile(mode='w+', delete=False, suffix='.csv') as temp_out:
            temp_out_path = temp_out.name
        
        # Run minidb.exe with the required arguments
        process = subprocess.run(
            [minidb_path, sql_file, temp_out_path],
            capture_output=True,
            text=True
        )
        
        if process.returncode != 0:
            print("Error running minidb.exe:")
            print(process.stderr)
            return None
            
        # Read the output CSV file
        with open(temp_out_path, 'r') as f:
            output = f.read()
            
        # Clean up the temporary file
        os.unlink(temp_out_path)
        
        return output
        
    except Exception as e:
        print(f"Error running minidb: {e}")
        return None

def compare_results(sql_output, expected_json):
    """Compare SQL output with expected results from JSON."""
    with open(expected_json, 'r') as f:
        expected = json.load(f)
    
    # Parse the CSV output
    actual_results = parse_csv_output(sql_output)
    
    if not actual_results:
        print("❌ Failed to parse CSV output")
        return False
    
    # Get expected results
    expected_results = expected['result']
    
    if len(actual_results) != len(expected_results):
        print(f"❌ Row count mismatch. Expected {len(expected_results)}, got {len(actual_results)}")
        return False
    
    # Compare each row
    passed = True
    for i, (actual, expected) in enumerate(zip(actual_results, expected_results)):
        # Convert expected values to match actual types
        converted_expected = {}
        for key, value in expected.items():
            if isinstance(value, str):
                converted_expected[key] = value
            elif isinstance(value, (int, float)):
                # Match the type from actual result if present
                if key in actual and isinstance(actual[key], float):
                    converted_expected[key] = float(value)
                else:
                    converted_expected[key] = value
        
        # Compare rows
        if actual != converted_expected:
            print(f"❌ Row {i+1} mismatch:")
            print(f"Expected: {converted_expected}")
            print(f"Got:      {actual}")
            passed = False
    
    if passed:
        print("✅ Results match")
    
    return passed

def main():
    if len(sys.argv) != 2:
        print("Usage: python verify.py <sql_file>")
        print("The expected JSON file should be named <sql_file>_expected.json")
        sys.exit(1)
    
    sql_file = sys.argv[1]
    expected_json = sql_file.rsplit('.', 1)[0] + '_expected.json'
    
    if not os.path.exists(sql_file):
        print(f"Error: SQL file {sql_file} does not exist")
        sys.exit(1)
    
    if not os.path.exists(expected_json):
        print(f"Error: Expected JSON file {expected_json} does not exist")
        sys.exit(1)
    
    print("Running minidb.exe...")
    sql_output = run_minidb(sql_file)
    
    if sql_output is None:
        print("Failed to run minidb.exe")
        sys.exit(1)
    
    print("\nComparing results...")
    success = compare_results(sql_output, expected_json)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
