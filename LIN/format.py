import json

input_file_path = 'perfcov.json'
output_file_path = 'perfcov.json'

with open(input_file_path, 'r') as input_file:
    data = json.load(input_file)

with open(output_file_path, 'w') as output_file:
    json.dump(data, output_file, indent=4, sort_keys=True)

print(f'Formatted JSON has been written to {output_file_path}')
