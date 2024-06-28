import os
import random
import string

def generate_random_string(length):
    letters = string.ascii_letters + string.digits
    return ''.join(random.choice(letters) for i in range(length))

def create_large_file(filename, line_length, total_size_mb):
    total_size_bytes = total_size_mb * 1024 * 1024
    lines_written = 0

    # check file exists
    if os.path.exists(filename):
        print(f"File {filename} already exists. Exiting.")
        return
    with open(filename, 'w') as f:
        while f.tell() < total_size_bytes:
            random_string = generate_random_string(line_length)
            f.write(random_string + '\n')
            lines_written += 1
    print(f"File created with {lines_written} lines, each of length {line_length} characters.")

filename = 'database.txt'
line_length = 200
total_size_mb = 400

create_large_file(filename, line_length, total_size_mb)
