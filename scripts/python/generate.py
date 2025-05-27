import os
import subprocess
import glob
import argparse

def main(lps2lts_path):
    # Directory containing .lps files
    LPS_DIR = "../../examples"
    # Output directory for .lts files
    OUTPUT_DIR = "./lts"

    # Create output directory if it doesn't exist
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # Loop through all .lps files in the LPS_DIR
    for lps_file in glob.glob(os.path.join(LPS_DIR, '**', '*.lps'), recursive=True):
        # Get the base name of the file (without directory and extension)
        base_name = os.path.splitext(os.path.basename(lps_file))[0]

        if f"{base_name}.lts" in os.listdir(OUTPUT_DIR):
            print(f"Skipping {base_name} as it already exists.")
            continue
        
        # Generate the corresponding .lts file
        print(base_name)
        proc = subprocess.run(['timeout', '-s2', '10m', lps2lts_path, '-v', '--cached', '-rjittyc', lps_file, os.path.join(OUTPUT_DIR, f"{base_name}.lts")], check=True)

        # Check if the .lts file was generated successfully
        if proc.returncode != 0:
            print(f"Error: {base_name} failed to generate.")
            # Remove the generated file
            try:
                os.remove(os.path.join(OUTPUT_DIR, f"{base_name}.lts"))
            except FileNotFoundError:
                pass

    print("Labelled transition systems generated successfully.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generate labelled transition systems from LPS files.')
    parser.add_argument('-t', '--tool', required=True, help='Path to the lps2lts tool')
    args = parser.parse_args()
    print(args.tool)
    main(args.tool)
