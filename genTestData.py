import os
import argparse

def generate_random_binary_data(size_in_bytes: int, output_file: str):
    """
    Generate random binary data of the specified size and save it to a file.

    Args:
        size_in_bytes (int): The size of the random data in bytes.
        output_file (str): Path to the output file where data will be saved.
    """
    try:
        with open(output_file, 'wb') as file:
            file.write(os.urandom(size_in_bytes))
        print(f"Successfully saved {size_in_bytes} bytes of random binary data to '{output_file}'")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate random binary data and save it to a file.")
    parser.add_argument("size", type=int, help="Size of the random binary data in bytes.")
    parser.add_argument("output", type=str, help="Output file path to save the binary data.")

    args = parser.parse_args()

    generate_random_binary_data(args.size, args.output)
