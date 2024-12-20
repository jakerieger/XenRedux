import os
import argparse
from pathlib import Path


def generate_shader_header(shader_file, output_file):
    try:
        with open(shader_file, 'r') as f:
            source = f.read()
    except Exception as e:
        print(f"Error: {e}")

    name = os.path.splitext(os.path.basename(shader_file))[0] + "_Source"
    header_content = f'const char* {name} = R"(\n{source}\n)";\n'

    with open(output_file, 'w') as out:
        out.write(header_content)

    print(f" -- Successfully wrote header: {name}.")


def process_shader_directory(source_dir, output_dir):
    print(f" -- Processing source directory: {source_dir}")
    source_path = Path(source_dir)
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)
    for shader_file in source_path.glob("*.glsl"):
        output_file = output_path / (shader_file.stem + ".h")
        print(f" -- Generating header: {output_file}")
        generate_shader_header(shader_file, output_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate C++ header files from GLSL shader sources.")
    parser.add_argument("source_dir", type=str, help="Directory containing shader source files.")
    parser.add_argument("output_dir", type=str, help="Output directory to save the header file.")
    args = parser.parse_args()
    process_shader_directory(args.source_dir, args.output_dir)
